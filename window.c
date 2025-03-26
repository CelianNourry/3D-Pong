/*!\file window.c
 * Jeu de Pong en 3D Edition Plage
 * Nourry Célian
 * date : 5 janvier 2024
 */

#include "elluleRaster.h"
#include <time.h>

#define SW 640 /* Largeur de l'écran */
#define SH 480 /* Taille de l'écran */

#define z_quad 13.0f
#define z_adv_quad -10.0f
#define TAILLE_QUAD 2.0f
#define TAILLE_BALLE 0.3f

static void _quit(void);

static surface_t * _quad = NULL, * _adv_quad = NULL, * _balle = NULL, * _background = NULL;
static texture_t * _ball = NULL, * _Cassius = NULL, * _Amicus = NULL, * _background_texture = NULL;

static void dis(void) {
  mat4 projection, view, model;
  _frustum(projection, -1.0f, 1.0f, -SH / (float)SW, SH / (float)SW, 2.0f, 100.0f);

  _mat4identite(view);
  _translate(view, 0.0f, 0.0f, -6.5f);

  static float x_ball = 0.0f, y_ball = 0.0f, z_ball = 0.0f, x_quad = 0.0f, y_quad = 0.0f, x_adv_quad = 0.0f, y_adv_quad = 0.0f, periode = 0.0f, speed = 0.1f, orientation_x = 0.0f, orientation_y = 0.0f, points = 0;
  float attenuation_z = 2.0f + z_ball * 0.2f; //pour que x_ball ne dépasse pas les limites de la camera
  elClear();

  elDisable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  elEnable(EL_TEXTURE);

  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  //Coordonées de la souris
  x_quad = -((mouseX / (float)SW * 1.5f - 1.0f)) * 2;
  y_quad = (mouseY / (float)SH * 1.5f - 1.0f) * 2;

  _mat4identite(model);
  _translate(model, x_quad, y_quad, z_quad);
  elEnable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  _scale(model, 0.8f, 0.8f, 0.8f);
  elTransformations(_quad, model, view, projection);
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);

  _mat4identite(model);
  elEnable(EL_BACKFACE_CULLING);
  _translate(model, x_ball, y_ball, z_ball);
  _rotate(model, 0.5f * periode * 180.0f / M_PI, 0.2, 0.5, 1); //On fait tourner la balle
  _scale(model, 0.3f, 0.3f, 0.3f);
  elTransformations(_balle, model, view, projection);

  _mat4identite(model);
  _translate(model, x_adv_quad, y_adv_quad, z_adv_quad);
  elEnable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  _scale(model, 0.8f, 0.8f, 0.8f);
  elTransformations(_adv_quad, model, view, projection);
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);

  _mat4identite(model);
  _translate(model, 0, 0, -10.5);
  elEnable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  _scale(model, 10, 5.2, 10);
  elTransformations(_background, model, view, projection);
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);

  elDraw(_background);
  elDraw(_adv_quad);
  elDraw(_balle);
  elDraw(_quad);

  elUpdate();

  speed *= 1.0001; // Acceleration de la vitesse de la balle au fur et à mesure du temps
  //On fait avancer la balle de façon constante
  z_ball += speed;
  x_ball += orientation_x;
  y_ball += orientation_y;

  x_adv_quad = x_ball;
  y_adv_quad = y_ball;

  //Represente l'adversaire, on triche car il est supposé tout le temps rattrapper la balle
  if (z_ball <= -10.0f) {
    speed = -speed;
  }

  //Quitte si le joueur perd
  if (z_ball >= 5.0f){
    //On reset la balle quand le joueur ne l'a pas rattrapé
    x_ball = 0.0f;
    y_ball = 0.0f;
    z_ball = 0.0f;
    orientation_x = 0.0f;
    orientation_y = 0.0f;
    points += 1; //Represente les points de l'adversaire

    //On quitte si l'adversaire a 3 points
    if (points >= 3){
      _quit();
    }
  }

  //Murs invisibles aux côtés x
  if (x_ball >= attenuation_z || x_ball <= -attenuation_z) {
    orientation_x = -orientation_x;
  }

  //Murs invisibles aux côtés y
  if (y_ball >= 0.5f || y_ball <= -0.5f) {
    orientation_y = -orientation_y;
  }

  //La balle touche le joueur
  if (x_ball + 0.3f > x_quad - TAILLE_QUAD && x_ball - 0.3f < x_quad + TAILLE_QUAD && y_ball + 0.3f > y_quad - TAILLE_QUAD && y_ball - 0.3f < y_quad + TAILLE_QUAD && z_ball + 0.3f >= 4.5f && z_ball + 0.3f <= 4.6f){
    z_ball -= 0.2f;
    
    srand(time(NULL));

    //La balle a une chance sur 2 d'aller vers un des deux côtés en touchant un mur invisible
    int random = rand() % 2;
    if (random == 0){
      orientation_x += 0.05f;
    }
    else{
      orientation_x -= 0.05f;
    }

    random = rand() % 2;
    if (random == 0){
      orientation_y += 0.01f;
    }
    else{
      orientation_y -= 0.01f;
    }
    speed = -speed;
  }

  //Récupération du temps
  static Uint32 t0 = 0; 
  Uint32 t = SDL_GetTicks();
  float dt = (t - t0) / 1000.0f;
  t0 = t;

  periode += 2.0f * M_PI * dt / 5.0f;
}

int main(int argc, char ** argv) {
  const vec4 blanc = { 1.0f, 1.0f, 1.0f, 1.0f };

  if(!elInit(argc, argv,
	     "Pong à la plage",
	     1100, 825, SW, SH)){
    return 1;
  }
  
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);
  //Génération des textures
  _ball = elGenTexture("images/Balle.bmp");
  _Cassius = elGenTexture("images/Cassius.bmp");
  _Amicus = elGenTexture("images/Amicus.bmp");
  _background_texture = elGenTexture("images/Beach.bmp");

  _quad = elQuad();
  _adv_quad = elQuad();
  _background = elQuad();
  _balle = elSphere(11, 11);

  //Affectation des textures aux surfaces
  elSetTexture(_quad, _Amicus);
  elSetTexture(_adv_quad, _Cassius);
  elSetTexture(_balle, _ball);
  elSetTexture(_background, _background_texture);

  //Couleur par défaut
  elSetColor(_quad, blanc);
  elSetColor(_adv_quad, blanc);
  elSetColor(_balle, blanc);
  elSetColor(_background, blanc);

  atexit(_quit);
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}

void _quit(void) {
  //Libération des textures et des surfaces
  if(_quad) {
    elFreeSurface(_quad);
    _quad = NULL;
  }
  if(_balle) {
    elFreeSurface(_balle);
    _balle = NULL;
  }
  if(_adv_quad) {
    elFreeSurface(_adv_quad);
    _adv_quad = NULL;
  }
  if(_background) {
    elFreeSurface(_background);
    _background = NULL;
  }
  if(_Cassius) {
    elFreeTexture(_Cassius);
    _Cassius = NULL;
  }
  if(_Amicus) {
    elFreeTexture(_Amicus);
    _Amicus = NULL;
  }
  if(_ball) {
    elFreeTexture(_ball);
    _ball = NULL;
  }
  if(_background_texture) {
    elFreeTexture(_background_texture);
    _background_texture = NULL;
  }
}