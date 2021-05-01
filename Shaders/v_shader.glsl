#version 330

layout (location = 0) in vec3 vs_position; //������ ��ġ
layout (location = 1) in vec3 vs_normal; //������ �븻����
layout (location = 2) in vec4 vs_color;//������ ����
layout (location = 3) in vec2 vs_uv;//texture����

uniform mat4 proj_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
//�׸��ڸ�����.
uniform mat4 shadow_matrix;
uniform bool p_shadow_mode = false;

out vec3 fs_normal;//��ȯ����� ����� �븻����
out vec4 fs_color;
out vec3 fs_eye_dir;	// modelview transform�� ����� �� vertex ��ġ. (viewer's eye���� vertex�� ���ϴ� ���ͷ� �� �� ����.) light
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
		fs_eye_dir = vec3(view_matrix * model_matrix * vec4(vs_position, 1.f));	// Projection�� �������� ����.

		fs_color = vs_color;
		fs_uv = vs_uv;

	}
}