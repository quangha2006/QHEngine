precision highp float;

in vec2 TexCoords;

uniform sampler2D depthMap;

out vec4 FragColor;

void main()
{    
	vec4 depthValue = texture(depthMap, TexCoords);
	FragColor = vec4(depthValue); // orthographic
}