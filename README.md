# DLA Generation
## Description
This project is a simulation of Diffusion-Limited Aggregation (DLA), which models the growth of particle clusters by random motion and attachment. The simulation outputs the generated structure as a PNG image and optionally as a text file representing the grid.

The project is cross-platform, supporting both Windows and Unix-based systems (e.g., Linux), with dynamic path handling for output files.

## Features
Simulates DLA particle clustering on a 2D grid.
Outputs the final cluster as a PNG image.
Optionally outputs the grid as a text file for further analysis.
Dynamic path setting based on the executable location.
Cross-platform support for Windows and Unix-like systems.

## Installation
Install one of the releases and then extract the zip file to the directory of your choice. There is no compilation needed to run the project as the exe is already included in the release.

### Compilation
#### VSCode
1. Open the folder in VSCode.
2. Press tab.
3. Type in "task " or click on the "Run Task" option.
4. Click on the compilation task that works with the compiler you are using. (MSVC) or (GCC)

## Usage
### Arguments
1. A positive integer that is the size of the grid, cannot be 0.
2. A positive integer that is the number of particles made available for each row. Do not make larger that the size of the grid or the program will break and must be ended manualy.
3. A positive integer that is the maximum number of iterations per particle before the particle position is reset.
4. A string that is the name of the png output of the grid example: output.png.
5. A boolean value that when enabled, will output a txt file with the grid data in it. (Requires a character but only "on" will enable the txt output)
6. A string that is the name of the txt output of the grid example: output.txt. (Only matters if argument five is "on") 

### Output
#### PNG File:
The primary output is a PNG file showing the DLA-generated structure.
#### Text File (optional):
A grid representation of the structure in a text format for debugging or further analysis.
#### Project Structure
```
DLA-generation
├── .vscode
│   ├── c_cpp_properties.json
│   ├── launch.json
│   ├── settings.json
│   └── tasks.json
├── build
│   ├── dla.obj
│   ├── dla_generation
│   ├── dla_generation.exe
│   ├── dla_generation.ilk
│   ├── dla_generation.pdb
│   ├── lodepng.obj
│   ├── main.obj
│   ├── save_grid.obj
│   └── vc140.pdb
├── include
│   ├── lodepng
│   │   ├── lodepng.c
│   │   └── lodepng.h
│   └── defs.h
├── out
│   ├── text-test.png
│   └── text-test.txt
├── src
│   ├── dla.c
│   ├── main.c
│   └── save_grid.c
└── .gitignore
```



## How it Works
### Dynamic Path Handling:
The program dynamically sets the output and log paths based on the executable’s location. If the executable is in a build/ directory, the paths are modified to use the corresponding out/ and log/ directories.

### Simulation:
The grid initializes with randomly placed particles, which move based on predefined rules and stick to adjacent particles, creating clusters.
Output Generation:

The result of the simulation is saved as a PNG image, and optionally, a text representation of the grid can be saved.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements
LodePNG: Used for PNG image encoding.
Contributors: Thank you to all contributors who have helped improve this project.
