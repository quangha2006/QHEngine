precision highp float;

in vec2 TexCoords;

uniform sampler2D material_texture_diffuse1;
uniform vec3 material_color_diffuse;
uniform float material_transparent;

uniform bool enableAlpha;
uniform float near_plane;
uniform float far_plane;
uniform bool useTexture;
out vec4 FragColor;
void main()
{             
	vec4 depthValue;
	if (useTexture == true)
	{
		depthValue = texture(material_texture_diffuse1, TexCoords);
	}
	else
	{
		depthValue = vec4(material_color_diffuse, material_transparent);
	}

	if (enableAlpha == true)
	{
		if(depthValue.a < 0.5)
			discard;
	}
 
	float z = depthValue.r * 2.0 - 1.0; // Back to NDC

    FragColor = vec4(vec3((2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane)) / far_plane), depthValue.a);
	
	//FragColor = vec4(texture2D(material_texture_diffuse1, TexCoords).rgb, 1.0);
}