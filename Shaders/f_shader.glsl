#version 330

in vec4 fs_color;//�ȼ�������ŭ ó���ȴٻ���. 
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
	vec3 intensity; // I_l ����Ҷ� ���Ľð� �߿��Ѱ�
	float cos_cutoff;
};

uniform int num_of_lights;		// ���� ���� ����	uniform�̴ϱ� c���� �õ�.
uniform Light lights[100];		// �ִ� ��밡�� ���� ������ 100�� ����. 


uniform vec3 light_position;	// Point Light ���� (���� ����ϴ� ��ġ) uniform�̱� c���� �������ɼ��� ����.
float I_l; // light intensity
float I_a = 1.0; // ambient light intensity

uniform vec3 light_dir;	// Directional Light ���� ����. �¾���� �Ѱ��ϱ� ��� �Ȱ���
uniform bool night_mode=false;

uniform float ambient_factor = 0.1f;
uniform float shininess_n;	
uniform vec3 K_s;

uniform int shading_mode=0;
uniform bool p_shadow_mode = false;

//texture
uniform sampler2D tex0;//�̹������� ������ ���ø��ؼ� ��������������.
uniform bool flag_texture=false;

void main()
{

	if(night_mode){//���̸�,
		if ( shading_mode == 1 )//����ָ�ȵǴ� ��������ֵ�.
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
						//difuse�� �������� ��ο��
			for ( int i=0; i<num_of_lights; i++ )
			{
				//���� �������ϱ�.
				// Directional Light
				if ( lights[i].type == 0 )
				{
					vec3 L = normalize(-lights[i].dir);//���ִ°� ��ġ
					vec3 N = normalize(fs_normal);//dot product(����,cos���ϱ�)�Ϸ��� ���̰� 1�̿�����. �׷��� �ٲ���.
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);

					diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
					specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
				}

				// Point Light���
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

				// Spot Light���
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
			}//for����
		
			color.rgb = ambient + diffuse + specular;
			color.rgb = min(vec3(1.0f, 1.0f, 1.0f), color.rgb);//�������� �׷��ٺ��� 1�� ���� ���� �ִ� ����!
			color.a = fs_color.a;//���İ��� �״�� ����Ͻÿ�

		
		}
	}
	else{//���̸�
		if (p_shadow_mode)
		{
			color = vec4(0.3f, 0.3f, 0.3f, 1.f);
		}

		else if(flag_texture){
			color =  texture(tex0, fs_uv);
			//color.rgb =  texture(tex0, fs_uv).rgb;
		}
		else{ //���϶��� �����߿��� Directional Light�� �׷��ش�.
		
			vec3 K_a;
			vec3 K_d;

			K_a = ambient_factor* fs_color.rgb;
			K_d = fs_color.rgb;
		
			vec3 ambient = I_a * K_a;
			vec3 diffuse = vec3(0.f, 0.f, 0.f);
			vec3 specular = vec3(0.f, 0.f, 0.f);
						//difuse�� �������� ��ο��
			for ( int i=0; i<num_of_lights; i++ )
			{
				if ( lights[i].type == 0 )
				{
					vec3 L = normalize(-lights[i].dir); //���ִ°� ��ġ
					vec3 N = normalize(fs_normal); //dot product(����,cos���ϱ�)�Ϸ��� ���̰� 1�̿�����. �׷��� �ٲ���.
					vec3 V = normalize(-fs_eye_dir);
					vec3 H = (L+V)/length(L+V);

					diffuse += lights[i].intensity * K_d * max(0.f, dot(L, N));
					specular += lights[i].intensity * K_s * pow(max(0.f, dot(N, H)), shininess_n);
				}
			}
			color.rgb = ambient + diffuse + specular;
			color.rgb = min(vec3(1.0f, 1.0f, 1.0f), color.rgb);//�������� �׷��ٺ��� 1�� ���� ���� �ִ� ����!
			color.a = fs_color.a;//���İ��� �״�� ����Ͻÿ�
		}
	}
}




