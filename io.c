

/**
 * @file
 * Hier sind alle CallBack-Funktionen
 *
 * @author Maurice Tollmien, Tilman Nedele
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "scene.h"
#include "logic.h"
#include "particle.h"
#include "texture.h"
#include "vector.h"

/* ---- Eigene Funktionen ---- */




/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer (int lastCallTime)
{
	/* Seit dem Programmstart vergangene Zeit in Millisekunden */
	int thisCallTime = glutGet (GLUT_ELAPSED_TIME);
    
	/* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
	double interval = (double) (thisCallTime - lastCallTime) / 1000.0f;
		
	calcTimeRelatedStuff(interval);
			
	/* Wieder als Timer-Funktion registrieren */
	glutTimerFunc (1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

	/* Neuzeichnen anstossen */
	glutPostRedisplay ();
    
}


/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay (void)
{
  /* Buffer zuruecksetzen */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
  glMatrixMode (GL_MODELVIEW);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity ();
  
  /* Kameraposition */
    if (!getParticleCamera ())
    {
        gluLookAt (getCameraPosition(0), getCameraPosition(1), getCameraPosition(2),
                 0.0, 0.0, 0.0,
                 0.0, 1.0, 0.0);
    } else  {
        Particle p = getCameraParticle ();
        if (p) 
        {
            /*CGVector3D tmp;*/
            CGVector3D tmp2;
/*            CGVector3D tmp3;*/
            
        /*    tmp[0] = p->up[0];
            tmp[1] = p->up[1];
            tmp[2] = p->up[2];
          */  
            tmp2[0] = p->v[0];
            tmp2[1] = p->v[1];
            tmp2[2] = p->v[2];
            normVector3D (tmp2);
            
            /*multiplyVectorScalar (tmp, CAMERA_PARTICLE_DISTANCE, &tmp);*/
            
            gluLookAt ( p->s[0]+p->v[0], p->s[1]+p->v[1], p->s[2]+p->v[2],
                        p->s[0],p->s[1],p->s[2],
                        p->up[0], p->up[1], p->up[2]);
                        
        } else {
            toggleParticleCamera ();
            cbDisplay ();
        }
    }
    
  /* Szene zeichnen */
  drawScene ();

  /* Objekt anzeigen */
  /* glFlush (); *//* fuer SingleBuffering */
  glutSwapBuffers ();           /* fuer DoubleBuffering */
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
void cbKeyboard (unsigned char key, int x, int y)
{
	
	switch (key)
	{
		case 'q':
		case 'Q':
		case ESC:
			exit (0);
			break;
		case 'r':
        case 'R':
			initGame ();
			break;
		case 'h':
        case 'H':
            setHelpStatus(!getHelpStatus());
            break;
        case '+':
            incQuadCount();
            break;
        case '-':
            decQuadCount();
            break;
        case 'n':
		case 'N':
			nextCameraParticle();
            break;
        case 'm':
		case 'M':
			toggleSpheresMoving ();
			break;
		case 't':
        case 'T':
            nextTexture();
            break;
		case 'p':
        case 'P':
            toggleParticleFreeze ();
            break;
		case 'f':
        case 'F':
            toggleParticleType ();
            break;
        case 's':
        case 'S':
            toggleShadows ();
            break;    
        case 'k':
        case 'K':
			printf ("k recognized.\n");
            startMovingBall ();
            break;   
        case 'v':
        case 'V':
            togglePersecutorParticle ();
            break;  
            
	}	
}

/* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
* (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
* Einfuegen) gedrueckt wird */
void cbSpecial (int key, int x, int y)
{
	switch (key)
	{
        case GLUT_KEY_LEFT: 

			break;
		case GLUT_KEY_RIGHT:

			break;
		case GLUT_KEY_UP:
            setKey (1, 1);
			break;
		case GLUT_KEY_DOWN:
            setKey(0,1);
			break;
		case GLUT_KEY_F1:
            toggleWireframeMode();
			break;
        case GLUT_KEY_F2:
            setLightStatus(!getLightStatus());
            break;
		case GLUT_KEY_F3:
            setTextureStatus(!getTextureStatus());
			break;
        case GLUT_KEY_F4:

            break;
        case GLUT_KEY_F5:

            break;
        case GLUT_KEY_F6:

            break;
        case GLUT_KEY_F7:

            break;
        case GLUT_KEY_F8:

            break;
	}
}

/* Wird aufgerufen, wenn eine Spezialtaste wieder losgelassen wird1 */
void cbUpSpecial (int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: 

			break;
		case GLUT_KEY_RIGHT:

			break;
		case GLUT_KEY_UP:
            setKey (1,0);
			break;
		case GLUT_KEY_DOWN:
            setKey (0,0);
			break;
	}
}

/**
 * Verarbeitung eines Mausereignisses.
 * Durch Bewegung der Maus bei gedrueckter Maustaste kann die aktuelle
 * Zeichenfarbe beeinflusst werden.
 * Falls Debugging aktiviert ist, wird jedes Mausereignis auf stdout
 * ausgegeben.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
static void
handleMouseEvent (int x, int y, CGMouseEventType eventType, int button, int buttonState)
{
    switch (eventType)
    {
        case mouseButton:
            switch (button)
            {
                case GLUT_LEFT_BUTTON:
                    setMouseEvent(MOVE,x,y);
                break;
                case GLUT_RIGHT_BUTTON:
                    setMouseEvent(ZOOM,x,y);
                break;
                default:
                  break;
            }
        break;
        default:
          break;
    }
    if (buttonState)
        setMouseEvent(NONE,x,y);

}

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
static void
cbMouseButton (int button, int state, int x, int y)
{
	handleMouseEvent (x, y, mouseButton, button, state);
}

void mouseMovement(int x, int y) 
{
    
    if (getMouseEvent() == MOVE)
		setCameraMovement(x,y);
	
    if (getMouseEvent() == ZOOM)
        setCameraZoom(x,y);
}


/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Berücksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void
setProjection (GLdouble aspect)
{
  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode (GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity ();
    
  {
      /* perspektivische Projektion */
      gluPerspective (90.0,     /* Oeffnungswinkel */
                      aspect,   /* Seitenverhaeltnis */
                      10.0,      /* nahe ClipPIEng-Ebene */
                      1000.0 /* ferne ClipPIEng-Ebene */ );
  }
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veraenderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape (int w, int h)
{
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport (0, 0, (GLsizei) w, (GLsizei) h);

  /* Anpassen der Projektionsmatrix an das Seitenverhaeltnis des Fensters */
  setProjection ((GLdouble) w / (GLdouble) h);
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void registerCallBacks (void)
{
    /* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
     *  gedrueckt oder losgelassen wird) */
    glutMouseFunc (cbMouseButton);
    
    glutMotionFunc(mouseMovement);
	
	/* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
	glutTimerFunc (1000 / TIMER_CALLS_PS, /* msecs - bis Aufruf von func */
                 cbTimer,       /* func  - wird aufgerufen    */
                 glutGet (GLUT_ELAPSED_TIME));  /* value - Parameter, mit dem
                                                   func aufgerufen wird */

	/* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
	* Erzeugen oder Groessenaenderungen des Fensters) */
	glutReshapeFunc (cbReshape);

	/* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
	* Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
	glutDisplayFunc (cbDisplay);
	
	/* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
	glutKeyboardFunc (cbKeyboard);
	
	/* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
	* (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
	* Einfuegen) gedrueckt wird */
	glutSpecialFunc (cbSpecial);
	
    /* Wird aufgerufen, wenn eine Taste wieder losgelassen wird! */
	glutSpecialUpFunc(cbUpSpecial);
    
	/* Automat. Tastendruckwiederholung ignorieren */
	glutIgnoreKeyRepeat (1);
    
    /* Wenn die Fenstergröße verändert wird. */
    /*glutReshapeFunc(cgReshape);*/
}




/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO (char *title, int width, int height)
{
	int windowID = 0;
	
	/* Kommandozeile immitieren */
	int argc = 1;
	char *argv = "cmd";

	/* Glut initialisieren */
	glutInit (&argc, &argv);
	
	/* FensterInitialisierung */
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	/* FensterGröße */
	glutInitWindowSize (width, height);
	/* FensterPosition */
	glutInitWindowPosition (0, 0);
	
	windowID = glutCreateWindow (title);
	
	if (windowID)
	{
		/* Hintergrund und so werden initialisiert (Farben) */
		if (initScene ())
		{
            /* Texturen laden hat geklappt*/
            if (initTextures ())
            { 
                /* CallBack zum Malen und die Zeit dazwischen... */
                registerCallBacks ();
                /* Die Endlosschleife wird angestoßen */
                glutMainLoop ();
            }
            else
            {
                glutDestroyWindow (windowID);
                windowID = 0;
            }
		} 
		else
		{
			glutDestroyWindow (windowID);
			windowID = 0;
		}
	}
	return windowID;
}
