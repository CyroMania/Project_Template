#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec3 Kd;
uniform vec3 Ld;
uniform vec4 LightPosition;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;


void main()
{
    vec3 n = normalize( NormalMatrix * VertexNormal);
    vec4 pos = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(LightPosition-pos));
    LightIntensity = Kd*Ld*dot(s,n);
    gl_Position = MVP*vec4(VertexPosition, 1.0);
}
