#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVector;
in  vec3 pass_ViewerVector;

uniform  sampler2D ColorTex;

in vec2 pass_TexCoord;
out vec4 out_Color;

void main() {

    vec4 TexColor = texture(ColorTex, pass_TexCoord);
    vec3 h = normalize(pass_LightVector.xyz + pass_ViewerVector);
    vec3 normal = normalize(pass_Normal);

    // k_d + k_s <=1, k_a <= 1
    float ka = 0.3;
    float kd = 0.3;
    float ks = 0.7;
    
    vec3 ia = vec3(0.3f, 0.3f, 0.3f);
    vec3 id = vec3(0.3f, 0.3f, 0.3f);
    vec3 is = vec3(0.7f, 0.7f, 0.7f);
    
    //shininess
    float b = 2.0f;
    
    // Ambient Light
    vec3 ambient = ka * ia+TexColor.rgb;

    // Defuse Light
    vec3 diffuse = kd * id+ TexColor.rgb;

    // Specular Light
    vec3 specular = ks * is;

    vec3 result = vec3(ambient+ diffuse * dot(pass_LightVector.rgb, normal)+ 
                    specular* pow(dot(normal,h),b));

    out_Color = vec4(result,1.0f);

    //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}