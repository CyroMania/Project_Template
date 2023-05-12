#include "scenebasic_uniform.h"
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include <string>
#include "errorhandling.h"
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"
#include "helper/objmesh.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(40.0f, 40.0f, 1, 1), lightPos(5.0f, 5.0f, 5.0f, 1.0f), tPrev(0.0f)
{
	mesh = ObjMesh::load("media/spot/spot_triangulated.obj");
	wall = ObjMesh::load("media/model/wall.obj", false, false);
	bucket = ObjMesh::load("media/model/bucket.obj", false, true);

	//stoneTex = Texture::loadTexture("media/texture/highresWall.jpg");
	//mossTex = Texture::loadTexture("media/texture/moss.png");
	//metalTex = Texture::loadTexture("media/texture/scrapedMetal.jpg");
	//grassTex = Texture::loadTexture("media/texture/grass.png");

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stoneTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mossTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metalTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassTex);*/
}

void SceneBasic_Uniform::initScene()
{
	compile();

	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	view = glm::lookAt(
		glm::vec3(0.0f, 3.0f, 7.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.5f, 100.0f);

	lightAngle = 0.0f;
	lightRotationSpeed = 1.5f;

	prog.setUniform("Light[0].L", glm::vec3(45.0f));
	prog.setUniform("Light[0].Position", view * lightPos);
	prog.setUniform("Light[1].L", glm::vec3(0.3f));
	prog.setUniform("Light[1].Position", glm::vec4(0, 0.15f, -1.0f, 0));
	prog.setUniform("Light[2].L", glm::vec3(45.0f));
	prog.setUniform("Light[2].Position", view * glm::vec4(-7, 3, 7, 1));
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	}
	catch (GLSLProgramException& e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& colour)
{
	model = glm::mat4(1.0f);
	prog.setUniform("Material.Rough", rough);
	prog.setUniform("Material.Metal", metal);
	prog.setUniform("Material.Colour", colour);
	model = glm::translate(model, pos);
	model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

	setMatrices();
	mesh->render();
}

void SceneBasic_Uniform::update(float t)
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f)
		deltaT = 0.0f;
	tPrev = t;

	if (animating())
	{
		lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
		lightPos.x = glm::cos(lightAngle) * 7.0f;
		lightPos.y = 3.0f;
		lightPos.z = glm::sin(lightAngle) * 7.0f;
	}

	if (cameraZ > -3.5f && movingForward) {
		view = glm::translate(view, vec3(0.0f, 0.0f, -0.01f));
		cameraZ = view[3].z;
	}
	else {
		movingForward = false;
	}

	if (cameraZ < 4.0f && !movingForward) {
		view = glm::translate(view, vec3(0.0f, 0.0f, 0.01f));
		cameraZ = view[3].z;
	}
	else {
		movingForward = true;
	}
}

void SceneBasic_Uniform::render()
{
	GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	prog.setUniform("Light[0].Position", view * lightPos);
	drawScene();
}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)width / height,
		0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(mv));
	prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::drawScene()
{
	drawFloor();

	//int numCows = 9;

	//glm::vec3 cowBaseColor(0.1f, 0.33f, 0.97f);
	//for (int i = 0;i < numCows; i++)
	//{
	//	float cowX = i * (10.0f / (numCows - 1)) - 5.0f;
	//	float rough = (i + 1) * (1.0f / numCows);
	//	drawSpot(glm::vec3(cowX, 0, 0), rough, 0, cowBaseColor);
	//}

	// Draw metal cows
	//float metalRough = 0.5f;
	////Gold
	//drawSpot(glm::vec3(-3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(1.0f, 0.71f, 0.29f));
	////Copper
	//drawSpot(glm::vec3(-1.5f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.64f, 0.54f));
	////Aluminium
	//drawSpot(glm::vec3(0.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.91f, 0.92f, 0.92f));
	////Titanium
	//drawSpot(glm::vec3(1.5f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.542f, 0.497f, 0.449f));
	////Silver
	//drawSpot(glm::vec3(3.0f, 0.0f, 3.0f), metalRough, 1, glm::vec3(0.95f, 0.93f, 0.88f));

	drawBuckets(5, 0.5f, 1, vec3(0.1f, 0.1f, 0.1f));
}

void SceneBasic_Uniform::drawFloor()
{
	model = glm::mat4(1.0f);
	prog.setUniform("Material.Rough", 0.9f);
	prog.setUniform("Material.Metal", 0);
	prog.setUniform("Material.Colour", glm::vec3(0.2f));
	model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));

	setMatrices();
	plane.render();
}

void SceneBasic_Uniform::drawBuckets(int number, float rough, int metal, const glm::vec3& colour)
{
	prog.setUniform("Material.Rough", 0.5f);
	prog.setUniform("Material.Metal", 1);
	prog.setUniform("Material.Colour", glm::vec3(0.1f));

	for (int i = 0; i < number; i++)
	{
		model = glm::scale(mat4(1.0f), vec3(3.0f, 3.0f, 3.0f));
		model = glm::translate(model, vec3((0.6f * (i % 2)), 0.0f, i * -0.75f));
		setMatrices();
		bucket->render();
	}
}

//void SceneBasic_Uniform::setDiffuseAmbientSpecular(std::string structure, float dif, float amb, float spec)
//{
//	if (structure == "Material")
//	{
//		prog.setUniform("Material.Kd", vec3(dif));
//		prog.setUniform("Material.Ka", vec3(amb));
//		prog.setUniform("Material.Ks", vec3(spec));
//	}
//	else if (structure == "PointLight")
//	{
//		prog.setUniform("PointLight.Ld", vec3(dif));
//		prog.setUniform("PointLight.La", vec3(amb));
//		prog.setUniform("PointLight.Ls", vec3(spec));
//	}
//}
