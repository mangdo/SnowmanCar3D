
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "../BaseCodes/BasicShapeObjs.h"
#include "../BaseCodes/Mesh.h"
#include <iostream>
#include <string>
#include <stack>

static float g_d = 0.03f;
static float g_d2 = 0.02f;

static float g_delta = glm::radians(25.f);//재밌게하고 싶으면 델타2도 만들어서 여러개로 꺾어라.
float g_delta2 = glm::radians(45.f);
float g_delta3 = glm::radians(60.f);

static float g_deltaL = 0;

static std::string g_initial_str = "X";



std::string Rule(char in)//과제 ppt뒷부분.
{
	std::string out;
	if (in == 'X') out = "S[&&[+X]][&[-XL]]LF[^^[-Xf]][^[+X]]FX!L";

	else if (in == 'S') out = "SS";
	else if (in == 'F') out = "FF";

	else                  out = in;//원래모양그대로 아웃풋해라

	return out;
}

std::string Reproduce(std::string input)
{
	std::string output;

	for (unsigned int i = 0; i<input.size(); i++)
	{
		output = output + Rule(input[i]);
	}

	return output;
}

void CreateLSystemTree(int iteration, Mesh &out_mash)
{
	int fnum = 0;
	// Element 1
	Mesh trunk = glm::scale(glm::vec3(0.01f, g_d, 0.01f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_cylinder_mesh;
	trunk.SetColor(0.50, 0.35, 0.10, 1.0);

	Mesh trunk2 = glm::scale(glm::vec3(0.01f, g_d2, 0.01f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_cylinder_mesh;
	trunk2.SetColor(0.50, 0.35, 0.10, 1.0);

	Mesh leaf = glm::scale(glm::vec3(0.01f, 0.01f, 0.05f))
		* glm::translate(glm::vec3(0.f, 0.5f, -0.8f))
		* g_cylinder_mesh;
	float leafColor = 0.3;
	float leafChange=0;
	leaf.SetColor(leafColor, 1.0, leafColor, 1.0);

	if (iteration == 1) {
		leafChange = 0.5;
	}
	else if (iteration == 2) {
		leafChange = 0.2;
	}
	else if (iteration == 3) {
		leafChange = 0.06;
	}
	else if (iteration == 4) {
		leafChange = 0.005;
	}
	else if (iteration == 5) {
		leafChange = 0.0008;
	}


	Mesh fruit = glm::scale(glm::vec3(0.04f, 0.035f, 0.04f))
		* glm::translate(glm::vec3(0.f, 0.5f, 0.f))
		* g_sphere_mesh;
	fruit.SetColor(1.f, 0.5f, 0.f, 1.f);

	Mesh fruit2 = glm::scale(glm::vec3(0.01f, 0.01f, 0.01f))
		* glm::translate(glm::vec3(0.f, 4.5f, 0.f))
		* g_sphere_mesh;
	fruit2.SetColor(0.08f, 0.4f, 0.1f, 0.1f);

	g_delta2 = glm::radians(60.f);

	// String Reproduction
	std::string str = g_initial_str; // 엄청긴게 들어오겠지?

	for (int i = 0; i<iteration; i++)
		str = Reproduce(str);


	std::stack<glm::mat4> stack_T;
	glm::mat4 T(1);
	for (unsigned int i = 0; i<str.size(); i++)
	{
		if (str[i] == 'F')
		{
			out_mash += T*trunk;
			T = T * glm::translate(glm::vec3(0, g_d, 0));
		}
		else if (str[i] == 'S')
		{
			out_mash += T*trunk;
			T = T * glm::translate(glm::vec3(0, g_d2, 0));
		}
		else if (str[i] == '+')
		{
			glRotated(g_delta, 1, 0, 0);
			T = T * glm::rotate(g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '-')
		{
			glRotated(-g_delta, 1, 0, 0);
			T = T * glm::rotate(-g_delta, glm::vec3(1, 0, 0));
		}
		else if (str[i] == '[')
		{
			stack_T.push(T);
		}
		else if (str[i] == ']')
		{
			T = stack_T.top();
			stack_T.pop();
		}
		else if (str[i] == 'L') {
			out_mash += T*glm::rotate(-g_deltaL, glm::vec3(0, 1, 0))*leaf;
			g_deltaL += 10;
		}
		else if (str[i] == 'f') {
			fnum++;

			if (fnum > 40) {
				fnum = 0;
				out_mash += T*fruit2;

				out_mash += T*fruit;
			}
		}
		else if (str[i] == '^')
		{
			glRotated(g_delta2, 1, 0, 0);
			T = T * glm::rotate(g_delta2, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '&')
		{
			glRotated(-g_delta2, 1, 0, 0);
			T = T * glm::rotate(-g_delta2, glm::vec3(0, 1, 0));
		}
		else if (str[i] == '!')//색바꾸기
		{
			if (leafColor < 0.95) {
				leafColor += leafChange;
				leaf.SetColor(leafColor, 1.0, leafColor, 1.0);
			}


		}
		else if (str[i] == '/')
		{
			glRotated(g_delta3, 1, 0, 0);
			T = T * glm::rotate(g_delta3, glm::vec3(0, 0, 1));
		}
		else if (str[i] == '|')
		{
			glRotated(-g_delta3, 1, 0, 0);
			T = T * glm::rotate(-g_delta3, glm::vec3(0, 0, 1));
		}
	}
}
