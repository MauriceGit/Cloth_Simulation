#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "types.h"


/** Anzahl der Texturen. */
#define TEX_COUNT (8)

/**
 * Typ fuer Texturenbezeichner.
 */
typedef enum
{
  texGround, texMetal,texParkett,texSky, texBlack,
  texPlasma, texLabyrinth, texBlack2,texStone,texWood
} TexName;

/**
 * Berechnet die eigenen Texturen neu
 */
void makeNewOwnTexture();


/**
 * Laed Texturen und initialisiert das Texturmapping.
 * @return 1, wenn Laden und Initialisieren erfolgreich war, sonst 0.
 */
int initTextures (void);

/**
 * Bindet die Textur texture, so dass sie fuer alle nachfolgende gezeichneten
 * Primitiven verwendet wird.
 * @param texture Bezeichner der Textur, die gebunden werden soll.
 */
void bindTexture (TexName texture);

/**
 * Schaltet den Status der automatischen Texturierung um.
 */
void toggleAutomaticTextureCoordinates (void);

/**
 * Liefert den Status der Texturierung.
 * @return Status der Texturierung (an/aus).
 */
int getTexturingStatus (void);

/**
 * Liefert den Typ der Textur
 */
textureType getTextureType (void);

/**
 * Setzt den Typ der Textur
 */
void setTextureType (textureType status);

/**
 * Setzt den Status der Texturierung.
 * @param status Status der Texturierung (an/aus).
 */
void setTexturingStatus (int status);



#endif
