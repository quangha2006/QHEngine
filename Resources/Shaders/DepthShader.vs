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

out vec2 TexCoords;
uniform mat4 WorldViewLightSpaceMatrix;
uniform mat4 gBones[64];

void main()
{
	highp vec4 PosL = vec4(aPos, 1.0f);
	#ifdef SKINNED
		int index = int(sIDs.x);

		mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs.y);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs.z);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs.w);
		BoneTransform += gBones[index] * sWeights[3];

		PosL   = BoneTransform * vec4(aPos, 1.0f);

	#endif
#ifdef INSTANCING	
    gl_Position =  WorldViewLightSpaceMatrix * aInstanceMatrix * PosL;
#else
	gl_Position =  WorldViewLightSpaceMatrix * PosL;
#endif
	TexCoords = aTexCoords;
}