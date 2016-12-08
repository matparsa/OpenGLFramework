#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_TexCoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 pass_TexCoord;

void main(void)
{
    vec4 p = vec4(in_Position, 1.0f);
	gl_Position = p;
    pass_TexCoord = in_TexCoord;
}