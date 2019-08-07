precision highp float;
in vec3 aPos;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;

out vec2 TexCoords;
uniform mat4 WorldViewLightSpaceMatrix;
uniform mat4 gBones[64];

void main()
{
	highp vec4 PosL = vec4(aPos, 1.0f);
	#ifdef SKINNED
		int index = int(sIDs[0]);
		mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs[1]);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs[2]);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs[3]);
		BoneTransform += gBones[index] * sWeights[3];

		PosL   = BoneTransform * vec4(aPos, 1.0f);
	#else
		PosL    = vec4(aPos, 1.0f);
	#endif
	
    gl_Position =  WorldViewLightSpaceMatrix * PosL;

	TexCoords = aTexCoords;
}