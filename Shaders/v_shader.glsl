#version 330

layout (location = 0) in vec3 vs_position; //꼭지점 위치
layout (location = 1) in vec3 vs_normal; //꼭지점 노말벡터
layout (location = 2) in vec4 vs_color;//꼭지점 색깔
layout (location = 3) in vec2 vs_uv;//texture위해

uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
//그림자만들라고.
uniform mat4 shadow_matrix;
uniform bool p_shadow_mode = false;

out vec3 fs_normal;//변환행렬이 적용된 노말벡터
out vec4 fs_color;
out vec3 fs_eye_dir;	// modelview transform이 적용된 후 vertex 위치. (viewer's eye에서 vertex로 향하는 벡터로 볼 수 있음.) light
out vec2 fs_uv;//texture

void main()
{
	if ( p_shadow_mode )
	{
		gl_Position = proj_matrix * view_matrix * shadow_matrix * model_matrix * vec4(vs_position, 1.f);
	}
	else
	{

		gl_Position = proj_matrix * view_matrix * model_matrix * vec4(vs_position, 1.f);
		fs_normal = inverse(transpose(mat3(view_matrix * model_matrix))) * vs_normal;
		fs_eye_dir = vec3(view_matrix * model_matrix * vec4(vs_position, 1.f));	// Projection을 적용하지 않음.

		fs_color = vs_color;
		fs_uv = vs_uv;

	}
}