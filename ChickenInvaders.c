#include<GL/glut.h>
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
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

static float v_x=0.4; /*brzina kokoske po x*/
static float BOSS_CHICKEN_SPEED = 0.1;
static float x_curr, y_curr;    /* Tekuce koordinate aviona. */
static int napred=0;/*samo da nije 1 ili -1*/
static int levo=0;/*flegovi smera kretanja aviona*/

static float cx_curr, cy_curr;	/* Tekuce koordinate kokoske*/
static float bcx_curr, bcy_curr;
bool lvl2;

static int bossChickenHealth;

static int animation_ongoing;   /* Fleg koji odredjuje da li je
                                 * animacija u toku. */
//void drawChicken();
void drawPlane();
void drawBullet();
void drawChicken();
void drawChickenColider();
void drawBossChicken();
void drawBossChickenColider();
void drawBulletColider();
static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer1(int value);
static void on_timer2(int value);
static void on_timer3(int value);
void inicijalizacija_osvetljenja();
void kolizija_avion_kokoska();
void kolizija_avion_boss_kokoska();
int numOfDeathChickens;
void init();


typedef struct{
	double x;
	double y;
	bool alive;
}Kokoska;

Kokoska kokoske1[10], kokoske2[10], kokoske3[10];

int main(int argc, char **argv){
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();

	glutMainLoop();
	return 0;
}

void init(){

	glutDisplayFunc(on_display);
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);

    	x_curr = 0;
    	y_curr = 0;
	cx_curr=0;
	cy_curr=0;
	bcx_curr=0;
	bcy_curr=0;

	/*inicijalizacija pozicija za metke*/
        for (int i = 0; i < MAX_BULLET_NUMBER; i++) {
            bullet_xpos[i] = 0;
            bullet_ypos[i] = -1;
        }

	numOfDeathChickens=0;
	lvl2=false;
	bossChickenHealth=5;
	/*inicijalizacija 3 reda kokosaka(colideri njihovi)*/
	for(int i =0; i<10; i++){
		kokoske1[i].x = 0;
		kokoske1[i].y = 0;
		kokoske1[i].alive=true;
		kokoske2[i].x = 0;
		kokoske2[i].y = 0;
		kokoske2[i].alive=true;
		kokoske3[i].x = 0;
		kokoske3[i].y = 0;
		kokoske3[i].alive=true;
	}

    	animation_ongoing = 0;

	inicijalizacija_osvetljenja();
	
	glClearColor(0.1, 0.1, 0.1, 0);
	glEnable(GL_DEPTH_TEST);

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
	//drawBulletColider();
	glTranslatef(-x_curr, -y_curr, 0);/*ponistavamo transformaciju da ne utice na kokoske*/

	/*iscrtavanje 3 reda kokoski*/
	glPushMatrix();
	glTranslatef(cx_curr, 10+cy_curr, 0);
	for(int i=0; i<10;i++){
		glTranslatef(4, 0, 0);
		if(kokoske1[i].alive)
			drawChicken();
		kokoske1[i].x=(cx_curr+4*i-19.6);
		kokoske1[i].y=cy_curr+20.5;
	}

	//crtanje ostalih kokosaka
	/*
	glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){s
		glTranslatef(4, 0, 0);
		drawChicken();
	}
	glTranslatef(-40, -4, 0);
	for(int i=0; i<10;i++){
		glTranslatef(4, 0, 0);
		drawChicken();
	}
	glTranslatef(-40, -12, 0);
	*/
	glPopMatrix();

	if(lvl2){//dodaj uslov za helte
		drawBossChicken();
		drawBossChickenColider();
	}

	
	//drawChickenColider();

	
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
void drawBulletColider(){
	glColor3f(0, 0.5, 0.5);
	for(int i=0; i<MAX_BULLET_NUMBER;i++){
	glBegin(GL_POLYGON);
		glVertex3f(bullet_xpos[i], -9 + bullet_ypos[i]/2.5, 0);//delimo sa 2.5 da bi usporili metak a -9
		glVertex3f(bullet_xpos[i] + 1,-9+ bullet_ypos[i]/2.5, 0);//oduzimamo 9 da ga spustimo dole
		glVertex3f(bullet_xpos[i] + 1,-9 +(bullet_ypos[i] - 1)/2.5, 0);//donja desna
		glVertex3f(bullet_xpos[i], -9 + (bullet_ypos[i] - 1)/2.5, 0);
	glEnd();
	}
}

