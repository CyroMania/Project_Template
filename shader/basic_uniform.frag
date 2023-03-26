#version 440

in vec4 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform struct SpotlightInfo {
    vec4 Position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;

const int level = 5;
const float scaleFactor = 1.0/level;

uniform struct MaterialInfo {
    vec3 Ka; //ambient
    vec3 Kd; //diffuse
    vec3 Ks; //specular
    float Shininess; //shininess factor
} Material; 

vec3 blinnPhongSpot(vec3 n, vec4 pos) {

    vec3 ambient = Spot.La * Material.Ka;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    vec3 s = normalize(vec3(Spot.Position - (pos*Spot.Position.w)));

    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    if (angle<Spot.Cutoff) {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max(dot(s,n),0.0);
        diffuse = Spot.Ld * Material.Kd * (floor(sDotN * level) * scaleFactor);
        specular = vec3(0.0);
        if (sDotN > 0.0) {
            vec3 v = normalize(-pos.xyz);
            vec3 h = normalize(v + s);
            specular = Spot.Ls * Material.Ks * pow(max(dot(h,n),0.0),Material.Shininess);
        }
    }

    return ambient+diffuse+specular;
}

void main() {
    FragColor = vec4(blinnPhongSpot(Normal,Position), 1.0);
}
