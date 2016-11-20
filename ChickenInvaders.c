#include <GL/glut.h>
#include <stdio.h>
static int width, height;
static void on_reshape(int w, int h);
static void on_keyboard(unsigned char key, int x, int y);
static void on_display(void);

int main(int argc, char **argv){
   /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(1200, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(2);

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27: //na esc se zavrsava nas program
			exit(0); 
			break;
	}
}
static void on_reshape(int w, int h){
	width=w;
	height=h;
}
static void on_display(void){
	/* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(140, width/(float)height,1, 5);

    /* Podesava se vidna tacka. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 2, 
	      0, 0, 0, 
              0, 1, 0);

    /*
     * Kreira se kocka i primenjuje se geometrijska transformacija na
     * istu.
     */
    glPushMatrix();
        glColor3f(0, 0, 1);
        glScalef(1, 2, 1); /*(x,y,z)*/
        glRotatef(0, 0, 1, 0); /*(rotacija levo-desno, rotacija gore-dole,nzm sta je, krivljenje odvratno) */
        glTranslatef(0, .2, 0);
        glutWireCube(1);
    glPopMatrix();

    /* Nova slika se salje na ekran. */
    glutSwapBuffers();

}
