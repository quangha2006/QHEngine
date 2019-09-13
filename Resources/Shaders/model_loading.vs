precision highp float;
in vec3 aPos;
in vec4 aColor;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;
in float atransformId;

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

void main()
{
	highp vec4 PosL = vec4(aPos, 1.0f);
	vec4 NormalL = vec4(aNormal, 0.0f);

	#ifdef SKINNED
		int index = int(sIDs.x);
		if (index > -1)
		{
			mat4 BoneTransform	= gBones[index] * sWeights[0];
			index = int(sIDs.y);
			BoneTransform += gBones[index] * sWeights[1];
			index = int(sIDs.z);
			BoneTransform += gBones[index] * sWeights[2];
			index = int(sIDs.w);
			BoneTransform += gBones[index] * sWeights[3];

			PosL    = BoneTransform * vec4(aPos, 1.0f);

			NormalL = BoneTransform * vec4(aNormal, 0.0f);
		}
	#endif

    gl_Position = WorldViewProjectionMatrix * PosL;

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