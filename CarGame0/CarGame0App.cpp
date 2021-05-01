

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
//// 자동차 제어 변수들.
//////////////////////////////////////////////////////////////////////
glm::vec3 g_car_poisition(0.f, 0.f, 0.f); //위치
float g_car_speed = 0;			          // 속도 (초당 이동 거리)
float g_car_rotation_y = 0;		          // 현재 방향 (y축 회전)
float g_car_angular_speed = 0;	          // 회전 속도 (각속도 - 초당 회전 각)

float g_cameraRadian;
glm::vec3 g_ro;
glm::vec3 g_cameralook;

bool night_mode;




/**
InitOpenGL: 프로그램 초기 값 설정을 위해 최초 한 번 호출되는 함수. (main 함수 참고)
OpenGL에 관련한 초기 값과 프로그램에 필요한 다른 초기 값을 설정한다.
예를들어, VAO와 VBO를 여기서 생성할 수 있다.
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
	glEnable(GL_DEPTH_TEST); // depth사용헌더, 이거ㅎ안하면 앞뒤간격 몰라서 무슨순서로 그려져있느지를 모른대.막뚫고나오고 그럼
	glEnable(GL_CULL_FACE);


	// Initial State of Camera
	// 카메라 초기 위치 설정한다.
	g_camera.lookAt(glm::vec3(3.f, 2.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

	// basic meshes
	InitBasicShapeObjs();

	// Tree
	InitTreeModel();

	// Car
	InitCarModel();
	// 바닥 격자 VAO 생성
	InitGroundTexture();

	InitSnowManModel();

	InitBasketModel();


}







/**
ClearOpenGLResource: 프로그램이 끝나기 메모리 해제를 위해 한 번 호출되는 함수. (main 함수 참고)
프로그램에서 사용한 메모리를 여기에서 해제할 수 있다.
예를들어, VAO와 VBO를 여기서 지울 수 있다.
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
Display: 윈도우 화면이 업데이트 될 필요가 있을 때 호출되는 callback 함수.

윈도우 상에 최종 결과를 렌더링 하는 코드는 이 함수 내에 구현해야한다.
원도우가 처음 열릴 때, 윈도우 크기가 바뀔 때, 다른 윈도우에 의해 화면의 일부
또는 전체가 가려졌다가 다시 나타날 때 등 시스템이 해당 윈도우 내의 그림에 대한
업데이트가 필요하다고 판단하는 경우 자동으로 호출된다.
강제 호출이 필요한 경우에는 glutPostRedisplay() 함수를 호출하면된다.

이 함수는 불시에 빈번하게 호출된다는 것을 명심하고, 윈도우 상태 변화와 무관한
1회성 코드는 가능한한 이 함수 밖에 구현해야한다. 특히 메모리 할당, VAO, VBO 생성
등의 하드웨어 점유를 시도하는 코드는 특별한 이유가 없다면 절대 이 함수에 포함시키면
안된다. 예를 들어, 메시 모델을 정의하고 VAO, VBO를 설정하는 부분은 최초 1회만
실행하면되므로 main() 함수 등 외부에 구현해야한다. 정의된 메시 모델을 프레임 버퍼에
그리도록 지시하는 코드만 이 함수에 구현하면 된다.

만일, 이 함수 내에서 동적 메모리 할당을 해야하는 경우가 있다면 해당 메모리는 반드시
이 함수가 끝나기 전에 해제 해야한다.

ref: https://www.opengl.org/resources/libraries/glut/spec3/node46.html#SECTION00081000000000000000
*/
void Display()
{
	// 전체 화면을 지운다.
	// glClear는 Display 함수 가장 윗 부분에서 한 번만 호출되어야한다.
	if (night_mode) {
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	}
	else {
		glClearColor(0.85f, 0.80f, 0.96f, 1.f);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //depgth 버퍼를 최대값으로 채워넣어라
	//지금 버퍼가 두개야.
	//이삼각형이 모니터에서부터 거리가 몇이야를 알랴줌.
	//칠하기전에 검사를 먼저해본다. 칠할필요가있니?
	//z Depth에서 z값끼리 검사해본다. 검사해보고 가까우면 덮어쓰고. 멀리있는 건 안그려도된다.
	//다 구현되어있다 넣을거니 안넣을거니만 하면됨.

	

	// Vertex shader 의 matrix 변수들의 location을 받아온다.
	int m_proj_loc = glGetUniformLocation(s_program_id, "proj_matrix");
	int m_view_loc = glGetUniformLocation(s_program_id, "view_matrix");
	int m_model_loc = glGetUniformLocation(s_program_id, "model_matrix");
	int m_shadow_loc = glGetUniformLocation(s_program_id, "shadow_matrix");


	glm::mat4 projection_matrix;
	glm::mat4 view_matrix;

	if ( g_camera_mode == 6 )// Top view 여기 중요!! 이번 코드에서 이거 바꿔야함 
	{
		// Projection Transform Matrix 설정.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix 설정.
		view_matrix = glm::lookAt(glm::vec3(0.f, 13.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}
	else if (g_camera_mode == 2) {

		// Projection Transform Matrix 설정.
		projection_matrix = glm::perspective(glm::radians(90.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix 설정.

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

		// Projection Transform Matrix 설정.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		
		// Camera Transform Matrix 설정.

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

	else//마우스 누르면 움직이는 그 기본상태!!
	{
		// Projection Transform Matrix 설정.
		projection_matrix = glm::perspective(glm::radians(45.f), (float)g_window_w / g_window_h, 0.01f, 10000.f);
		glUniformMatrix4fv(m_proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		// Camera Transform Matrix 설정.
		view_matrix = g_camera.GetGLViewMatrix();
		glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	}
	

	//lookAt perspective
	//카메라가 자동차 위치 따라가야되니까 자동차위치알아내는거 잘 해봐야지.
	// Light 설정
	{
		// 빛의 개수를 Shader 변수 num_of_lights 로 넘겨준다.
		int num_of_lights_loc = glGetUniformLocation(s_program_id, "num_of_lights");
		glUniform1i(num_of_lights_loc, 5);//배열크기 5을 넣어줌.
		
										  
		// Directional Light 설정(해,달빛)
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
			glUniform1i(type_loc, 0);//변수가져오고 거기다가 0넣어라

			// 빛의 방향 설정.
			glm::vec3 light_dir(-.7f, -1.f, -.7f);
			light_dir = glm::normalize(light_dir);

			// Apply Camera Matrices.
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));

			int light_dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
			glUniform3f(light_dir_loc, light_dir[0], light_dir[1], light_dir[2]);

			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
			if(night_mode)
				glUniform3f(intensity_loc, 0.1f, 0.1f, 0.1f);
			else
				glUniform3f(intensity_loc, 1.0f, 1.0f, 1.0f);

			// Shadow Projection Matrix
			//햇빛에 맞춰서 그림자 그릴라고
			glm::mat4 shadow_matrix = glm::mat4(1.f);
			shadow_matrix[1][0] = light_dir.x;
			shadow_matrix[1][1] = 0.f;
			shadow_matrix[3][1] = 0.001f;
			shadow_matrix[1][2] = light_dir.z;

			glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));

		}
		
		// Point Light 설정(반딧불1)
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[1].type");
			glUniform1i(type_loc, 1);


			// 빛이 출발하는 위치(광원) 설정.
			// 시간에 따라 위치가 변하도록 함.
			glm::vec3 pos(0.5f * cos(g_elaped_time_s) + 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) + 1.f);

			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[1].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);

			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[1].intensity");
			glUniform3f(intensity_loc, 0.f, 1.f, 0.f);
		}



		// Spot Light 설정.(왼쪽 전조등 빛)
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[2].type");
			glUniform1i(type_loc, 2);
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));


			// 빛이 출발하는 위치(광원) 설정.
			glm::vec3 pos= car_T*glm::vec4(0.1f, 0.3f, 0.5f,1.f);

			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[2].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// 빛의 방향 설정.
			// 시간에 따라 방향이 변하도록 함.
			glm::vec3 dir = glm::rotateY(glm::vec3(0.f, -.1f, 0.5f), g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[2].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);


			// 빛의 세기 설정 (노랑색).
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[2].intensity");
			glUniform3f(intensity_loc, 1.f, 1.f, 0.f);

			// 빛의 퍼짐 정도 설정.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[2].cos_cutoff");
			glUniform1f(light_cos_cutoff_loc, cos(25.f / 180.f * glm::pi<float>()));//rad으로 고쳐주기
																					//.cos_cutoff에 설정
		}
		
		// Spot Light 설정. (오른쪽 전조등빛)
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[3].type");
			glUniform1i(type_loc, 2);
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));


			// 빛이 출발하는 위치(광원) 설정.
			glm::vec3 pos = car_T*glm::vec4(-0.1f, 0.3f, 0.5f,1.f);

			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[3].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// 빛의 방향 설정.
			// 시간에 따라 방향이 변하도록 함.
			glm::vec3 dir = glm::rotateY(glm::vec3(0.f, -.1f, 0.5f), g_car_rotation_y);
			dir = glm::normalize(dir);

			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다. (네 번째 요소 0.f으로 셋팅)
			dir = glm::vec3(view_matrix * glm::vec4(dir, 0.f));

			int dir_loc = glGetUniformLocation(s_program_id, "lights[3].dir");
			glUniform3f(dir_loc, dir[0], dir[1], dir[2]);


			// 빛의 세기 설정 (노랑색).
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[3].intensity");
			glUniform3f(intensity_loc, 1.0f, 1.f, 0.f);

			// 빛의 퍼짐 정도 설정.
			int light_cos_cutoff_loc = glGetUniformLocation(s_program_id, "lights[3].cos_cutoff");
			glUniform1f(light_cos_cutoff_loc, cos(25.f / 180.f * glm::pi<float>()));//rad으로 고쳐주기
																					//.cos_cutoff에 설정
		}

		// Point Light 설정 (반딧불2의 빛)
		{
			// 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light), fshader_MultiLights.glsl 참고.
			int type_loc = glGetUniformLocation(s_program_id, "lights[4].type");
			glUniform1i(type_loc, 1);


			// 빛이 출발하는 위치(광원) 설정.
			// 시간에 따라 위치가 변하도록 함.
			glm::vec3 pos(0.5f * cos(g_elaped_time_s) - 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) - 1.f);

			//반딧불 배열? 반딧불갯수 변수
			//랜덤으로하다가 똑같은곳 생기면 어떻게함?

			// Apply Camera Matrices
			////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
			//  이때 pos는 위치를 나타내는 포인트이므로 이동(Translation)변환이 적용되도록 한다. (네 번째 요소 1.f으로 셋팅)
			pos = glm::vec3(view_matrix * glm::vec4(pos, 1.f));

			int pos_loc = glGetUniformLocation(s_program_id, "lights[4].position");
			glUniform3f(pos_loc, pos[0], pos[1], pos[2]);


			// 빛의 세기 설정.
			int intensity_loc = glGetUniformLocation(s_program_id, "lights[4].intensity");
			glUniform3f(intensity_loc, 0.f, 1.f, 0.f);
		}


		
	}//빛 배열 끝.

	
