precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D txScene;
uniform sampler2D txBloomBlur;
uniform bool isEnablebloom;
uniform bool GammaCorrection;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(txScene, TexCoords).rgb;      
    vec3 bloomColor;
    if(isEnablebloom == true)
	{
		bloomColor = texture(txBloomBlur, TexCoords).rgb;
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