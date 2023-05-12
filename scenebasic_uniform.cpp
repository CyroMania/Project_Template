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

SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0.0f),
											shadowMapWidth(512), shadowMapHeight(512),
											teapot(14, glm::mat4(1.0f)), plane(40.0f, 40.0f, 2, 2),
											torus(0.7f * 2.0f, 0.3f * 2.0f, 50, 50)
{
	//
}

void SceneBasic_Uniform::initScene()
{
	compile();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	angle = glm::quarter_pi<float>();

	// Setup the framebuffer object
	setupFBO();

	unsigned int programHandle = prog.getHandle();
	pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f));

	float c = 1.65f;
	vec3 lightPos = vec3(0.0f, c * 5.25f, c * 7.5f); // World Coords
	lightFrustum.orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	lightFrustum.setPerspective(50.0f, 1.0f, 1.0f, 25.0f);
	lightPV = shadowBias * lightFrustum.getProjectionMatrix() * lightFrustum.getViewMatrix();

	prog.setUniform("Light.Intensity", vec3(0.85f));
	prog.setUniform("ShadowMap", 0);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();

		//use when rendering LightFrustum

		solidProg.compileShader("shader/solid.vs", GLSLShader::VERTEX);
		solidProg.compileShader("shader/solid.fs", GLSLShader::FRAGMENT);
		solidProg.link();
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
	angle += 0.2f * deltaT;

	if (angle > glm::two_pi<float>()) 
	{
		angle -= glm::two_pi<float>();
	}
}

void SceneBasic_Uniform::render()
{
	prog.use();
	//Pass 1 (shadow map generation)
	view = lightFrustum.getViewMatrix();
	projection = lightFrustum.getProjectionMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.5f, 10.0f);
	drawScene();
	glCullFace(GL_BACK);
	glFlush();
	//spitOutDepthBuffer();

	//Pass 2 (render)
	float c = 2.0f;
	vec3 cameraPos(c * 11.5f * cos(angle), c * 7.0f, c * 11.5f * sin(angle));
	view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
	prog.setUniform("Light.Position", view * vec4(lightFrustum.getOrigin(), 1.0f));
	projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
	drawScene();

	//draw the light's frustum

	solidProg.use();
	solidProg.setUniform("Colour", vec4(1.0f, 0.0f, 0.0f, 1.0f));
	mat4 mv = view * lightFrustum.getInverseViewMatrix();
	solidProg.setUniform("MVP", projection * mv);
	lightFrustum.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
	//setup the viewport and the projection matrix
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	//projection = glm::perspective(glm::radians(70.0f), (float)w / h,
	//	0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
	prog.setUniform("ShadowMatrix", lightPV * model);
}

void SceneBasic_Uniform::setupFBO()
{
	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	// The depth buffer texture
	unsigned int depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	//Assign the depth buffer texture to texture channel 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTex);

	//Create and setup the FBO
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is complete.\n");
	}
	else {
		printf("Framebuffer is not complete.\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::drawScene()
{
	vec3 colour = vec3(0.2f, 0.5f, 0.9f);
	prog.setUniform("Material.Ka", colour * 0.05f);
	prog.setUniform("Material.Kd", colour);
	prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Shininess", 150.0f);
	model = mat4(1.0f);
	model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	teapot.render();

	prog.setUniform("Material.Ka", colour * 0.05f);
	prog.setUniform("Material.Kd", colour);
	prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
	prog.setUniform("Material.Shininess", 150.0f);
	model = mat4(1.0f);
	model = glm::translate(model, vec3(0.0f, 2.0f, 5.0f));
	model = glm::rotate(model, glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	torus.render();

	prog.setUniform("Material.Kd", 0.25f, 0.25f, 0.25f);
	prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
	prog.setUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
	prog.setUniform("Material.Shininess", 1.0f);
	model = mat4(1.0f);
	setMatrices();
	plane.render();
	model = mat4(1.0f);
	model = glm::translate(model, vec3(-5.0f, 5.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
	setMatrices();
	plane.render();
	model = glm::translate(model, vec3(0.0f, 5.0f, -5.0f));
	model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	plane.render();
	model = mat4(1.0f);

}

void SceneBasic_Uniform::spitOutDepthBuffer()
{
}
