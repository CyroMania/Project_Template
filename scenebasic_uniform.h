#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/frustum.h"
#include "helper/teapot.h"
#include "helper/torus.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog, solidProg;
    unsigned int shadowFBO, pass1Index, pass2Index;

    Teapot teapot;
    Plane plane;
    Torus torus;

    int shadowMapWidth, shadowMapHeight;
    float tPrev;

    glm::mat4 lightPV, shadowBias;

    Frustum lightFrustum;
    float angle;

    void compile();
    void setMatrices();
    void setupFBO();
    void drawScene();
    void spitOutDepthBuffer();
    
public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
