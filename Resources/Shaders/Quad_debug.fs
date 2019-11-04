precision highp float;

in vec2 TexCoords;

uniform sampler2D textureid;

out vec4 FragColor;

void main()
{    
	vec4 depthValue = texture(textureid, TexCoords);
	FragColor = vec4(depthValue); // orthographic
}