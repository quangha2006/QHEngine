precision highp float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 sWeights;
layout (location = 4) in vec4 sIDs;
layout (location = 5) in vec3 aNormal;
layout (location = 6) in vec3 aTangent;
layout (location = 7) in vec3 aBitangent;
#ifdef INSTANCING
layout (location = 8) in mat4 aInstanceMatrix;
#endif

out	vec3 fs_FragPos;
out	vec2 fs_TexCoords;
out	vec3 fs_Normal;
out	mat3 fs_TBN;
out	vec4 fs_FragPosLightSpace; //shadow
out	vec4 fs_we;
out	vec4 fs_Color;

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
#ifdef INSTANCING
		gl_Position = WorldViewProjectionMatrix * aInstanceMatrix * PosL;
		NormalL = aInstanceMatrix * NormalL;
#else
		gl_Position = WorldViewProjectionMatrix * PosL;
#endif
	vec3 fragpos = vec3(world * PosL);
	fs_FragPos = fragpos;
	fs_Normal = (world_inverse * NormalL).xyz; 
    fs_TexCoords = aTexCoords; 
	fs_FragPosLightSpace = lightSpaceMatrix * vec4(fragpos, 1.0f);
	fs_Color = aColor;

	vec3 T = normalize(vec3(world * vec4(aTangent,   0.0f)));
	vec3 B = normalize(vec3(world * vec4(aBitangent, 0.0f)));
	vec3 N = normalize(vec3(world * NormalL));
	fs_TBN = mat3(T, B, N);
	fs_we = sWeights;
}