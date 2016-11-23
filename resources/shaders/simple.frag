#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVector;
in  vec3 pass_ViewerVector;

uniform  sampler2D ColorTex;

in vec2 pass_TexCoord;
out vec4 out_Color;

//Got the Idea from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model
const float ka = 0.3f;
const float kd = 0.2f;
const float ks = 0.8f;

const vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);
const vec3 diffuseColor = vec3(0.3f,0.3f,0.3f);
const vec3 specColor = vec3(0.7f, 0.7f, 0.7f);
const float shininess = 2.0f;
void main() {

    vec4 TexColor = texture(ColorTex, pass_TexCoord);
    vec3 h = normalize(pass_LightVector.xyz + pass_ViewerVector);
    vec3 normal = normalize(pass_Normal);


    vec3 ambient = ka * ambientColor+TexColor.rgb;

    // Defuse Light
    vec3 diffuse = kd * diffuseColor+ TexColor.rgb;

    // Specular Light
    vec3 specular = ks * specColor;

    vec3 result = vec3(ambient+ diffuse * dot(pass_LightVector.rgb, normal)+
                    specular* pow(dot(normal,h),shininess));

    out_Color = vec4(result,1.0f);

    //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
