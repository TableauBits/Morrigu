#version 450

#include "../definitions.glsl"

layout(location = 0) in vec2 vs_UV;
layout(location = 1) in vec3 vs_Position;

layout(set = 1, binding = 0) uniform SceneLights
{
	DirectionalLight directional;
	PointLight pointLights[MC_POINT_LIGHT_COUNT];
	float pointLightCount;
}
u_SceneLights;

layout(set = 3, binding = 1) uniform sampler2D u_Albedo;
layout(set = 3, binding = 2) uniform sampler2D u_Normals;
layout(set = 3, binding = 3) uniform sampler2D u_MetalRoughness;
layout(set = 3, binding = 4) uniform sampler2D u_AmbiantOcclusion;
layout(set = 3, binding = 5) uniform sampler2D u_Emission;

layout(push_constant) uniform CameraData
{
	mat4 viewProjection;
	vec4 worldPos;
}
pc_CameraData;

layout(location = 0) out vec4 f_Color;

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.f - F0) * pow(1.f - cosTheta, 5.f); }
float distributionGGX(vec3 N, vec3 H, float roughness)
{
	float a      = roughness * roughness;
	float a2     = a * a;
	float NdotH  = max(dot(N, H), 0.f);
	float NdotH2 = NdotH * NdotH;

	float num   = a2;
	float denom = (NdotH2 * (a2 - 1.f) + 1.f);
	denom       = PI * denom * denom;

	return num / denom;
}
float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.f);
	float k = (r * r) / 8.f;

	float num   = NdotV;
	float denom = NdotV * (1.f - k) + k;

	return num / denom;
}
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.f);
	float NdotL = max(dot(N, L), 0.f);
	float ggx1  = geometrySchlickGGX(NdotL, roughness);
	float ggx2  = geometrySchlickGGX(NdotV, roughness);

	return ggx1 * ggx2;
}

void main()
{
	vec3 albedo            = texture(u_Albedo, vs_UV).rgb;
	vec3 normals           = texture(u_Normals, vs_UV).rgb;
	float metallic         = texture(u_MetalRoughness, vs_UV).b;
	float roughness        = texture(u_MetalRoughness, vs_UV).g;
	float ambiantOcclusion = texture(u_AmbiantOcclusion, vs_UV).r;
	vec3 emission          = texture(u_Emission, vs_UV).rgb;

	vec3 ambiant = vec3(0.03f) * albedo * ambiantOcclusion;
	vec3 color   = ambiant + emission;

	if (u_SceneLights.directional.direction.xyz == vec3(0.f, 0.f, 0.f)) {
		f_Color = vec4(color, 1.f);
		return;
	}

	vec3 N = normalize(normals);
	vec3 V = normalize(pc_CameraData.worldPos.xyz - vs_Position);

	vec3 L = normalize(-u_SceneLights.directional.direction.xyz);
	vec3 H = normalize(V + L);

	vec3 F0 = vec3(0.04f);
	F0      = mix(F0, albedo, metallic);
	vec3 F  = fresnelSchlick(abs(dot(H, V)), F0);

	float NDF = distributionGGX(N, H, roughness);
	float G   = geometrySmith(N, V, L, roughness);

	vec3 numerator    = NDF * G * F;
	float denominator = 4.f * max(dot(N, V), 0.f) * max(dot(N, L), 0.f) + 0.0001f;
	vec3 specular     = numerator / denominator;

	vec3 ks = F;
	vec3 kd = vec3(1.f) - ks;
	kd *= 1.f - metallic;

	float NdotL = max(dot(N, L), 0.f);
	vec3 Lo     = (kd * albedo / PI + specular) * u_SceneLights.directional.color.rgb * NdotL;
	color += Lo;

	f_Color = vec4(color, 1.f);
}
