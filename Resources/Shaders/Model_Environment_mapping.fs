precision highp float;

uniform sampler2D material_texture_diffuse1;
uniform sampler2D material_texture_specular1;
uniform sampler2D material_texture_normal1;
uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform float material_shininess;

uniform vec3 material_color_ambient;
uniform vec3 material_color_diffuse;
uniform vec3 material_color_specular;
uniform float material_transparent;

// light
uniform vec3 light_position;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;

// point light
uniform float pointlight_constant;
uniform float pointlight_linear;
uniform float pointlight_quadratic;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform bool enableAlpha;
uniform bool useTexture;
uniform bool useNormalMap;
uniform bool isPicking;
uniform vec3 color_pick;
uniform bool uselighting;

out vec4 FragColor;

void main()
{   
	vec4 color, color_ambient, color_specular, color_diffuse;
	if (useTexture == true)
	{
		color = texture(material_texture_diffuse1, TexCoords);
		color_ambient = color;
		color_specular = texture(material_texture_specular1, TexCoords);
		color_diffuse = color;
	}
	else
	{
		color = vec4(material_color_diffuse, material_transparent);
		color_ambient = color;
		color_specular = vec4(material_color_specular,1.0);
		color_diffuse = vec4(material_color_diffuse,1.0);
	}

	if ((enableAlpha == true) && (color.a < 0.5))
	{
		discard;
	}

	//*********** calculate shadow **********************
	// perform perspective divide
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
	vec3 lightDir = normalize(light_position - FragPos);

	float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	ivec2 texsize = textureSize(shadowMap, 0);

    float texelSize = 1.0 / float(texsize.x);

    for(int m = -1; m <= 1; ++m)
    {
        for(int n = -1; n <= 1; ++n)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(float(m), float(n)) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;
	// END calculate shadow

	if (uselighting == false)
	{
		
		if (enableAlpha == true)
			FragColor = vec4(color.rgb + color_pick, color.a);
		else
			FragColor = vec4(color.rgb + color_pick, material_transparent);
		return;
	}

	vec3 ambient = light_ambient * color_ambient.rgb;

	// diffuse
	vec3 norm;
	if (useNormalMap == true)
	{
		norm = normalize( TBN * (normalize(texture(material_texture_normal1, TexCoords).rgb * 2.0 - 1.0)));
	}
	else
	{
		norm = normalize(Normal);
	}
   
    float diff = max(dot(norm, lightDir), 0.0);

	vec3 diffuse = light_diffuse * diff * color_diffuse.rgb;  

	 // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);

	vec3 specular = light_specular * spec * color_specular.rgb;  

	// pointlight
	float distance = length(light_position - FragPos);
	float attenuation = 1.0 / (pointlight_constant + pointlight_linear * distance + pointlight_quadratic * (distance * distance));

	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;


	// Total lighting + shadow
	vec3 lighting = ambient + (diffuse + specular) * (1.0 - shadow) + color_pick;

	// Total lighting + not shadow
	//vec3 lighting = ambient + (diffuse + specular) + color_pick;

	vec4 finalcolor;

	if (enableAlpha == true)
		finalcolor = vec4(lighting, color.a);
	else
		finalcolor = vec4(lighting, material_transparent);

	vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
	vec4 envmapping = vec4(texture(skybox, R).rgb, 1.0);

	finalcolor.rgb = pow(finalcolor.rgb,vec3(2.2));

	vec4 finalcolor2 = mix(finalcolor, envmapping, color_diffuse);

	//finalcolor2.rgb = pow(finalcolor2.rgb,vec3(.4545));

    FragColor = finalcolor2;
}