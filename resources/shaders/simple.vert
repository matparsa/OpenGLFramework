#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

uniform vec3 ColorVector;
uniform vec3 LightSource;

out vec3 pass_Normal;
out vec3 pass_Color;
out vec3 pass_LightVec;
out vec3 pass_ViewerVec;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
    //calculating vertex position in View Space and pass to Fragment Shader
	vec3 position = vec3((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0f));
	// vectors need to be normalized
  	pass_LightVec = normalize(LightSource - position.xyz);
  	pass_ViewerVec = normalize(- position.xyz);
	pass_Color  = ColorVector;
}
