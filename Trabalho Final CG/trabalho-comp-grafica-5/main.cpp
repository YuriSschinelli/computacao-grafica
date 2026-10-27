#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "RgbImage.h"

// --- Variáveis Globais de Câmera e Interação ---
float angleX = 20.0f;
float angleY = -30.0f;
float panX = 0.0f;
float panY = 0.0f;
float zoom = 15.0f;

int lastMouseX = 0, lastMouseY = 0;
bool mouseLeftDown = false;
bool mouseRightDown = false;

// --- Variáveis de Animação e Movimento da Formiga ---
float walkCycle = 0.0f;
bool isWalking = false;
float headAngleY = 0.0f;
float headAngleX = 0.0f;
float antennaAngle = 0.0f;
float mandibleAngle = 0.0f;

// Identificadores de Textura
GLuint textureIDs[5];

// --- Carregamento de Texturas ---
void loadTextures() {
glGenTextures(6, textureIDs);
    // Configuração de alinhamento para evitar distorção em BMPs
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(6, textureIDs);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 1. Abdômen
    RgbImage imgThorax("thorax.bmp");
    if (imgThorax.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgThorax.GetNumCols(), imgThorax.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgThorax.ImageData());
    }

    // 2. Tórax
    RgbImage imgAbdomen("abdomen.bmp");
    if (imgAbdomen.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgAbdomen.GetNumCols(), imgAbdomen.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgAbdomen.ImageData());
    }

    // 3. Pernas (Cristais)
    RgbImage imgPernas("legs.bmp");
    if (imgPernas.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[2]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgPernas.GetNumCols(), imgPernas.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgPernas.ImageData());
    }

    // 4. Olhos
    RgbImage imgOlhos("eyes.bmp");
    if (imgOlhos.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[3]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgOlhos.GetNumCols(), imgOlhos.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgOlhos.ImageData());
    }

    // 5. Cabeça
    RgbImage imgCabeca("head.bmp");
    if (imgCabeca.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[4]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgCabeca.GetNumCols(), imgCabeca.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgCabeca.ImageData());
    }

    // 6. Solo (Deserto)
    RgbImage imgSolo("sand.bmp");
    if (imgSolo.ImageLoaded()) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[5]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgSolo.GetNumCols(), imgSolo.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, imgSolo.ImageData());
    }
}

void setStoneMaterial() {
    GLfloat matAmbient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat matDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f }; // 1.0 deixa a textura 100% visível sob a luz
    GLfloat matSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Brilho de pedra muito sutil
    GLfloat shininess[]   = { 10.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void setCrystalMaterial() {
    // Cor base vermelha ou ciano translúcida
    GLfloat matAmbient[]  = { 0.2f, 0.0f, 0.0f, 1.0f };
    GLfloat matDiffuse[]  = { 0.9f, 0.1f, 0.1f, 1.0f };
    // Especularidade máxima para criar o reflexo brilhante de gema/cristal
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess[]   = { 120.0f }; // Brilho altamente concentrado (ponto de luz vivo)

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

// --- Função para Desenhar uma Perna Articulada ---
void drawLeg(float side, float zOffset, float phase, GLUquadric* quadric) {
setCrystalMaterial(); // Aplica propriedades de reflexo de cristal nas pernas

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, zOffset);

    // Junta Coxa (Rotação baseada no ciclo de caminhada)
    float legRot = isWalking ? sin(walkCycle + phase) * 25.0f : 0.0f;
    glRotatef(legRot, 1.0f, 0.0f, 0.0f);

    // Ativa a textura de cristal das pernas (legs.bmp)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[2]);

    // Coxa
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, 0.1f, 0.1f, 0.8f, 12, 12);
    glPopMatrix();

    // Fêmur (Articulação intermediária)
    glTranslatef(0.0f, -0.8f, 0.0f);
    glRotatef(30.0f + (isWalking ? cos(walkCycle + phase) * 15.0f : 0.0f), 0.0f, 0.0f, 1.0f * side);

    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, 0.08f, 0.08f, 1.2f, 12, 12);
    glPopMatrix();

    // Tíbia (Extremidade)
    glTranslatef(0.0f, -1.2f, 0.0f);
    glRotatef(-45.0f, 0.0f, 0.0f, 1.0f * side);

    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadric, 0.06f, 0.06f, 1.0f, 12, 12);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

