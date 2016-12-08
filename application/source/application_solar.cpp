#include "application_solar.hpp"
#include "launcher.hpp"
#include <iostream>
#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"
#include <glbinding/gl/gl.h>
// use gl definitions from glbinding
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <random>

model planet_model{};
model star_model{};
bool celMode;
model_object star_object;
char test='s';
int numberOfStars=2000;

std::map<std::string, texture_object> textureMap;
struct quadOb {
    GLuint vertex_AO = 0;
    GLuint vertex_BO = 0;
    GLuint element_BO = 0;
};

struct fb_texOb {
    GLenum context = GL_TEXTURE0;
    GLenum target = GL_TEXTURE_2D;
    GLuint obj_ptr = 0;
};
texture_object sunTexture;



fb_texOb screenQuadTexture;
quadOb screenQuadObj;
GLuint rb_handle;
GLuint fbo_handle;

bool isFlipV = false;
bool isFlipH = false;
bool isGreyscale = false;
bool isGaussian = false;



// initializing of planets
 /*name{n},size{s},color{1.0f, 1.0f, 1.0f},rot_speed{r},dist2origin{d},moon{m} {};*/
ApplicationSolar::planet sun0     {"sun"    , 0.7f,  {1.0f, 1.0f , 0.0f}, 0.0f,         900.0f,       };
ApplicationSolar::planet sun     {"sun"    , 0.7f,  {1.0f, 1.0f , 0.0f}, 0.0f,         0.0f,       };
ApplicationSolar::planet mercury {"mercury", 0.05f,  {0.5f, 0.5f , 0.5f}, 365/88.0f,    15.0f,     };
ApplicationSolar::planet venus   {"venus"  , 0.2f,  {0.5f, 0.5f , 0.5f}, 365/225.0f,   18.0f, true };
ApplicationSolar::planet earth   {"earth"  , 0.15f, {0.1f, 0.4f , 0.7f}, 1.0f,         21.0f, true };
ApplicationSolar::planet mars    {"mars"   , 0.1f,  {0.8f, 0.6f , 0.3f}, 365/687.0f,   26.0f, true };
ApplicationSolar::planet jupiter {"jupiter", 0.35f, {0.8f, 0.8f , 0.5f}, 365/4329.f,   31.0f,      };
ApplicationSolar::planet saturn  {"saturn" , 0.2f,  {0.9f, 0.7f , 0.5f}, 365/1751.0f,  36.0f, true };
ApplicationSolar::planet uranus  {"uranus" , 0.2f,  {0.5f, 1.0f , 1.0f}, 365/30664.0f, 40.0f,      };
ApplicationSolar::planet galaxy   {"galaxy",    20.0f,  {0.5f, 1.0f , 1.0f}, 0.0f, 1.0f,       };

//ApplicationSolar::planet galaxy   {"galaxy",   0.20f, {1.5f, 1.5f, 1.5f},  0.0f,         0.0f,       };


//container for all planets
std::vector<ApplicationSolar::planet> planetVector = {sun,earth,mercury,venus, mars, jupiter, saturn, uranus,galaxy};


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

    auto sunTexture      = texture_loader::file(m_resource_path+"textures/sun.png");
  auto mercuryTexture  = texture_loader::file( m_resource_path+"textures/mercury.png");
  auto venusTexture    = texture_loader::file(m_resource_path+"textures/venus.png");
  auto earthTexture    = texture_loader::file( m_resource_path+"textures/earth.png");
  auto marsTexture     = texture_loader::file( m_resource_path+"textures/mars.png");
  auto jupiterTexture  = texture_loader::file( m_resource_path+"textures/jupiter.png");
  auto saturnTexture   = texture_loader::file( m_resource_path+"textures/saturn.png");
  auto uranusTexture   = texture_loader::file( m_resource_path+"textures/uranus.png");
  auto galaxyTexture   = texture_loader::file( m_resource_path+"textures/galaxymap.png");

  //
  textureMap = {
          {"sun"      , utils::create_texture_object(sunTexture,0)},
          {"mercury"  , utils::create_texture_object(mercuryTexture,1)},
          {"venus"    , utils::create_texture_object(venusTexture,2)},
          {"earth"    , utils::create_texture_object(earthTexture,3)},
          {"mars"     , utils::create_texture_object(marsTexture,4)},
          {"jupiter"  , utils::create_texture_object(jupiterTexture,5)},
          {"saturn"   , utils::create_texture_object(saturnTexture,6)},
          {"uranus"   , utils::create_texture_object(uranusTexture,7)},
          {"galaxy"   , utils::create_texture_object(galaxyTexture,8)}
  };

  initializeGeometry();
  initializeShaderPrograms();
    initializeScreenquad();
}

