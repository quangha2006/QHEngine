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
    if(bloom == true)
	{
		bloomColor = texture(bloomBlur, TexCoords).rgb;
		hdrColor += bloomColor;
	}
    if(GammaCorrection == true)    
    {
	// tone mapping
		//hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
	
	// also gamma correct while we're at it       
		hdrColor = pow(hdrColor, vec3(1.0 / gamma));
	}
    FragColor = vec4(hdrColor, 1.0);
}