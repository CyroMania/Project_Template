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
using glm::mat4;

//SceneBasic_Uniform::SceneBasic_Uniform() : torus(0.7f, 0.3f, 30, 30) {}
SceneBasic_Uniform::SceneBasic_Uniform() : teapot(50, glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))) {}

void SceneBasic_Uniform::initScene()
{
	compile();

	glEnable(GL_DEPTH_TEST);

	model = mat4(1.0f);
	model = glm::rotate(model, glm::radians(270.0f), vec3(1.0f, 0.0f, 0.0f));
	view = glm::lookAt(vec3(-1.0f, 5.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.1f, 0.0f));

	projection = mat4(1.0f);
	float x, z;
	for (int i = 0; i < 3; i++)
	{
		std::stringstream name;
		name << "Lights[" << i << "].Position";
		x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
		z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
		prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
	}

	// 3 Directional Light Uniforms
	prog.setUniform("Lights[0].La", vec3(0.0f, 0.0f, 0.1f));
	prog.setUniform("Lights[1].La", vec3(0.0f, 0.1f, 0.0f));
	prog.setUniform("Lights[2].La", vec3(0.1f, 0.0f, 0.0f));

	prog.setUniform("Lights[0].Ld", vec3(0.0f, 0.0f, 1.8f));
	prog.setUniform("Lights[1].Ld", vec3(0.0f, 1.8f, 0.0f));
	prog.setUniform("Lights[2].Ld", vec3(1.8f, 0.0f, 0.0f));

	prog.setUniform("Lights[0].Ls", vec3(0.0f, 0.0f, 0.8f));
	prog.setUniform("Lights[1].Ls", vec3(0.0f, 0.8f, 0.0f));
	prog.setUniform("Lights[2].Ls", vec3(0.8f, 0.0f, 0.0f));
	//

	prog.setUniform("Material.Kd", vec3(0.1f, 0.1f, 0.1f));
	prog.setUniform("Material.Ka", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Ks", vec3(0.1f, 0.1f, 0.1f));
	prog.setUniform("Material.Shininess", 180.0f);
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
	GlCall(glClear(GL_COLOR_BUFFER_BIT));
	GlCall(glClear(GL_DEPTH_BUFFER_BIT));

	setMatrices();
	teapot.render();
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
}
