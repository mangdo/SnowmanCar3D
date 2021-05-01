

#include "GL/glew.h"
#include "glm/glm.hpp"
#include <iostream>
#include <fstream>
#include "../BaseCodes/Mesh.h"
#include "CarModel.h"


///////////////////////////////////////////////
// Two-Triangle

static GLuint g_car_vao;
static GLuint g_car_vbo_pos;
static GLuint g_car_vbo_color;
static GLuint g_car_vbo_normal;
static GLuint g_car_index_buf;

static Mesh g_car_mesh;

void InitCarModel()
{
	// 3D데이터 파일 obj
	g_car_mesh.ReadObjFormatFile("../Data/newcar.obj");
	
	g_car_mesh.TriangulateAndUniformizeBuffers();
	//obj파일내에서 삼각형이 아닌것들도 있음 그걸 찾아내서 삼각형으로 만들어준다.

	//g_car_mesh.ScaleUniformlyVertices(0.1f);//크기를 1/100로 줄여준다.
	//g_car_mesh.RotateVertices(glm::radians(-90.f), glm::vec3(1, 0, 0)); //꼭지점을 전체돌림 
	//g_car_mesh.RotateVertices(glm::radians(180.f), glm::vec3(0, 1, 0));
	g_car_mesh.SetColor(0.40, 0.30, 0.10, 1.0);//전체적인 컬러를 모든꼭지점에대해 똑같이 해준다.

	// Vertex Array Object
	glGenVertexArrays(1, &g_car_vao);
	glBindVertexArray(g_car_vao);

	//g_car_mesh.num_points 
	//g_car_mesh.points() : 배열이름 나옴.
	// Position VBO
	glGenBuffers(1, &g_car_vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*g_car_mesh.num_points(), g_car_mesh.points(), GL_STATIC_DRAW);

	// Color VBO
	glGenBuffers(1, &g_car_vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*g_car_mesh.num_points(), g_car_mesh.colors(), GL_STATIC_DRAW);

	// Normal VBO
	glGenBuffers(1, &g_car_vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*g_car_mesh.num_points(), g_car_mesh.normals(), GL_STATIC_DRAW);


	// Index Buffer Object
	glGenBuffers(1, &g_car_index_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*3*g_car_mesh.num_triangles(), g_car_mesh.triangle_ids(), GL_STATIC_DRAW);


	// Vertex Attribute
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_color);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
}


void DrawCarModel()
{
	glBindVertexArray(g_car_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);//그릴때 g_car_index_buf순서로 그릴예정.
	

	glDrawElements(GL_TRIANGLES, g_car_mesh.num_triangles()*3, GL_UNSIGNED_INT, NULL);//진짜로 그림.
}


void DeleteCarModel()
{
	glDeleteBuffers(1, &g_car_vbo_pos);
	glDeleteBuffers(1, &g_car_vbo_color);
	glDeleteBuffers(1, &g_car_vbo_normal);
	glDeleteBuffers(1, &g_car_index_buf);
	glDeleteVertexArrays(1, &g_car_vao);
}




