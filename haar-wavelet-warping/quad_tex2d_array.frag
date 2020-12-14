#version 430

uniform sampler2DArray srcTex;
layout(r32f, binding=0) uniform image2D warpingTex;
in vec2 texCoord;
out vec4 color;

void main() {
	int ix = int(floor(texCoord.x*16));
	int iy = int(floor(texCoord.y*16));
	
	if (ix == 16) ix = 15;
	if (iy == 16) iy = 15;

	vec2 coord = 16*texCoord - vec2(ix, iy);
	int layer = ix+iy*16;

	vec3 rgb = texture(srcTex, vec3(coord, layer)).rgb;
	vec3 warping_rgb = imageLoad(warpingTex, ivec2(gl_FragCoord.xy)).rgb;
	
	color = vec4(mix(rgb, warping_rgb, 0.5), 1.0);
}