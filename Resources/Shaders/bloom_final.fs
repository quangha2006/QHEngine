precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform bool GammaCorrection;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor;
	vec3 result;
    if(bloom == true)
	{
		bloomColor = texture(bloomBlur, TexCoords).rgb;
		result = hdrColor + bloomColor;
	}
    if(GammaCorrection == true)    
    {
	// tone mapping
		//result = vec3(1.0) - exp(-hdrColor * exposure);
	
	// also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));
	}
    FragColor = vec4(result, 1.0);
}