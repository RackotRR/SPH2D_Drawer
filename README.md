# SPH2D_Drawer
Visualization tool for my SPH2D project

It's based on parts of my first public c++ game [RAB_RTS_2](https://github.com/RackotRR/RAB_RTS_2). 
Graphics here is **SDL2** library.
In order to accelerate data reading I'm using **mio** library with **OpenMP**: there are a lot of data files, which are loaded in parallel using memory mapping.

The whole data is loaded into memory and you can navigate through time layers by keyboard: *A*/*D* for changing one layer per click, and *S*/*W* for changing layers while keys are pressed.
You can click *Space* to run/stop automatic layers changing. *Enter* click returns time layers iterator at the beggining.

You can move across current time layer by mouse or by *IJKL* keys.
*Z*/*X* click scales layer.
