==== ezloader ====
ezloader is a small library providing a simple interface to load Wavefront .obj files with OpenGL.

Funciton ezload(...) accepts a file pointer to a .obj file. It immediately draws the given object without transformations of any kind. All provided texture coordinates and surface normals are used. ezload will autogenerate surface normals if they are not specified. ezload returns 0 on success.

Loading textures from texture libraries will be supported in the future.

Function ezloadCallList(...) accepts a call list index as returned by glGenLists(...), and a file pointer. It creates a call list to render the object, essentially wrapping ezload(...). It returns a 0 on success.

Currently a work in progress.

Uses SOIL, the Simple OpenGL Image Library found at http://www.lonesock.net/soil.html.

To compile the test program, use
gcc loadertest.c ezloader.c -g -lm -lSOIL -lGL -lGLU -lglut

==== Detailed operation description ====
