#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // defining struct with properties for the planets
  struct planet{

  std::string name;
  float size;
  glm::vec3 color;
  float rot_speed;
  float dist2origin;
  bool moon;

  planet(std::string n, float s, float r, float d, bool m = false):
        name{n},
        size{s},
        color{1.0f, 1.0f, 1.0f},
        rot_speed{r},
        dist2origin{d},
        moon{m} {};

  planet(std::string n, float s,  glm::vec3 c, float r, float d, bool m = false):
        name{n},
        size{s},
        color{c.x, c.y, c.z},
        rot_speed{r},
        dist2origin{d},
        moon{m} {};
};


  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //void glfwGetCursorPos( double xpos, double ypos);
  // draw all objects
  void render() const;
  void upload_planet_transforms(planet  &p,  texture_object const& tex_obj) const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry(model& mdl, model_object& object);
  void updateView();
  //GLuint loadTexture(pixel_data const& tex) const;
  // cpu representation of model
  model_object planet_object;
  model_object star_object;

};

#endif