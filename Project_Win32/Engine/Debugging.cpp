#include "Debugging.h"
#include <iostream>
#include "Utils.h"
#include "Timer.h"

Debugging *Debugging::instance = NULL;

void Debugging::InitBallData()
{
	string path_modif = Utils::getResourcesFolder() + "Debug/Ball_debug.dae";
	Assimp::Importer mImporter;
	const aiScene* m_pScene = mImporter.ReadFile(path_modif, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!m_pScene)
	{
		LOGE("ERROR::ASSIMP:: %s\n", mImporter.GetErrorString());
		return;
	}

	const aiMesh* mesh = m_pScene->mMeshes[0];

	mNumVertices_ball = mesh->mNumVertices;
	mVertices_ball = new float[mNumVertices_ball * 3];

	std::memcpy(mVertices_ball, mesh->mVertices, sizeof(float) * mNumVertices_ball * 3);

	mNumIndices_ball = mesh->mNumFaces * 3;

	mIndices_ball = new unsigned int[mNumIndices_ball];

	unsigned int *pIndices_tmp = mIndices_ball;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		std::memcpy(pIndices_tmp, mesh->mFaces[i].mIndices, sizeof(unsigned int) * 3);
		pIndices_tmp += 3;
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

	char vtxSrc[] = {
		"layout(location = 0) in vec3 aPos;\n"
		"\n"
		"uniform mat4 view_projection;\n"
		"uniform mat4 model;\n"
		"\n"
		"void main()\n"
		"{\n"
			"gl_Position = view_projection * model * vec4(aPos, 1.0);\n"
		"}\n"
	};

	char fragSrc[] = {
		"precision highp float;\n"
		"layout(location = 0) out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
			"FragColor = vec4(1.0, 0.0, 0.0 ,1.0);\n"
		"}\n"
	};
	mBall_shader.LoadShader(vtxSrc, fragSrc, true);
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

void Debugging::RenderBall(glm::vec3 pos, float scale)
{
	if (!mIsInitBall)
	{
		InitBallData();
		mIsInitBall = true;
	}

	if (mNumVertices_ball == 0U) // Check ball model is loaded
		return;

	Camera *camera = Camera::getInstance();
	glm::mat4 view = camera->GetWorldViewProjectionMatrix();
	glm::mat4 model = glm::translate(glm::mat4(), pos);
	model = glm::scale(model, glm::vec3(scale));
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
	: mNumVertices(0U)
	, numDrawCall(0)
	, mIsInitBall(false)
	, mNumVertices_ball(0U)
	, mNumIndices_ball(0U)
	, mVertices_ball(NULL)
	, mIndices_ball(NULL)
	, mVBO_ball(0U)
	, mEBO_ball(0U)
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

	// Destroy ball data
	if (mVertices_ball)
		delete[] mVertices_ball;
	if (mIndices_ball)
		delete[] mIndices_ball;

	glDeleteBuffers(1, &mVBO_ball);
	glDeleteBuffers(1, &mEBO_ball);
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

void QHEngine::EnablePolygonModeGLLine()
{
#ifdef _WINDOWS
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
}

void QHEngine::DisablePolygonModeGLLine()
{
#ifdef _WINDOWS
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}