/*kokoska kolajder*/
void drawChickenColider(){
	glColor3f(0, 0.5, 0.5);
	for(int i=0; i<10;i++){
	glBegin(GL_POLYGON);
		glVertex3f(kokoske1[i].x,kokoske1[i].y, 0);//gornja leva
		glVertex3f(kokoske1[i].x + 3,kokoske1[i].y, 0);
		glVertex3f(kokoske1[i].x + 3,kokoske1[i].y - 2, 0);//donja desna
		glVertex3f(kokoske1[i].x,kokoske1[i].y - 2, 0);
	glEnd();
	}
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

void drawBossChicken(){
glPushMatrix();
		/*telo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_c);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_c);
		glColor3f(0.5, 0.1, 0.1);
		glTranslatef(bcx_curr+0, -bcy_curr+30, 0);
		glScalef(2.5, 3, 2.5);
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

void drawBossChickenColider(){
	glColor3f(0, 0.5, 0.5);
	glBegin(GL_POLYGON);
		glVertex3f(bcx_curr - 8, -bcy_curr+33, 0);//gornja leva
		glVertex3f(bcx_curr +8,-bcy_curr+33, 0);
		glVertex3f(bcx_curr +8,-bcy_curr +26, 0);//donja desna
		glVertex3f(bcx_curr - 8,-bcy_curr + 26, 0);
	glEnd();

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
		cy_curr=cy_curr-0.1; 
		v_x *= -1; /*menjamo smer kretanja*/
	}
	else if(cx_curr <= LEVO_MAX_K+1){
		cy_curr=cy_curr-0.1;
	}
	if (cx_curr >= DESNO_MAX_K){ /*desna granica*/
		cy_curr=cy_curr-0.1;
		v_x *= -1;
	}
	else if (cx_curr >= DESNO_MAX_K-1){
		cy_curr=cy_curr-0.1;
	}

	napred=0;
	levo=0;

	if(numOfDeathChickens!=10)
		kolizija_avion_kokoska();

	if(lvl2){
		
			if(bcy_curr<20)
				bcy_curr += 0.1;
				printf("%f\n", bcy_curr);
	
			kolizija_avion_boss_kokoska();
	}
		

	glutPostRedisplay();
	if (animation_ongoing) 
		glutTimerFunc(TIMER_INTERVAL2, on_timer2, TIMER_ID2);
}
static void on_timer3(int value){
	if (value != TIMER_ID3)
		return;
	/*if(fire)
		bullety_curr+=BULLET_SPEED;*/
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
	for(int i=0;i<10;i++){
			//printf("%d. kokoska: %lf %lf, %s\n",i, kokoske1[i].x, kokoske1[i].y, kokoske1[i].alive? "yes" : "no");
		for(int m=0;m<MAX_BULLET_NUMBER;m++){
			//printf("%d. metak: %lf %lf\n",m, x_curr, bullet_ypos[m]);
			if(kokoske1[i].y+5<=bullet_ypos[m]/2.5 && kokoske1[i].x<=x_curr && kokoske1[i].x + 3 >=x_curr
			&& kokoske1[i].y+8>=bullet_ypos[m]/2.5){
				if(kokoske1[i].alive){
					kokoske1[i].alive=false;
					bullet_ypos[i] = -1;
					numOfDeathChickens++;
					if(numOfDeathChickens==10)
						lvl2=true;
					//printf("%d\n", numOfDeathChickens);
				}
				//printf("%d\n", i);
				//animation_ongoing = 0;
				//printf("%d %s\n", i, kokoske1[i].alive ? "true":"false");
			}
		}
	}

}

void kolizija_avion_boss_kokoska(){
	for(int i=0;i<MAX_BULLET_NUMBER;i++){
		if(x_curr>=(bcx_curr - 7) && x_curr <= (bcx_curr+7) && bullet_ypos[i]/2.5>=32-bcy_curr
		&& bullet_ypos[i]/2.5<=39-bcy_curr){
			animation_ongoing=0;
		}
	}


}
