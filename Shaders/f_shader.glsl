#version 330

in vec4 fs_color;//픽셀갯수만큼 처리된다생각. 
in vec3 fs_normal;
in vec3 fs_eye_dir;
in vec2 fs_uv;


layout (location = 0) out vec4 color;

struct Light
{
	// 0: Directionl Light
	// 1: Point Light
	// 2: Spot Light
	int type;


	vec3 dir;		
	vec3 position;
	vec3 intensity; // I_l 출발할때 인탠시가 중요한거
	float cos_cutoff;
};

uniform int num_of_lights;		// 사용될 빛의 개수	uniform이니까 c에서 올듯.
uniform Light lights[100];		// 최대 사용가능 빛의 개수를 100로 한정. 


uniform vec3 light_position;	// Point Light 광원 (빛이 출발하는 위치) uniform이군 c에서 왔을가능성이 높군.
float I_l; // light intensity
float I_a = 1.0; // ambient light intensity

uniform vec3 light_dir;	// Directional Light 빛의 방향. 태양빛은 한개니까 모두 똑같이
uniform bool night_mode=false;

uniform float ambient_factor = 0.1f;
uniform float shininess_n;	
uniform vec3 K_s;

uniform int shading_mode=0;
uniform bool p_shadow_mode = false;

//texture
uniform sampler2D tex0;//이미지에서 색깔을 샘플링해서 가져오기위함임.
uniform bool flag_texture=false;

void main()
{

	if(night_mode){//밤이면,
		if ( shading_mode == 1 )//명암주면안되는 전조등같은애들.
		{
			color = fs_color;
		}
		else{
			vec3 K_a;
			vec3 K_d;
			if(flag_texture){
							//K_a = ambient_factor * texture(tex0, fs_uv).rgb;

				K_a = 0.1 * texture(tex0, fs_uv).rgb;
				K_d =texture(tex0, fs_uv).rgb;
			}
			else{
				K_a = ambient_factor* fs_color.rgb;
				K_d = fs_color.rgb;
			}
		
			vec3 ambient = I_a * K_a;
			vec3 diffuse = vec3(0.f, 0.f, 0.f);
			vec3 specular = vec3(0.f, 0.f, 0.f);
						//difuse가 내려가면 어두운색
			for ( int i=0; i<num_of_lights; i++ )
			{
				//빛이 여러개니까.
				// Directional Light
				if ( lights[i].type == 0 )
				{
					vec3 L = normalize(-lights[i].dir);//빛있는곳 위치
					vec3 N = normalize(fs_normal);//dot product(내적,cos구하기)하려면 길이가 1이여야함. 그래서 바꿔줌.
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);

					diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
					specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
				}

				// Point Light경우
				else if ( lights[i].type == 1 )
				{
					vec3 L = normalize(lights[i].position-fs_eye_dir);
					vec3 N = normalize(fs_normal);
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);
					float d =  length(lights[i].position-fs_eye_dir);

					vec3 I_l = lights[i].intensity;
					if ( d > 0.1f )
						I_l = lights[i].intensity * min(0.2f/(3.0*d*d), 1.0f);

					diffuse += I_l * K_d * max(0.f, dot(L, N));
					specular += I_l * K_s * pow(max(0.f, dot(N, H)), shininess_n);
				}

				// Spot Light경우
				else if ( lights[i].type == 2 )
				{
					vec3 L = normalize(lights[i].position-fs_eye_dir);
					vec3 N = normalize(fs_normal);
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);
					vec3 Sd = normalize(-lights[i].dir);

					if ( dot(Sd,L) >= lights[i].cos_cutoff )
					{
						diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
						specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
					}
				}
			}//for문끝
		
			color.rgb = ambient + diffuse + specular;
			color.rgb = min(vec3(1.0f, 1.0f, 1.0f), color.rgb);//세개더함 그러다보니 1을 넘을 수도 있다 주의!
			color.a = fs_color.a;//알파값을 그대로 사용하시오

		
		}
	}
	else{//낮이면
		if (p_shadow_mode)
		{
			color = vec4(0.3f, 0.3f, 0.3f, 1.f);
		}

		else if(flag_texture){
			color =  texture(tex0, fs_uv);
			//color.rgb =  texture(tex0, fs_uv).rgb;
		}
		else{ //낮일때는 빛들중에서 Directional Light만 그려준다.
		
			vec3 K_a;
			vec3 K_d;

			K_a = ambient_factor* fs_color.rgb;
			K_d = fs_color.rgb;
		
			vec3 ambient = I_a * K_a;
			vec3 diffuse = vec3(0.f, 0.f, 0.f);
			vec3 specular = vec3(0.f, 0.f, 0.f);
						//difuse가 내려가면 어두운색
			for ( int i=0; i<num_of_lights; i++ )
			{
				if ( lights[i].type == 0 )
				{
					vec3 L = normalize(-lights[i].dir); //빛있는곳 위치
					vec3 N = normalize(fs_normal); //dot product(내적,cos구하기)하려면 길이가 1이여야함. 그래서 바꿔줌.
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);

					diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
					specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
				}
			}
			color.rgb = ambient + diffuse + specular;
			color.rgb = min(vec3(1.0f, 1.0f, 1.0f), color.rgb);//세개더함 그러다보니 1을 넘을 수도 있다 주의!
			color.a = fs_color.a;//알파값을 그대로 사용하시오
		}
	}
}




