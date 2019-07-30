precision highp float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

float weight[5] = float[] (0.2270270270f, 0.1945945946f, 0.1216216216f, 0.0540540541f, 0.0162162162f);

void main()
{  

	ivec2 texsize = textureSize(image, 0);
	            
    highp vec2 tex_offset = vec2(1.0f, 1.0f); // Fix compile error on Android
	
    vec3 result = texture(image, TexCoords).rgb * weight[0];

#ifdef HORIZONTAL
	tex_offset.x = 1.0f / float(texsize.x);

	result += texture(image, TexCoords + vec2(tex_offset.x, 0.0f)).rgb * weight[1];
	result += texture(image, TexCoords - vec2(tex_offset.x, 0.0f)).rgb * weight[1];

	result += texture(image, TexCoords + vec2(tex_offset.x * 2.0f, 0.0f)).rgb * weight[2];
	result += texture(image, TexCoords - vec2(tex_offset.x * 2.0f, 0.0f)).rgb * weight[2];

	result += texture(image, TexCoords + vec2(tex_offset.x * 3.0f, 0.0f)).rgb * weight[3];
	result += texture(image, TexCoords - vec2(tex_offset.x * 3.0f, 0.0f)).rgb * weight[3];

	result += texture(image, TexCoords + vec2(tex_offset.x * 4.0f, 0.0f)).rgb * weight[4];
	result += texture(image, TexCoords - vec2(tex_offset.x * 4.0f, 0.0f)).rgb * weight[4];
#endif

#ifdef VERTICAL
	tex_offset.y = 1.0 / float(texsize.y);

	result += texture(image, TexCoords + vec2(0.0f, tex_offset.y)).rgb * weight[1];
	result += texture(image, TexCoords - vec2(0.0f, tex_offset.y)).rgb * weight[1];

	result += texture(image, TexCoords + vec2(0.0f, tex_offset.y * 2.0f)).rgb * weight[2];
	result += texture(image, TexCoords - vec2(0.0f, tex_offset.y * 2.0f)).rgb * weight[2];

	result += texture(image, TexCoords + vec2(0.0f, tex_offset.y * 3.0f)).rgb * weight[3];
	result += texture(image, TexCoords - vec2(0.0f, tex_offset.y * 3.0f)).rgb * weight[3];

	result += texture(image, TexCoords + vec2(0.0f, tex_offset.y * 4.0f)).rgb * weight[4];
	result += texture(image, TexCoords - vec2(0.0f, tex_offset.y * 4.0f)).rgb * weight[4];
#endif
    FragColor = vec4(result, 1.0f);
}