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

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

//SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}
//SceneBasic_Uniform::SceneBasic_Uniform() : teapot(50, glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))) {}
//SceneBasic_Uniform::SceneBasic_Uniform() : plane(50.0f, 50.0f, 1, 1)
SceneBasic_Uniform::SceneBasic_Uniform() : angle(0.0), tPrev(0), plane(50.0f, 50.0f, 1, 1), cameraZ(3.25f)
{
	//mesh = ObjMesh::load("../Project_Template/media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
	compile();
	glEnable(GL_DEPTH_TEST);
	view = glm::lookAt(vec3(2.5f, 1.25f, cameraZ), vec3(2.5f, 1.25f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);

	// 3 Directional Light Uniforms
	// Will keep these here for reference
	//prog.setUniform("Lights[0].La", vec3(0.0f, 0.0f, 0.1f));
	//prog.setUniform("Lights[1].La", vec3(0.0f, 0.1f, 0.0f));
	//prog.setUniform("Lights[2].La", vec3(0.1f, 0.0f, 0.0f));

	//prog.setUniform("Lights[0].Ld", vec3(0.0f, 0.0f, 1.8f));
	//prog.setUniform("Lights[1].Ld", vec3(0.0f, 1.8f, 0.0f));
	//prog.setUniform("Lights[2].Ld", vec3(1.8f, 0.0f, 0.0f));

	//prog.setUniform("Lights[0].Ls", vec3(0.0f, 0.0f, 0.8f));
	//prog.setUniform("Lights[1].Ls", vec3(0.0f, 0.8f, 0.0f));
	//prog.setUniform("Lights[2].Ls", vec3(0.8f, 0.0f, 0.0f));
	//

	prog.setUniform("MainLight.Ls", vec3(0.8f));
	prog.setUniform("MainLight.Ld", vec3(0.8f));
	prog.setUniform("MainLight.La", vec3(0.2f));

	//loading separate textures
	brickTex = Texture::loadTexture("media/texture/brick1.jpg");
	mossTex = Texture::loadTexture("media/texture/moss.png");
	woodTex = Texture::loadTexture("media/texture/hardwood2_diffuse.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mossTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, woodTex);
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

	if (cameraZ < 2.5f && !movingForward) {
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

	vec4 lightPos = vec4(10.0f * cos(angle), 8.0f, 10.0f * sin(angle), 1.0f);
	prog.setUniform("MainLight.Position", vec4(view * lightPos));
	mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));

	setDiffuseAmbientSpecular("Material", 0.7f, 0.95f, 0.2f);
	prog.setUniform("Material.Shininess", 100.0f);
	prog.setUniform("TexIndex", 0);

	//Creates two walls of blocks with varying vertex x positions
	createBlockWall(mat4(1.0), 5, 5, 0.0f);
	createBlockWall(mat4(1.0), 5, 5, 5.0f);

	setDiffuseAmbientSpecular("Material", 0.7f, 0.9f, 0.2f);
	prog.setUniform("Material.Shininess", 180.0f);
	prog.setUniform("TexIndex", 1);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
	setMatrices();
	plane.render();
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

void SceneBasic_Uniform::createBlockWall(mat4 initialModel, int length, int height, float xPos)
{
	for (int z = 0; z < length; z++)
	{
		for (int y = 0; y < height; y++)
		{
			model = glm::translate(initialModel, vec3(xPos, y * 1.0f, z * -1.0f));
			setMatrices();
			cube.render();
		}
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
	else if (structure == "MainLight")
	{
		prog.setUniform("MainLight.Ld", vec3(dif));
		prog.setUniform("MainLight.La", vec3(amb));
		prog.setUniform("MainLight.Ls", vec3(spec));
	}
}

void SetupTeapotUniform() 
{
	/*prog.setUniform("Light.Direction", normalMatrix * vec3(-lightPos));
	float specularScalar = 0.3f;
	prog.setUniform("Material.Kd", vec3(0.2f, 0.5f, 0.8f));
	prog.setUniform("Material.Ka", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Ks", vec3(0.2f * specularScalar, 0.55f * specularScalar, 0.9f * specularScalar));
	prog.setUniform("Material.Shininess", 100.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 0.0f, -2.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	teapot.render();*/
}