void ApplicationSolar::initializeScreenquad(){
    std::vector<GLfloat> vertices {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };

    std::vector<GLuint> indices {
        0, 1, 2,
        0, 2, 3
    };

    auto num_bytes = 5 * sizeof(GLfloat);
    glGenVertexArrays(1, &screenQuadObj.vertex_AO);
    glBindVertexArray(screenQuadObj.vertex_AO);
    glGenBuffers(1, &screenQuadObj.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadObj.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(GLsizei(sizeof(float) * vertices.size())), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    uintptr_t offset0 = 0 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(num_bytes), (const GLvoid*) offset0);
    glEnableVertexAttribArray(1);
    uintptr_t offset1 = 3 * sizeof(GLfloat);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(num_bytes), (const GLvoid*) offset1);

}

void ApplicationSolar::upload_planet_transforms(planet  &p,int index) const{
     //glActiveTexture(GL_TEXTURE0);

  if( p.name=="galaxy")
  {
      glBindTexture(GL_TEXTURE_2D, textureMap.at(p.name).handle);
  glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"), textureMap.at(p.name).handle);

       glm::fmat4 model_matrix = glm::scale(glm::fmat4{}, glm::vec3(p.size));

  //determine the distance of the planets to the origin
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f,0.0f, -1.2f}); // here change for distance and size



  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // Setting the colors for the planets
  glUniform3f(m_shaders.at("planet").u_locs.at("ColorVector"), p.color.x, p.color.y, p.color.z);


  glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"),index);
  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }
  else
  {
      glBindTexture(GL_TEXTURE_2D, textureMap.at(p.name).handle);
  glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"), textureMap.at(p.name).handle);

      glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * p.rot_speed), glm::fvec3{0.0f, 1.0f, 0.0f});

  // set the size for the planets
  model_matrix = glm::scale(model_matrix, glm::vec3(p.size));

  //determine the distance of the planets to the origin
  model_matrix = glm::translate(model_matrix, glm::fvec3{p.dist2origin,0.0f, -1.0f}); // here change for distance and size



  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // Setting the colors for the planets
  glUniform3f(m_shaders.at("planet").u_locs.at("ColorVector"), p.color.x, p.color.y, p.color.z);


  glUniform1i(m_shaders.at("planet").u_locs.at("ColorTex"),index);
  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  if(p.moon)
  {
       // bind shader to upload uniforms
    //glUseProgram(m_shaders.at(shader).handle);


    // Using the already used model_matrix from above instantly
    model_matrix = glm::rotate(model_matrix,  float(glfwGetTime() * 10.0f), glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix,  glm::fvec3{1.5f,0.0f, -1.0f});
    model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));

    glUniform3f(m_shaders.at("planet").u_locs.at("ColorVector"), 1.0f, 1.0f, 0.0f);

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
  }
  }


}


void ApplicationSolar::render() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 // draw stars
  glUseProgram(m_shaders.at("star").handle);
  glBindVertexArray(star_object.vertex_AO);
  glDrawArrays(GL_POINTS, 0, numberOfStars);
  // bind the VAO to draw
  glUseProgram(m_shaders.at("planet").handle);
  glBindVertexArray(planet_object.vertex_AO);
    int i=0;
  for(auto & planet : planetVector) {

    upload_planet_transforms(planet,i);
++i;
  }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw screen squad
    renderScreenQuad();
}

