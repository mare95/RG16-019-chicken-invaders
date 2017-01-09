#include<GL/glut.h>
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define TIMER_ID1 0
#define TIMER_INTERVAL1 50
#define TIMER_ID2 1
#define TIMER_INTERVAL2 50
#define TIMER_ID3 2
#define TIMER_INTERVAL3 50

#define GORE_MAX 6
#define DOLE_MAX 0
#define LEVO_MAX -20.1
#define DESNO_MAX 20.1
#define LEVO_MAX_K -15
#define DESNO_MAX_K 15

#define MAX_BULLET_NUMBER 100
#define SPEED 0.5 /*brzina kojom se krece avion*/
#define BULLET_SPEED 3

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

/*komponente za metak*/
GLfloat ambient_z[] 	= {0.1, 1, 0.3, 1 }; /*plavo*/
GLfloat difuz_z[] 	= {0.1, 1, 0.1, 1};

static float bullet_xpos[MAX_BULLET_NUMBER];
static float bullet_ypos[MAX_BULLET_NUMBER];


static int window_width;
static int window_height;

static float v_x=1.5; /*brzina kokoske po x*/

typedef struct{
double x_pos;
double y_pos;
bool alive;
}chicken;
chicken Chicken[10];


static float x_curr, y_curr;    /* Tekuce koordinate aviona. */
static int napred=0;/*samo da nije 1 ili -1*/
static int levo=0;/*flegovi smera kretanja aviona*/

static float cx_curr, cy_curr;	/* Tekuce koordinate kokoske*/
static float bullety_curr;
static float bulletx_curr;
static bool fire;
static int run;

static int animation_ongoing;   /* Fleg koji odredjuje da li je
                                 * animacija u toku. */
void drawChicken(int x, int y, int z);
void drawPlane();
void drawBullet();
void drawColider(int x, int y, int z);
void drawColider1(int x, int y, int z);

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer1(int value);
static void on_timer2(int value);
static void on_timer3(int value);
void inicijalizacija_osvetljenja();
void kolizija_avion_kokoska();

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
	bullety_curr=0;
	bulletx_curr=0;
	fire=false;

	/*inicijalizacija pozicija za metke*/
        for (int i = 0; i < MAX_BULLET_NUMBER; i++) {
            bullet_xpos[i] = 0;
            bullet_ypos[i] = -1;
        }

    	animation_ongoing = 0;

	inicijalizacija_osvetljenja();

	glClearColor(0.1, 0.1, 0.1, 0);
	glEnable(GL_DEPTH_TEST);

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
			break;
    		case 'a':
		case 'A':
			levo=1;
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
			glutTimerFunc(TIMER_INTERVAL3, on_timer3, TIMER_ID3);
        		animation_ongoing = 1;
			}
      		  break;
		case 'Q':
		case 'q':
			glutFullScreen();
			break;
		case ' ':
       			 /* Trazimo prazno mesto tj gledamo koji metak je otisao predaleko i na to
			mesto postavljamo novi metak */
       		 	{
         		   int i, run = 1;
         		   for (i = 0; run && (i < MAX_BULLET_NUMBER); i++)
          		    if (bullet_ypos[i] < 0) {
          		      bullet_ypos[i] = 0;
          		      run = 0;
          		  }
        		}
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
	drawBullet();/*nije najsrecnije resenje jer su metak i avion u fazi, mora fix*/
	glTranslatef(-x_curr, -y_curr, 0);/*ponistavamo transformaciju da ne utice na kokoske*/
	
	/*iscrtavanje 3 reda kokoski*/
	glPushMatrix();
	glTranslatef(-22+cx_curr, 20+cy_curr, 0);
	for(int i=0; i<10;i++){
		drawChicken(4, 0, 0);
	}
	/*glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){
		drawChicken(4, 0, 0);
	}
	glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){
		drawChicken(4, 0, 0);
	}*/
	glPopMatrix();
	kolizija_avion_kokoska();
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
/*metak*/
void drawBullet(){
	for (int i = 0; i < MAX_BULLET_NUMBER; i++)
        	if (bullet_ypos[i] >= 0){
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_z);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_z);
			glColor3f(0.7, 0.7, 0.8);
			glScalef(0.07,0.4,0.1);
			glTranslatef(0,-25,0);
			glTranslatef(bullet_xpos[i],bullet_ypos[i],0);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		}
}

/*kokoska*/
void drawColider(int x, int y, int z){
	glTranslatef(x, y, z);
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tp);
		glScalef(3, 3, 0.5);
		glutSolidCube(1);
	glPopMatrix();
	glTranslatef(-x,-y,-z);
}
void drawColider1(int x, int y, int z){
	glTranslatef(x, y, z);
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tp);
		glScalef(40, 3, 0.5);
		glutSolidCube(1);
	glPopMatrix();
	glTranslatef(-x,-y,-z);
}
void drawChicken(int x, int y, int z){
	glTranslatef(x, y, z);
	glPushMatrix();
		/*telo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_c);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_c);
		glColor3f(0.5, 0.1, 0.1);
		glTranslatef(0, 0, 0);
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
		if (animation_ongoing)
			glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
	}
}

static void on_timer2(int value){
    /*proverava se da li poziv dolazi od odgovarajuceg tajmera*/
	if (value != TIMER_ID2)
		return;

	cx_curr += v_x;
	if (cx_curr <= LEVO_MAX_K){ /*leva granica*/
		cy_curr=cy_curr-0.5; 
		v_x *= -1; /*menjamo smer kretanja*/
	}
	else if(cx_curr <= LEVO_MAX_K+1){
		cy_curr=cy_curr-0.5;
	}
	if (cx_curr >= DESNO_MAX_K){ /*desna granica*/
		cy_curr=cy_curr-0.5;
		v_x *= -1;
	}
	else if (cx_curr >= DESNO_MAX_K-1){
		cy_curr=cy_curr-0.5;
	}

	napred=0;
	levo=0;
	glutPostRedisplay();
	if (animation_ongoing) 
		glutTimerFunc(TIMER_INTERVAL2, on_timer2, TIMER_ID2);
}
static void on_timer3(int value){
	if (value != TIMER_ID3)
		return;
        for (int i = 0; i < MAX_BULLET_NUMBER; i++)
        	if (bullet_ypos[i] >= 0) {
        		bullet_ypos[i] += BULLET_SPEED;
        		if (bullet_ypos[i] > 80)
        			bullet_ypos[i] = -1;
        	}

	glutPostRedisplay();
	if (animation_ongoing)
		glutTimerFunc(TIMER_INTERVAL3, on_timer3, TIMER_ID3);
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

void kolizija_avion_kokoska(){
	float ax=x_curr;
	float ay=y_curr-11.5;
	float bx=cx_curr;
	float by=cy_curr+20;
	
	if(ax>bx-20 && ax<bx+20 && ay>by-3.4 && ay<by)
		animation_ongoing=0;

}

