#version 440

const float PI = 3.141592653589793238462;

layout(location = 0) out vec4 FragColour;

in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

uniform struct LightInfo {
    vec4 Position; //light pos in camera coordinates
    vec3 La; //ambient
    vec3 Ld; //diffuse
    vec3 Ls; //specular
    vec3 Intensity;
} Light;

uniform struct MaterialInfo {
    float Rough; //Roughness
    bool Metal; //true if Metallic, or false if dielectric
    vec3 Colour;  //Diffuse colour for dilectrics, f0 for metallic
} Material;

uniform sampler2DShadow ShadowMap;

float ggxDistribution(float nDotH) {
    float alpha2 = pow(Material.Rough, 4.0);
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith(float dotProd) {
    float k = (Material.Rough + 1.0) * (Material.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH) {
    vec3 f0 = vec3(0.04);
    
    if (Material.Metal) {
        f0 = Material.Colour;
    }

    return f0 + (1 - f0) * pow(1.0 - lDotH, 5.0);
}

vec3 blinnPhong(vec3 pos, vec3 n) {
    vec3 ambient = Light.La * Material.Colour;
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, n);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = sDotN * Light.Ld * Material.Colour;
    vec3 spec = vec3(0.0);

    if (sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 h = normalize(v+s);
        spec = (Light.Ls * Material.Colour) * pow(max(dot(h,n), 0.0), Material.Rough);
    }

    return ambient + diffuse + spec;
}

vec3 microfacetModel(vec3 position, vec3 n) {
    vec3 diffuseBrdf = vec3(0.0); //metallic

    if (!Material.Metal) {
        diffuseBrdf = Material.Colour;
    }

    vec3 l = vec3(0.0);
    vec3 lightI = Light.Intensity;

    if (Light.Position.w == 0.0) { //Directional Light
        l = normalize(Light.Position.xyz);
    }
    else {                         //Positional Light
        l = Light.Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l); 
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * schlickFresnel(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotL;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow() 
{
    vec3 ambient = Light.La * Material.Colour;
    vec3 diffAndSpec = blinnPhong(Position, Normal);

    float shadow = 1.0;
    if (ShadowCoord.z >= 0) 
    {
        shadow = textureProj(ShadowMap, ShadowCoord);
    }

    // If the fragment is in shadow, use ambient light only
    FragColour = vec4(diffAndSpec * shadow + ambient, 1.0);

    //Gamma correct
    vec4 shadowFactor = pow(FragColour, vec4(1.0/2.2));

    vec3 sum = vec3(0.0);
    vec3 n = normalize(Normal);
    //for (int i = 0; i < 3; i++) {
    sum = microfacetModel(Position, n);
    //}

    // Gamma
    sum = pow(sum, vec3(1.0/2.2));

    FragColour = vec4(sum * shadowFactor.xyz, 1.0);
}

subroutine (RenderPassType)
void recordDepth()
{
    //Do nothing, will be done automatically
}

void main() 
{
    RenderPass();
}