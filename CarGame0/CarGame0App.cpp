

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "CarGame0App.h"
#include "../BaseCodes/Camera.h"
#include "../BaseCodes/GroundObj2.h"
#include "../BaseCodes/InitShader.h"
#include "../BaseCodes/BasicShapeObjs.h"
#include "CarModel.h"
#include "TreeModelL.h"
#include "GroundTexture.h"

#include "snowmanModel.h"
#include "basketModel.h"

#include <iostream>




// Window and User Interface
static bool g_left_button_pushed;
static bool g_right_button_pushed;
static int g_last_mouse_x;
static int g_last_mouse_y;

extern GLuint g_window_w;
extern GLuint g_window_h;

//////////////////////////////////////////////////////////////////////
// Camera 
//////////////////////////////////////////////////////////////////////
static Camera g_camera;
static int g_camera_mode = 0;


//////////////////////////////////////////////////////////////////////
//// Define Shader Programs
//////////////////////////////////////////////////////////////////////
GLuint s_program_id;



//////////////////////////////////////////////////////////////////////
//// Animation Parameters
//////////////////////////////////////////////////////////////////////
float g_elaped_time_s = 0.f;	// 


//////////////////////////////////////////////////////////////////////
//// Car Position, Rotation, Velocity
//// �ڵ��� ���� ������.
//////////////////////////////////////////////////////////////////////
glm::vec3 g_car_poisition(0.f, 0.f, 0.f); //��ġ
float g_car_speed = 0;			          // �ӵ� (�ʴ� �̵� �Ÿ�)
float g_car_rotation_y = 0;		          // ���� ���� (y�� ȸ��)
float g_car_angular_speed = 0;	          // ȸ�� �ӵ� (���ӵ� - �ʴ� ȸ�� ��)

float g_cameraRadian;
glm::vec3 g_ro;
glm::vec3 g_cameralook;

bool night_mode;




