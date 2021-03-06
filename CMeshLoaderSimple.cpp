/* Christina Taggart
 * Program 3: Lighting
 * CSC 471 with Professor Zoe Wood
 * Description: Creates useful data structures (and operations) for vertices, triangles, colors, normals and positions 
 * Loads VBO from Mesh. This includes:
 *	Resizing and repositioning mesh at origin, caluclating normals for all faces, applying normals to each vertex
 *	Creating a VBO by generating buffers using handles, creating stl vectors to store vertices, colors, normals, and indices
 *	Iterating through all the triangles in the mesh and copying data into corresponding stl vectors
 *	Using an IBO to ensure vertices are in order (optional, may implement without an IBO)
 *	Computes necessary binding for data such as position and color
*/

#include "CMeshLoaderSimple.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <iomanip>
#include <string>
#include <streambuf>

using namespace std;

class SColor
{

public:

    float Red, Green, Blue;

    SColor()
        : Red(0.7f), Green(0.7f), Blue(0.7f)
    {}

};

class SVector3
{

public:

	float X, Y, Z;

	SVector3()
		: X(0), Y(0), Z(0)
	{}

	SVector3(float in)
		: X(in), Y(in), Z(in)
	{}

	SVector3(float in_x, float in_y, float in_z)
		: X(in_x), Y(in_y), Z(in_z)
	{}

	SVector3 crossProduct(SVector3 const & v) const
	{
		return SVector3(Y*v.Z - v.Y*Z, v.X*Z - X*v.Z, X*v.Y - v.X*Y);
	}

	float dotProduct(SVector3 const & v) const
	{
		return X*v.X + Y*v.Y + Z*v.Z;
	}

	float length() const
	{
		return sqrtf(X*X + Y*Y + Z*Z);
	}

	SVector3 operator + (SVector3 const & v) const
	{
		return SVector3(X+v.X, Y+v.Y, Z+v.Z);
	}

	SVector3 & operator += (SVector3 const & v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;

		return * this;
	}

	SVector3 operator - (SVector3 const & v) const
	{
		return SVector3(X-v.X, Y-v.Y, Z-v.Z);
	}

	SVector3 & operator -= (SVector3 const & v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;

		return * this;
	}

	SVector3 operator * (SVector3 const & v) const
	{
		return SVector3(X*v.X, Y*v.Y, Z*v.Z);
	}

	SVector3 & operator *= (SVector3 const & v)
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;

		return * this;
	}

	SVector3 operator / (SVector3 const & v) const
	{
		return SVector3(X/v.X, Y/v.Y, Z/v.Z);
	}

	SVector3 & operator /= (SVector3 const & v)
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;

		return * this;
	}

	SVector3 operator * (float const s) const
	{
		return SVector3(X*s, Y*s, Z*s);
	}

	SVector3 & operator *= (float const s)
	{
		X *= s;
		Y *= s;
		Z *= s;

		return * this;
	}

	SVector3 operator / (float const s) const
	{
		return SVector3(X/s, Y/s, Z/s);
	}

	SVector3 & operator /= (float const s)
	{
		X /= s;
		Y /= s;
		Z /= s;

		return * this;
	}

};

class SVertex
{

public:

    SVector3 Position;
    SColor Color;
    SVector3 Normal;
};

class CMesh
{

    friend class CVertexBufferObject;
    friend class CMeshLoader;

    struct STriangle
    {
        unsigned int VertexIndex1, VertexIndex2, VertexIndex3;
		SColor Color;

    };

    std::vector<SVertex> Vertices;
    std::vector<STriangle> Triangles;

    CMesh();

public:

    ~CMesh();

	void centerMeshByAverage(SVector3 const & CenterLocation);
	void centerMeshByExtents(SVector3 const & CenterLocation);
	void resizeMesh(SVector3 const & Scale);

};

