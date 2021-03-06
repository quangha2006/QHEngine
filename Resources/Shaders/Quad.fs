precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D txScene;

#ifdef ENABLEBLOOM
uniform sampler2D txBloomBlur;
#endif

uniform bool GammaCorrection;
uniform float exposure;

void main()
{             
    const float gamma = 2.2f;
    vec3 hdrColor = texture(txScene, TexCoords).rgb;      
   
#ifdef ENABLEBLOOM

	vec3 bloomColor;
	bloomColor = texture(txBloomBlur, TexCoords).rgb;
	hdrColor += bloomColor;

#endif
    if(GammaCorrection == true)    
    {
	// tone mapping
		hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
	
	// also gamma correct while we're at it       
		hdrColor = pow(hdrColor, vec3(1.0f / gamma));
	}
    FragColor = vec4(hdrColor, 1.0f);
}