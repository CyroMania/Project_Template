#version 440

in vec4 Position;
in vec3 Normal;
layout (location = 0) out vec4 FragColor;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //ambient
    vec3 Ld; //diffuse
    vec3 Ls; //specular
} Lights[3];

uniform struct MaterialInfo {
    vec3 Ka; //ambient
    vec3 Kd; //diffuse
    vec3 Ks; //specular
    float Shininess; //shininess factor
} Material; 

vec3 phong(int light, vec3 n, vec4 pos) {

    vec3 ambient = Lights[light].La * Material.Ka;

    vec3 s = normalize(vec3(Lights[light].Position-pos*Lights[light].Position.w));
    float sDotN = max(dot(s,n),0.0);
    vec3 diffuse = Lights[light].Ld * Material.Kd*sDotN;
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect(-s,n);
        specular = Lights[light].Ls * Material.Ks * pow(max(dot(r,v),0.0),Material.Shininess);
    }

    return ambient+diffuse+specular;
}

vec3 blinnPhong(int light, vec3 n, vec4 pos) {

    vec3 ambient = Lights[light].La * Material.Ka;

    vec3 s = normalize(vec3(Lights[light].Position-pos*Lights[light].Position.w));
    float sDotN = max(dot(s,n),0.0);
    vec3 diffuse = Lights[light].Ld * Material.Kd*sDotN;
    vec3 specular = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 h = normalize(v + s);
        specular = Lights[light].Ls * Material.Ks * pow(max(dot(h,v),0.0),Material.Shininess);
    }

    return ambient+diffuse+specular;
}

void main() {
    vec3 Colour = vec3(0.0);
    for(int i = 0;i<3;i++) { 
        Colour += blinnPhong(i, Normal, Position);
    }
    FragColor = vec4(Colour, 1.0);
}