// --- Desenho da Formiga Robô ---
void drawAnt() {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricTexture(quadric, GL_TRUE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    // 1. Abdômen (Traseira)
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -2.2f);
    glScalef(1.2f, 1.2f, 2.0f);
    setStoneMaterial();
    glBindTexture(GL_TEXTURE_2D, textureIDs[1]);
    glEnable(GL_TEXTURE_2D);
    gluSphere(quadric, 1.0f, 20, 20);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // 2. Tórax (Centro)
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glScalef(0.9f, 0.9f, 1.5f);
    glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
    glEnable(GL_TEXTURE_2D);
    gluSphere(quadric, 0.8f, 20, 20);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // --- 6 Pernas (3 Pares ligados ao Tórax) ---
    // Lado Esquerdo (side = 1.0)
    drawLeg(1.0f, 0.6f, 0.0f, quadric);
    drawLeg(1.0f, 0.0f, 3.14f, quadric);
    drawLeg(1.0f, -0.6f, 0.0f, quadric);

    // Lado Direito (side = -1.0)
    drawLeg(-1.0f, 0.6f, 3.14f, quadric);
    drawLeg(-1.0f, 0.0f, 0.0f, quadric);
    drawLeg(-1.0f, -0.6f, 3.14f, quadric);

    // 3. Cabeça (Frente com rotação independente)
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 1.5f);
    glRotatef(headAngleY, 0.0f, 1.0f, 0.0f); // Rotação autônoma da cabeça
    glRotatef(headAngleX, 1.0f, 0.0f, 0.0f);

    // Estrutura da Cabeça
    glPushMatrix();
    glScalef(1.0f, 0.9f, 0.9f);
    glBindTexture(GL_TEXTURE_2D, textureIDs[4]);
    glEnable(GL_TEXTURE_2D);
    gluSphere(quadric, 0.7f, 20, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Olhos (Esquerdo e Direito)
    setCrystalMaterial();
    glPushMatrix();
    glTranslatef(0.5f, 0.15f, 0.15f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[3]);
    gluSphere(quadric, 0.25f, 10, 10);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(-0.5f, 0.15f, 0.15f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[3]);
    gluSphere(quadric, 0.25f, 10, 10);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Antenas (Articuladas com movimento independente)
    glColor3f(0.2f, 0.2f, 0.2f); // Cinza escuro/metal
    setStoneMaterial();

    // Antena Esquerda
    glPushMatrix();
    glTranslatef(0.3f, 0.5f, 0.5f);
    glRotatef(20.0f + antennaAngle, 20.0f, 0.0f, 1.0f);
    glScalef(0.08f, 0.8f, 0.08f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[4]);
    gluCylinder(quadric, 0.4f, 0.2f, 0.8f, 10, 10);
    glPopMatrix();

    // Antena Direita
    glPushMatrix();
    glTranslatef(-0.3f, 0.5f, 0.5f);
    glRotatef(20.0f - antennaAngle, 10.0f, 0.0f, -1.0f);
    glScalef(0.08f, 0.8f, 0.08f);
    gluCylinder(quadric, 0.4f, 0.2f, 0.8f, 10, 10);
    glPopMatrix();

    // Mandíbula Esquerda
    glPushMatrix();
    glTranslatef(-0.2f, -0.2f, 0.6f);
    glRotatef(20.0f - mandibleAngle, 0.0f, 1.0f, 0.0f); // Abre para a esquerda
    glBindTexture(GL_TEXTURE_2D, textureIDs[3]);
    glScalef(0.08f, 0.08f, 0.5f);
    gluCylinder(quadric, 1.5f, 0.0f, 1.0f, 10, 10);
    glPopMatrix();

    // Mandíbula Direita
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.6f);
    glRotatef(-20.0f + mandibleAngle, 0.0f, 1.0f, 0.0f); // Abre para a direita (sentido oposto)
    glScalef(0.08f, 0.08f, 0.5f);
    gluCylinder(quadric, 1.5f, 0.0f, 1.0f, 10, 10);
    glPopMatrix();

    glPopMatrix();// Fim da cabeça

    glDisable(GL_TEXTURE_2D);

    gluDeleteQuadric(quadric);

}
// --- Callback de Desenho ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Configuração da Câmera (Zoom e Pan)
    glTranslatef(panX, panY, -zoom);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glColor3f(0.85f, 0.72f, 0.50f);
    // Desenhar Plano de Fundo (deserto)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureIDs[5]);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-30.0f, -2.6f, -30.0f);
        glTexCoord2f(5.0f, 0.0f); glVertex3f( 30.0f, -2.6f, -30.0f);
        glTexCoord2f(5.0f, 5.0f); glVertex3f( 30.0f, -2.6f,  30.0f);
        glTexCoord2f(0.0f, 5.0f); glVertex3f(-30.0f, -2.6f,  30.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Desenhar a Formiga Robô
    drawAnt();

    glutSwapBuffers();
}

