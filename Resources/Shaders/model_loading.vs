precision highp float;
in vec3 aPos;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace; //shadow
out vec4 we;
out vec4 id;

uniform mat4 model;
uniform mat4 model_inverse;
uniform mat4 lookat;
uniform mat4 lightSpaceMatrix; //shadow
uniform mat4 gBones[48];
uniform bool useAnim; 
void main()
{
	highp vec4 PosL = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 NormalL = vec4(aNormal, 0.0);
	if (useAnim == true)
	{
	int index = int(sIDs[0]);
	mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs[1]);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs[2]);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs[3]);
		BoneTransform += gBones[index] * sWeights[3];

		PosL    = BoneTransform * vec4(aPos, 1.0);

		NormalL = BoneTransform * vec4(aNormal, 0.0);
	}
	else
		PosL    =  vec4(aPos, 1.0);
    gl_Position = lookat * PosL;

	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(model_inverse) * NormalL.xyz; 
    TexCoords = aTexCoords; 
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * NormalL));
	TBN = mat3(T, B, N);
}