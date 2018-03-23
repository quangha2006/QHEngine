precision highp float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

varying vec2 TexCoord;
varying vec3 thecolor;
varying vec3 Normal;
varying vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

uniform float transparent;
uniform bool useTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;

void main()
{
	if (useTexture == true)
		gl_FragColor = mix(texture2D(ourTexture, TexCoord) , texture2D(ourTexture2, TexCoord), transparent);
	else
	{
		float ambientStrength = 0.2;
		vec3 ambient = lightColor * material.ambient;

		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos); 

		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = lightColor * (diff * material.diffuse);

		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = specularStrength * lightColor * (spec * material.specular);  

		vec3 result = (ambient +  diffuse + specular);

		gl_FragColor = vec4(result, 1.0);
	}
}