// --- Callback de Redimensionamento (Viewport e Projeção) ---
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// --- Callback de Teclado ---
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '1': zoom -= 0.5f; break; // Zoom In
        case '2': zoom += 0.5f; break; // Zoom Out
        case ' ': isWalking = !isWalking; break; // Alternar Caminhada
        case 'a': if (headAngleY > 90.0f) headAngleY = 90.0f; headAngleY += 5.0f; break;     // Girar cabeça esquerda
        case 'w': if (headAngleX < -90.0f) headAngleX = -90.0f; headAngleX -= 5.0f; break;
        case 's': if (headAngleX > 90.0f) headAngleX = 90.0f; headAngleX += 5.0f; break;
        case 'd': if (headAngleY < -90.0f) headAngleY = -90.0f; headAngleY -= 5.0f; break;     // Girar cabeça direita
        case 'q': if (antennaAngle > 20.0f) antennaAngle = 20.0f; antennaAngle += 5.0f; break;   // Mover antenas
        case 'e': if (antennaAngle < -20.0f) antennaAngle = -20.0f; antennaAngle -= 5.0f; break;   // Mover antenas
        case 'z': if (mandibleAngle > 40.0f) mandibleAngle = 40.0f; mandibleAngle += 5.0f; break; //Expande as mandibulas
        case 'x': if (mandibleAngle < 0.0f) mandibleAngle = 0.0f; mandibleAngle -= 5.0f; break; //Contrai as mandibulas
        case 27: exit(0);                        // ESC para sair
    }
    if (zoom < 2.0f) zoom = 2.0f;
    glutPostRedisplay();
}

// --- Callback de Mouse (Pan e Rotação) ---
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            mouseLeftDown = false;
        }
    }
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseRightDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else if (state == GLUT_UP) {
            mouseRightDown = false;
        }
    }
}

void motion(int x, int y) {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    if (mouseLeftDown) {
        angleX += dy * 0.5f;
        angleY += dx * 0.5f;
    } else if (mouseRightDown) {
        panX += dx * 0.02f;
        panY -= dy * 0.02f;
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

// --- Timer para Animação ---
void timer(int value) {
    if (isWalking) {
        walkCycle += 0.15f;
        if (walkCycle > 6.28f) walkCycle = 0.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

// --- Função Principal ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Vestigios Formiga Golem - OpenGL 1.x");
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
        // Configuração de luz ambiente
    GLfloat ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Posição inicial da luz na cena
    GLfloat lightPos[] = { 5.0f, 5.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glClearColor(0.85f, 0.65f, 0.45f, 1.0f);

    loadTextures();
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