//그리기 시작
	// 바닥 격자 Ground
	{
		glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), true);

		// Ground를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");
		glUniform3f(K_s_loc, 0.5f, 0.5f, 0.5f);

		glm::mat4 T0(1.f); // 단위 행렬
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(T0));
		DrawGroundTexture();

	}



	// Moving Car
	{
		glUniform1i(glGetUniformLocation(s_program_id, "flag_texture"), false);


		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);//n값을 크게하면 작은 면적에서만 빛나게된다.약(0,100)사이값을 주어야한다.
		//이미 반짝거려서 낮춤,,

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");//빛이 얼마나 더빛나는지.
		glUniform3f(K_s_loc, 0.6f, 0.6f, 0.6f);

		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE,  glm::value_ptr(car_T));


		DrawCarModel();
		if (g_camera_mode != 2) {
			DrawSnowManModel();
		}

	}

	// 트레이
	{


		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shininess_loc = glGetUniformLocation(s_program_id, "shininess_n");
		glUniform1f(shininess_loc, 10.f);//n값을 크게하면 작은 면적에서만 빛나게된다.약(0,100)사이값을 주어야한다.
										 //이미 반짝거려서 낮춤,,

		int K_s_loc = glGetUniformLocation(s_program_id, "K_s");//빛이 얼마나 더빛나는지.
		glUniform3f(K_s_loc, 0.6f, 0.6f, 0.6f);

		glm::mat4 car_T = glm::translate(glm::vec3(2.f,0.f,2.f))* glm::scale(glm::vec3(3.f, 3.0f, 3.0f));
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));


		DrawBasketModel();

	}


	// 나무
	{
		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
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

	// 전조등
	if (night_mode){
		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::mat4 car_M = glm::translate(glm::vec3(0.1f, 0.3f, 0.5f))* glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));
		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))*car_M;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

		// 전체 꼭지점에 적용될 Color 값을 설정한다. 
		glVertexAttrib4f(2, 1.0f, 1.0f, 0.f, 1.f);

		// 구를 그린다.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);

	}
	// 전조등2
	if (night_mode)	{
		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);
		glm::mat4 car_M = glm::translate(glm::vec3(-0.1f, 0.3f, 0.5f))* glm::scale(glm::vec3(0.03f, 0.03f, 0.03f));

		glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f))*car_M;
		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

		// 전체 꼭지점에 적용될 Color 값을 설정한다. 
		glVertexAttrib4f(2, 1.0f, 1.0f, 0.f, 1.f);

		// 구를 그린다.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);

	}
	//반딧불1 그리기
	if (night_mode)
	{
		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::vec3 star_loc = glm::vec3(0.5f * cos(g_elaped_time_s) - 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) - 1.f);
		// 변환 행렬을 설정한다.
		glm::mat4 model_T;
		model_T = glm::translate(star_loc) * glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));

		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// 전체 꼭지점에 적용될 Color 값을 설정한다. 
		glVertexAttrib4f(2, 0.3f, 0.8f, 0.4f, 1.f);

		// 구를 그린다.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);
	}

	//반딧불2 그리기
	if (night_mode)
	{
		// Sphere를 위한 Phong Shading 관련 변수 값을 설정한다.
		int shading_mode_loc = glGetUniformLocation(s_program_id, "shading_mode");
		glUniform1i(shading_mode_loc, 1);

		glm::vec3 star_loc = glm::vec3(0.5f * cos(g_elaped_time_s) + 1.f, 0.2*sin(g_elaped_time_s) + .4f, 0.5f* sin(g_elaped_time_s) + 1.f);
		// 변환 행렬을 설정한다.
		glm::mat4 model_T;
		model_T = glm::translate(star_loc) * glm::scale(glm::vec3(0.05f, 0.05f, 0.05f));

		glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model_T));

		// 전체 꼭지점에 적용될 Color 값을 설정한다. 
		glVertexAttrib4f(2, 0.3f, 0.8f, 0.4f, 1.f);

		// 구를 그린다.
		DrawSphere();
		glUniform1i(shading_mode_loc, 0);
	}


	/////////////////////////////// 아래 부터는 그림자를 그린다. ////////////////////////////////////////////////////
	if (!night_mode) {
		glUniform1i(glGetUniformLocation(s_program_id, "p_shadow_mode"), true);
		// Moving Car
		{
			glm::mat4 car_T = glm::translate(g_car_poisition) * glm::rotate(g_car_rotation_y, glm::vec3(0.f, 1.f, 0.f));
			glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(car_T));

			DrawCarModel();
		}

		// 나무
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
	// glutSwapBuffers는 항상 Display 함수 가장 아래 부분에서 한 번만 호출되어야한다.
	glutSwapBuffers();
}


