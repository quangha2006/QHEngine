#include "Debugging.h"
#include <iostream>
#include "Utils.h"

Debugging *Debugging::instance = NULL;

void Debugging::InitBallData()
{
	//Ball
	string path_modif = Utils::getResourcesFolder() + "3DBreakOutGame/UVCircle2.dae";
	const aiScene* m_pScene = mImporter.ReadFile(path_modif, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	const aiMesh* mesh = m_pScene->mMeshes[0];
	LOGI("Num Mesh: %d\n", m_pScene->mNumMeshes);
	mNumVertices_ball = mesh->mNumVertices;
	mVertices_ball = new float[mNumVertices_ball * 3];
	LOGI("mVertices_ball: %u\n", mNumVertices_ball);
	//memcpy(mVertices_ball, mesh->mVertices, sizeof(float) * mNumVertices_ball * 3);
	unsigned int currentindex_vertex = 0;
	for (unsigned int i = 0; i < mNumVertices_ball; i++)
	{
		const aiVector3D &pPos = mesh->mVertices[i];
		mVertices_ball[currentindex_vertex++] = pPos.x;
		mVertices_ball[currentindex_vertex++] = pPos.y;
		mVertices_ball[currentindex_vertex++] = pPos.z;
	}
	LOGI("currentindex_vertex: %u\n", currentindex_vertex);

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		mNumIndices_ball += mesh->mFaces[i].mNumIndices;
	}
	mIndices_ball = new unsigned int[mNumIndices_ball];
	unsigned int currentindex = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		std::memcpy(&mIndices_ball[currentindex], face.mIndices, sizeof(unsigned int) * face.mNumIndices);
		currentindex += face.mNumIndices;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &mVBO_ball);
	glGenBuffers(1, &mEBO_ball);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_ball);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices_ball * 3 * sizeof(float), mVertices_ball, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_ball);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices_ball * sizeof(GLuint), mIndices_ball, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	mBall_shader.LoadShader("Shaders/cubeVS.vs", "Shaders/cubeFS.fs", false);

}

Debugging * Debugging::getInstance()
{
	if (instance == NULL)
		instance = new Debugging();
	return instance;
}

void Debugging::addNumVertices(unsigned int count)
{
	mNumVertices += count;
}

unsigned int Debugging::getNumVertices()
{
	return mNumVertices;
}

void Debugging::addDrawCall(unsigned short count)
{
	numDrawCall += count;
}

unsigned short Debugging::getNumDrawCall()
{
	return numDrawCall;
}

void Debugging::resetCount()
{
	mNumVertices = 0;
	numDrawCall = 0;
}

void Debugging::RenderTexture(GLuint TexId)
{
	//Debug
	glEnable(GL_DEPTH_TEST);
	mQuadDebug_shader.use();
	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexId);	// use the color attachment texture as the texture of the quad plane

	mQuadDebug_shader.setInt("textureid", 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	//End debug
}

void Debugging::RenderBall(glm::vec3 pos)
{
	if (!mIsInitBall)
	{
		InitBallData();
		mIsInitBall = true;
	}
		
	Camera *camera = Camera::getInstance();
	glm::mat4 view = camera->GetWorldViewProjectionMatrix();
	glm::mat4 model = glm::translate(glm::mat4(), pos);
	model = glm::scale(model, glm::vec3(0.2f));
	mBall_shader.use();
	mBall_shader.setMat4("model", model);
	mBall_shader.setMat4("view_projection", view);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO_ball);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO_ball);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	QHEngine::DrawElements(GL_TRIANGLES, mNumIndices_ball, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Debugging::Debugging()
	: mNumVertices(0)
	, numDrawCall(0)
	, mIsInitBall(false)
{
	quadVertices = new float[5 * 4]{
		// positions   // texCoords
		0.2f,  1.0f, 0.0f, 0.0f, 1.0f,
		0.2f,  0.2f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  0.2f, 0.0f, 1.0f, 0.0f
	};
	glBindVertexArray(0);
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	mQuadDebug_shader.LoadShader("Shaders/Quad_debug.vs", "Shaders/Quad_debug.fs");
}

Debugging::~Debugging()
{
	delete[] quadVertices;
	glDeleteBuffers(1, &quadVBO);
}

void QHEngine::DrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	glDrawElements(mode, count, type, indices);
	Debugging::getInstance()->addDrawCall(1);
	Debugging::getInstance()->addNumVertices(count);
}

void QHEngine::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
	glDrawArrays(mode, first, count);
	Debugging::getInstance()->addDrawCall(1);
	Debugging::getInstance()->addNumVertices(count);
}
void QHEngine::DrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount)
{
	glDrawElementsInstanced(mode, count, type, indices, instancecount);
	Debugging::getInstance()->addDrawCall(1);
	Debugging::getInstance()->addNumVertices(count * instancecount);
}

void QHEngine::RenderDebugTexture(GLuint textureId)
{
	Debugging::getInstance()->RenderTexture(textureId);
}
