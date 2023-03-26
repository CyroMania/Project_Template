#version 440

layout (binding = 0) uniform sampler2D Tex1;
layout (location = 0) out vec4 FragColor;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

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

vec3 phong(vec3 n, vec4 pos) {

    vec3 ambient = Light.La * Material.Ka;
    vec3 s = normalize(vec3(Light.Position-pos*Light.Position.w));

    float sDotN = max(dot(s,n),0.0);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect(-s,n);
        specular = Light.Ls * Material.Ks * pow(max(dot(r,v),0.0),Material.Shininess);
    }

    return ambient+diffuse+specular;
}

vec3 blinnPhong(vec3 n, vec4 pos) {
    vec3 texColour = texture(Tex1,TexCoord).rgb;

    vec3 ambient = Light.La * Material.Ka;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 s = normalize(vec3(Light.Position-(pos*Light.Position.w)));

    float sDotN = max(dot(s,n),0.0);
    diffuse = Light.Ld * Material.Kd * sDotN * texColour;

    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 h = normalize(v + s);
        specular = Light.Ls * Material.Ks * pow(max(dot(h,n),0.0),Material.Shininess);
    }

    return ambient + diffuse + specular;
}

void main() {
    FragColor = vec4(blinnPhong(Normal, Position), 1.0f);
}