/**
Timer: 지정된 시간 후에 자동으로 호출되는 callback 함수.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node64.html#SECTION000819000000000000000
*/
void Timer(int value)
{

	//타이머안에서 일어나는 일 : 자동차 위치 업뎃.
	// Timer 호출 시간 간격을 누적하여, 최초 Timer가 호출된 후부터 현재까지 흘러간 계산한다.
	g_elaped_time_s += value/1000.f;


	// Turn
	g_car_rotation_y += g_car_angular_speed;

	//밑에 세개 코드 잘알아야함
	// Calculate Velocity
	glm::vec3 speed_v = glm::vec3(0.f, 0.f, g_car_speed);
	glm::vec3 velocity = glm::rotateY(speed_v, g_car_rotation_y);	
	//rotate( , )랑은 또다른것.
	// rotateY( , )는 speed_v 를 y축을 기준으로 g_car_rotation_y 만큼 회전한다.

	// Move
	g_car_poisition += velocity;
	//자동차 위치계산을 어떻게 하고 있는지 알아야함. -> 그래야 카메라하지.


	//카메라의 위치는 서서히 더해가는걸로 한다.
	glm::vec3 c_velocity;
	float rad_velocity;
	glm::vec3 c_look_velocity;
	//glm::vec3 speed_c = glm::vec3(0.f, 0.f, g_car_speed);
	//glm::vec3 c_velocity = glm::rotateY(speed_c, g_car_rotation_y);
	//rotate( , )랑은 또다른것.
	// rotateY( , )는 speed_v 를 y축을 기준으로 g_car_rotation_y 만큼 회전한다.
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
	
	// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
	// 결과적으로 Display() 함수가 호출 된다.
	glutPostRedisplay();

	// 1/60 초 후에 Timer 함수가 다시 호출되로록 한다.
	// Timer 함수 가 동일한 시간 간격으로 반복 호출되게하여,
	// 애니메이션 효과를 표현할 수 있다
	glutTimerFunc((unsigned int)(1000 / 60), Timer, (1000 / 60));
}




