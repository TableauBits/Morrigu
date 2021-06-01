#version 450

layout(location = 0) in vec2 in_TexCoord;

layout(set = 2, binding = 0) uniform sampler2D in_Texture;

layout(location = 0) out vec4 out_FragColor;

void main() {
    out_FragColor = texture(in_Texture, in_TexCoord);
}
