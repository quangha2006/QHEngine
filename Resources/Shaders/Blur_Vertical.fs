precision highp float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{  

	ivec2 texsize = textureSize(image, 0);
	            
    highp vec2 tex_offset = vec2(1.0, 1.0); // Fix compile error on Android
	tex_offset.y = 1.0 / float(texsize.y);

    vec3 result = texture(image, TexCoords).rgb * weight[0];

	for(int i = 1; i < 5; ++i)
	{
		result += texture(image, TexCoords + vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
		result += texture(image, TexCoords - vec2(0.0, tex_offset.y * float(i))).rgb * weight[i];
	}
    FragColor = vec4(result, 1.0);
}