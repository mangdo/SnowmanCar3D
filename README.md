# SnowmanCar3D
> OpenGL을 사용한 3D 프로젝트입니다.   
> 3학년 2학기, '컴퓨터 그래픽스' 수업에서 진행한 개인 프로젝트입니다.

<br>

## 1. 제작 기간 & 참여 인원
- 2019년 11월 5일 ~ 2019년 12월 5일
- 개인 프로젝트

<br>

## 2. 사용기술
 - C++
 - OpenGL 3.3   
 : (glew-2.1.0, glm-0.9.9.1, freeglut-MSVC-3.0.0-2.mp, DevIL 1.8.0 라이브러리 사용)

<br>

## 3. 설치
&nbsp; 해당 프로젝트는 Visual Studio 2015환경에서 제작되었습니다.   
실행을 위해서 [Visual Studio 2015용 visual C++ 재배포 기능 패키지]의 다운이 필요합니다.   
[Microsoft 공식페이지](https://www.microsoft.com/ko-kr/download/details.aspx?id=49984)에서 다운받으실 수 있습니다.

<br>

## 4. 실행 영상

[![https://youtu.be/MVAKFsSTSFU](https://user-images.githubusercontent.com/70243735/116778425-c6b85c80-aaac-11eb-9bd2-3b6271c91259.PNG)](https://youtu.be/MVAKFsSTSFU)

<br>

## 5. 핵심 기능

+ ### 시점 이동 :eyes:
  : Transform Matrix을 이용하여 총 4가지의 시점을 구현하였습니다.
 
  <details>
  <summary> 시점 이동 설명 펼치기 :pushpin: </summary>
  
    <img src = "https://user-images.githubusercontent.com/70243735/116774987-358cba00-aa9b-11eb-87c2-0e461c868afb.PNG" width = "600px">
  
   &nbsp; 가상의 3차원 공간을 2차원 모니터에 표현하기 위해서 가장 중요한 일은 3차원 좌표계를 2차원 좌표계로 변환해주는 **ProjectionTransform**입니다.
  ProjectionTransform은 Camera Space에서 View Frustum형태로 자르고 정육면체로 변형시킵니다.

  현 프로젝트에서는 총 4개의 시점이 존재하고 있고 **각각 다른 Transform Matrix**를 가지고 있습니다.   
  (1) 3인칭 시점 (마우스로 원하는 곳을 줌인, 줌아웃)   
  (2) 1인칭 Snowman 시점   
  (3) Bird View 시점   
  (4) Top View 시점
  
  시점의 일부인 **1인칭 Snowman 시점의 코드** 입니다.   
  관련된 전체 코드는 [CarGame0App.cpp](./CarGame0/CarGame0App.cpp)의 189 ~ 248줄에서 확인 할 수 있습니다.
  ```c++
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
  ```
  </details>

 + ### L-System :deciduous_tree:
    : 나이에 따라 성장하는 나무을 구현하였습니다.
    <details>
    <summary> L-System 설명 펼치기 :pushpin: </summary>
    <img src = "https://user-images.githubusercontent.com/70243735/116775304-56eea580-aa9d-11eb-82d5-3fe5463d0a1f.PNG" width ="800px">
    
    L-system이란, 식물의 성장 과정을 기초로 한 다양한 자연물을 표현 가능하게 하는 알고리즘입니다.   
    프로젝트의 나무들은 **모두 같은 규칙**으로 만들어졌습니다.
    
    ```c++
      std::string Rule(char in)//과제 ppt뒷부분.
      {
        std::string out;
        if (in == 'X') out = "S[&&[+X]][&[-XL]]LF[^^[-Xf]][^[+X]]FX!L";

        else if (in == 'S') out = "SS";
        else if (in == 'F') out = "FF";

        else                  out = in;

        return out;
      }
    ```
     [코드 전문 확인](./CarGame0/LSystem.cpp)
     
    **성장에 따라** 나무는 다음과 같은 특징을 가집니다.
    
    - **달라지는 잎의 색깔**   
     프로젝트의 배경은 겨울로써, 나무에 눈이 쌓인 모습을 표현하고 싶었습니다.   
    눈은 나무 위에 가장 많이 쌓이기때문에 이를 표현하기 위해서 각 단계에 맞는 색깔 변화량을 주었습니다.   
    - **얇아지는 나무의 줄기**   
     실제 나무의 줄기는 위로 갈 수록 얇아집니다.   
     이를 표현하기 위해서 두가지의 나무 줄기 종류를 두었습니다.   
    - **많아지는 열매의 수**   
     나무가 크게 성장한 4단계, 5단계 나무부터는 감귤 열매를 가지게 됩니다.  
    단계가 높아질 수록 감귤은 더 많이 생기고 너무 낮은 단계에서는 감귤이 열리지 않도록 f_num을 통해 열매의 수를 제어하였습니다.   
    - **다양한 방향의 잎사귀**    
     실제 나무는 잎사귀가 한방향에서만 자라지 않습니다.  
     잎을 한번 그릴때마다 그 각도를 계속해서 변화시켜 나무가 입체적으로 보이게 하였습니다.   

    

    </details>
    
+ ### Shading :sunny:
  : 빛의 원리를 이용한 음영 표현을 구현하였습니다.
  
  <details>
  <summary> Shading 설명 펼치기 :pushpin: </summary>
  
  <br>
  
  프로젝트에서는 2개의 모드가 있습니다. 각 모드에는 가지고 있는 빛이 다르며 이에 따라 shading방식이 달라집니다.   
  **Night 모드**에서는 햇빛이 약해지고 반딧불과 전조등 빛이 생깁니다.   
  **Day 모드**에서는 햇빛만 존재하며 햇빛에 대해서 그림자가 생깁니다.   
  
  **빛에 대해 음영표현**을 하는 식은 다음과 같습니다.
  <img src = "https://user-images.githubusercontent.com/70243735/116775737-87cfda00-aa9f-11eb-842a-0f3192002637.PNG" width ="600px">

  해당식을 코드로 구현하면 다음과 같습니다.
  
  ```c++
    vec3 K_a;
    vec3 K_d;

    K_a = ambient_factor* fs_color.rgb;
    K_d = fs_color.rgb;

    vec3 ambient = I_a * K_a;
    vec3 diffuse = vec3(0.f, 0.f, 0.f); // difuse가 내려가면 어두운색
    vec3 specular = vec3(0.f, 0.f, 0.f);
    
    for ( int i=0; i<num_of_lights; i++ )
    {
      if ( lights[i].type == 0 )
      {
        vec3 L = normalize(-lights[i].dir); // 빛의 위치
        vec3 N = normalize(fs_normal); // dot product(내적,cos구하기)하려면 길이가 1이여야 한다.
        vec3 V = normalize(-fs_eye_dir);
        vec3 H = (L+V)/length(L+V);

        diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
        specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
      }
    }
    color.rgb = ambient + diffuse + specular;
    color.rgb = min(vec3(1.0f, 1.0f, 1.0f), color.rgb);// 세개를 더한 값이 1을 넘는 것을 방지
    color.a = fs_color.a;
  ```
  [코드 전문 확인](./Shaders/f_shader.glsl)

  **빛을 설정하는 코드**는 다음과 같습니다.    
  빛의 일부인 햇빛에 대한 코드이며 반딧불이나 전조등을 위한 코드는 [CarGame0App.cpp](./CarGame0/CarGame0App.cpp)의 260 ~ 434줄에서 확인 할 수 있습니다.
  
  ```c++
  // Directional Light 설정(해,달빛)
  {
    // 빛의 종류 설정 (0: Directionl Light, 1: Point Light, 2: Spot Light)
    int type_loc = glGetUniformLocation(s_program_id, "lights[0].type");
    glUniform1i(type_loc, 0);

    // 빛의 방향 설정
    glm::vec3 light_dir(-.7f, -1.f, -.7f);
    light_dir = glm::normalize(light_dir);

    // Apply Camera Matrices
    ////// *** 현재 카메라 방향을 고려하기 위해 view transform 적용  ***
    //  light_dir는 방향을 나타내는 벡터이므로 이동(Translation)변환은 무시되도록 한다.
    light_dir = glm::vec3(view_matrix * glm::vec4(light_dir, 0.f));

    int light_dir_loc = glGetUniformLocation(s_program_id, "lights[0].dir");
    glUniform3f(light_dir_loc, light_dir[0], light_dir[1], light_dir[2]);

    // 빛의 세기 설정
    int intensity_loc = glGetUniformLocation(s_program_id, "lights[0].intensity");
    if(night_mode)
      glUniform3f(intensity_loc, 0.1f, 0.1f, 0.1f);
    else
      glUniform3f(intensity_loc, 1.0f, 1.0f, 1.0f);

    // Shadow Projection Matrix (햇빛에 대한 그림자)
    glm::mat4 shadow_matrix = glm::mat4(1.f);
    shadow_matrix[1][0] = light_dir.x;
    shadow_matrix[1][1] = 0.f;
    shadow_matrix[3][1] = 0.001f;
    shadow_matrix[1][2] = light_dir.z;

    glUniformMatrix4fv(m_shadow_loc, 1, GL_FALSE, glm::value_ptr(shadow_matrix));

  }
  ```
  
  </details>

<br>
