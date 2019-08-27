precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;

#ifdef DOWNFILTER

uniform vec2 u_TexelOffsets;
mediump vec4 temp1;
mediump float luminance;

#endif

void main()
{
#ifdef DOWNFILTER
	temp1 = texture(scene,TexCoords+vec2(u_TexelOffsets));
	temp1 += texture(scene,TexCoords+vec2(u_TexelOffsets.x,-u_TexelOffsets.y));
	temp1 += texture(scene,TexCoords+vec2(-u_TexelOffsets.x,u_TexelOffsets.y));
	temp1 += texture(scene,TexCoords+vec2(-u_TexelOffsets));
	temp1 *= 0.25;
	
	luminance = dot(temp1.rgb,vec3(0.2126f, 0.7152f, 0.0722f));

	FragColor = vec4(temp1.rgb*(luminance-0.25), 1.0f);
#else
    vec3 Color = texture(scene, TexCoords).rgb;      
	float brightness = dot(Color, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
        FragColor = vec4(Color, 1.0f);
    else
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
#endif
}