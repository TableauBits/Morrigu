#version 450

layout(push_constant) uniform PushConstants { mat4 viewProjection; }
u_constants;

layout(binding = 0) uniform UniformBufferObject { mat4 transform; }
u_ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = u_ubo.transform * u_constants.viewProjection * vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
