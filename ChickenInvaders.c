#include<GL/glut.h>
#include<stdio.h>

static int window_width;
static int window_height;

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(on_display);
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3);

	glutMainLoop();
	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(1);
	}
}

static void on_reshape(int width, int height){
	window_width=width;
	window_height=height;
}

static void on_display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*podesavamo pogled valjda*/
	glViewport(0, 0, window_width, window_height);

	/*podesavamo projekciju*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();/*jedinicna matra*/
	gluPerspective(120, window_width/(float)window_height, 1, 100);

	/*podesavamo pogled*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -2, 10, 0, 0, 0, 0, 1, 0);
	
	/*avion*/
	glPushMatrix();
		/*prednje krilo*/
		glColor3f(0, 0, 1);
		glTranslatef(0, -10.7, 0);
		glScalef(2, 0.5, 0.1);
		glutWireCube(1);
	
		/*sredina aviona*/
		glColor3f(0, 0, 1);
		glTranslatef(0, -0.7, 0);
		glScalef(0.1, 5, 2);
		glutWireCube(1);
		
		/*zadnje krilo*/
		glColor3f(0, 0, 1);
		glScalef(15, 0.2, 0.5);
		glTranslatef(0, -1.5, 0);
		glutWireCube(1);

		/*rep*/
		glColor3f(0, 0, 1);
		glScalef(0.5, 1, 3); /*1 = 15 iz prethodnog*/
		glTranslatef(0, -1, 0);
		glutWireCube(1);
	glPopMatrix();
	/*kokoska*/
	glPushMatrix();
		/*telo*/
		glColor3f(1, 0, 0);
		glTranslatef(0, 1, 0);
		glScalef(1, 2, 1);
		glutWireSphere(1, 10, 10);
		/*glava*/
		glColor3f(1, 0, 0);
		glTranslatef(0, 1, 0);
		glScalef(0.5, 0.5, .5);
		glutWireSphere(1, 10, 10);
		
		/*desno krilo*/
		glColor3f(1, 0, 0);
		glScalef(6, 1, 1);
		glTranslatef(0.5, -0.5, 0);
		glutWireCube(1);
		/*levo krilo*/
		glColor3f(1, 0, 0);
		glScalef(1, 1, 1);
		glTranslatef(-1, 0, 0);
		glutWireCube(1);
		
		glColor3f(1, 0, 0);
		glScalef(0.3, 4, 1);
		glTranslatef(-1, -0.5, 0);
		glutWireCube(1);

		glColor3f(1, 0, 0);
		glScalef(1, 1, 1);
		glTranslatef(5.5, 0, 0);
		glutWireCube(1);

		glColor3f(1, 0, 0);
		glScalef(1, 0.7, 1);
		glTranslatef(-1.1, 0.2, 0);
		glutWireCube(1);


		glColor3f(1, 0, 0);
		glScalef(1, 1, 1);
		glTranslatef(-3.5, 0, 0);
		glutWireCube(1);
		
	glPopMatrix();
	glutSwapBuffers();
}
