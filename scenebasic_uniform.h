#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    Cube cube;
    Plane plane;
    Teapot teapot;

    unsigned int brickTex;
    unsigned int mossTex;
    unsigned int woodTex;
    unsigned int cementTex;

    float cameraZ;
    float angle;
    float tPrev;

    bool movingForward;

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
    void setMatrices();
    void createBlockWall(glm::mat4, int, int, float);
    void setDiffuseAmbientSpecular(std::string structure, float dif, float amb, float spec);
};

#endif // SCENEBASIC_UNIFORM_H
