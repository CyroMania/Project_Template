#version 440

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D Tex2;
layout (binding = 2) uniform sampler2D Tex3;

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform int TexIndex;

uniform struct LightInfo {
    vec4 Position;
    vec3 La; //ambient
    vec3 Ld; //diffuse
    vec3 Ls; //specular
} MainLight;

uniform LightInfo DirLights[3];

uniform struct MaterialInfo {
    vec3 Ka; //ambient
    vec3 Kd; //diffuse
    vec3 Ks; //specular
    float Shininess; //shininess factor
} Material;

vec3 blinnPhong(vec3 n, vec4 pos) {
    vec3 mixedColour = vec3(0.0);

    if (TexIndex == 0) {
        vec4 brickColour = texture(Tex1, TexCoord);
        vec4 mossColour = texture(Tex2, TexCoord);
        mixedColour = mix(brickColour.rgb, mossColour.rgb, mossColour.a);
    }
    else {
        vec4 woodColour = texture(Tex3, TexCoord);
        mixedColour = woodColour.rgb;
    }

    vec3 ambient = MainLight.La * Material.Ka;
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 s = normalize(vec3(MainLight.Position - (pos * MainLight.Position.w)));

    float sDotN = max(dot(s,n),0.0);
    diffuse = MainLight.Ld * Material.Kd * sDotN * mixedColour;

    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 h = normalize(v + s);
        specular = MainLight.Ls * Material.Ks * pow(max(dot(h,n),0.0), Material.Shininess);
    }

    return ambient + diffuse + specular;
}

void main() {
    FragColor = vec4(blinnPhong(normalize(Normal), Position), 1.0f);
}
