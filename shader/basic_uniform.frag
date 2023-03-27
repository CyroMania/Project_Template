#version 440

layout (location = 0) out vec4 FragColour;

layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D Tex2;
layout (binding = 2) uniform sampler2D Tex3;
layout (binding = 3) uniform sampler2D Tex4;

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

uniform struct MaterialInfo {
    vec3 Ka; //ambient
    vec3 Kd; //diffuse
    vec3 Ks; //specular
    float Shininess; //shininess factor
} Material;

uniform struct FogInfo {
    float MaxDist;
    float MinDist;
    vec3 Colour;
} Fog;

vec3 blinnPhong(vec4 pos, vec3 n) {
    vec3 mixedColour = vec3(0.0);

    if (TexIndex == 0) {
        vec4 brickColour = texture(Tex1, TexCoord);
        vec4 mossColour = texture(Tex2, TexCoord);
        mixedColour = mix(brickColour.rgb, mossColour.rgb, mossColour.a);
    }
    else  if (TexIndex == 1) {
        vec4 woodColour = texture(Tex3, TexCoord);
        mixedColour = woodColour.rgb;
    }
    else {
        vec4 cementColour = texture(Tex4, TexCoord);
        mixedColour = cementColour.rgb;
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
    float dist = abs( Position.z ); //distance calculations

    //fogFactor calculation based on the formula presented earlier
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    fogFactor = clamp( fogFactor, 0.0, 1.0 ); //we clamp values

    //colour we receive from blinnPhong calculation
    vec3 shadeColour = blinnPhong(Position, normalize(Normal));

    //we assign a colour based on the fogFactor using mix
    vec3 Colour = mix( Fog.Colour, shadeColour, fogFactor );
    FragColour = vec4(Colour, 1.0); //final colour
}
