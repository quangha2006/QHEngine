precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 sWeights;
layout (location = 4) in vec4 sIDs;
layout (location = 5) in vec3 aNormal;
layout (location = 6) in vec3 aTangent;
layout (location = 7) in vec3 aBitangent;
layout (location = 8) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;
out vec4 FragPosLightSpace; //shadow
out vec4 we;
out vec4 id;
out vec4 Color;

uniform mat4 world;
uniform mat4 world_inverse;
uniform mat4 WorldViewProjectionMatrix;
uniform mat4 lightSpaceMatrix; //shadow
uniform mat4 gBones[64];
uniform mat4 localTranform;
void main()
{
	highp vec4 PosL = vec4(aPos, 1.0f);
	vec4 NormalL = vec4(aNormal, 0.0f);

	#ifdef SKINNED
		int index = int(sIDs.x);

		mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs.y);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs.z);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs.w);
		BoneTransform += gBones[index] * sWeights[3];

		PosL    = BoneTransform * vec4(aPos, 1.0f);

		NormalL = BoneTransform * vec4(aNormal, 0.0f);

	#endif

    gl_Position = WorldViewProjectionMatrix * aInstanceMatrix * PosL;
	NormalL = aInstanceMatrix * NormalL;

	FragPos = vec3(world * PosL);
	Normal = (world_inverse * NormalL).xyz; 
    TexCoords = aTexCoords; 
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
	Color = aColor;

	vec3 T = normalize(vec3(world * vec4(aTangent,   0.0f)));
	vec3 B = normalize(vec3(world * vec4(aBitangent, 0.0f)));
	vec3 N = normalize(vec3(world * NormalL));
	TBN = mat3(T, B, N);
	we = sWeights;
}