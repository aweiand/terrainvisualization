// **********************************************************************
//	Basico.cpp
//  Programa de testes com OpenGL 3D
//	Marcio Sarroglia Pinho
//  pinho@pucrs.br
// **********************************************************************

//#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "SOIL/SOIL.h"
#include "ImageClass.h"

ImageClass *Image;

GLfloat ratio;
float xLook = 0;
float yLook = 500;
float zLook = 0;

////// Defines
#define MAP_X	1000			         // size of map along x-axis
#define MAP_Z	400		         // size of map along z-axis
#define MAP_SCALE	20.0f	         // the scale of the terrain map

////// Terrain Data
float terrain[MAP_X][MAP_Z][3];		// heightfield terrain data (0-255); 256x256


using namespace std;
// **********************************************************************
//  void init(void)
//		Inicializa os parâmetros globais de OpenGL
//
// **********************************************************************
void init(void)
{
    glClearColor(0.6f, 1.0f, 0.6f, 0.0f); // Fundo de tela preto

    glShadeModel(GL_SMOOTH);
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    glEnable(GL_DEPTH_TEST);
    glEnable ( GL_CULL_FACE );

}
// **********************************************************************
//  void PosicUser()
//
//
// **********************************************************************
void PosicUser()
{
    // Set the clipping volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90,ratio,1,30000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(xLook, yLook, zLook,
              xLook, yLook, zLook-10,
              0.0f,1.0f,0.0f);

}
// **********************************************************************
//  void reshape( int w, int h )
//		trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    ratio = 1.0f * w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    PosicUser();

}
// **********************************************************************
//  void DesenhaCubo()
//
//
// **********************************************************************
void DesenhaCubo()
{
    glBegin ( GL_QUADS );
    // Front Face
    glNormal3f(0,0,1);
    glVertex3f(-1.0f, 0.0f,  1.0f);
    glVertex3f( 1.0f, 0.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back Face
    glNormal3f(0,0,-1);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f, 0.0f, -1.0f);
    // Top Face
    glNormal3f(0,1,0);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Face
    glNormal3f(0,-1,0);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f( 1.0f, 0.0f, -1.0f);
    glVertex3f( 1.0f, 0.0f,  1.0f);
    glVertex3f(-1.0f, 0.0f,  1.0f);
    // Right face
    glNormal3f(1,0,0);
    glVertex3f( 1.0f, 0.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glVertex3f( 1.0f, 0.0f,  1.0f);
    // Left Face
    glNormal3f(-1,0,0);
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 0.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}

void InitializeTerrain()
{
	// loop through all of the heightfield points, calculating
	// the coordinates for each point
	for (int z = 0; z < MAP_Z; z++) {
		for (int x = 0; x < MAP_X; x++) {
			terrain[x][z][0] = float(x)*MAP_SCALE;
			terrain[x][z][1] = float(Image->GetPointIntensity(x,z));
			terrain[x][z][2] = -float(z)*MAP_SCALE;
		}
	}
}

void DesenhaMapa(){
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// we are going to loop through all of our terrain's data points,
	// but we only want to draw one triangle strip for each set along the x-axis.
	for (int z = 0; z < MAP_Z-1; z++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < MAP_X-1; x++)
		{
			// draw vertex 0
			glColor3f(terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f);
			glVertex3f(terrain[x][z][0], terrain[x][z][1], terrain[x][z][2]);

			// draw vertex 1
			glColor3f(terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f);
			glVertex3f(terrain[x+1][z][0], terrain[x+1][z][1], terrain[x+1][z][2]);

			// draw vertex 2
			glColor3f(terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f);
			glVertex3f(terrain[x][z+1][0], terrain[x][z+1][1], terrain[x][z+1][2]);

			// draw vertex 3
			glColor3f(terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f);
			glVertex3f(terrain[x+1][z+1][0], terrain[x+1][z+1][1], terrain[x+1][z+1][2]);
		}
		glEnd();
	}
}

// **********************************************************************
//  void display( void )
//
//
// **********************************************************************
//float alturas[]= {1,2,0.5,4,1,2,3,6,1};
void display( void )
{
    static double angY = 0;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    PosicUser();

    //glMatrixMode(GL_MODELVIEW);

    DesenhaMapa();

    /*
    for(float tx=-20; tx<20; tx+=2)
        for(float tz=5; tz>-20; tz-=2)
        {
            glPushMatrix();
            glTranslatef ( tx, 0.0, tz );
            glScalef(1,rand()%8,1);

            float verm = (rand()%100)/100.0;
            float verd = (rand()%100)/100.0;
            glColor3f(verm,verd,0.0f); // verde
            DesenhaCubo();
            glPopMatrix();
        }
    */

    glutSwapBuffers();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27:        // Termina o programa qdo
        exit ( 0 );   // a tecla ESC for pressionada
        break;
    case 'r':
        yLook+=10;
        break;
    case 'f':
        yLook-=10;
        break;
    case 'a':
        xLook-=10;
        break;
    case 'd':
        xLook+=10;
        break;
    case 'w':
        zLook-=10;
        break;
    case 's':
        zLook+=10;
        break;
    default:
        break;
    }
}

// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
    switch ( a_keys )
    {
    case GLUT_KEY_UP:       // When Up Arrow Is Pressed...
        glutFullScreen ( ); // Go Into Full Screen Mode
        break;
    case GLUT_KEY_DOWN:     // When Down Arrow Is Pressed...
        glutInitWindowSize  ( 700, 500 );
        break;
    default:
        break;
    }
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int main ( int argc, char** argv )
{
    Image = new ImageClass();
    //int r = Image->Load("mapa.jpg");
    int r = Image->Load("world.png");

    if (!r)
        exit(1);
    else {
        cout << ("Imagem carregada!\n");
        cout << "Canais: "  << Image->Channels() << endl;
    }

    xLook = (Image->SizeX() * MAP_SCALE) / 2 ;
    zLook = 500;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );// | GLUT_STEREO);// | GLUT_DOUBLE | GLUT_RGBA );
    //glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);// | GLUT_DOUBLE | GLUT_RGBA );

    glutInitWindowPosition (0,0);
    glutInitWindowSize  ( 700, 500 );
    glutCreateWindow    ( "Computacao Grafica - Exemplo Basico 3D" );

    InitializeTerrain();

    init ();

    glutDisplayFunc ( display );
    glutReshapeFunc ( reshape );
    glutKeyboardFunc ( keyboard );
    glutSpecialFunc ( arrow_keys );
    glutIdleFunc ( display );

    glutMainLoop ( );
    return 0;
}



