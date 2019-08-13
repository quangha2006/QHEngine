precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;

void main()
{             
    vec3 Color = texture(scene, TexCoords).rgb;      
	float brightness = dot(Color, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
        FragColor = vec4(Color, 1.0f);
    else
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}