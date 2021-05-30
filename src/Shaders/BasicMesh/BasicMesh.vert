#version 450

layout(location = 0) in vec3 in_VertexPosition;
layout(location = 1) in vec3 in_VertexNormal;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix;
} in_PushConstants;

layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} in_CameraData;

layout(location = 0) out vec3 out_Color;

void main() {
    mat4 transform = in_CameraData.viewProjection * in_PushConstants.modelMatrix;
    gl_Position = transform * vec4(in_VertexPosition, 1.f);
    out_Color = vec3(0.3, 0.3, 0.3);
}