/**
InitOpenGL: ���α׷� �ʱ� �� ������ ���� ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
OpenGL�� ������ �ʱ� ���� ���α׷��� �ʿ��� �ٸ� �ʱ� ���� �����Ѵ�.
�������, VAO�� VBO�� ���⼭ ������ �� �ִ�.
*/
void InitOpenGL()
{
	night_mode = false;
	s_program_id = CreateFromFiles("../Shaders/v_shader.glsl", "../Shaders/f_shader.glsl");
	glUseProgram(s_program_id);

	g_ro = glm::vec3(0, 2.0f, -3.0f);
	g_cameraRadian = 45.f;
	g_cameralook = glm::vec3(0, 0, +3.0f);

	glViewport(0, 0, (GLsizei)g_window_w, (GLsizei)g_window_h);
	glEnable(GL_DEPTH_TEST); // depth������, �̰Ť����ϸ� �յڰ��� ���� ���������� �׷����ִ����� �𸥴�.���հ����� �׷�
	glEnable(GL_CULL_FACE);


	// Initial State of Camera
	// ī�޶� �ʱ� ��ġ �����Ѵ�.
	g_camera.lookAt(glm::vec3(3.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	// basic meshes
	InitBasicShapeObjs();

	// Tree
	InitTreeModel();

	// Car
	InitCarModel();
	// �ٴ� ���� VAO ����
	InitGroundTexture();

	InitSnowManModel();

	InitBasketModel();


}







/**
ClearOpenGLResource: ���α׷��� ������ �޸� ������ ���� �� �� ȣ��Ǵ� �Լ�. (main �Լ� ����)
���α׷����� ����� �޸𸮸� ���⿡�� ������ �� �ִ�.
�������, VAO�� VBO�� ���⼭ ���� �� �ִ�.
*/
void ClearOpenGLResource()
{
	// Delete (VAO, VBO)
	DeleteBasicShapeObjs();
	DeleteTreeModel();
	DeleteCarModel();
	DeleteGround2();
	DeleteSnowManModel();
	DeleteBasketModel();
}





/**
Display: ������ ȭ���� ������Ʈ �� �ʿ䰡 ���� �� ȣ��Ǵ� callback �Լ�.

������ �� ���� ����� ������ �ϴ� �ڵ�� �� �Լ� ���� �����ؾ��Ѵ�.
�����찡 ó�� ���� ��, ������ ũ�Ⱑ �ٲ� ��, �ٸ� �����쿡 ���� ȭ���� �Ϻ�
�Ǵ� ��ü�� �������ٰ� �ٽ� ��Ÿ�� �� �� �ý����� �ش� ������ ���� �׸��� ����
������Ʈ�� �ʿ��ϴٰ� �Ǵ��ϴ� ��� �ڵ����� ȣ��ȴ�.
���� ȣ���� �ʿ��� ��쿡�� glutPostRedisplay() �Լ��� ȣ���ϸ�ȴ�.

�� �Լ��� �ҽÿ� ����ϰ� ȣ��ȴٴ� ���� ����ϰ�, ������ ���� ��ȭ�� ������
1ȸ�� �ڵ�� �������� �� �Լ� �ۿ� �����ؾ��Ѵ�. Ư�� �޸� �Ҵ�, VAO, VBO ����
���� �ϵ���� ������ �õ��ϴ� �ڵ�� Ư���� ������ ���ٸ� ���� �� �Լ��� ���Խ�Ű��
�ȵȴ�. ���� ���, �޽� ���� �����ϰ� VAO, VBO�� �����ϴ� �κ��� ���� 1ȸ��
�����ϸ�ǹǷ� main() �Լ� �� �ܺο� �����ؾ��Ѵ�. ���ǵ� �޽� ���� ������ ���ۿ�
�׸����� �����ϴ� �ڵ常 �� �Լ��� �����ϸ� �ȴ�.

����, �� �Լ� ������ ���� �޸� �Ҵ��� �ؾ��ϴ� ��찡 �ִٸ� �ش� �޸𸮴� �ݵ��
�� �Լ��� ������ ���� ���� �ؾ��Ѵ�.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// ��ü ȭ���� �����.
	// glClear�� Display �Լ� ���� �� �κп��� �� ���� ȣ��Ǿ���Ѵ�.
	if (night_mode) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}
	else {
		glClearColor(0.85f, 0.80f, 0.96f, 1.f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //depgth ���۸� �ִ밪���� ä���־��
	//���� ���۰� �ΰ���.
	//�̻ﰢ���� ����Ϳ������� �Ÿ��� ���̾߸� �˷���.
	//ĥ�ϱ����� �˻縦 �����غ���. ĥ���ʿ䰡�ִ�?
	//z Depth���� z������ �˻��غ���. �˻��غ��� ������ �����. �ָ��ִ� �� �ȱ׷����ȴ�.
	//�� �����Ǿ��ִ� �����Ŵ� �ȳ����Ŵϸ� �ϸ��.

	

	// Vertex shader �� matrix �������� location�� �޾ƿ´�.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");
	int m_shadow_loc = glGetUniformLocation(s_program_id, "shadow_matrix");


	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	if ( g_camera_mode == 6 )// Top view ���� �߿�!! �̹� �ڵ忡�� �̰� �ٲ���� 
	{
		// Projection Transform Matrix ����.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.
		view_matrix = glm::lookAt(glm::vec3(0.f, 13.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}
	else if (g_camera_mode == 2) {

		// Projection Transform Matrix ����.
		projection_matrix = glm::perspective(glm::radians(90.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.

		glm::vec3 ro = glm::rotateY(glm::vec3(0, 0.6f, -0.16f), g_car_rotation_y);
		glm::vec3 camera2positionx = ro + g_car_poisition;
		glm::vec3 r = glm::rotateY(glm::vec3(0, 1.5f, +6.0f), g_car_rotation_y);
		glm::vec3 camera2look = r + g_car_poisition;

		view_matrix = glm::lookAt(camera2positionx, camera2look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

		g_ro = glm::vec3(0, 0.6f, -0.16f);
		g_cameraRadian = 90.f;
		g_cameralook = glm::vec3(0, 1.5f, +6.0f);
	}
	else if (g_camera_mode == 3) {

		// Projection Transform Matrix ����.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		
		// Camera Transform Matrix ����.

		glm::vec3 ro = glm::rotateY(glm::vec3(0, 2.0f, -3.0f), g_car_rotation_y);
		glm::vec3 camera2positionx = ro + g_car_poisition;
		glm::vec3 r = glm::rotateY(glm::vec3(0, 0, +3.0f), g_car_rotation_y);
		glm::vec3 camera2look=r+ g_car_poisition;

		view_matrix = glm::lookAt(camera2positionx, camera2look, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		g_ro = glm::vec3(0, 2.0f, -3.0f);
		g_cameraRadian = 45.f;
		g_cameralook = glm::vec3(0, 0, +3.0f);
	}

	else//���콺 ������ �����̴� �� �⺻����!!
	{
		// Projection Transform Matrix ����.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix ����.
		view_matrix = g_camera.GetGLViewMatrix();
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}
	

	//lookAt perspective
	//ī�޶� �ڵ��� ��ġ ���󰡾ߵǴϱ� �ڵ�����ġ�˾Ƴ��°� �� �غ�����.
	// Light ����
	{
		// ���� ������ Shader ���� num_of_lights �� �Ѱ��ش�.
		int num_of_lights_loc = glGetUniformLocation(s_program_id, "num_of_lights");
		glUniform1i(num_of_lights_loc, 5);//�迭ũ�� 5�� �־���.
		
										  
		// Directional Light ����(��,�޺�)
		{
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);//������������ �ű�ٰ� 0�־��

			// ���� ���� ����.
			glm::vec3 light_dir(-.7f, -1.f, -.7f);
			light_dir = glm::normalize(light_dir);

			// Apply Camera Matrices.
			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  light_dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
			light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));

			int light_dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(light_dir_loc, light_dir[0], light_dir[1], light_dir[2]);

			// ���� ���� ����.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			if(night_mode)
				glUniform3f(intensity_loc, 0.1f, 0.1f, 0.1f);
			else
				glUniform3f(intensity_loc, 1.0f, 1.0f, 1.0f);

			// Shadow Projection Matrix
			//�޺��� ���缭 �׸��� �׸����
			glm::mat4 shadow_matrix = glm::mat4(1.f);
			shadow_matrix[1][0] = light_dir.x;
			shadow_matrix[1][1] = 0.f;
			shadow_matrix[3][1] = 0.001f;
			shadow_matrix[1][2] = light_dir.z;

			glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));

		}
		
		// Point Light ����(�ݵ���1)
		{
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
			glUniform1i(type_loc, 1);


			// ���� ����ϴ� ��ġ(����) ����.
			// �ð��� ���� ��ġ�� ���ϵ��� ��.
			glm::vec3 pos(0.5f * cos(g_elaped_time_s) + 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) + 1.f);

			// Apply Camera Matrices
			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[1].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);

			// ���� ���� ����.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[1].intensity");
			glUniform3f(intensity_loc, 0.f, 1.f, 0.f);
		}



		// Spot Light ����.(���� ������ ��)
		{
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[2].type");
			glUniform1i(type_loc, 2);
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));


			// ���� ����ϴ� ��ġ(����) ����.
			glm::vec3 pos= car_T*glm::vec4(0.1f, 0.3f, 0.5f,1.f);

			// Apply Camera Matrices
			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[2].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// ���� ���� ����.
			// �ð��� ���� ������ ���ϵ��� ��.
			glm::vec3 dir = glm::rotateY(glm::vec3(0.f, -.1f, 0.5f), g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[2].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);


			// ���� ���� ���� (�����).
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[2].intensity");
			glUniform3f(intensity_loc, 1.f, 1.f, 0.f);

			// ���� ���� ���� ����.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[2].cos_cutoff");
			glUniform1f(light_cos_cutoff_loc, cos(25.f / 180.f * glm::pi<float>()));//rad���� �����ֱ�
																					//.cos_cutoff�� ����
		}
		
		// Spot Light ����. (������ �������)
		{
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[3].type");
			glUniform1i(type_loc, 2);
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));


			// ���� ����ϴ� ��ġ(����) ����.
			glm::vec3 pos = car_T*glm::vec4(-0.1f, 0.3f, 0.5f,1.f);

			// Apply Camera Matrices
			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[3].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// ���� ���� ����.
			// �ð��� ���� ������ ���ϵ��� ��.
			glm::vec3 dir = glm::rotateY(glm::vec3(0.f, -.1f, 0.5f), g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  dir�� ������ ��Ÿ���� �����̹Ƿ� �̵�(Translation)��ȯ�� ���õǵ��� �Ѵ�. (�� ��° ��� 0.f���� ����)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[3].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);


			// ���� ���� ���� (�����).
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[3].intensity");
			glUniform3f(intensity_loc, 1.0f, 1.f, 0.f);

			// ���� ���� ���� ����.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[3].cos_cutoff");
			glUniform1f(light_cos_cutoff_loc, cos(25.f / 180.f * glm::pi<float>()));//rad���� �����ֱ�
																					//.cos_cutoff�� ����
		}

		// Point Light ���� (�ݵ���2�� ��)
		{
			// ���� ���� ���� (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl ����.
			int type_loc = glGetUniformLocation(s_program_id, "lights[4].type");
			glUniform1i(type_loc, 1);


			// ���� ����ϴ� ��ġ(����) ����.
			// �ð��� ���� ��ġ�� ���ϵ��� ��.
			glm::vec3 pos(0.5f * cos(g_elaped_time_s) - 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) - 1.f);

			//�ݵ��� �迭? �ݵ��Ұ��� ����
			//���������ϴٰ� �Ȱ����� ����� �����?

			// Apply Camera Matrices
			////// *** ���� ī�޶� ������ ����ϱ� ���� view transform ����  ***
			//  �̶� pos�� ��ġ�� ��Ÿ���� ����Ʈ�̹Ƿ� �̵�(Translation)��ȯ�� ����ǵ��� �Ѵ�. (�� ��° ��� 1.f���� ����)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[4].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// ���� ���� ����.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[4].intensity");
			glUniform3f(intensity_loc, 0.f, 1.f, 0.f);
		}


		
	}//�� �迭 ��.

	
//�׸��� ����
	// �ٴ� ���� Ground
	{
		glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), true);

		// Ground�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.5f, 0.5f, 0.5f);

		glm::mat4 T0(1.f); // ���� ���
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T0));
		DrawGroundTexture();

	}



	// Moving Car
	{
		glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), false);


		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);//n���� ũ���ϸ� ���� ���������� �����Եȴ�.��(0,100)���̰��� �־���Ѵ�.
		//�̹� ��¦�ŷ��� ����,,

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");//���� �󸶳� ����������.
		glUniform3f(K_s_loc, 0.6f, 0.6f, 0.6f);

		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE,  glm::value_ptr(car_T));


		DrawCarModel();
		if (g_camera_mode != 2) {
			DrawSnowManModel();
		}

	}

	// Ʈ����
	{


		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);//n���� ũ���ϸ� ���� ���������� �����Եȴ�.��(0,100)���̰��� �־���Ѵ�.
										 //�̹� ��¦�ŷ��� ����,,

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");//���� �󸶳� ����������.
		glUniform3f(K_s_loc, 0.6f, 0.6f, 0.6f);

		glm::mat4 car_T = glm::translate(glm::vec3(2.f,0.f,2.f))* glm::scale(glm::vec3(3.f, 3.0f, 3.0f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));


		DrawBasketModel();

	}


	// ����
	{
		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 20.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.5f, 0.5f, 0.5f);

		for (int i = 0; i <= 5; i++)
		{
			for (int j = 0; j <= 5; j++)
			{
				glm::mat4 tree_T;
				tree_T = glm::translate(glm::vec3(i*2.f - 5.f, 0.f, j*2.f - 5.f));
				glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(tree_T));

				DrawTreeModel((i + j) % NUM_TREES);

			}
		}
	}

	// ������
	if (night_mode){
		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::mat4 car_M = glm::translate(glm::vec3(0.1f, 0.3f, 0.5f))* glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))*car_M;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

		// ��ü �������� ����� Color ���� �����Ѵ�. 
		glVertexAttrib4f(2, 1.0f, 1.0f, 0.f, 1.f);

		// ���� �׸���.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);

	}
	// ������2
	if (night_mode)	{
		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);
		glm::mat4 car_M = glm::translate(glm::vec3(-0.1f, 0.3f, 0.5f))* glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));

		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))*car_M;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

		// ��ü �������� ����� Color ���� �����Ѵ�. 
		glVertexAttrib4f(2, 1.0f, 1.0f, 0.f, 1.f);

		// ���� �׸���.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);

	}
	//�ݵ���1 �׸���
	if (night_mode)
	{
		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::vec3 star_loc = glm::vec3(0.5f * cos(g_elaped_time_s) - 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) - 1.f);
		// ��ȯ ����� �����Ѵ�.
		glm::mat4 model_T;
		model_T = glm::translate(star_loc) * glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));

		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// ��ü �������� ����� Color ���� �����Ѵ�. 
		glVertexAttrib4f(2, 0.3f, 0.8f, 0.4f, 1.f);

		// ���� �׸���.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);
	}

	//�ݵ���2 �׸���
	if (night_mode)
	{
		// Sphere�� ���� Phong Shading ���� ���� ���� �����Ѵ�.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::vec3 star_loc = glm::vec3(0.5f * cos(g_elaped_time_s) + 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) + 1.f);
		// ��ȯ ����� �����Ѵ�.
		glm::mat4 model_T;
		model_T = glm::translate(star_loc) * glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));

		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// ��ü �������� ����� Color ���� �����Ѵ�. 
		glVertexAttrib4f(2, 0.3f, 0.8f, 0.4f, 1.f);

		// ���� �׸���.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);
	}


	/////////////////////////////// �Ʒ� ���ʹ� �׸��ڸ� �׸���. ////////////////////////////////////////////////////
	if (!night_mode) {
		glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), true);
		// Moving Car
		{
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

			DrawCarModel();
		}

		// ����
		{

			for (int i = 0; i <= 5; i++)
			{
				for (int j = 0; j <= 5; j++)
				{
					glm::mat4 tree_T;
					tree_T = glm::translate(glm::vec3(i*2.f - 5.f, 0.f, j*2.f - 5.f));
					glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(tree_T));

					DrawTreeModel((i + j) % NUM_TREES);
				}
			}
		}
		glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), false);

	}
	// flipping the double buffers
	// glutSwapBuffers�� �׻� Display �Լ� ���� �Ʒ� �κп��� �� ���� ȣ��Ǿ���Ѵ�.
	glutSwapBuffers();
}


/**
Timer: ������ �ð� �Ŀ� �ڵ����� ȣ��Ǵ� callback �Լ�.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{

	//Ÿ�̸Ӿȿ��� �Ͼ�� �� : �ڵ��� ��ġ ����.
	// Timer ȣ�� �ð� ������ �����Ͽ�, ���� Timer�� ȣ��� �ĺ��� ������� �귯�� ����Ѵ�.
	g_elaped_time_s += value/1000.f;


	// Turn
	g_car_rotation_y += g_car_angular_speed;

	//�ؿ� ���� �ڵ� �߾˾ƾ���
	// Calculate Velocity
	glm::vec3 speed_v = glm::vec3(0.f, 0.f, g_car_speed);
	glm::vec3 velocity = glm::rotateY(speed_v, g_car_rotation_y);	
	//rotate( , )���� �Ǵٸ���.
	// rotateY( , )�� speed_v �� y���� �������� g_car_rotation_y ��ŭ ȸ���Ѵ�.

	// Move
	g_car_poisition += velocity;
	//�ڵ��� ��ġ����� ��� �ϰ� �ִ��� �˾ƾ���. -> �׷��� ī�޶�����.


	//ī�޶��� ��ġ�� ������ ���ذ��°ɷ� �Ѵ�.
	glm::vec3 c_velocity;
	float rad_velocity;
	glm::vec3 c_look_velocity;
	//glm::vec3 speed_c = glm::vec3(0.f, 0.f, g_car_speed);
	//glm::vec3 c_velocity = glm::rotateY(speed_c, g_car_rotation_y);
	//rotate( , )���� �Ǵٸ���.
	// rotateY( , )�� speed_v �� y���� �������� g_car_rotation_y ��ŭ ȸ���Ѵ�.
	if (g_camera_mode == 5) {
		c_velocity = (glm::vec3(0, 2.0f, -3.0f) - g_ro) / 100.f;
		g_ro += c_velocity;
		rad_velocity = (45.f - g_cameraRadian) / 100.f;
		g_cameraRadian += rad_velocity;
		c_look_velocity= (glm::vec3(0, 0, +3.0f) - g_cameralook) / 100.f;
		g_cameralook += c_look_velocity;


	}
	else if (g_camera_mode == 4) {
		c_velocity = (glm::vec3(0, 0.6f, -0.16f) - g_ro) / 100.f;
		g_ro += c_velocity;
		rad_velocity = (90.f - g_cameraRadian) / 100.f;
		g_cameraRadian += rad_velocity;

		c_look_velocity = (glm::vec3(0, 1.5f, +6.0f) - g_cameralook) / 100.f;
		g_cameralook += c_look_velocity;
	}

	// Move
	//g_car_poisition += c_velocity;
	
	// glutPostRedisplay�� ������ ���� �ð� �ȿ� ��ü �׸��� �ٽ� �׸� ���� �ý��ۿ� ��û�Ѵ�.
	// ��������� Display() �Լ��� ȣ�� �ȴ�.
	glutPostRedisplay();

	// 1/60 �� �Ŀ� Timer �Լ��� �ٽ� ȣ��Ƿη� �Ѵ�.
	// Timer �Լ� �� ������ �ð� �������� �ݺ� ȣ��ǰ��Ͽ�,
	// �ִϸ��̼� ȿ���� ǥ���� �� �ִ�
	glutTimerFunc((unsigned int)(1000 / 60), Timer, (1000 / 60));
}




/**
Reshape: �������� ũ�Ⱑ ������ ������ �ڵ����� ȣ��Ǵ� callback �Լ�.

@param w, h�� ���� ������ �������� ���� ũ��� ���� ũ�� (�ȼ� ����).
ref: https://www.opengl.org/resources/libraries/glut/spec3/node48.html#SECTION00083000000000000000
*/
void Reshape(int w, int h)
{
	//  w : window width   h : window height
	g_window_w = w;
	g_window_h = h;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glutPostRedisplay();
}

/**
Keyboard: Ű���� �Է��� ���� ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� ������ Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = -0.01f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.01f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = glm::radians( 1.f );		// ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = -1 * glm::radians( 1.f );		//  ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;
	case 'n':
	{
		int night_mode_loc = glGetUniformLocation(s_program_id, "night_mode");
		if (night_mode) {
			night_mode = false;
			// 'shading_mode' ������ false�� ����.
			glUniform1i(night_mode_loc, false);
		}
		else {
			night_mode = true;
			// 'shading_mode' ������ false�� ����.
			glUniform1i(night_mode_loc, true);

		}
		// glutPostRedisplay�� ������ ���� �ð� �ȿ� ��ü �׸��� �ٽ� �׸� ���� �ý��ۿ� ��û�Ѵ�.
		// ��������� Display() �Լ��� ȣ���ϰ� �ȴ�.
		//glutPostRedisplay();
	}
		break;

	case '1':
		g_camera_mode = 1;
		glutPostRedisplay();
		break;

	case '2':
		g_camera_mode = 2;
		glutPostRedisplay();
		break;

	case '3':
		g_camera_mode = 3;
		glutPostRedisplay();
		break;


	case '6':
		g_camera_mode = 6;
		glutPostRedisplay();
		break;
	}

}

/**
KeyboardUp: �������� Ű�� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@param key�� �ش� Ű������ ���ڰ�.
@param x,y�� ���� ���콺 �������� ��ǥ��.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = 0.f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.f;		// ���� �ӵ� ����
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = 0.f;		// ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = 0.f;		//  ��ȸ�� ���ӵ� ����
		glutPostRedisplay();
		break;

	}

}



/**
Mouse: ���콺 ��ư�� �Էµ� ������ �ڵ����� ȣ��Ǵ� �Լ�.
�Ķ������ �ǹ̴� ������ ����.
@param button: ���� ��ư�� ����
  GLUT_LEFT_BUTTON - ���� ��ư
  GLUT_RIGHT_BUTTON - ������ ��ư
  GLUT_MIDDLE_BUTTON - ��� ��ư (���� �������� ��)
  3 - ���콺 �� (���� ���� ���� ����).
  4 - ���콺 �� (���� �Ʒ��� ���� ����).
@param state: ���� ����
  GLUT_DOWN - ���� ����
  GLUT_UP - ��������
@param x,y: ������ �Ͼ�� ��, ���콺 �������� ��ǥ��.
*/
void Mouse(int button, int state, int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_left_button_pushed = true;

	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_left_button_pushed = false;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		g_right_button_pushed = true;

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		g_right_button_pushed = false;
	else if (button == 3)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, -1, 0.01f);
		glutPostRedisplay();
	}
	else if (button == 4)
	{
		g_camera.inputMouse(Camera::IN_TRANS_Z, 0, 1, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}





/**
MouseMotion: ���콺 �����Ͱ� ������ ������ �ڵ����� ȣ��Ǵ� �Լ�.
@prarm x,y�� ���� ���콺 �������� ��ǥ���� ��Ÿ����.
*/
void MouseMotion(int x, int y)
{
	float mouse_xd = (float)x / g_window_w;
	float mouse_yd = 1 - (float)y / g_window_h;
	float last_mouse_xd = (float)g_last_mouse_x / g_window_w;
	float last_mouse_yd = 1 - (float)g_last_mouse_y / g_window_h;

	if (g_left_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_ROTATION_Y_UP, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd);
		glutPostRedisplay();
	}
	else if (g_right_button_pushed)
	{
		g_camera.inputMouse(Camera::IN_TRANS, last_mouse_xd, last_mouse_yd, mouse_xd, mouse_yd, 0.01f);
		glutPostRedisplay();
	}

	g_last_mouse_x = x;
	g_last_mouse_y = y;
}