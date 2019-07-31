in vec3 aPos;
in vec2 aTexCoords;
in vec4 sWeights;
in vec4 sIDs;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBitangent;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0f); 
}  