/**
Reshape: 윈도우의 크기가 조정될 때마다 자동으로 호출되는 callback 함수.

@param w, h는 각각 조정된 윈도우의 가로 크기와 세로 크기 (픽셀 단위).
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
Keyboard: 키보드 입력이 있을 때마다 자동으로 호출되는 함수.
@param key는 눌려진 키보드의 문자값.
@param x,y는 현재 마우스 포인터의 좌표값.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = -0.01f;		// 후진 속도 설정
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.01f;		// 전진 속도 설정
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = glm::radians( 1.f );		// 좌회전 각속도 설정
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = -1 * glm::radians( 1.f );		//  우회전 각속도 설정
		glutPostRedisplay();
		break;
	case 'n':
	{
		int night_mode_loc = glGetUniformLocation(s_program_id, "night_mode");
		if (night_mode) {
			night_mode = false;
			// 'shading_mode' 값으로 false을 설정.
			glUniform1i(night_mode_loc, false);
		}
		else {
			night_mode = true;
			// 'shading_mode' 값으로 false을 설정.
			glUniform1i(night_mode_loc, true);

		}
		// glutPostRedisplay는 가능한 빠른 시간 안에 전체 그림을 다시 그릴 것을 시스템에 요청한다.
		// 결과적으로 Display() 함수를 호출하게 된다.
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
KeyboardUp: 눌려졌던 키가 놓여질 때마다 자동으로 호출되는 함수.
@param key는 해당 키보드의 문자값.
@param x,y는 현재 마우스 포인터의 좌표값.
ref: https://www.opengl.org/resources/libraries/glut/spec3/node49.html#SECTION00084000000000000000

*/
void KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)						
	{
	case 's':
		g_car_speed = 0.f;		// 후진 속도 설정
		glutPostRedisplay();
		break;

	case 'w':
		g_car_speed = 0.f;		// 전진 속도 설정
		glutPostRedisplay();
		break;

	case 'a':
		g_car_angular_speed = 0.f;		// 좌회전 각속도 설정
		glutPostRedisplay();
		break;

	case 'd':
		g_car_angular_speed = 0.f;		//  우회전 각속도 설정
		glutPostRedisplay();
		break;

	}

}



/**
Mouse: 마우스 버튼이 입력될 때마다 자동으로 호출되는 함수.
파라메터의 의미는 다음과 같다.
@param button: 사용된 버튼의 종류
  GLUT_LEFT_BUTTON - 왼쪽 버튼
  GLUT_RIGHT_BUTTON - 오른쪽 버튼
  GLUT_MIDDLE_BUTTON - 가운데 버튼 (휠이 눌러졌을 때)
  3 - 마우스 휠 (휠이 위로 돌아 갔음).
  4 - 마우스 휠 (휠이 아래로 돌아 갔음).
@param state: 조작 상태
  GLUT_DOWN - 눌러 졌음
  GLUT_UP - 놓여졌음
@param x,y: 조작이 일어났을 때, 마우스 포인터의 좌표값.
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
MouseMotion: 마우스 포인터가 움직일 때마다 자동으로 호출되는 함수.
@prarm x,y는 현재 마우스 포인터의 좌표값을 나타낸다.
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