precision highp float;

varying vec2 TexCoords;
varying vec3 thecolor;
varying vec3 Normal;
varying vec3 FragPos;

uniform vec3 lightColor;
uniform bool use_texture;

uniform sampler2D material_diffuse;
uniform sampler2D material_specular;
uniform float     material_shininess;


uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform vec3 viewPos;

void main()
{
	if (use_texture == true)
	{
		// ambient
		vec3 ambient = light_ambient * texture2D(material_diffuse, TexCoords).rgb;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(light_position - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light_diffuse * diff * texture2D(material_diffuse, TexCoords).rgb;
    
		// specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
		vec3 specular = light_specular * spec * texture2D(material_specular, TexCoords).rgb;  

		gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
	}
	else
	{
		gl_FragColor = vec4(lightColor,1.0);
	}
}
