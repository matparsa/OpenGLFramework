#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVec;
in  vec3 pass_ViewerVec;

out vec4 out_Color;
//Got the Idea from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model
const float ka = 0.7f;
const float kd = 0.35f;
const float ks = 0.8f;

const vec3 ambientColor = vec3(0.8f, 0.8f, 0.8f);
const vec3 diffuseColor = vec3(0.5f,0.5f,0.5f);
const vec3 specColor = vec3(1.1f, 1.1f, 1.1f);
const float shininess = 1.8f;
void main() {


    vec3 h = normalize(pass_LightVec.xyz + pass_ViewerVec);
    vec3 normal = normalize(pass_Normal);
    // Ambient Light
    vec3 ambient = ka * ambientColor * pass_Color;

    // Defuse Light
    vec3 diffuse = kd * diffuseColor * pass_Color;

    // Specular Light
    vec3 specular = ks * specColor;

    vec3 result = vec3(ambient+ diffuse * dot(pass_LightVec.rgb, normal)+
    				specular* pow(dot(normal,h),shininess));

    out_Color = vec4(result,1.0f);
}
