#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

uniform vec3 ColorVector;
uniform vec3 LightSource;

out vec3 pass_Normal;
out vec3 pass_Color;
out vec3 pass_LightVector;
out vec3 pass_ViewerVector;
out vec2 pass_TexCoord;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;

	vec3 planet_Position = vec3((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0f));
  	pass_LightVector = normalize(LightSource - planet_Position.xyz);
  	pass_ViewerVector = normalize(- planet_Position.xyz);

	pass_Color  = ColorVector;
	pass_TexCoord = in_TexCoord;
}