bool CMeshLoader::loadVertexBufferObjectFromMesh(std::string const & fileName, int & TriangleCount, GLuint & PositionBufferHandle, GLuint & ColorBufferHandle, GLuint & NormalBufferHandle, GLuint & IBOHandle)
{
	CMesh * Mesh = loadASCIIMesh(fileName);
	if (! Mesh)
		return false;

    //resize the mesh and to reposition it at the origin
  	Mesh->resizeMesh(SVector3(1));
  	Mesh->centerMeshByExtents(SVector3(0));

  	for(int i = 0; i< Mesh->Triangles.size(); i++){
  		SVector3 norm = (Mesh->Vertices[Mesh->Triangles[i].VertexIndex2].Position - Mesh->Vertices[Mesh->Triangles[i].VertexIndex1].Position).crossProduct( 
    		(Mesh->Vertices[Mesh->Triangles[i].VertexIndex3].Position - Mesh->Vertices[Mesh->Triangles[i].VertexIndex1].Position) );
  		Mesh->Vertices[Mesh->Triangles[i].VertexIndex1].Normal += norm;
  		Mesh->Vertices[Mesh->Triangles[i].VertexIndex2].Normal += norm;
  		Mesh->Vertices[Mesh->Triangles[i].VertexIndex3].Normal += norm;

  	}

  	//create a VBO for the mesh
  	createVertexBufferObject(* Mesh, TriangleCount, PositionBufferHandle, ColorBufferHandle, NormalBufferHandle, IBOHandle);

	return true;
}


void CMeshLoader::createVertexBufferObject(CMesh const & Mesh, int & TriangleCount, GLuint & PositionBufferHandle, GLuint & ColorBufferHandle, GLuint & NormalBufferHandle, GLuint & IBOHandle)
{

	//create stl vectors to store the vertices, colors, normals (and indices for the IBO)
	std::vector<GLfloat> VertexPositions;
    std::vector<GLfloat> Colors;
    std::vector<GLfloat> Normals;
    std::vector<GLuint> Indices; 

    // Iterate over all the triangles in the Mesh and copy the vertices into the stl vector (likewise for the colors) 
    //note that you can do this without an IBO by ordering the vertices correctly
    for(int i = 0; i< Mesh.Vertices.size(); i++){ 

    	VertexPositions.push_back(Mesh.Vertices[i].Position.X);
    	VertexPositions.push_back(Mesh.Vertices[i].Position.Y);
    	VertexPositions.push_back(Mesh.Vertices[i].Position.Z);

		Normals.push_back(Mesh.Vertices[i].Normal.X);
		Normals.push_back(Mesh.Vertices[i].Normal.Y);
		Normals.push_back(Mesh.Vertices[i].Normal.Z);

    }

    for(int j = 0; j<Mesh.Triangles.size(); j++){
        Colors.push_back(Mesh.Triangles[j].Color.Red);
    	Colors.push_back(Mesh.Triangles[j].Color.Green);
    	Colors.push_back(Mesh.Triangles[j].Color.Blue);

    	Indices.push_back(Mesh.Triangles[j].VertexIndex1);
    	Indices.push_back(Mesh.Triangles[j].VertexIndex2);
    	Indices.push_back(Mesh.Triangles[j].VertexIndex3);
    }

    //generate buffers using handles
   	//compute all the necessary binding (eg glBindBuffer and glBufferData for the position data and color data
    //send to BUFFER on Graphics card
    glGenBuffers(1, &PositionBufferHandle); 
    glBindBuffer(GL_ARRAY_BUFFER, PositionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, VertexPositions.size()*sizeof(GLfloat), & VertexPositions.front(), GL_STATIC_DRAW); 
    
    glGenBuffers(1, &ColorBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, Colors.size()*sizeof(GLfloat), & Colors.front(), GL_STATIC_DRAW); 

    glGenBuffers(1, &NormalBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, NormalBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, Normals.size()*sizeof(GLfloat), & Normals.front(), GL_STATIC_DRAW); 

    glGenBuffers(1, &IBOHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(GLuint), & Indices.front(), GL_STATIC_DRAW); 

    //reset the triangle count to now be the size of the mesh's # triangles
    TriangleCount = Indices.size();

}


