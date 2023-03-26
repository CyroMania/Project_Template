#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    //Torus torus;
    Teapot teapot;
    Plane plane;
    float angle;
    float tPrev;
    std::unique_ptr<ObjMesh> mesh;
    GLSLProgram prog;
    glm::mat4 Kd;
    glm::mat4 Ld;
    glm::mat4 LightPosition;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void setMatrices();
};

#endif // SCENEBASIC_UNIFORM_H
