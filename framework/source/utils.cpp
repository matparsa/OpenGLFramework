#include "utils.hpp"
#include "pixel_data.hpp"
#include "structs.hpp"

#include <glbinding/gl/functions.h>
// use gl definitions from glbinding 
using namespace gl;

#include <iostream>
#include <sstream>
#include <fstream>

namespace utils {

texture_object create_texture_object(pixel_data const& tex) {

  texture_object t_obj{};
  t_obj.target = GL_TEXTURE_2D;
  
  glGenTextures(1, &t_obj.handle);
  glBindTexture(t_obj.target, t_obj.handle);

  glTexParameteri(t_obj.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(t_obj.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  GLenum internal_format = GL_NONE;
  if (tex.channels == GL_RED) {
    internal_format = GL_R8;
  }
  else if (tex.channels == GL_RG) {
    internal_format = GL_RG8;
  }
  else if (tex.channels == GL_RGB) {
    internal_format = GL_RGB8;
  }
  else if (tex.channels == GL_RGBA) {
    internal_format = GL_RGBA8;
  }
    glTexImage2D(t_obj.target, 0, GLint(internal_format), tex.width, 
      tex.height, 0, tex.channels, tex.channel_type, tex.ptr());
  return t_obj;
}

void print_bound_textures() {
  GLint id1, id2, id3, active_unit, texture_units = 0;
  glGetIntegerv(GL_ACTIVE_TEXTURE, &active_unit);
  std::cout << "Active texture unit: " << active_unit - GLint(GL_TEXTURE0) << std::endl;

  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

  for(GLint i = 0; i < texture_units; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glGetIntegerv(GL_TEXTURE_BINDING_3D, &id3);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &id2);
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &id1);
    if(id1 != 0 || id2 != 0 || id3 != 0) {
      std::cout <<"Texture unit " << i << " - ";
      if(id1 != 0) std::cout << "1D: " << id1 << ", ";
      if(id2 != 0) std::cout << "2D: " << id2 << ", ";
      if(id3 != 0) std::cout << "3D: " << id3;
      std::cout << std::endl;
    }
  }
  // reactivate previously active unit
  glActiveTexture(GLenum(active_unit));
}

GLint glGetUniformLocation(GLuint program, const GLchar* name) {
  // use function from outer namespace to prevent recursion
  GLint loc = ::glGetUniformLocation(program, name);
  // if location invalid, output info similar to gl errors
  if (loc == -1) {
    std::cerr <<  "OpenGL Error: " << "glGetUniformLocation" << "(";
    std::cerr << program << ", " << name << ")";
    std::cerr << " -> " << loc;
    std::cerr  << ", " << name <<"  is not an active uniform variable in program" << std::endl;
    // dont throw, allow retrying
    // throw std::runtime_error("Execution of " + std::string("glGetUniformLocation"));
    // exit(EXIT_FAILURE);
  }
  return loc;
}

void validate_program(GLuint program) {
  glValidateProgram(program);
  // check if validation was successfull
  GLint success = 0;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
  if(success == 0) {
    // get log length
    GLint log_size = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    // get log
    GLchar* log_buffer = (GLchar*)malloc(sizeof(GLchar) * log_size);
    glGetProgramInfoLog(program, log_size, &log_size, log_buffer);
    // output errors
    output_log(log_buffer, "program nr. " + std::to_string(program));
    // free broken program
    glDeleteProgram(program);
    free(log_buffer);

    throw std::logic_error("Validation of program nr. " + std::to_string(program));
  }
}

GLint get_bound_VAO() {
  GLint array = -1;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &array);

  return array;
}

std::string file_name(std::string const& file_path) {
  return file_path.substr(file_path.find_last_of("/\\") + 1);
}

void output_log(GLchar const* log_buffer, std::string const& prefix) {
  std::string error{};
  std::istringstream error_stream{log_buffer};
  while(std::getline(error_stream, error)) {
    std::cerr << prefix << " - " << error << std::endl;
  }
}

std::string read_file(std::string const& name) {
  std::ifstream file_in{name};
  if(file_in) {
    std::string contents;
    file_in.seekg(0, std::ios::end);
    contents.resize(file_in.tellg());
    file_in.seekg(0, std::ios::beg);
    file_in.read(&contents[0], contents.size());
    file_in.close();
    return(contents);
  }
  else {
    std::cerr << "File \'" << name << "\' not found" << std::endl;
    
    throw std::invalid_argument(name);
  } 
}

};