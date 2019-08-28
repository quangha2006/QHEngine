precision highp float;

uniform sampler2D material_texture_diffuse1;
uniform sampler2D material_texture_specular1;
uniform sampler2D material_texture_normal1;
uniform sampler2D shadowMap;
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
in vec4 Color;

uniform vec3 viewPos;
uniform bool enableAlpha;
uniform bool useNormalMap;
uniform bool uselighting;
uniform bool usepointlight;
uniform bool useShadowMap;
uniform bool GammaCorrection;

layout (location = 0) out vec4 FragColor;

void main()
{   
	vec4 color = Color;//vec4(material_color_diffuse, material_transparent);
	//color.a = material_transparent;
	if (material_color_diffuse.r < 0.f)
	{
		color = texture(material_texture_diffuse1, TexCoords);
	}

	vec4 color_diffuse = vec4(color.rgb,1.f);
	vec4 color_ambient = color;

	vec4 color_specular = vec4(material_color_specular,1.f);
	if (material_color_specular.r < 0.f)
	{
		color_specular = texture(material_texture_specular1, TexCoords);
	}
	float shadow = 0.f;
	vec3 lighting;

	if ((enableAlpha == true) && (color.a < 0.2f))
	{
		discard;
	}

	// check whether current frag pos is in shadow
	vec3 lightDir = normalize(light_position - FragPos);

	//*********** calculate shadow **********************
	if (useShadowMap == true)
	{
		// perform perspective divide
		vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;

		// transform to [0,1] range
		projCoords = projCoords * 0.5f + 0.5f;

		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		float closestDepth = texture(shadowMap, projCoords.xy).r;

		// get depth of current fragment from light's perspective
		float currentDepth = projCoords.z;

		//highp float bias = 0.003f;
		highp float bias = max(0.001 * (1.0 - dot(Normal, lightDir)), 0.005); //0.001
		//bias = clamp(bias, 0, 0.01);

		shadow = currentDepth + bias > closestDepth  ? 1.0f : 0.0f;

		ivec2 texsize = textureSize(shadowMap, 0);

		float texelSize = 1.0 / float(texsize.x);

		for(int m = -1; m <= 1; ++m)
		{
			for(int n = -1; n <= 1; ++n)
			{
				float pcfDepth = texture(shadowMap, projCoords.xy + vec2(float(m), float(n)) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth  ? 1.0f : 0.0f;        
			}    
		}
		shadow /= 10.0f;

		if(projCoords.z > 1.0f)
			shadow = 0.0f;
	} // END calculate shadow
		
	if (uselighting == false)
	{	
		if (GammaCorrection == true)
		{
			color.rgb = pow(color.rgb, vec3(2.2));
		}
		if (enableAlpha == true)
			FragColor = vec4(color.rgb, color.a);
		else
			FragColor = vec4(color.rgb, material_transparent);
		return;
	}

	vec3 ambient = light_ambient * color_ambient.rgb;

	// diffuse
	vec3 norm;
	if (useNormalMap == true)
	{
		norm = normalize( TBN * (normalize(texture(material_texture_normal1, TexCoords).rgb * 2.0f - 1.0f)));
	}
	else
	{
		norm = normalize(Normal);
	}
   
    float diff = max(dot(norm, lightDir), 0.0f);

	vec3 diffuse = light_diffuse * diff * color_diffuse.rgb;  

	 // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material_shininess);

	vec3 specular = light_specular * spec * color_specular.rgb;  

	// pointlight
	//if (usepointlight == true)
	//{
	//	float distance = length(light_position - FragPos);
	//	float attenuation = 1.0 / (pointlight_constant + pointlight_linear * distance + pointlight_quadratic * (distance * distance));

	//	ambient  *= attenuation; 
	//	diffuse  *= attenuation;
	//	specular *= attenuation;
	//}

	if (uselighting == true)
	{
		// Total lighting + shadow
		lighting = ambient + (diffuse + specular) * (1.0f - shadow);
	}
	else
	{
		// Total lighting + not shadow
		lighting = ambient + diffuse + specular;
	}
	if (GammaCorrection == true)
	{
		lighting = pow(lighting, vec3(2.2));
	}
	if (enableAlpha == true)
		FragColor = vec4(lighting, color.a);
	else
		FragColor = vec4(lighting, material_transparent);
}