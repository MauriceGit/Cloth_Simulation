

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef MACOSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "texture.h"
#include "cgimage.h"
#include "types.h"
#include "logic.h"

/* ---- Typen ---- */
typedef struct
{
  GLuint id;
  textureType type;
  char *filename;
} Texture;

/* ---- Konstanten ---- */


/* ---- Globale Daten ---- */
static int g_texturingStatus = 1;

textureType g_textureType = NORMALTEXTURE;

static Texture g_textures[TEX_COUNT];

/* ---- Funktionen ---- */

/**
 * Liefert den Typ der Textur
 */
textureType getTextureType (void)
{
  return g_textureType;
}

/**
 * Setzt den Typ der Textur
 */
void setTextureType (textureType status)
{
  g_textureType = status;
}

/**
 * Get-Funktion für den Status der Texturen (an/aus)
 */
int getTexturingStatus (void)
{
  return g_texturingStatus;
}

/**
 * Setzte den Status der Texturen
 */
void setTexturingStatus (int status)
{
  g_texturingStatus = status;
}

/**
 * Inizialisiert das Textur-Array und berechnet die IDs
 */
static int initTextureArray (void)
{
  int i;

  GLuint texIDs[TEX_COUNT];
  glGenTextures (TEX_COUNT, texIDs);

  for (i = 0; i < TEX_COUNT; i++)
    {
      g_textures[i].id = texIDs[i];
    }

  g_textures[texGround].filename = "textures/ground.tga";
  g_textures[texGround].type = NORMALTEXTURE;
  g_textures[texMetal].filename = "textures/metal.tga";
  g_textures[texMetal].type = NORMALTEXTURE;
  g_textures[texParkett].filename = "textures/parkett.tga";
  g_textures[texParkett].type = NORMALTEXTURE;
  g_textures[texSky].filename = "textures/sky.tga";
  g_textures[texSky].type = NORMALTEXTURE;
  g_textures[texBlack].filename = "textures/black.tga";
  g_textures[texBlack].type = NORMALTEXTURE;
  g_textures[texPlasma].filename = "textures/linen.tga";
  g_textures[texPlasma].type = NORMALTEXTURE;
  g_textures[texLabyrinth].filename = "textures/labyrinth.tga";
  g_textures[texLabyrinth].type = NORMALTEXTURE;
  g_textures[texBlack2].filename = "textures/black2.tga";
  g_textures[texBlack2].type = NORMALTEXTURE;
  /*g_textures[texStone].filename = "textures/stone.pcx";
  g_textures[texStone].type = NORMALTEXTURE;
  g_textures[texWood].filename = "textures/wood.tga";
  g_textures[texWood].type = NORMALTEXTURE;*/

  return 1;
}

/**
 * Läd eine Textur
 */
void bindTexture (TexName texture)
{
  glBindTexture (GL_TEXTURE_2D, g_textures[texture].id);
}

static unsigned int
calculateGLBitmapMode (CGImage * self)
{
  switch (self->bpp)
    {
    case 1:
      return GL_LUMINANCE;
    case 2:
      return GL_LUMINANCE_ALPHA;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
    default:
      return GL_RGB;
    }
}

/**
 * Erstellt ein kariertes ImageData-Array
 */
CGImage* loadTextureImageDataChecker(CGImage * image, int checkerSize) {
   int value = 0,
       row = 0,
       col = 0;
       
   for (row = 0; row < IMAGE_ROWS; row++) {
      for (col = 0; col < IMAGE_COLS; col++) {
         /* Berechnen der Größe */
         value = (((row & checkerSize) == 0) ^ ((col & checkerSize) == 0)) * 255;

         (*image).data[col + IMAGE_COLS * (row +  IMAGE_ROWS *0)] = value;
         (*image).data[col + IMAGE_COLS * (row +  IMAGE_ROWS *1)] = value;
         (*image).data[col + IMAGE_COLS * (row +  IMAGE_ROWS *2)] = value;

      }
   }
   return image;
}

/**
 * Erstellt ein kariertes ImageData-Array
 */
CGImage* loadTextureImageDataDiagonal(CGImage * image, int checkerSize) {
    
   int row = 0,
       col = 0,
       shift = 0;
    
    /*
     * rechte-obere Seite
     */
    for(row = 0; row < IMAGE_ROWS; ++row)
    {
        for (col = 0; col < IMAGE_COLS; ++col)
        {
            /*
             * shift nach rechts
             */
            if ((col+shift < IMAGE_COLS))
            {
                if ((col/checkerSize)%2 == 1) {
                    (*image).data[col+shift + IMAGE_COLS * (row +  IMAGE_ROWS *0)] = 255;
                    (*image).data[col+shift + IMAGE_COLS * (row +  IMAGE_ROWS *1)] = 255;
                    (*image).data[col+shift + IMAGE_COLS * (row +  IMAGE_ROWS *2)] = 255;
                }
            }
        }
        ++shift;
    }

    shift = 0;
    /*
     * linke-untere Seite
     */
    for(col = 0; col < IMAGE_COLS; ++col)
    {
        for (row = 0; row < IMAGE_ROWS; ++row)
        {
            /*
             * shift nach unten
             */
            if ((row+shift < IMAGE_ROWS))
            {
                if ((row/checkerSize)%2 == 0) {
                    (*image).data[col + IMAGE_COLS * (row+shift +  IMAGE_ROWS *0)] = 255;
                    (*image).data[col + IMAGE_COLS * (row+shift +  IMAGE_ROWS *1)] = 255;
                    (*image).data[col + IMAGE_COLS * (row+shift +  IMAGE_ROWS *2)] = 255;
                }
            }
        }
        ++shift;
    }
    return image;
}

