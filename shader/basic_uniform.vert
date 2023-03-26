#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //ambient
    vec3 Ld; //diffuse
    vec3 Ls; //specular
} Light;

//out vec4 Position;
//out vec3 Normal;

out vec2 TexCoord;
out vec3 LightDir;
out vec3 ViewDir;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

void getCamSpaceValues(out vec3 norm, out vec4 position) {
    norm = normalize(NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition, 1.0));
}

void main()
{
    //Transform normal and tangent to eye space
    vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));

    // Compute the binormal
    vec3 binormal = normalize(cross(norm, tang)) * VertexTangent.w;

    // Matrix for transformation to tangent space
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z);

    //Transform the Light direction and View direction to tangent space
    vec3 pos = vec3(ModelViewMatrix * vec4(VertexPosition,1.0));
    LightDir = toObjectLocal * (Light.Position.xyz - pos);
    ViewDir = toObjectLocal * normalize(-pos);

    TexCoord = VertexTexCoord;
    gl_Position = MVP*vec4(VertexPosition, 1.0);
}