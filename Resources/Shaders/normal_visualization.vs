layout (location = 0) in vec3 aPos;
layout (location = 3) in vec4 sWeights;
layout (location = 4) in vec4 sIDs;
layout (location = 5) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#ifdef SKINNED
uniform mat4 gBones[64];
#endif

void main()
{
#ifdef SKINNED
		int index = int(sIDs.x);

		mat4 BoneTransform	= gBones[index] * sWeights[0];
		index = int(sIDs.y);
		BoneTransform += gBones[index] * sWeights[1];
		index = int(sIDs.z);
		BoneTransform += gBones[index] * sWeights[2];
		index = int(sIDs.w);
		BoneTransform += gBones[index] * sWeights[3];

		mat3 normalMatrix = mat3(transpose(inverse(view * model)));

		vec3 normal = mat3(BoneTransform) * aNormal;

		vs_out.normal = vec3(projection * vec4(normalMatrix * normal, 0.0));

		gl_Position   = projection * view * model *  BoneTransform * vec4(aPos, 1.0);
#else

    mat3 normalMatrix = mat3(transpose(inverse(view * model)));

    vs_out.normal = vec3(projection * vec4(normalMatrix * aNormal, 0.0));

    gl_Position = projection * view * model * vec4(aPos, 1.0);
#endif
}