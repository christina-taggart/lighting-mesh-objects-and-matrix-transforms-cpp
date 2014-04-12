all:
	g++ MainSimple.cpp CMeshLoaderSimple.cpp GLSL_helper.cpp -DGL_GLEXT_PROTOTYPES -framework OpenGL -framework GLUT 
