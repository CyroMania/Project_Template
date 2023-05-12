#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/plane.h"
#include "helper/objmesh.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    GLFWwindow* window;
    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    std::unique_ptr<ObjMesh> wall;
    std::unique_ptr<ObjMesh> bucket;

    //unsigned int stoneTex;
    //unsigned int mossTex;
    //unsigned int metalTex;
    //unsigned int grassTex;

    float tPrev, lightAngle, lightRotationSpeed;
    glm::vec4 lightPos;

    float cameraZ;
    //float angle;
    //float tPrev;

    bool movingForward;

    void setMatrices();
    void drawScene();
    void drawFloor();
    void compile();
    void drawSpot(const glm::vec3& pos, float rough, int metal, const glm::vec3& colour);
    void drawWalls(float rough, int metal, const glm::vec3& colour);
    void drawBuckets(int number, float rough, int metal, const glm::vec3& colour);
    void handleInput(GLFWwindow* window);
public:
    SceneBasic_Uniform();

    void initScene(GLFWwindow* window);
    void update( float t );
    void render();
    void resize(int, int);

    //void setDiffuseAmbientSpecular(std::string structure, float dif, float amb, float spec);
};

#endif // SCENEBASIC_UNIFORM_H
