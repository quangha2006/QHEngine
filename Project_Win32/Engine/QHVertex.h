#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec4 weight;
	glm::vec4 id;
	Vertex()
	{
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		Normal = glm::vec3(1.0f, 1.0f, 1.0f);
		TexCoords = glm::vec2(0.0f, 0.0f);
		Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
		weight = glm::vec4(0.0f);
		id = glm::vec4(0.0f);
	}
};
struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float transparent;
};

struct BoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;

	BoneInfo()
	{
		BoneOffset = glm::mat4(0);
		FinalTransformation = glm::mat4(0);
	}
};