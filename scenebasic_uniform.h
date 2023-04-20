#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/plane.h"
#include "helper/objmesh.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    Plane plane;
    std::unique_ptr<ObjMesh> wall;
    std::unique_ptr<ObjMesh> bucket;

    unsigned int stoneTex;
    unsigned int mossTex;
    unsigned int metalTex;
    unsigned int grassTex;

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
    void RenderBuckets(int);
    void setDiffuseAmbientSpecular(std::string structure, float dif, float amb, float spec);
};

#endif // SCENEBASIC_UNIFORM_H
