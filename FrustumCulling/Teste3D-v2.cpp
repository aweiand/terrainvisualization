// **********************************************************************
//	Teste3D-v2.cpp
//  Programa de reconstrução de terreno 3D com mapa de
//  tons de cinza carregadoa partir de uma imagem externa
//  com 2 algoritmos de aceleração de renderização.
//
//	Augusto Weiand <guto.weiand@gmail.com>
// **********************************************************************

#include <stdio.h>
#include <ctime>
#include <math.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <sstream>
#include <iomanip>
#include "SOIL/SOIL.h"
#include "ImageClass.h"

// **********************************************************************
// Tamanho do Mapa em X
#define MAP_X	3000

// Tamanho do Mapa em Z
#define MAP_Z	1000

// Escala do Mapa
#define MAP_SCALE	20.0f
// **********************************************************************

ImageClass *Image;
GLfloat ratio;

// **********************************************************************
// Variáveis de Posicionamento da câmera, velocidade
// e algoritmo frustum culling ligado/desligado
float xLook = 0;
float yLook = 1000;
float zLook = 0;
int velocity= 100;
bool fruston= false;
float teste;
// **********************************************************************

// **********************************************************************
// Variáveis de Tempo
clock_t start;
double ttime;
// **********************************************************************

// **********************************************************************
// Matriz de Terreno e Frustum
float terrain[MAP_X][MAP_Z][3];
float frustum[6][4];
// **********************************************************************

using namespace std;

// **********************************************************************
//      Inicializa os parâmetros globais de OpenGL
// **********************************************************************
void init(void){
    glClearColor(0.6f, 1.0f, 0.6f, 0.0f); // Fundo de tela preto

    glShadeModel(GL_SMOOTH);
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    glEnable(GL_DEPTH_TEST);
    glEnable ( GL_CULL_FACE );

}
// **********************************************************************
//      Posiciona o Usuário visualizador de
//      acordo com as coordenadas globais
// **********************************************************************
void PosicUser(){
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
//      Trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape( int w, int h ){
    if(h == 0)
        h = 1;

    ratio = 1.0f * w / h;

    glMatrixMode(GL_PROJECTION);

    glViewport(0, 0, w, h);

    PosicUser();

}

// **********************************************************************
//      Desenha o texto na tela
// **********************************************************************
void drawText(const std::string str, int x = 10, int y = 9){
    glColor3f( 1, 1, 1 );
    glRasterPos3f(xLook+x, yLook+y, zLook-10);

    for( size_t i = 0; i < str.size(); ++i ){
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, str[i] );
    }
}

// **********************************************************************
//      Inicializando a matriz do terreno e preenchendo com a
//      escala de cinza
// **********************************************************************
void InitializeTerrain(){
	for (int z = 0; z < MAP_Z; z++) {
		for (int x = 0; x < MAP_X; x++) {
			terrain[x][z][0] = float(x)*MAP_SCALE;
			terrain[x][z][1] = float(Image->GetPointIntensity(x,z));
			terrain[x][z][2] = -float(z)*MAP_SCALE;
		}
	}
}

// **********************************************************************
//      Verificação se o ponto esta na Frustum
// **********************************************************************
bool PointInFrustum( float x, float y, float z ){
    if (fruston){
        return true;
    }

    int p;

    for( p = 0; p < 6; p++ )
        if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0 )
            return false;
    return true;
}

// **********************************************************************
//      Desenhando o mapa na tela, com a verificação do PointInFrustum
//      de acordo com a variável global
// **********************************************************************
void DesenhaMapa(){
    int tri = 1;
    int z = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	do {
        int x = 0;
		glBegin(GL_TRIANGLE_STRIP);
		do {
            if (PointInFrustum(terrain[x][z][0], terrain[x][z][1], terrain[x][z][2])){

                if (terrain[x][z][2] < 1)
                    teste = -(terrain[x][z][2]);
                else
                    teste = (terrain[x][z][2]);

                if (!fruston){
                    if (zLook < 1){
                        if ((-(zLook) + 5000) < teste){
                            tri = 3;
                        } else {
                            tri = 1;
                        }
                    } else {
                        if (((zLook) + 5000) < teste){
                            tri = 5;
                        } else {
                            tri = 1;
                        }
                    }
                }

                glColor3f(terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f, terrain[x][z][1]/255.0f);
                glVertex3f(terrain[x][z][0], terrain[x][z][1], terrain[x][z][2]);

                glColor3f(terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f, terrain[x+1][z][1]/255.0f);
                glVertex3f(terrain[x+1][z][0], terrain[x+1][z][1], terrain[x+1][z][2]);

                glColor3f(terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f, terrain[x][z+1][1]/255.0f);
                glVertex3f(terrain[x][z+1][0], terrain[x][z+1][1], terrain[x][z+1][2]);

                glColor3f(terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f, terrain[x+1][z+1][1]/255.0f);
                glVertex3f(terrain[x+1][z+1][0], terrain[x+1][z+1][1], terrain[x+1][z+1][2]);
            }

            x+=tri;
		} while (x < MAP_X-1);

		glEnd();

		z+=tri;
	} while (z < MAP_Z-1);
}

