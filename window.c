/*!\file window.c
 * \brief Test de la lib Ellule réalisée dans le cadre du cours de L2
 * <<algo prog graphique>> en collaboration avec le groupe L2-B.
 * \author Farès Belhadj
 * \date septembre à décembre 2023
 */

#include "elluleRaster.h"

#define SW 640 /* screen width */
#define SH 480 /* screen height */

static int _pw = 1, _ph = 1;

static void _quit(void);

static surface_t * _quad = NULL, * _cube = NULL, * _balle = NULL;
/* les textures utilisées dans cet exemple */
static texture_t * _terre = NULL, * _feuilles = NULL, * _parquet = NULL;

static void dis(void) {
  mat4 projection, view, model;
  /* on teste les matrices de projection */
  /* _ortho2D(projection, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f); */
  _frustum(projection, -1.0f, 1.0f, -SH / (float)SW, SH / (float)SW, 2.0f, 100.0f);


  /* on teste la vue avec un simple translate */
  /* n'oubliez pas _lookAt si vous en avez besoin */
  _mat4identite(view);
  _translate(view, 0.0f, 0.0f, -6.5f);

  static float a = 0.0f, b = 0.0f, c = 0.0f, z_ball = 0.0f, periode = 0.0f;
  elClear();

  /* on teste plusieurs "model" pour des surfaces différentes */
  elDisable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  elEnable(EL_TEXTURE);

/*
  _mat4identite(model);
  _scale(model, 0.3f, 0.3f, 0.3f);
  _translate(model, 1.0f, 1.0f, 10.0f);
  _scale(model, 0.4f, 0.4f, 0.4f);
  //_rotate(model, 0.5f * periode * 180.0f / M_PI, 0, 0, 1); //on fait tourner doucement chaque cube
  elTransformations(_cube, model, view, projection);
  elDraw(_cube);
*/
  // Get mouse coordinates
  int mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);

  // Définissez un facteur d'échelle pour augmenter la portée du mouvement
  float scaleFactor = 1.0f;

  // Appliquez le facteur d'échelle aux coordonnées de la souris
  float scaledMouseX = mouseX / (float)SW * 2.0f - 1.0f * scaleFactor;
  float scaledMouseY = -(mouseY / (float)SH * 2.0f - 1.0f) * scaleFactor;

  _mat4identite(model);
  _scale(model, 0.3f, 0.3f, 0.3f);
  coordonneesModele cubeTransPos = _translate(model, scaledMouseX, scaledMouseY, 17.0f);
  _scale(model, 0.4f, 0.4f, 0.4f);

  elEnable(EL_ALPHA);
  elEnable(EL_BACKFACE_CULLING);
  elTransformations(_cube, model, view, projection);
  elDraw(_cube);

// Quad
  elDisable(EL_ALPHA);
  elDisable(EL_BACKFACE_CULLING);
  elDisable(EL_TEXTURE);
  elDisable(EL_SHADING);
  _mat4identite(model);
  _translate(model, 0.0f, 0.0f, -5.0f);
  //_scale(model, 4.0f, 4.0f, 4.0f);
  elTransformations(_quad, model, view, projection);
  //elDraw(_quad);
  
  elEnable(EL_SHADING);

  elEnable(EL_TEXTURE);
  elEnable(EL_BACKFACE_CULLING);
  _mat4identite(model);
  coordonneesModele balleTransPos = _translate(model, 0.0f, 0.0f, z_ball);
  _rotate(model, -b, 0.0f, 0.0f, 1.0f);
  _scale(model, 0.3f, 0.2f, 0.3f);

  elTransformations(_balle, model, view, projection);
  elDraw(_balle);

  elUpdate();

  printf("Cube Position: %f, %f, %f\n", cubeTransPos.tx, cubeTransPos.ty, cubeTransPos.tz);
  printf("Sphere Position: %f, %f, %f\n", balleTransPos.tx, balleTransPos.ty, balleTransPos.tz);

  /* récupération du temps (important pour simulation (idle)) */
  static Uint32 t0 = 0; 
  Uint32 t = SDL_GetTicks();
  float dt = (t - t0) / 1000.0f;
  t0 = t;
  /* fin de récupération du temps (dt) */
  
  z_ball = -5.0f * sin(periode);

  periode += 2.0f * M_PI * dt / 3.0f; /* un aller-retour en 5 secondes */ 
  
  /* SDL_Delay(50); sin on souhaite ralentir exprès le FPS, pour tester le mvt par rapport au temps */
}

int main(int argc, char ** argv) {
  const vec4 rouge = { 0.7f, 0.0f, 0.0f, 1.0f };
  const vec4 blanc = { 1.0f, 1.0f, 1.0f, 1.0f };
  const vec4 bleu  = { 0.4f, 0.4f, 1.0f, 1.0f };
  if(!elInit(argc, argv, /* args du programme */
	     "Ellule' Hello World", /* titre */
	     1100, 825, SW, SH) /* largeur_f, hauteur_f, largeur_e, hauteur_e */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* SDL_GL_SetSwapInterval(1); si vous souhaitez la synchro verticale */
  elEnable(EL_SHADING);
  elEnable(EL_TEXTURE);
  _terre = elGenTexture("images/terre.bmp");
  _feuilles = elGenTexture("images/feuilles.bmp");
  _parquet = elGenTexture("images/parquet.bmp");
  _quad = elQuad();
  _cube = elCube();
  _balle = elSphere(11, 11);
  /* on affecte des textures aux surfaces */
  elSetTexture(_quad, _terre);
  elSetTexture(_cube, _feuilles);
  elSetTexture(_balle, _parquet);
  /* on change les couleurs par défaut */
  /* possible de le faire dans dis */
  elSetColor(_cube, blanc);
  elSetColor(_quad, rouge); //background
  elSetColor(_balle, bleu);
  atexit(_quit);
  gl4duwDisplayFunc(dis);
  gl4duwMainLoop();
  return 0;
}

void _quit(void) {
  /* des choses à libérer ... */
  if(_quad) {
    elFreeSurface(_quad);
    _quad = NULL;
  }
  if(_cube) {
    elFreeSurface(_cube);
    _cube = NULL;
  }
  if(_balle) {
    elFreeSurface(_balle);
    _balle = NULL;
  }
  if(_terre) {
    elFreeTexture(_terre);
    _terre = NULL;
  }
  if(_feuilles) {
    elFreeTexture(_feuilles);
    _feuilles = NULL;
  }
  if(_parquet) {
    elFreeTexture(_parquet);
    _parquet = NULL;
  }
}
