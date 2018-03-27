#extension GL_EXT_gpu_shader4 : enable
precision highp float;
attribute vec3 aPos;
attribute vec2 aTexCoords;
attribute vec4 sWeights;
attribute ivec4 sIDs;
attribute vec3 aNormal;
attribute vec3 aTangent;
attribute vec3 aBitangent;
                                                             
varying vec2 TexCoord0;

const int MAX_BONES = 33;

uniform mat4 lookat;
uniform mat4 gBones[MAX_BONES];

void main()
{   
	TexCoord0    = aTexCoords;

	highp vec4 PosL = vec4(0.0, 0.0, 0.0, 1.0);
	highp int index = int(sIDs[0]);
	highp mat4 BoneTransform	= gBones[index] * sWeights.x;
		index = int(sIDs[1]);
		BoneTransform += gBones[index] * sWeights.y;
		index = int(sIDs[2]);
		BoneTransform += gBones[index] * sWeights.z;
		index = int(sIDs[3]);
		BoneTransform += gBones[index] * sWeights.w;

    PosL    =  BoneTransform * vec4(aPos, 1.0);

    gl_Position  = lookat * PosL;;
}
