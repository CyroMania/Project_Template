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

SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0), tPrev(0), plane(20.0f, 20.0f, 1, 1), cameraZ(4.0f), movingForward(false)
{
	wall = ObjMesh::load("media/model/wall.obj", false, false);
	bucket = ObjMesh::load("media/model/bucket.obj", false, true);

	stoneTex = Texture::loadTexture("media/texture/highresWall.jpg");
	mossTex = Texture::loadTexture("media/texture/moss.png");
	metalTex = Texture::loadTexture("media/texture/scrapedMetal.jpg");
	grassTex = Texture::loadTexture("media/texture/grass.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stoneTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mossTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metalTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grassTex);
}

void SceneBasic_Uniform::initScene()
{
	compile();
	glEnable(GL_DEPTH_TEST);
	view = glm::lookAt(vec3(2.5f, 1.25f, cameraZ), vec3(2.5f, 1.25f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	setDiffuseAmbientSpecular("PointLight", 0.9f, 0.2f, 0.8f);

	prog.setUniform("Fog.MaxDist", 15.0f);
	prog.setUniform("Fog.MinDist", 5.0f);
	prog.setUniform("Fog.Colour", vec3(0.5f, 0.5f, 0.5f));
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

void SceneBasic_Uniform::update(float t)
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f)
		deltaT = 0.0f;
	tPrev = t;
	angle += 0.25f * deltaT;
	if (angle > glm::two_pi<float>())
		angle -= glm::two_pi<float>();

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

	vec4 lightPos = vec4(12.0f * cos(angle), 8.0f, 12.0f * sin(angle), 1.0f);
	prog.setUniform("PointLight.Position", vec4(lightPos));
	mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));

	prog.setUniform("Material.Shininess", 100.0f);
	prog.setUniform("TexIndex", 0);
	setDiffuseAmbientSpecular("Material", 0.7f, 0.8f, 0.2f);

	for (int i = 0; i < 3; i++)
	{
		model = glm::translate(mat4(1.0f), vec3((i * 2.5f), 0.0f, -2.0f - (8.0f * (i % 2))));
		model = glm::rotate(model, glm::radians(i * -90.0f), vec3(0.0f, 1.0f, 0.0f));
		setMatrices();
		wall->render();
	}

	setDiffuseAmbientSpecular("Material", 0.8f, 0.9f, 0.4f);
	prog.setUniform("Material.Shininess", 180.0f);
	prog.setUniform("TexIndex", 1);

	RenderBuckets(5);

	setDiffuseAmbientSpecular("Material", 0.5f, 0.6f, 0.2f);
	prog.setUniform("Material.Shininess", 50.0f);
	prog.setUniform("TexIndex", 2);

	for (int i = 0; i < 9; i++)
	{
		model = glm::translate(mat4(1.0f), vec3(((i % 3) * 20.0f) - 20.0f, 0.0f, (i % 2) * 20.0f));
		setMatrices();
		plane.render();
	}


}

void SceneBasic_Uniform::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h,
		0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
	prog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::RenderBuckets(int number)
{
	for (int i = 0; i < number; i++)
	{
		model = glm::scale(mat4(1.0f), vec3(3.0f, 3.0f, 3.0f));
		model = glm::translate(model, vec3(0.6f + (0.6f * (i % 2)), 0.0f, i * -0.75f));
		setMatrices();
		bucket->render();
	}
}

void SceneBasic_Uniform::setDiffuseAmbientSpecular(std::string structure, float dif, float amb, float spec)
{
	if (structure == "Material") 
	{
		prog.setUniform("Material.Kd", vec3(dif));
		prog.setUniform("Material.Ka", vec3(amb));
		prog.setUniform("Material.Ks", vec3(spec));
	}
	else if (structure == "PointLight")
	{
		prog.setUniform("PointLight.Ld", vec3(dif));
		prog.setUniform("PointLight.La", vec3(amb));
		prog.setUniform("PointLight.Ls", vec3(spec));
	}
}