/**
 * Läd die eigenen Texturen
 */
CGImage * CGImage_load_own (textureType type) {
  CGImage *image = NULL;
  
  
  image = CGImage_create(IMAGE_COLS, IMAGE_ROWS, CG_IMAGE_COLOR);

  if (image == NULL)
    exit(1);
  
  
  image->data = calloc((*image).width*(*image).height*CG_IMAGE_COLOR,sizeof(char));

  if (image->data == NULL)
    exit(1);

  /*
  switch(type)
  {
      case NORMALTEXTURE: 
      break;
      case CHECKER: 
        image = loadTextureImageDataChecker(image,getTextureSize());
      break;
      case DIAGONAL:
        image = loadTextureImageDataDiagonal(image,getTextureSize());
      break;
  }
  */
  
  return image;
}

/**
 * Berechnet die eigenen Texturen neu
 */
void  makeNewOwnTexture() {
    CGImage* image;
    int i = 0,
        j = 0;
    
    for (j = 0; j < COUNTOWNTEX; ++j)
    {
        
        i = TEX_COUNT-(j+1);
                
        image = CGImage_load_own (g_textures[i].type);

        if (image != NULL)
        {
            glBindTexture (GL_TEXTURE_2D, g_textures[i].id);

            gluBuild2DMipmaps (GL_TEXTURE_2D,
                            image->bpp,
                            image->width,
                            image->height,
                            calculateGLBitmapMode (image),
                            GL_UNSIGNED_BYTE, image->data);

            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            CGImage_free (image);
        }
    }
}

/**
 * Läd die Texturen
 */
static int
loadTextures (void)
{
  int i;
  CGImage *image = NULL;

  if (initTextureArray ())
    {
      /* Alle Texturen nacheinander laden. */
      for (i = 0; i < TEX_COUNT; i++)
        {

          switch(g_textures[i].type)
          {
              case NORMALTEXTURE:  
                image = CGImage_load (g_textures[i].filename);
                break;
              case CHECKER:
                image = CGImage_load_own (CHECKER);
                break;
              case DIAGONAL:
                image = CGImage_load_own (DIAGONAL);
                break;
              default: fprintf(stdout,"Typ der Texture nicht gefunden\n");
          }

          
          if (image != NULL)
            {
              glBindTexture (GL_TEXTURE_2D, g_textures[i].id);

    /*          glTexImage2D (GL_TEXTURE_2D,
                            0,
                            image->bpp,
                            image->width,
                            image->height,
                            0,
                            calculateGLBitmapMode (image),
                            GL_UNSIGNED_BYTE,
                            image->data);*/

              gluBuild2DMipmaps (GL_TEXTURE_2D,
                                image->bpp,
                                image->width,
                                image->height,
                                calculateGLBitmapMode (image),
                                GL_UNSIGNED_BYTE, image->data);

              glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
              glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
              glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

              CGImage_free (image);
            }
          else
            {
              return 0;
            }
        }

      /* Alles in Ordnung? */
      return 1;
    }
  else
    {
      return 0;
    }
}

/**
 * Setzt die automatische Koordinatenberechnung der Texturen 
 */
void
toggleAutomaticTextureCoordinates (void)
{
  static int automatic = GL_FALSE;

  automatic = !automatic;

  if (automatic)
    {
      glEnable (GL_TEXTURE_GEN_S);
      glEnable (GL_TEXTURE_GEN_T);
    }
  else
    {
      glDisable (GL_TEXTURE_GEN_S);
      glDisable (GL_TEXTURE_GEN_T);
    }
}

int
initTextures (void)
{
  /* Ebenen-Koeffizienten für Generierung von Textur-Koordinaten */
  /* Parallele zur y-z-Ebene */
  GLfloat map_s[4] = { 2.0f, 0.0f, 0.0f, 0.0f };
  /* Parallele zur x-z-Ebene */
  GLfloat map_t[4] = { 0.0f, 2.0f, 0.0f, 0.0f };

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  /* Einstellungen für die automatische Generierung von Texturkoordinaten
   * nur gültig im Falle von impliziter Texturierung */
  glTexGenfv (GL_S, GL_OBJECT_PLANE, map_s);
  glTexGenfv (GL_T, GL_OBJECT_PLANE, map_t);
  glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
  glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

  /* Texturen laden */
  return loadTextures ();
}
