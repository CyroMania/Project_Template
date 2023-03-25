#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //ambient
    vec3 Ld; //diffuse
    vec3 Ls; //specular
} Light;

uniform struct MaterialInfo {
    vec3 Ka; //ambient
    vec3 Kd; //diffuse
    vec3 Ks; //specular
    float Shininess; //shininess factor
} Material; 

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

vec3 phong(vec3 n, vec4 pos) {

    vec3 ambient = Light.La * Material.Ka;

    vec3 s = normalize(vec3(Light.Position-pos*Light.Position.w));
    float sDotN = max(dot(s,n),0.0);
    vec3 diffuse = Light.Ld * Material.Kd*sDotN;
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect(-s, n);
        specular = Light.Ls * Material.Ks * pow(max(dot(r,v),0.0),Material.Shininess);
    }

    return ambient+diffuse+specular;
}

void getCamSpaceValues(out vec3 norm, out vec3 position) {
    norm = normalize(NormalMatrix * VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
}

void main()
{
    vec3 camNorm,camPosition; 
    getCamSpaceValues(camNorm,camPosition);

    LightIntensity = phong(camNorm,vec4(camPosition,1.0));

    gl_Position = MVP*vec4(VertexPosition, 1.0);
}
