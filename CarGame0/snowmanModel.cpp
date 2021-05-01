
#include "GL/glew.h"
#include "glm/glm.hpp"
#include <iostream>
#include <fstream>
#include "../BaseCodes/Mesh.h"
#include "../BaseCodes/BasicShapeObjs.h"
#include "snowmanModel.h"


///////////////////////////////////////////////
// Two-Triangle

static GLuint g_car_vao;
static GLuint g_car_vbo_pos;
static GLuint g_car_vbo_color;
static GLuint g_car_vbo_normal;
static GLuint g_car_index_buf;

static Mesh g_car_mesh;

void InitSnowManModel()
{
	CreateSnowMan(g_car_mesh);


	//	g_car_mesh.ScaleUniformlyVertices(0.1f);//크기를 1/100로 줄여준다.
	//g_car_mesh.RotateVertices(glm::radians(-90.f), glm::vec3(1, 0, 0)); //꼭지점을 전체돌림 
	//g_car_mesh.RotateVertices(glm::radians(180.f), glm::vec3(0, 1, 0));
//	g_car_mesh.SetColor(1.f, 0.f, 0.f, 1);//전체적인 컬러를 모든꼭지점에대해 똑같이 해준다.

	// Vertex Array Object
	glGenVertexArrays(1, &g_car_vao);
	glBindVertexArray(g_car_vao);

	//g_car_mesh.num_points 
	//g_car_mesh.points() : 배열이름 나옴.
	// Position VBO
	glGenBuffers(1, &g_car_vbo_pos);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * g_car_mesh.num_points(), g_car_mesh.points(), GL_STATIC_DRAW);

	// Color VBO
	glGenBuffers(1, &g_car_vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * g_car_mesh.num_points(), g_car_mesh.colors(), GL_STATIC_DRAW);

	// Normal VBO
	glGenBuffers(1, &g_car_vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, g_car_vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * g_car_mesh.num_points(), g_car_mesh.normals(), GL_STATIC_DRAW);


	// Index Buffer Object
	glGenBuffers(1, &g_car_index_buf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * g_car_mesh.num_triangles(), g_car_mesh.triangle_ids(), GL_STATIC_DRAW);


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


void DrawSnowManModel()
{
	glBindVertexArray(g_car_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_car_index_buf);//그릴때 g_car_index_buf순서로 그릴예정.


	glDrawElements(GL_TRIANGLES, g_car_mesh.num_triangles() * 3, GL_UNSIGNED_INT, NULL);//진짜로 그림.
}


void DeleteSnowManModel()
{
	glDeleteBuffers(1, &g_car_vbo_pos);
	glDeleteBuffers(1, &g_car_vbo_color);
	glDeleteBuffers(1, &g_car_vbo_normal);
	glDeleteBuffers(1, &g_car_index_buf);
	glDeleteVertexArrays(1, &g_car_vao);
}

