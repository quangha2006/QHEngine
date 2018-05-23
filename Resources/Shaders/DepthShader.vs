precision highp float;
in vec3 aPos;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;

out vec2 TexCoords;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 gBones[48];
uniform bool useAnim;

void main()
{
	highp vec4 PosL = vec4(0.0, 0.0, 0.0, 1.0);
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

		PosL   = BoneTransform * vec4(aPos, 1.0);
	}
	else
		PosL   = vec4(aPos, 1.0);
	
    gl_Position =  lightSpaceMatrix * model * PosL;

	TexCoords = aTexCoords;
}