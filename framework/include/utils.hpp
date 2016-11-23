#ifndef UTILS_HPP
#define UTILS_HPP

#include <glbinding/gl/types.h>
// use gl definitions from glbinding
using namespace gl;

struct pixel_data;
struct texture_object;

namespace utils {
  // generate texture object from texture struct
  texture_object create_texture_object(pixel_data const& tex,int index);
  // print bound textures for all texture units
  void print_bound_textures();

  // get uniform location, throwing exception if name describes no active uniform variable
  GLint glGetUniformLocation(GLuint, const GLchar*);

  // test program for drawing validity
  void validate_program(GLuint program);

  // return handle of bound vertex array object
  GLint get_bound_VAO();

  // extract filename from path
  std::string file_name(std::string const& file_path);
  // output a gl error log in cerr
  void output_log(GLchar const* log_buffer, std::string const& prefix);
  // read file and write content to string
  std::string read_file(std::string const& name);
}

#endif