void CreateSnowMan(Mesh &out_mash)
{

	Mesh foot = glm::scale(glm::vec3(0.09f, 0.07f, 0.09f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_sphere_mesh;
	foot.SetColor(0.9f, 0.9f, 1.f, 1.f);

	Mesh body = glm::scale(glm::vec3(0.25f, 0.2f, 0.25f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_sphere_mesh;
	foot.SetColor(0.9f, 0.9f, 1.f, 1.f);
	
	Mesh head = glm::scale(glm::vec3(0.19f, 0.2f, 0.19f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_sphere_mesh;
	head.SetColor(0.9f, 0.9f, 1.f, 1.f);

	Mesh hand = glm::scale(glm::vec3(0.015f, 0.24, 0.015f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_cylinder_mesh;
	hand.SetColor(0.50, 0.35, 0.10, 1.0);

	Mesh eye= glm::scale(glm::vec3(0.025f, 0.025f, 0.025f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_sphere_mesh;
	eye.SetColor(0.f, 0.f, 0.f);

	Mesh nose= 
		glm::scale(glm::vec3(0.03f, 0.03f, 0.04f))*
		glm::rotate(glm::radians(+90.f), glm::vec3(1.f, 0.f, 0.f))
		*g_capsule_mesh;
	nose.SetColor(1.f, 0.5f, 0.1f);

	Mesh clothes=
		glm::scale(glm::vec3(0.03f, 0.03f, 0.03f))*
		glm::rotate(glm::radians(+90.f), glm::vec3(0.f, 0.f, 1.f))
		*g_capsule_mesh;
	clothes.SetColor(0.f, 0.f, 0.f);

	Mesh hear = glm::scale(glm::vec3(0.005f, 0.14, 0.005f))
		* glm::translate(glm::vec3(0.f, 0.8f, 0.f))
		* g_cylinder_mesh;
	hear.SetColor(0.0, 0.0, 0.0, 1.0);

	Mesh fruit = glm::scale(glm::vec3(0.04f, 0.035f, 0.04f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_sphere_mesh;
	fruit.SetColor(1.f, 0.5f, 0.f, 1.f);

	Mesh fruit2 = glm::scale(glm::vec3(0.01f, 0.01f, 0.01f))
		* glm::translate(glm::vec3(0.f, 4.5f, 0.f))
		* g_sphere_mesh;
	fruit2.SetColor(0.08f, 0.4f, 0.1f, 0.1f);

	out_mash += glm::translate(glm::vec3(.17f, 0.35f, 0.f))*foot;
	out_mash += glm::translate(glm::vec3(-.17f, .35f, 0.f))*foot;
	out_mash += glm::translate(glm::vec3(0.f, .45f, 0.f))*body;
	out_mash += glm::translate(glm::vec3(0.f, .75f, 0.f))*glm::scale(glm::vec3(0.65f, .6f, 0.65f))*body;
	out_mash += glm::translate(glm::vec3(0.f, .95f, 0.f))*head;
	out_mash += glm::translate(glm::vec3(0.1f, .78f, 0.f))* glm::rotate(glm::radians(-50.f), glm::vec3(0.f, 0.0f, 1.f))*hand;//오른손
	out_mash += glm::translate(glm::vec3(-0.1f, .78f, 0.f))* glm::rotate(glm::radians(+50.f), glm::vec3(0.f, 0.0f, 1.f))*hand;//왼손

	out_mash += glm::translate(glm::vec3(-.08f, 1.15f, .15f))*eye;//왼쪽눈
	out_mash += glm::translate(glm::vec3(+.08f, 1.15f, .15f))*eye;//오른쪽눈
	out_mash += glm::translate(glm::vec3(.0f, 1.1f, .2f))*nose;

	out_mash += glm::translate(glm::vec3(.0f, .85f, .15f))*clothes;
	out_mash += glm::translate(glm::vec3(.0f, .7f, .23f))*clothes;

	out_mash += glm::translate(glm::vec3(.0f, .6f, .25f))*clothes;


	out_mash += glm::translate(glm::vec3(.0f, 1.2f, 0.f))*hear;
	out_mash += glm::translate(glm::vec3(-0.f, 1.2f, 0.f))* glm::rotate(glm::radians(+15.f), glm::vec3(0.f, 0.0f, 1.f))*hear;//왼손
	out_mash += glm::translate(glm::vec3(-0.f, 1.2f, 0.f))* glm::rotate(glm::radians(-15.f), glm::vec3(0.f, 0.0f, 1.f))*hear;//왼손

	out_mash += glm::scale(glm::vec3(1.5f, 1.5f, 1.5f))*glm::translate(glm::vec3(-.15f, .25f, 0.2f))*fruit;
	out_mash += glm::scale(glm::vec3(1.5f, 1.5f, 1.5f))*glm::translate(glm::vec3(-.15f, .25f, 0.2f))*fruit2;

	out_mash += glm::scale(glm::vec3(1.7f, 1.7f, 1.7f))*glm::translate(glm::vec3(-.16f, .22f, 0.23f))*fruit;
	out_mash += glm::scale(glm::vec3(1.7f, 1.7f, 1.7f))*glm::translate(glm::vec3(-.16f, .22f, 0.23f))*fruit2;



}
