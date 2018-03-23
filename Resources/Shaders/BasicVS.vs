precision highp float;
in vec3 aPos;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;
                                                             
out vec2 TexCoord0;
out float sIDs0;
const int MAX_BONES = 33;

uniform mat4 lookat;
uniform mat4 gBones[MAX_BONES];

void main()
{   
	TexCoord0    = aTexCoords;

	highp vec4 PosL = vec4(0.0, 0.0, 0.0, 1.0);
	highp int index = int(sIDs.x);
	highp mat4 BoneTransform	= gBones[index] * sWeights.x;
		index = int(sIDs.y);
		BoneTransform += gBones[index] * sWeights.y;
		index = int(sIDs.z);
		BoneTransform += gBones[index] * sWeights.z;
		index = int(sIDs.w);
		BoneTransform += gBones[index] * sWeights.w;

    PosL    =  BoneTransform * vec4(aPos, 1.0);

    gl_Position  = lookat * PosL;
	sIDs0 = sIDs.z;
}
