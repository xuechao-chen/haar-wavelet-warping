#version 430

uniform sampler2D srcTex;
in vec2 texCoord;
out vec4 color;

void main() {
	vec3 rgb = texture(srcTex, texCoord).rgb;
	
	//float grayscale = rgb.r*0.3+rgb.g*0.59+rgb.b*0.11;
	//color = vec4(grayscale,grayscale,grayscale,1.0);
	
	color = vec4(rgb, 1.0);
}