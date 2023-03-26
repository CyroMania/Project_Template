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

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

//SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}
//SceneBasic_Uniform::SceneBasic_Uniform() : teapot(50, glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))) {}
//SceneBasic_Uniform::SceneBasic_Uniform() : plane(50.0f, 50.0f, 1, 1)
SceneBasic_Uniform::SceneBasic_Uniform() : plane(50.0f, 50.0f, 1, 1), teapot(14, glm::mat4(1.0f)), torus(1.75f, 1.0f, 50, 50)
{
	//mesh = ObjMesh::load("../Project_Template/media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
	compile();

	glEnable(GL_DEPTH_TEST);
	//model = mat4(1.0f);
	//model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection = mat4(1.0f);
	prog.setUniform("Spot.Ld", vec3(0.9f));
	prog.setUniform("Spot.Ls", vec3(0.9f));
	prog.setUniform("Spot.La", vec3(0.5f));
	prog.setUniform("Spot.Exponent", 50.0f);
	prog.setUniform("Spot.Cutoff", glm::radians(15.0f));
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
	//model = glm::rotate(model, glm::radians(-1.0f), vec3(1.0f, 1.0f, 1.0f));
}

void SceneBasic_Uniform::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
	prog.setUniform("Spot.Position", vec4(view * lightPos));
	mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
	prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

	prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
	prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
	prog.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);
	prog.setUniform("Material.Shininess", 100.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 0.0f, -2.0f));
	model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	teapot.render();

	prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
	prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f); 
	prog.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);
	prog.setUniform("Material.Shininess", 100.0f);

	model = mat4(1.0f);
	model = glm::translate(model, vec3(-1.0f, 0.75f, 3.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	torus.render();

	prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Material.Shininess", 180.0f);

	model = mat4(1.0f);
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
