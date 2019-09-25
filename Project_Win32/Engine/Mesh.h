#ifndef __MESH_H__
#define __MESH_H__

#include <assimp/Importer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "QHTexture.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "QHVertex.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "PhysicsSimulation.h"


class Mesh {
private:
	std::string mMeshName;
	bool mHasNormals;
	bool mHasBone;

	GLuint mIndices_index;
	GLuint mIndices_size;

	GLuint mMaterial_Id;

	Vertex *mVertex;
	GLuint mNumVertex;

	GLuint *mIndices;
	GLuint mNumIndices;
	glm::mat4 mLocalTransformation;

	btRigidBody* mRigidBody;
	bool isDynamic;

public:
	void SetUseLighting(bool isuse);
	void SetVertex(Vertex * vertex, GLuint numvertex);
	void SetIndices(GLuint *indices, GLuint numindices);
	void SetLocalTransformation(glm::mat4 transformation);
	Vertex *GetVertex(GLuint &numvertex);
	GLuint *GetIndices(GLuint &numindices);
	const std::string &GetName();
	GLuint GetIndicesIndex();
	GLuint GetIndicesSize();
	GLuint GetMaterialId();
	void SetMaterialId(GLuint newid);
	void registerShapeTriangle(float mass, bool isOptimize = true);
	void Draw(RenderTargetType RT_Type
		, bool isDrawWireFrame
		, bool useCustomColor = false
		, const glm::vec3 &customColor = glm::vec3(0.0f, 0.0f, 0.0f));

	Mesh(GLuint index_begin
		,GLuint index_size
		,GLuint material_id
		,const std::string &meshname
		,bool hasnormals = false
		,bool hasbone = false);
	~Mesh();
};
#endif //!__MESH_H__