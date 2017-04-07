CSE 40166 / 60166, Computer Graphics, Fall 2011
Alexandri Zavodny

Code Example: Model Viewer / Projection

    This program is a simple model viewer, where the user can look at and rotate
    a 3D model in the scene with the mouse. The user can also, more importantly, 
    change the current projection matrix by pressing 'p' - this lets them
    toggle between perspective projection (the default) and orthographic
    projection.

    Note that the camera never moves; in this example, the model is sitting
    in the same place, but it rotates about its center of mass.


Usage: 
    The user can click and drag with the left mouse button to affect the
    current rotational value of the model. The user can press 'p' to toggle
    the current matrix that is fed into GL_PROJECTION; by default it is
    a perspective projection matrix, but the user can switch it back and
    forth to and from an orthographic projection matrix as well.

    The user can also press the 'q' key to quit the program.

Compilation Instructions:
    Simply navigate to the directory and type 'make.' Only main.cpp needs
    to be linked with the OpenGL / freeglut libraries.

Notes:
    Users are encouraged to open the code and play with the parameters of the
    projections to see what the results look like.
