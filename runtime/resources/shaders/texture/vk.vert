#version 450

layout(push_constant) uniform PushConstants
{
	mat4 transform;
	vec4 color;
	float tiling;
}
u_constants;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 v_fragTexCoord;
layout(location = 1) out float v_tiling;
layout(location = 2) out vec4 v_color;

void main()
{
	gl_Position = u_constants.transform * vec4(inPosition, 0.0, 1.0);
	v_fragTexCoord = inTexCoord;
	v_tiling = u_constants.tiling;
	v_color = u_constants.color;
}
