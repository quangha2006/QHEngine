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

uniform mat4 world;
uniform mat4 world_inverse;
uniform mat4 WorldViewProjectionMatrix;
uniform mat4 lightSpaceMatrix; //shadow
uniform mat4 gBones[64];
uniform bool useAnim; 
void main()
{
	highp vec4 PosL = vec4(aPos, 1.0);
	vec4 NormalL = vec4(aNormal, 0.0);
	if (useAnim == true)
	{
		PosL = vec4(0.0, 0.0, 0.0, 1.0);
		NormalL = vec4(0.0, 0.0, 0.0, 0.0);

		int index = int(sIDs[0]);
		PosL.xyz += sWeights[0] * (gBones[index] * vec4(aPos, 1.0)).xyz;
		NormalL.xyz += sWeights[0] * (gBones[index] * vec4(aNormal, 0.0)).xyz;

		index = int(sIDs[1]);
		PosL.xyz += sWeights[1] * (gBones[index] * vec4(aPos, 1.0)).xyz;
		NormalL.xyz += sWeights[1] * (gBones[index] * vec4(aNormal, 0.0)).xyz;

		index = int(sIDs[2]);
		PosL.xyz += sWeights[2] * (gBones[index] * vec4(aPos, 1.0)).xyz;
		NormalL.xyz += sWeights[2] * (gBones[index] * vec4(aNormal, 0.0)).xyz;

		index = int(sIDs[3]);
		PosL.xyz += sWeights[3] * (gBones[index] * vec4(aPos, 1.0)).xyz;
		NormalL.xyz += sWeights[3] * (gBones[index] * vec4(aNormal, 0.0)).xyz;
	}

    gl_Position = WorldViewProjectionMatrix * PosL;

	FragPos = vec3(world * PosL);
	Normal = mat3(world_inverse) * NormalL.xyz; 
    TexCoords = aTexCoords; 
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	vec3 T = normalize(vec3(world * vec4(aTangent,   0.0)));
	vec3 B = normalize(vec3(world * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(world * NormalL));
	TBN = mat3(T, B, N);
}