CMesh * const CMeshLoader::loadASCIIMesh(std::string const & fileName)
{
	std::ifstream File;
	File.open(fileName.c_str());

	if (! File.is_open())
	{
		std::cerr << "Unable to open mesh file: " << fileName << std::endl;
		return 0;
	}

	CMesh * Mesh = new CMesh();

	while (File)
	{
		std::string ReadString;
		std::getline(File, ReadString);

		std::stringstream Stream(ReadString);

		std::string Label;
		Stream >> Label;

		if (Label.find("#") != std::string::npos)
		{
			// Comment, skip
			continue;
		}

		if ("Vertex" == Label)
		{
			int Index;
			Stream >> Index; // We don't care, throw it away

			SVector3 Position;
			Stream >> Position.X;
			Stream >> Position.Y;
			Stream >> Position.Z;

			SVertex Vertex;
			Vertex.Position = Position;

			Mesh->Vertices.push_back(Vertex);
		}
		else if ("Face" == Label)
		{
			int Index;
			Stream >> Index; // We don't care, throw it away

			int Vertex1, Vertex2, Vertex3;
			Stream >> Vertex1;
			Stream >> Vertex2;
			Stream >> Vertex3;

			CMesh::STriangle Triangle;
			Triangle.VertexIndex1 = Vertex1 - 1;
			Triangle.VertexIndex2 = Vertex2 - 1;
			Triangle.VertexIndex3 = Vertex3 - 1;

			size_t Location;
			if ((Location = ReadString.find("{")) != std::string::npos) // there is a color
			{
				Location = ReadString.find("rgb=(");
				Location += 5; // rgb=( is 5 characters

				ReadString = ReadString.substr(Location);
				std::stringstream Stream(ReadString);
				float Color;
				Stream >> Color;
				Triangle.Color.Red = Color;
				Stream >> Color;
				Triangle.Color.Green = Color;
				Stream >> Color;
				Triangle.Color.Blue = Color;
			}

			Mesh->Triangles.push_back(Triangle);
		}
		else if ("" == Label)
		{
			// Just a new line, carry on...
		}
		else if ("Corner" == Label)
		{
			// We're not doing any normal calculations... (oops!)
		}
		else
		{
			std::cerr << "While parsing ASCII mesh: Expected 'Vertex' or 'Face' label, found '" << Label << "'." << std::endl;
		}
	}

	if (! Mesh->Triangles.size() || ! Mesh->Vertices.size())
	{
		delete Mesh;
		return 0;
	}

	return Mesh;
}



CMesh::CMesh()
{}

CMesh::~CMesh()
{}

void CMesh::centerMeshByAverage(SVector3 const & CenterLocation)
{
	/* not using this function */
}


void CMesh::centerMeshByExtents(SVector3 const & CenterLocation)
{
    if (Vertices.size() < 2)
    return;

    SVector3 Min, Max;
    {
        std::vector<SVertex>::const_iterator it = Vertices.begin();
        Min = it->Position;
        Max = it->Position;
        for (; it != Vertices.end(); ++ it)
        {
            if (Min.X > it->Position.X)
                Min.X = it->Position.X;
            if (Min.Y > it->Position.Y)
                Min.Y = it->Position.Y;
            if (Min.Z > it->Position.Z)
                Min.Z = it->Position.Z;

            if (Max.X < it->Position.X)
                Max.X = it->Position.X;
            if (Max.Y < it->Position.Y)
                Max.Y = it->Position.Y;
            if (Max.Z < it->Position.Z)
                Max.Z = it->Position.Z;
        }
    }

    SVector3 Center = (Max + Min) / 2;

    SVector3 VertexOffset = CenterLocation - Center;
    for (std::vector<SVertex>::iterator it = Vertices.begin(); it != Vertices.end(); ++ it)
        it->Position += VertexOffset;
}


void CMesh::resizeMesh(SVector3 const & Scale)
{
    if (Vertices.size() < 2)
    return;

    SVector3 Min, Max;
    {
        std::vector<SVertex>::const_iterator it = Vertices.begin();
        Min = it->Position;
        Max = it->Position;
        for (; it != Vertices.end(); ++ it)
        {
            if (Min.X > it->Position.X)
                Min.X = it->Position.X;
            if (Min.Y > it->Position.Y)
                Min.Y = it->Position.Y;
            if (Min.Z > it->Position.Z)
                Min.Z = it->Position.Z;

            if (Max.X < it->Position.X)
                Max.X = it->Position.X;
            if (Max.Y < it->Position.Y)
                Max.Y = it->Position.Y;
            if (Max.Z < it->Position.Z)
                Max.Z = it->Position.Z;
        }
    }

    SVector3 Extent = (Max - Min);
    SVector3 Resize = Scale / std::max(Extent.X, std::max(Extent.Y, Extent.Z));
    for (std::vector<SVertex>::iterator it = Vertices.begin(); it != Vertices.end(); ++ it)
        it->Position *= Resize;
}


