#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm> 
#include <random>						// std::default_random_engine
#include<dos.h>							 // for delay()

using namespace std::this_thread;		 // sleep_for, sleep_until
using namespace std::chrono_literals;	 // ns, us, ms, s, h, etc.
using namespace std;

class LBM : public olc::PixelGameEngine
{
private:
	int W = 300;
	int H = 80;
	int length = W * H;
	float fx = 0.0;
	float U0 = 0.0;
	float fy = 0.0;

	const int ex[9] = { 0,1,0,-1,0,1,-1,-1,1 };
	const int ey[9] = { 0,0,1,0,-1,1,1,-1,-1 };
	const int inv[9] = { 0,3,4,1,2,7,8,5,6 };
	const float w[9] = { 4 / 9.,1 / 9.,1 / 9.,1 / 9.,1 / 9.,1 / 36.,1 / 36.,1 / 36.,1 / 36. };
	const float pi = 3.141592f;
	string name;

	int c = 0, evals = 0;
	float maxVpx, minVpx, minVpy, maxVpy;
	float U[2][300][80], V[2][300][80], R[2][300][80];
	int F[300][80];

public:

	void initializeLDC()
	{
		U0 = 0.7;
		name = "LDC";
		for (int i = 0; i < W; i++) 
		{
			for (int j = 0; j < H; j++) 
			{
				U[0][i][j] = U[1][i][j] = 0;
				V[0][i][j] = V[1][i][j] = 0;
				R[0][i][j] = R[1][i][j] = 1;
				F[i][j] = 0;

				if (i == 0 || i == W - 1 || j == H - 1) { F[i][j] = 1;}
				if (j == 0) { U[0][i][j] = U[1][i][j] = U0; }
			}
		}


	}
	void initializeTunnel()
	{
		fx = 0.000001;
		name = "TUNNEL";
		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				U[0][i][j] = U[1][i][j] = 0;
				V[0][i][j] = V[1][i][j] = 0;
				R[0][i][j] = R[1][i][j] = 1;
				F[i][j] = 0;

				if (j == 0 || j == H - 1) { F[i][j] = 1; }
			}
		}


	}
	void initializeVCS()
	{
		fx = 0.00005;
		name = "VKS";
		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				U[0][i][j] = U[1][i][j] = 0;
				V[0][i][j] = V[1][i][j] = 0;
				R[0][i][j] = R[1][i][j] = 1;
				F[i][j] = 0;

				if (j == 0 || j == H - 1) { F[i][j] = 1; }
				if (pow(i - W/4, 2) + pow(j - H/2, 2) <= 60) { F[i][j] = 1;}
			}
		}


	}

	void keyboard_input() {


		if (GetKey(olc::Key::U).bHeld && GetKey(olc::Key::UP).bPressed) {
			fx += 0.00001;
		}
		else if (GetKey(olc::Key::U).bHeld && GetKey(olc::Key::DOWN).bPressed) {
			fx -= 0.00001;
		}
		if (GetKey(olc::Key::S).bPressed) {
			safe_file();
		}

		if (GetKey(olc::Key::R).bPressed) {
			initializeVCS();
		}
		

	}
	void mouse_input() {
		int mx = GetMouseX();
		int my = GetMouseY();


		if (GetMouse(0).bHeld)
		{
			for (int i = mx - 5; i < mx + 5; i++) {
				for (int j = my - 5; j < my + 5; j++) {

					if(pow(i-mx, 2) + pow(j-my, 2) <= 6)
						U[1 - c][i][j] = 0;
						V[1 - c][i][j] = 0;
				}
			}
		}

		if (GetMouse(1).bHeld)
		{
			F[mx][my] = 1;
			drawF();
		}

	}

	void update()
	{
		maxVpx = 0.0;
		minVpx = 0.0;

		maxVpy = 0.0;
		minVpy = 0.0;

		float r, u, v, f;

		for (int i = 0; i < W; i++) 
		{

			for (int j = 1; j < H; j++) 
			{
				if (F[i][j] == 0) 
				{
					U[c][i][j] = V[c][i][j] = R[c][i][j] = 0;
					for (int k = 0; k < 9; k++) 
					{
						int ip = (i + ex[k] + W ) % W, jp = (j + ey[k] + H ) % H, ik = inv[k];

						if (F[ip][jp] == 0) 
						{
							r = R[1 - c][ip][jp];
							u = (U[1 - c][ip][jp] + fx) / r;
							v = (V[1 - c][ip][jp] + fy) / r;

							f = w[ik] * r * (1 - (3 / 2.) * (u * u + v * v) + 3. * (ex[ik] * u + ey[ik] * v) + (9 / 2.) * (ex[ik] * u + ey[ik] * v) * (ex[ik] * u + ey[ik] * v));
						}
						else 
						{
							f = w[ik] * R[1 - c][i][j];
						}

						R[c][i][j] += f;
						U[c][i][j] += ex[ik] * f;
						V[c][i][j] += ey[ik] * f;
					}

					if (U[c][i][j] > maxVpx) maxVpx = U[c][i][j];
					if (U[c][i][j] < minVpx) minVpx = U[c][i][j];

					if (V[c][i][j] > maxVpy) maxVpy = V[c][i][j];
					if (V[c][i][j] < minVpy) minVpy = V[c][i][j];
				}
			}
		}
		c = 1 - c;
		evals++;
	}

	void print() {

		string result = to_string(evals);
		DrawString(20, H + 10, result, olc::WHITE, 1);

		result = to_string(fx);
		DrawString(20, H + 30, result, olc::WHITE, 1);

	}

	void drawF()
	{
		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				(F[i][j] == 1) ? Draw(i, j, olc::BLACK) : Draw(i, j, olc::WHITE);

			}
		}
	}	
	void draw() {

		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				if(F[i][j] == 0)
				{
					float u = (U[c][i][j] + abs(minVpx)) / (abs(minVpx) + maxVpx);
					//float v = (V[c][i][j] + abs(minVpy)) / (abs(minVpy) + maxVpy);
					float v = 0;
					float absvel = sqrt(pow(u, 2) + pow(v, 2));
					
					float colu1 = 255 * (0.5 + 0.5 * cos(2 * pi * (1.0 * absvel + 0.3)));
					float colu2 = 255 * (0.5 + 0.5 * cos(2 * pi * (1.0 * absvel + 0.2)));
					float colu3 = 255 * (0.5 + 0.5 * cos(2 * pi * (1.0 * absvel + 0.2)));

					Draw(i, j, olc::Pixel(255 - colu1, 255 - colu2, 255 - colu3));
				}

			}
		}

		FillRect(0, H, 100, 50, olc::BLACK);
	}

	void safe_file() {
		// Writing the data into a textfile to be visualized later on.
		ofstream outdata;
		string file_name = "data/" + name + to_string(evals) + ".txt";
		outdata.open(file_name);
		if (!outdata) { cout << "Error: Could not creaste textfile to save data." << endl; exit(1); }
		outdata << W << endl;
		outdata << H << endl;
		outdata << fx << endl;
		outdata << evals << endl;

		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				outdata << U[c][i][j] << endl;
			}
		}
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				outdata << V[c][i][j] << endl;
			}
		}
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				outdata << R[c][i][j] << endl;
			}
		}
		for (int i = 0; i < W; i++) {
			for (int j = 0; j < H; j++) {
				outdata << F[i][j] << endl;
			}
		}

		std::cout << "The file: " + file_name + " was saved." << std::endl;
		outdata.close();
	}

	bool OnUserCreate() override
	{
		initializeVCS();
		drawF();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{		
		keyboard_input();
		if (evals % 200 == 0) {
			safe_file();
		}
		mouse_input();
		update();
		draw();
		print();
		return true;
	}
};

int main()
{
	LBM demo;
	if (demo.Construct(300, 80, 4, 4))
		demo.Start();
	return 0;
}
