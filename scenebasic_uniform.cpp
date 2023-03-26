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
SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), plane(50.0f, 50.0f, 1, 1), teapot(14, glm::mat4(1.0f))
{
	//mesh = ObjMesh::load("../Project_Template/media/pig_triangulated.obj", true);
}

void SceneBasic_Uniform::initScene()
{
	angle = 0.0;
	compile();
	glEnable(GL_DEPTH_TEST);
	view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
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

	prog.setUniform("Light.Ls", vec3(0.9f));
	prog.setUniform("Light.Ld", vec3(0.9f));
	prog.setUniform("Light.La", vec3(0.2f));
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
}

void SceneBasic_Uniform::render()
{
	GlCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
	prog.setUniform("Light.Position", vec4(view * lightPos));
	mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
	prog.setUniform("Light.Direction", normalMatrix * vec3(-lightPos));
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
	teapot.render();

	prog.setUniform("Material.Kd", vec3(0.7f, 0.7f, 0.7f));
	prog.setUniform("Material.Ka", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Ks", vec3(0.2f, 0.2f, 0.2f));
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
