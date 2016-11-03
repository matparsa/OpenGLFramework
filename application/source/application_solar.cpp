#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>

model planet_model{};
model star_model{};

model_object star_object;

int numberOfStars=2000;
// initializing of planets
 /*name{n},size{s},color{1.0f, 1.0f, 1.0f},rot_speed{r},dist2origin{d},moon{m} {};*/
ApplicationSolar::planet sun     {"sun"    , 0.7f,  {1.0f, 0.8f , 0.0f}, 0.0f,         0.0f,       };
ApplicationSolar::planet mercury {"mercury", 0.05f,  {0.5f, 0.5f , 0.5f}, 365/88.0f,    15.0f,     };
ApplicationSolar::planet venus   {"venus"  , 0.2f,  {0.5f, 0.5f , 0.5f}, 365/225.0f,   18.0f, true };
ApplicationSolar::planet earth   {"earth"  , 0.15f, {0.1f, 0.4f , 0.7f}, 1.0f,         21.0f, true };
ApplicationSolar::planet mars    {"mars"   , 0.1f,  {0.8f, 0.6f , 0.3f}, 365/687.0f,   26.0f, true };
ApplicationSolar::planet jupiter {"jupiter", 0.35f, {0.8f, 0.8f , 0.5f}, 365/4329.f,   31.0f,      };
ApplicationSolar::planet saturn  {"saturn" , 0.2f,  {0.9f, 0.7f , 0.5f}, 365/1751.0f,  36.0f, true };
ApplicationSolar::planet uranus  {"uranus" , 0.2f,  {0.5f, 1.0f , 1.0f}, 365/30664.0f, 40.0f,      };


//container for all planets
std::vector<ApplicationSolar::planet> planetVector = {sun,earth,mercury,venus, mars, jupiter, saturn, uranus};


// initializing of stars

std::random_device rd;
//container for all star coordinates
std::vector<float> stars;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
{

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(-2.0f, 2.0f);
  std::uniform_real_distribution<float> colorDis(0.0, 1.0);
  for(int i= 0; i< numberOfStars; i++){
		//Position
        stars.push_back( dis(gen) );
        stars.push_back( dis(gen) );
        stars.push_back( dis(gen) );
		//Color
        stars.push_back( colorDis(gen) );
        stars.push_back( colorDis(gen) );
        stars.push_back( colorDis(gen));
  }

  planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
  star_model  = {stars, model::POSITION | model::NORMAL};

  initializeGeometry();
  initializeShaderPrograms();

}

void ApplicationSolar::upload_planet_transforms(planet  &p) const{

   // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);

  p.model = glm::rotate(glm::fmat4{}, float(glfwGetTime() * p.rot_speed), glm::fvec3{0.0f, 1.0f, 0.0f});

  p.model = glm::scale(p.model, glm::vec3(p.size));

  p.model = glm::translate(p.model, glm::fvec3{p.dist2origin,0.0f, -1.0f}); // here change for distance and size

  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(p.model));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * p.model);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);



  if(p.moon)
  {
       // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);


    glm::fmat4 model_matrix = glm::rotate(p.model,  float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix,  glm::fvec3{1.5f,0.0f, -1.0f});
    model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }

}


void ApplicationSolar::render() const {
 // draw stars
  glUseProgram(m_shaders.at("star").handle);
  glBindVertexArray(star_object.vertex_AO);
  glDrawArrays(GL_POINTS, 0, numberOfStars);
  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  for(auto & planet : planetVector) {

    upload_planet_transforms(planet);

  }

//


}


void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);

  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle); //back to the planets
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("star").handle);

  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("planet").handle); //back to the planets

}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();

  // bind new shader
  glUseProgram(m_shaders.at("planet").handle);

  updateView();
  updateProjection();

}




// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {

  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    updateView();
  }
  else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{1.0f, 0.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{-1.0f, 0.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_UP && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -1.0f, 0.0f});
    updateView();
  }
  else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 1.0f, 0.0f});
    updateView();
  }
  else if(key == GLFW_KEY_X && action == GLFW_PRESS){
    m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, -1.0f, 0.0f});
    updateView();
  }
  else if(key == GLFW_KEY_V && action == GLFW_PRESS){
    m_view_transform = glm::rotate(m_view_transform, 0.05f, glm::fvec3{0.0f, 1.0f, 0.0f});
    updateView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

  //zooming in by moving the mouse up
  if ( pos_x>0 && pos_y <0) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    updateView();
  }
  // zooming out by moving the mouse down
  else if ( pos_x<0 && pos_y >0){

    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    updateView();
  }
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  // store Star shader program objects in container
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
                                           m_resource_path + "shaders/stars.frag"});
    m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry()
{
  planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);


    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

     // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planet_model.indices.size());




    //##star

	glGenVertexArrays(1, &star_object.vertex_AO);
	//bind it to VAO
	glBindVertexArray(star_object.vertex_AO);

	//generate a new VertexBufferObject
	glGenBuffers(1, &star_object.vertex_BO );
	//bind it to VBO
	glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_model.data.size(), star_model.data.data(), GL_STATIC_DRAW);

    // active first attribute
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::NORMAL]);

    // generate generic buffer
    glGenBuffers(1, &star_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * star_model.indices.size(), star_model.indices.data(), GL_STATIC_DRAW);

    // store type of primitive to draw
    star_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    star_object.num_elements = GLsizei(star_model.indices.size());





	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);


  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}
