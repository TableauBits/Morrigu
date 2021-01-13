// clang-format off
#version 450

layout(binding = 1) uniform sampler2D u_textures[32];

layout(location = 0) in vec4 v_color;
layout(location = 1) in vec2 v_texCoord;
layout(location = 2) in flat float v_texIndex;
layout(location = 3) in float v_tilingFactor;
layout(location = 4) in flat uint v_objectID;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 objectIDBuffer;

void main() {
    color = v_color;
	switch (int(v_texIndex)) {
		case  0: color *= texture(u_textures[ 0], v_texCoord * v_tilingFactor); break;
		case  1: color *= texture(u_textures[ 1], v_texCoord * v_tilingFactor); break;
		case  2: color *= texture(u_textures[ 2], v_texCoord * v_tilingFactor); break;
		case  3: color *= texture(u_textures[ 3], v_texCoord * v_tilingFactor); break;
		case  4: color *= texture(u_textures[ 4], v_texCoord * v_tilingFactor); break;
		case  5: color *= texture(u_textures[ 5], v_texCoord * v_tilingFactor); break;
		case  6: color *= texture(u_textures[ 6], v_texCoord * v_tilingFactor); break;
		case  7: color *= texture(u_textures[ 7], v_texCoord * v_tilingFactor); break;
		case  8: color *= texture(u_textures[ 8], v_texCoord * v_tilingFactor); break;
		case  9: color *= texture(u_textures[ 9], v_texCoord * v_tilingFactor); break;
		case 10: color *= texture(u_textures[10], v_texCoord * v_tilingFactor); break;
		case 11: color *= texture(u_textures[11], v_texCoord * v_tilingFactor); break;
		case 12: color *= texture(u_textures[12], v_texCoord * v_tilingFactor); break;
		case 13: color *= texture(u_textures[13], v_texCoord * v_tilingFactor); break;
		case 14: color *= texture(u_textures[14], v_texCoord * v_tilingFactor); break;
		case 15: color *= texture(u_textures[15], v_texCoord * v_tilingFactor); break;
		case 16: color *= texture(u_textures[16], v_texCoord * v_tilingFactor); break;
		case 17: color *= texture(u_textures[17], v_texCoord * v_tilingFactor); break;
		case 18: color *= texture(u_textures[18], v_texCoord * v_tilingFactor); break;
		case 19: color *= texture(u_textures[19], v_texCoord * v_tilingFactor); break;
		case 20: color *= texture(u_textures[20], v_texCoord * v_tilingFactor); break;
		case 21: color *= texture(u_textures[21], v_texCoord * v_tilingFactor); break;
		case 22: color *= texture(u_textures[22], v_texCoord * v_tilingFactor); break;
		case 23: color *= texture(u_textures[23], v_texCoord * v_tilingFactor); break;
		case 24: color *= texture(u_textures[24], v_texCoord * v_tilingFactor); break;
		case 25: color *= texture(u_textures[25], v_texCoord * v_tilingFactor); break;
		case 26: color *= texture(u_textures[26], v_texCoord * v_tilingFactor); break;
		case 27: color *= texture(u_textures[27], v_texCoord * v_tilingFactor); break;
		case 28: color *= texture(u_textures[28], v_texCoord * v_tilingFactor); break;
		case 29: color *= texture(u_textures[29], v_texCoord * v_tilingFactor); break;
		case 30: color *= texture(u_textures[30], v_texCoord * v_tilingFactor); break;
		case 31: color *= texture(u_textures[31], v_texCoord * v_tilingFactor); break;
	}

	// the object ID buffer is set to be 64 bits per pixel, so entity id becomes:
	// ID=0x12345678 => R=0x5678 G=0x1234 G=0xffff A=0xffff
	objectIDBuffer = vec4(float((v_objectID & 0xffff)) / 0xffff, float(((v_objectID >> 16) & 0xffff)) / 0xffff, 1, 1);
}
