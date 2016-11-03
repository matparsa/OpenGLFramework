#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Color;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

//out vec4 pass_Color;
out vec3 pass_Color;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix) * vec4(in_Position, 1.0f);
	pass_Color 	= in_Color;
}
