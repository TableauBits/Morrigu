#version 450

layout(location = 0) in vec2 vs_UV;

layout(set = 3, binding = 1) uniform sampler2D u_Texture;

layout(location = 0) out vec4 f_Color;

void main()
{
	if (vs_UV.x < 0.05 || vs_UV.x > 0.95 || vs_UV.y < 0.05 || vs_UV.y > 0.95)
		f_Color = vec4(1.f, 1.f, 0.f, 1.f);
	else
		f_Color = texture(u_Texture, vs_UV);
}
