#include<GL/glut.h>
#include<stdio.h>
#include <time.h>
#include <stdlib.h>

#define TIMER_ID1 0
#define TIMER_INTERVAL1 50
#define TIMER_ID2 1
#define TIMER_INTERVAL2 50

#define GORE_MAX 6
#define DOLE_MAX 0
#define LEVO_MAX -20.1
#define DESNO_MAX 20.1
#define LEVO_MAX_K -15
#define DESNO_MAX_K 15

#define SPEED 0.2 /*brzina kojom se krece avion*/

/*komponente za avion*/
GLfloat ambient_tp[] 	= {0.6, 0.2, 0.2, 1}; /*crveno*/
GLfloat difuz_tp[] 	= {0.1, 0.1, 0.5, 1};
GLfloat ambient_sp[] 	= {0.6, 0.2, 0.2, 1}; /*crveno*/
GLfloat difuz_sp[] 	= {0.1, 0.1, 0.6, 1};

/*komponente za kokosku*/
GLfloat ambient_c[] 	= {0.3, 0.3, 0.7, 1 }; /*plavo*/
GLfloat difuz_c[] 	= {0.5, 0.1, 0.1, 1};
GLfloat ambient_ttc[] 	= {0.3, 0.3, 0.7, 1 }; /*plavo*/
GLfloat difuz_ttc[] 	= {0.6, 0.1, 0.1, 1};
GLfloat ambient_tc[] 	= {0.1, 0.1, 0.3, 1 }; /*plavo*/
GLfloat difuz_tc[] 	= {0.4, 0.1, 0.1, 1};

static int window_width;
static int window_height;

static float v_x=0.2; /*brzina kokoske po x*/

static float x_curr, y_curr;    /* Tekuce koordinate aviona. */
static int napred=0;/*samo da nije 1 ili -1*/
static int levo=0;/*flegovi smera kretanja aviona*/

static float cx_curr, cy_curr;	/* Tekuce koordinate kokoske*/

static int animation_ongoing;   /* Fleg koji odredjuje da li je
                                 * animacija u toku. */
void drawChicken();
void drawPlane();

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer1(int value);
static void on_timer2(int value);
void inicijalizacija_osvetljenja();

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	glutDisplayFunc(on_display);
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);

    	x_curr = 0;
    	y_curr = 0;
	cx_curr=0;
	cy_curr=0;

    	animation_ongoing = 0;

	inicijalizacija_osvetljenja();

	glClearColor(0.1, 0.1, 0.1, 0);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3);

	glutMainLoop();
	return 0;
}

static void on_keyboard(unsigned char key, int x, int y){
	switch(key){
		case 27:
			exit(1);
		case 'w':
		case 'W':
			napred=1;
			/*printf("napred\n");*/
			break;
    		case 'a':
		case 'A':
			levo=1;
			/*printf("levo\n");*/
			break;
   		case 's':
		case 'S':
			napred=-1;
			break;
    		case 'd':
		case 'D':
			levo=-1;
			break;
    		case 'g':
   		case 'G':
    		    /* Pokrecemo igru*/
      		  if (animation_ongoing==0) {
        		glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
			glutTimerFunc(TIMER_INTERVAL2, on_timer2, TIMER_ID2);
        		animation_ongoing = 1;
			}
      		  break;
		case 'Q':
		case 'q':
			glutFullScreen();
			break;
    		case 'z':
    		case 'Z':
        		/* Zaustavljamo igru. */
        		animation_ongoing = 0;
        		break;
    		}
	}

static void on_reshape(int width, int height){
	window_width=width;
	window_height=height;
}

static void on_display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*podesavamo resize*/
	glViewport(0, 0, window_width, window_height);

	/*podesavamo projekciju*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();/*jedinicna matra*/
	gluPerspective(120, window_width/(float)window_height, 1, 100);

	/*podesavamo pogled*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -2, 10, 0, 0, 0, 0, 1, 0);

	glTranslatef(x_curr, y_curr, 0);
	drawPlane();
	glTranslatef(-x_curr, -y_curr, 0);/*ponistavamo transformaciju da ne utice na kokoske*/

	/*iscrtavanje 3 reda kokoski*/
	glTranslatef(cx_curr, 10+cy_curr, 0);
	for(int i=0; i<10;i++){
		glTranslatef(4, 0, 0);
		drawChicken();
	}
	glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){
		glTranslatef(4, 0, 0);
		drawChicken();
	}
	glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){
		glTranslatef(4, 0, 0);
		drawChicken();
	}
	glutSwapBuffers();
}

