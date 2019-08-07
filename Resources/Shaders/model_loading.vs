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
out vec4 temp;

uniform mat4 world;
uniform mat4 world_inverse;
uniform mat4 WorldViewProjectionMatrix;
uniform mat4 lightSpaceMatrix; //shadow
uniform mat4 gBones[64];
void main()
{
	highp vec4 PosL = vec4(aPos, 1.0f);
	vec4 NormalL = vec4(aNormal, 0.0f);
	temp = sWeights;

	#ifdef SKINNED
		int index = int(sIDs[0]);
		mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs[1]);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs[2]);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs[3]);
		BoneTransform += gBones[index] * sWeights[3];

		PosL    = BoneTransform * vec4(aPos, 1.0f);

		NormalL = BoneTransform * vec4(aNormal, 0.0f);
	#else
		PosL    =  vec4(aPos, 1.0f);
		NormalL =  vec4(aNormal, 0.0f);
	#endif

    gl_Position = WorldViewProjectionMatrix * PosL;

	FragPos = vec3(world * PosL);
	Normal = mat3(world_inverse) * NormalL.xyz; 
    TexCoords = aTexCoords; 
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);

	vec3 T = normalize(vec3(world * vec4(aTangent,   0.0f)));
	vec3 B = normalize(vec3(world * vec4(aBitangent, 0.0f)));
	vec3 N = normalize(vec3(world * NormalL));
	TBN = mat3(T, B, N);
}