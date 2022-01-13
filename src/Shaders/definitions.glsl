#define PI  3.141592653589793f
#define TAU 6.283185307179586f

float inverseMix(float a, float b, float v) { return (v - a) / (b - a); }

struct DirectionalLight
{
	vec4 direction;
	vec4 color;
};

#define MC_POINT_LIGHT_COUNT 16
struct PointLight
{
	vec4 position;
	vec4 color;
	vec4 attenuation;
};
