#version 330

in vec4 fs_color;
in vec2 fs_uv;
in vec3 fs_normal;

uniform int shading_mode = 1;

layout (location = 0) out vec4 color;

uniform sampler2D tex0;//이미지에서 색깔을 샘플링해서 가져오기위함임.
uniform bool flag_texture;

void main()
{
	vec3 N = normalize(fs_normal);
	vec3 L = vec3(1.f, 1.f, 1.f);
	L = normalize(L);

	float diffuse = clamp(dot(L, N), 0.2f, 1.f);

	if (flag_texture){
		//color.rgb =  texture(tex0, fs_uv).rgb;//텍스쳐라는 함수가 있음 그거그냥 써라. 어떤색깔을 가져오냐 uv좌표에 있는거 가져와라.
		//color.a =texture(tex0, fs_uv).a;
		color = vec4(diffuse, diffuse, diffuse, 1.f) * texture(tex0, fs_uv);
		}
	else 
		color = vec4(diffuse, diffuse, diffuse, 1.f) * fs_color;
}