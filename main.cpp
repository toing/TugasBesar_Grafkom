#include <windows.h>
#include <GL/glut.h>


double putar = -10.0;
double besar = -20.0;


void piramid(double putar) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, besar);
    glRotatef(putar, 0.0f, 1.0f, 0.0f);
    
    
  //lantai
    glPushMatrix();
    glScaled(0.8, 0.03, 0.8);
    glTranslatef(0.0, -30.2, 0.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5450980392156863, 0.2705882352941176, 0.0745098039215686);
    glutSolidCube(7.0);
    glPopMatrix();
    
    //atap
    glPushMatrix();
    glScaled(0.8, 1.0, 0.8);
    glTranslatef(0.0, -0.8, 0.0);
    glRotated(45, 0, 1, 0);
    glRotated(-90, 1, 0, 0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5450980392156863, 0.2705882352941176, 0.0745098039215686);
    glutSolidCone(4.5, 2, 4, 1);
    glPopMatrix();

    
     //lantai ungu kanan
    glPushMatrix();
    glScaled(0.3, 0.03, 0.8);
    glTranslatef(6.5, -30.2, 7.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5, 0.2, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    
    //lantai ungu kiri
    glPushMatrix();
    glScaled(0.3, 0.03, 0.8);
    glTranslatef(-6.5, -30.2, 7.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5, 0.2, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    
    //badan kanan
    glPushMatrix();
    glScaled(0.05, 0.03, 0.08);
    glTranslatef(39.0, -23.2, 68.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5, 0.2, 0.7);
    glutSolidCube(10.0);
    glPopMatrix();
    
    //kepala kiri
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(19.5, -5.2, 58.2);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5450980392156863, 0.2705882352941176, 0.0745098039215686);
    glutSolidSphere(2.5,20,50);
    glPopMatrix();
    
    //kaki kanan objek kanan
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(21.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.3, 0.43, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    //kaki kiri objek kanan 
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(18.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.3, 0.43, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    
    
    //badan kiri
    glPushMatrix();
    glScaled(0.05, 0.03, 0.08);
    glTranslatef(-39.0, -23.2, 68.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5, 0.2, 0.7);
    glutSolidCube(10.0);
    glPopMatrix();
    
    //kepala kiri
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-19.5, -5.2, 58.2);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.5450980392156863, 0.2705882352941176, 0.0745098039215686);
    glutSolidSphere(2.5,20,50);
    glPopMatrix();
    
     //kaki kiri objek kiri
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(-21.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.3, 0.43, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    //kaki kanan objek kiri 
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(-18.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.3, 0.43, 0.7);
    glutSolidCube(2.0);
    glPopMatrix();
    
    //mata kanan kanan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(20.8, -4.8, 60.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(7.0, 5.0, 9.0);
    glutSolidSphere(0.5,20,50);
    glPopMatrix();
    
    //mata kanan kiri
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(18.8, -4.8, 60.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(7.0, 5.0, 9.0);
    glutSolidSphere(0.5,20,50);
    glPopMatrix();
    
    //mata kiri kiri
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-20.8, -4.8, 60.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(7.0, 5.0, 9.0);
    glutSolidSphere(0.5,20,50);
    glPopMatrix();
    
    //mata kiri kanan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-18.8, -4.8, 60.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(7.0, 5.0, 9.0);
    glutSolidSphere(0.5,20,50);
    glPopMatrix();
    
    //lantai tiang kanan depan bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(11.5, -10.0, 22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri depan bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(-11.5, -10.0, 22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri belakang bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(-11.5, -10.0, -22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    
    //lantai tiang kiri belakang bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(11.5, -10.0, -22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kanan depan atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(17.25, 7.0, 33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri depan atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(-17.25, 7.0, 33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri belakang atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(-17.25, 7.0, -33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kanan belakang atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(17.25, 7.0, -33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.2, 0.2, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //batang tiang kanan depan
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(34.5, -0.2, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.1, 0.2, 0.5);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //batang tiang kiri depan
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(-34.5, -0.2, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.1, 0.2, 0.5);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //batang tiang kiri belakang
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(-34.5, -0.2, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.1, 0.2, 0.5);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //batang tiang kanan belakang
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(34.5, -0.2, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.1, 0.2, 0.5);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //lampu kanan depan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(34.5, 6.4, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(9.0, 9.0, 1.0);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kiri depan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-34.5, 6.4, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(9.0, 9.0, 1.0);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kiri belakang
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-34.5, 6.4, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(9.0, 9.0, 1.0);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kanan belakang
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(34.5, 6.4, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(9.0, 9.0, 1.0);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
      
             
    glFlush();
    glutSwapBuffers();
}



void besar_tam() {
    besar += 1;
    glutPostRedisplay();
}

void besar_kec() {
    besar -= 1;
    glutPostRedisplay();
}

void putar_skanan() {
    putar++;
    glutPostRedisplay();
}

void putar_skiri() {
    putar--;
    glutPostRedisplay();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(.0, .0, .0);
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0f, (GLfloat) 640 / (GLfloat) 480, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void display() {

    GLfloat lightIntensity[] = {1.7f, 1.7f, 1.7f, 1.0f};
    GLfloat light_position3[] = {0.0f, 15.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position3);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
    glFlush();
    glutSwapBuffers();

    piramid(putar);

}

void keyboard(unsigned char key, int putar, int y) {
   
    if (key == 'f' || key == 'F') {
        besar_tam();
    }
    if (key == 'g' || key == 'G') {
        besar_kec();
    }
    if (key == 'b' || key == 'B') {
        putar_skanan();
    }
    if (key == 'v' || key == 'V') {
        putar_skiri();
    }
    if (key == 'q' || key == 'Q') {
        exit(0);
    }

}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("piramid");
    glutDisplayFunc(display);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();
    return 0;
}

