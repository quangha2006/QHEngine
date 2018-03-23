#version 100
attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoords;
attribute vec3 aColor;

varying vec2 TexCoords;
varying vec3 thecolor;
varying vec3 Normal;
varying vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_inverse;

void main()
{
    TexCoords = aTexCoords;
	Normal = mat3(model_inverse) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));
	thecolor = aColor; 
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}