// **********************************************************************
//      Função para extrair a visão atual
// **********************************************************************
void ExtractFrustum(){
   float   proj[16];
   float   modl[16];
   float   clip[16];

   // Recupera a matriz de PROJECTION do OpenGL
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   // Recupera a matriz de MODELVIEW do OpenGL
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   //Combina as duas matrizes multiplicando-as
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   // extrai o plano da direita
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   // extrai o plano da esquerda
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   // extrai o plano de baixo
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   // extrai o plano de cima
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   // extrai o plano de longe
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   // extrai o plano de perto
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];
}


// **********************************************************************
//      Display
// **********************************************************************
void display( void ){
    start = clock();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    PosicUser();

    DesenhaMapa();

    ExtractFrustum();

    ttime = (clock() - start) / (double)CLOCKS_PER_SEC;

    std::ostringstream oss;
    oss << std::setprecision(4) << ttime;

    drawText(oss.str()+" ms");

    fruston? drawText("Frustum View Off", 5, 8) : drawText("Frustum View On", 5, 8);

    std::ostringstream vel;
    vel << velocity;
    drawText("Velocity "+vel.str(), 5, 7);

    glutSwapBuffers();
}

// **********************************************************************
//      Teclas comuns
//       'o' - Ativar/Desativar Algoritmo Frustum Culling e o de Corte de Visão;
//       'esc' - Fechar;
//       '+' - Aumentar a velocidade;
//       '-' - Reduzir a velocidade;
//       'r' - Subir no mapa;
//       'f' - Descer no mapa;
//       'w' - andar para frente no mapa;
//       's' - andar para trás no mapa;
//       'a' - andar para a esquerda no mapa;
//       'd' - andar para a direita no mapa.
// **********************************************************************
void keyboard ( unsigned char key, int x, int y ){
    switch ( key ){
    case 'o':
        fruston ? fruston = false : fruston = true;
        break;
    case 27:
        exit ( 0 );
        break;
    case '+':
        velocity+=10;
        cout << velocity << endl;
        break;
    case '-':
        velocity-=10;
        cout << velocity << endl;
        break;
    case 'r':
        yLook+= velocity;
        break;
    case 'f':
        yLook-= velocity;
        break;
    case 'a':
        xLook-= velocity;
        break;
    case 'd':
        xLook+= velocity;
        break;
    case 'w':
        zLook-= velocity;
        break;
    case 's':
        zLook+= velocity;
        break;
    default:
        break;
    }
}

// **********************************************************************
//      Teclas especiais
//       Seta para cima - Fullscreen;
//       Seta para baixo - Reduz janela.
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y ){
    switch ( a_keys ){
        case GLUT_KEY_UP:
            glutFullScreen ( );
            break;
        case GLUT_KEY_DOWN:
            glutInitWindowSize  ( 700, 500 );
            break;
        default:
            break;
    }
}

// **********************************************************************
//      Main.
//        Lê uma imagem world.png para a escala de
//        alturas em tons de cinza.
// **********************************************************************
int main ( int argc, char** argv ){
    Image = new ImageClass();
    //int r = Image->Load("mapa.jpg");
    int r = Image->Load("world.png");

    if (!r)
        exit(1);

    xLook = (Image->SizeX() * MAP_SCALE) / 2 ;
    zLook = 500;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );

    glutInitWindowPosition (0,0);
    glutInitWindowSize  ( 700, 500 );
    glutCreateWindow    ( "Computacao Grafica - Augusto Weiand" );

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
