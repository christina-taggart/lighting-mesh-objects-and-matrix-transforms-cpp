<a target="_blank" href="http://minus.com/i/leVl8ih9FN2C"><img src="http://i.minus.com/jleVl8ih9FN2C.png" border="0"/></a>

# Description

 * OpenGL, glut and GLSL application
 * Reads in a mesh file, and stores data in a VBO
 * Draws the data, uses glm for matrix transforms
 * Shaders implement Gouraud and full Phong shading
 * User may manipulate specified light variables through the keyboard
 * This program uses base code provided by I. Dunn and Z. Wood  
 * (original .m loader by H. Hoppe)

# Compile & Run

 * make
 * ./a.out

 # Keyboard Inputs:
 * 'e' - default: rotate (spins outside of virtual trackball, rotates inside trackball)
 * 'w' - effects view/camera transform (zooms in)
 * 's' - effects view/camera transform (zooms out)
 * 'a' - effects view/camera transform (revolves left around object)
 * 'd' - effects view/camera transform (revolves left around object)
 * 'n' - effects object/image translation (moves object to the left)
 * 'm' - effects object/image translation (moves object to the right)
 * 'j' - effects object/image translation (moves object downwards)
 * 'k' - effects object/image translation (moves object upwards)
 * '+' - "shift +" effects object's scale (move mouse left to scale smaller, right to scale larger)
 * 'p' - effects the object/image's material brightness (makes material brighter)
 * 'o' - effects the object/image's material brightness (lessens material brightness)
 * 'x' - effects position of the light (moves light to the left)
 * 'z' - effects position of the light (moves light to the right)
 * 'c' - effects position of the light (moves light upwards)
 * 'v' - effects position of the light (moves light downwards)
 * 'r' - resets the mesh back to origin of the world, with original size and orientation
 * 'q' - quit the program

# Setup
I set up my dinosaur so that initially it is darker, and the user may add light to make it look
its best (i.e., initially the tyrannosaurus is dark, but this is intentional.)
After adding some amount of 'p' or material brightness, it becomes illuminated and the user can see 
a nice chiaroscuro of light from dark).
 
