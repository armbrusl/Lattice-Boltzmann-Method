# LatticeBoltzmanMethod

In order for this code to work add the header file (olcPixelGameEngine.h) found at https://github.com/OneLoneCoder/olcPixelGameEngine.
<br /> <br />

This program is an Implementation of the LBM Tau 1 algorithm found in:

Maciej Matyka; Michal Dzikowski 2021. \textit{Memory-efficient Lattice Boltzmann method of low Reynold number flows}. Computer Physics Communications 267.

Commands one can use so far:

* **Left Click (Hold)** &rarr; Set a 5 x 5 blocks X and Y velocity to 0.  <br />
* **Right Click (Hold)** &rarr; Include an Obstalce (F=1) at the cursor position.  <br />

* **I Button (Pressed)** &rarr; Resets the entire grid. <br /> <br />

* **S Button (Pressed) ** &rarr; Safe all the Data to a file (matrix U, V, F, R and fx, fy, U0, steps). <br /> 
* **U Button (Hold)** +  **UP Button (Pressed)** / **DOWN Button (Pressed)**&rarr; Increasing / Decreasing the "pressure" velocity fx. <br /> 
* **R Button (Pressed)**&rarr; Reset the entire system. <br /> <br /> 
