CSCI444	
Advanced Computer Graphics
Colorado School of Mines
Spring 2017

Dr. Jeffrey Paone

Code Example: Assignment 1 Solution: OpenGL 4.1 Context using GLFW

    This program creates an OpenGL 4.1 Core Profile context using GLFW.  A cube
	is stored in a VAO with corresponding VBOs.  The associated shaders implement
	Blinn-Phong Shading per fragment.

Usage: 
    The user can click and drag with the left mouse button to affect the
    current rotational value of the model.  Holding left shift allows the user to zoom
	in/out while dragging the left mouse button.

    The user can also press the 'q' key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'mingw32-make.' 
	
	You can also type 'mingw32-make clean' to clean up the build files.
	
	'mingw32-make new' forces a rebuild of all object files.

Notes:
    This example will ONLY work in the computer labs on campus.  Developed in
	Computer Commons in CTLM.  GLEW and GLFW is included pre-compiled.  OpenGL and
	glm currently reside on the machines.  This example is not guaranteed
	to compile on your personal machine.  You may need to tweak your installations
	of GLFW and GLEW.  With the provided Makefile, this will on compile on Windows
	machines.

	
Copyright 2017 Dr. Jeffrey Paone
	
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.