void ApplicationSolar::renderScreenQuad() const{
    glUseProgram(m_shaders.at("ScreenQuad").handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenQuadTexture.obj_ptr);
    glUniform1i(m_shaders.at("ScreenQuad").u_locs.at("colorTex"), 0);

    glBindVertexArray(screenQuadObj.vertex_AO);
    utils::validate_program(m_shaders.at("ScreenQuad").handle);
    // glDrawElements(GL_TRIANGLES, GLsizei(6), GL_UNSIGNED_INT, NULL);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void ApplicationSolar::updateView() {
      glUseProgram(m_shaders.at("planet").handle);
    rendBuffer(600, 400);
    frameBuffer(600, 400);
   // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);


  glm::vec4 sun = {0.0f, 0.0f, 0.0f, 1.0f};
  sun = view_matrix * sun;
  glUniform3f(m_shaders.at("planet").u_locs.at("LightSource"), sun.x, sun.y, sun.z);

   glUseProgram(m_shaders.at("planet").handle);

   // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);

  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle);
   glUseProgram(m_shaders.at("ScreenQuad").handle);
    glUniformMatrix4fv(m_shaders.at("ScreenQuad").u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniform2f(m_shaders.at("ScreenQuad").u_locs.at("resoulotion"), GLfloat(600), GLfloat(400));
}

void ApplicationSolar::updateProjection() {
    glUseProgram(m_shaders.at("planet").handle);
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
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        if(isGreyscale)
            isGreyscale=false;
        else
            isGreyscale=true;
        glUseProgram(m_shaders.at("ScreenQuad").handle);
        glUniform1i(m_shaders.at("ScreenQuad").u_locs.at("isGreyscale"), isGreyscale);
         updateView();
    }
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    {
        if(isFlipH)
            isFlipH=false;
        else
            isFlipH=true;
        glUseProgram(m_shaders.at("ScreenQuad").handle);
        glUniform1i(m_shaders.at("ScreenQuad").u_locs.at("isFlipH"), isFlipH);
         updateView();
    }
    else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    {
        if(isFlipV)
            isFlipV=false;
        else
            isFlipV=true;
        glUseProgram(m_shaders.at("ScreenQuad").handle);
        glUniform1i(m_shaders.at("ScreenQuad").u_locs.at("isFlipV"), isFlipV);
         updateView();
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        if(isGaussian)
            isGaussian=false;
        else
            isGaussian=true;
        glUseProgram(m_shaders.at("ScreenQuad").handle);
        glUniform1i(m_shaders.at("ScreenQuad").u_locs.at("isGaussian"), isGaussian);
         updateView();
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
   if ( pos_x>0 && pos_y <0) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
  }
  // zooming out by moving the mouse down
  else if ( pos_x<0 && pos_y >0){

    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
  }
  updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {

  // store Star shader program objects in container
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/stars.vert",
                                           m_resource_path + "shaders/stars.frag"});
    m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
      // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["ColorVector"]=-1;
  m_shaders.at("planet").u_locs["LightSource"]=-1;
  m_shaders.at("planet").u_locs["ColorTex"]=-1;
    m_shaders.emplace("ScreenQuad", shader_program{m_resource_path + "shaders/ScreenQuad.vert",
        m_resource_path + "shaders/ScreenQuad.frag"});
    m_shaders.at("ScreenQuad").u_locs["ModelMatrix"] = -1;
    m_shaders.at("ScreenQuad").u_locs["ViewMatrix"] = -1;
    m_shaders.at("ScreenQuad").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("ScreenQuad").u_locs["colorTex"] = -1;
    m_shaders.at("ScreenQuad").u_locs["resoulotion"] = -1;
    m_shaders.at("ScreenQuad").u_locs["isGreyscale"] = -1;
    m_shaders.at("ScreenQuad").u_locs["isFlipH"] = -1;
    m_shaders.at("ScreenQuad").u_locs["isFlipV"] = -1;
    m_shaders.at("ScreenQuad").u_locs["isGaussian"] = -1;
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
// activate third attribute on gpu
    glEnableVertexAttribArray(2);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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
void ApplicationSolar::rendBuffer(GLsizei width, GLsizei height){
    glGenRenderbuffers(1, &rb_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24, width,height);
}
void ApplicationSolar::frameBuffer(GLsizei width, GLsizei height){
    glGenTextures(1, &screenQuadTexture.obj_ptr);
    glBindTexture(GL_TEXTURE_2D, screenQuadTexture.obj_ptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLint(GL_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLint(GL_LINEAR));
    glTexImage2D(GL_TEXTURE_2D, 0, GLint(GL_RGBA8), width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &fbo_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);

    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,screenQuadTexture.obj_ptr,0);

    glFramebufferRenderbuffer(
                              GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER_EXT,
                              rb_handle
                              );

    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

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