/*avion*/
void drawPlane(){
	glPushMatrix();
		/*prednje krilo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tp);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tp);
		glColor3f(0.7, 0.7, 0.9);
		glTranslatef(0, -10.7, 0);
		glScalef(2, 0.5, 0.1);
		glutSolidCube(1);
	
		/*sredina aviona*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_sp);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_sp);
		glColor3f(0.3, 0.3, 0.7);
		glTranslatef(0, -0.7, 0);
		glScalef(0.1, 5, 2);
		glutSolidCube(1);
		
		/*zadnje krilo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tp);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tp);
		glColor3f(0.7, 0.7, 0.8);
		glScalef(15, 0.2, 0.5);
		glTranslatef(0, -1.5, 0);
		glutSolidCube(1);

		/*rep*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tp);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tp);
		glColor3f(0.7, 0.7, 0.8);
		glScalef(0.5, 1, 3); /*1 = 15 iz prethodnog*/
		glTranslatef(0, -1, 0);
		glutSolidCube(1);
	glPopMatrix();
}

/*kokoska*/
void drawChicken(){
	glPushMatrix();
		/*telo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_c);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_c);
		glColor3f(0.5, 0.1, 0.1);
		glTranslatef(-22, 10, 0);
		glScalef(0.5, 1, 0.5);
		glutSolidSphere(1, 10, 10);
		/*glava*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_ttc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_ttc);
		glColor3f(0.7, 0.2, 0.2);
		glTranslatef(0, 1, 0);
		glScalef(0.5, 0.5, .5);
		glutSolidSphere(1, 10, 10);
		
		/*desno krilo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tc);
		glColor3f(0.7, 0.1, 0.1);
		glScalef(6, 1, 1);
		glTranslatef(0.5, -0.5, 0);
		glutSolidCube(1);
		/*levo krilo*/
		glColor3f(0.7, 0.1, 0.1);
		glScalef(1, 1, 1);
		glTranslatef(-1, 0, 0);
		glutSolidCube(1);
		
		glColor3f(0.7, 0.1, 0.1);
		glScalef(0.3, 4, 1);
		glTranslatef(-1.2, -0.5, 0);
		glutSolidCube(1);

		glColor3f(0.7, 0.1, 0.1);
		glScalef(1, 1, 1);
		glTranslatef(5.72, 0, 0);
		glutSolidCube(1);

		glColor3f(0.7, 0.1, 0.1);
		glScalef(1, 0.7, 1);
		glTranslatef(-1.1, 0.2, 0);
		glutSolidCube(1);

		glColor3f(0.7, 0.1, 0.1);
		glScalef(1, 1, 1);
		glTranslatef(-3.5, 0, 0);
		glutSolidCube(1);
	glPopMatrix();
}
static void on_timer1(int value){
    /*proverava se da li poziv dolazi od odgovarajuceg tajmera*/
	if (value == TIMER_ID1){
		if(napred==1){
		y_curr += SPEED;
		  	if (y_curr >= GORE_MAX) {
				y_curr = GORE_MAX;
		    	}
		}
		else if(napred==-1){
		y_curr -= SPEED;
			if (y_curr <= DOLE_MAX) {
				y_curr =DOLE_MAX;
		    	}
		}

		if(levo==1){
		x_curr -= SPEED;
			if (x_curr <= LEVO_MAX) {
				x_curr = LEVO_MAX;
			}
		}	
		else if(levo==-1){
		x_curr += SPEED;
		if (x_curr >= DESNO_MAX) {
				x_curr = DESNO_MAX; 
			}
		}

		/* Forsira se ponovno iscrtavanje prozora. */
		glutPostRedisplay();

		/* Po potrebi se ponovo postavlja tajmer. */
		if (animation_ongoing) {
			glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
		}
	}
}

static void on_timer2(int value){
    /*proverava se da li poziv dolazi od odgovarajuceg tajmera*/
	if (value != TIMER_ID2)
		return;

	cx_curr += v_x;
	if (cx_curr <= -15){ /*leva granica*/
		cy_curr=cy_curr-0.1; 
		v_x *= -1; /*menjamo smer kretanja*/
	}
	else if(cx_curr <= -14){
		cy_curr=cy_curr-0.1;
	}
	if (cx_curr >= 15){ /*desna granica*/
		cy_curr=cy_curr-0.1;
		v_x *= -1;
	}
	else if (cx_curr >= 14){
		cy_curr=cy_curr-0.1;
	}

	napred=0;
	levo=0;
	glutPostRedisplay();
	if (animation_ongoing) {
		glutTimerFunc(TIMER_INTERVAL2, on_timer2, TIMER_ID2);
	}
}

void inicijalizacija_osvetljenja(){
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	/*komponente osvetljenja*/
	GLfloat ambient[] = {0.1, 0.1, 0.1, 1};
	GLfloat difuz[] = {1, 1, 1, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, difuz);

	GLfloat shiness = 30;
	glMaterialf(GL_FRONT, GL_SHININESS, shiness);

	/*pozicija svetla*/
	GLfloat pozicija[] = {2, 5, 10, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, pozicija);
}
