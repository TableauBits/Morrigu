#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texIndex;
layout(location = 4) in float a_tilingFactor;

layout(push_constant) uniform PushConstants { mat4 viewProjection; }
u_constants;

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec2 v_texCoord;
layout(location = 2) out flat float v_texIndex;
layout(location = 3) out float v_tilingFactor;

void main()
{
	v_color = a_color;
	v_texCoord = a_texCoord;
	v_texIndex = a_texIndex;
	v_tilingFactor = a_tilingFactor;
	gl_Position = u_constants.viewProjection * vec4(a_position, 1.0);
}
