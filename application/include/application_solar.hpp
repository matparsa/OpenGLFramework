#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  struct planet{

  std::string name;
  float size;
  glm::vec3 color;
  float rot_speed;
  float dist2origin;
  bool moon;
  glm::fmat4 model;

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
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;
  void upload_planet_transforms(planet  &p) const;
  void drawOrbit(glm::mat4 model_matrix) const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  // cpu representation of model
  model_object planet_object;
};

#endif
