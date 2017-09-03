#include<GL/glut.h>
#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#define TIMER_ID1 0
#define TIMER_INTERVAL1 35

/*pocetak ogranicenja u kretanju aviona*/
#define GORE_MAX 6 
#define DOLE_MAX 0
#define LEVO_MAX -20.1
#define DESNO_MAX 20.1
/*kraj ogranicenja u kretanju aviona*/

/*pocetak ogranicenja u kretanju kokoske*/
#define LEVO_MAX_K -15
#define DESNO_MAX_K 15
/*kraj ogranicenja u kretanju kokoske*/

#define MAX_BULLET_NUMBER 100
/*brzina kojom se krece avion*/
#define SPEED 0.7
#define BULLET_SPEED 3
/*broj metaka koji je potreban za ubijanje boss kokoske*/
#define BOSS_CHICKEN_STRENGTH 100
#define EGG_MAX_Y 30

/*komponente za avion*/
GLfloat ambient_tp[] 	= {0.6, 0.2, 0.2, 1}; /*tamno plavo*/
GLfloat difuz_tp[] 	= {0.1, 0.1, 0.5, 1};
GLfloat ambient_sp[] 	= {0.6, 0.2, 0.2, 1}; /*svetlo plavo*/
GLfloat difuz_sp[] 	= {0.1, 0.1, 0.6, 1};

/*komponente za kokosku*/
GLfloat ambient_c[] 	= {0.3, 0.3, 0.7, 1 }; /*crveno*/
GLfloat difuz_c[] 	= {0.5, 0.1, 0.1, 1};
GLfloat ambient_ttc[] 	= {0.3, 0.3, 0.7, 1 }; /*tamno tamno crveno*/
GLfloat difuz_ttc[] 	= {0.6, 0.1, 0.1, 1};
GLfloat ambient_tc[] 	= {0.1, 0.1, 0.3, 1 }; /*tamno crveno*/
GLfloat difuz_tc[] 	= {0.4, 0.1, 0.1, 1};

/*komponente za metak*/
GLfloat ambient_z[] 	= {0.1, 1, 0.3, 1 }; /*zeleno*/
GLfloat difuz_z[] 	= {0.1, 1, 0.1, 1};


/*komponente za boss kokosku*/
GLfloat ambient_g[] 	= {0.3, 0.3, 0.3, 1 }; /*zlatno*/
GLfloat difuz_g[] 	= {1, 0.7, 0.1, 1};
GLfloat ambient_w[] 	= {0.2, 0.2, 0.2, 1 }; /*bela*/
GLfloat difuz_w[] 	= {0.9, 0.9, 0.9, 1};
GLfloat ambient_lw[] 	= {0.1, 0.1, 0.1, 1 }; /*svetlija bela*/
GLfloat difuz_lw[] 	= {0.1, 0.1, 0.1, 1};

static float bullet_xpos[MAX_BULLET_NUMBER]; /*x koordinata metka*/
static float bullet_ypos[MAX_BULLET_NUMBER]; /*y koordinata metka*/

static int window_width;
static int window_height;

/*flegovi smera kretanja aviona pocetak*/
static int napred=0;/*samo da nije 1 ili -1*/
static int levo=0;
/*flegovi smera kretanja aviona kraj*/

static float x_curr, y_curr;    /* Tekuce koordinate aviona. */
static float cx_curr, cy_curr;	/* Tekuce koordinate kokoske (chicken x, chicken y))*/
static float bcx_curr, bcy_curr; /* Tekuce koordinate boss kokoske (big chicken x, big chicken y)*/
static float eggx_curr, eggy_curr; /* Tekuce koordinate jajeta kokoske*/
static float bex_curr, bey_curr; /*Tekuce koordinate velikog jajeta (big egg x, big egg y)*/

static int bossChickenHealth; /*Holiko metaka moze istrpeti boss kokoska*/
static float BOSS_CHICKEN_SPEED = 0.1;

int numOfDeathChickens;
static float v_x=0.4; /*brzina kokoske po x*/
bool fullScreen;
bool lvl2; /*Indikator za iscrtavanje drugog nivoa*/
static int animation_ongoing;   /* Fleg koji odredjuje da li je                   
              * animacija u toku. */
void drawPlane();
void drawPlaneColider();
void drawBullet();
void drawBulletColider();
void drawChicken();
void drawChickenColider();
void drawChickenEgg();
void drawChickenEggColider();
void drawBossChicken();
void drawBossChickenColider();
void drawBossChickenEgg();
void drawBossChickenEggColider();

void drawElectricHorns();

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);

static void on_timer1(int value);

void kolizija_avion_kokoska();
void kolizija_avion_boss_kokoska();
void kolizija_avion_boss_jaja();
void kolizija_jaje_avion();

void inicijalizacija_osvetljenja();
void init();

typedef struct{
	double x;
	double y;
	bool alive;
}Kokoska;

typedef struct{
	double x;
	double y;
	bool go;
}Jaje;

Kokoska kokoske1[10], kokoske2[10], kokoske3[10];
Jaje jaje1[10];

int main(int argc, char **argv){
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(1200, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glutFullScreen();

	init();

	glutMainLoop();
	return 0;
}

void init(){

	glutDisplayFunc(on_display);
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	fullScreen=true;

	srand(time(NULL));

    	x_curr = 0;
    	y_curr = 0;
	cx_curr=0;
	cy_curr=0;
	bcx_curr=0;
	bcy_curr=0;
	bex_curr=0;
	bey_curr=0;
	eggx_curr=0;
	eggy_curr=0;
	
	/*inicijalizacija pozicija za metke*/
        for (int i = 0; i < MAX_BULLET_NUMBER; i++) {
            bullet_xpos[i] = 0;
            bullet_ypos[i] = -1;
        }
	numOfDeathChickens=0;
	lvl2=false;
	bossChickenHealth=BOSS_CHICKEN_STRENGTH;

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
		jaje1[i].x=0;
		jaje1[i].y=0;
		jaje1[i].go=false;
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
		case 'r':
		case 'R':
			init();
			break;
    		case 'g':
   		case 'G':
    		    /* Pokrecemo igru */
      		  if (animation_ongoing==0) {
        		glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
        		animation_ongoing = 1;
			}
      		  break;
		case 'Q':
		case 'q':
			fullScreen = !fullScreen;
			if(fullScreen){
				glutFullScreen();
			}
			else{
				glutReshapeWindow(1200, 800);
				glutInitWindowPosition(100, 100);
			}
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

	glPushMatrix();
		glTranslatef(x_curr, y_curr, 0);
		drawPlane();
		drawBullet();/*nije najsrecnije resenje jer su metak i avion u fazi, mora fix*/
		//drawBulletColider();
	glPopMatrix();
	//drawPlaneColider();

	/*iscrtavanje 3 reda kokoski*/
	glPushMatrix();
		glTranslatef(cx_curr, 10+cy_curr, 0);
		for(int i=0; i<10;i++){
			glTranslatef(4, 0, 0);
			if(kokoske1[i].alive){
				drawChicken();
				glPushMatrix();
					if(jaje1[i].go){
						glTranslatef(eggx_curr-22-cx_curr, -eggy_curr+11, 0);
						drawChickenEgg();
					}
				glPopMatrix();
			}
			kokoske1[i].x=(cx_curr+4*i-19.6);
			kokoske1[i].y=cy_curr+20.5;
			jaje1[i].x=kokoske1[i].x+1-cx_curr + eggx_curr;
			jaje1[i].y=kokoske1[i].y-1-eggy_curr;
			if(eggy_curr==0 && rand() % 10 ==i)//ako je jaje na pocetnoj poziciji moze da krene
				jaje1[i].go=true;
			else if(eggy_curr==30)//ako je jaje na zavrsnoj poziciji ne moze da krene odmah
				jaje1[i].go=false;
		}
		
		glTranslatef(-40, -4, 0);
		for(int i=0; i<10;i++){
			glTranslatef(4, 0, 0);
			if(kokoske2[i].alive)
				drawChicken();
			kokoske2[i].x=(cx_curr+4*i-19.6);
			kokoske2[i].y=cy_curr+16.5;
		}
		glTranslatef(-40, -4, 0);
		for(int i=0; i<10;i++){
			glTranslatef(4, 0, 0);
			if(kokoske3[i].alive)
				drawChicken();
			kokoske3[i].x=(cx_curr+4*i-19.6);
			kokoske3[i].y=cy_curr+12.5;
		}
	glPopMatrix();

	if(lvl2){
		glPushMatrix();	
			drawElectricHorns();
		glPopMatrix();
		glPushMatrix();
			glTranslatef(bcx_curr, -bcy_curr, 0);
			drawBossChicken();
			glTranslatef(0, -bey_curr, 0);
			drawBossChickenEgg();
		glPopMatrix();
		if(!bossChickenHealth){
			lvl2=false;
			init();
		}
		//drawBossChickenColider();
		//drawBossChickenEggColider();
	}
	//drawChickenEggColider();
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
		glScalef(0.5, 1, 3);
		glTranslatef(0, -1, 0);
		glutSolidCube(1);
	glPopMatrix();
}

/*avion - kolajder*/
void drawPlaneColider(){

	glColor3f(0, 0.5, 0.5);
	glBegin(GL_POLYGON);
		glVertex3f(x_curr - 1.5, +y_curr-10, 0);//gornja leva
		glVertex3f(x_curr +1.5,y_curr-10, 0);
		glVertex3f(x_curr +1.5,y_curr-13, 0);//donja desna
		glVertex3f(x_curr - 1.5,y_curr-13, 0);
	glEnd();
}

/*metak*/
void drawBullet(){

	for (int i = 0; i < MAX_BULLET_NUMBER; i++)
        	if (bullet_ypos[i] >= 0){
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
			glColor3f(0.7, 0.7, 0.8);
			glScalef(0.07,0.4,0.1);
			glTranslatef(0,-25,0);
			glTranslatef(bullet_xpos[i],bullet_ypos[i],0);
			glutSolidSphere(1, 10, 10);
		glPopMatrix();
		}	
}

/*metak - kolajder*/
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

/*kokoska*/
void drawChicken(){

	glPushMatrix();
		/*telo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_g);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_g);
		glColor3f(0.5, 0.1, 0.1);
		glTranslatef(-22, 10, 0);
		glScalef(0.5, 1, 0.5);
		glutSolidSphere(1, 10, 10);
		/*glava*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_tc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_tc);
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
		
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_ttc);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_ttc);
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

/*kokoske - kolajder*/
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

/*jaja kokosaka*/
void drawChickenEgg(){

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
		glColor3f(0.7, 0.7, 0.8);
		glScalef(0.06,0.1,0.02);
		glTranslatef(0, -20, 0);//zakomentarisi kad odradis koliziju
		glutSolidSphere(7, 10, 10);
	glPopMatrix();
}

/*jaja kokosaka- kolajder*/
void drawChickenEggColider(){

	glColor3f(0, 0.5, 0.5);
	for(int i=0; i<10;i++){
		if(jaje1[i].go && kokoske1[i].alive){
			glBegin(GL_POLYGON);
				//printf("%lf\n", cx_curr);
				glVertex3f(jaje1[i].x, jaje1[i].y, 0);//gornja leva
				glVertex3f(jaje1[i].x + 1 , jaje1[i].y, 0);
				glVertex3f(jaje1[i].x + 1, jaje1[i].y - 1, 0);//donja desna
				glVertex3f(jaje1[i].x, jaje1[i].y - 1, 0);
			glEnd();
		}	
	}
}


void drawElectricHorns(){

	float distanceX=0;
	float distanceY=0;
	glPushMatrix();
	glTranslatef(bcx_curr, 35-bcy_curr, 0);
	for(int i=0;i<30;i++){
		float distanceX = distanceX + rand()%15+2;
		float distanceY = distanceY + rand()%15+2;
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
			glScalef(0.01,0.01,0.007);
			glTranslatef(distanceX, distanceY, 0);
			glutSolidSphere(7, 10, 10);
		glPopMatrix();
		glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
			glScalef(0.01,0.01,0.007);
			glTranslatef(-distanceX, distanceY, 0);
			glutSolidSphere(7, 10, 10);
		glPopMatrix();
	}
	glPopMatrix();
}
/*boss kokoska iz lvl2*/
void drawBossChicken(){
		glPushMatrix();
		/*telo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
		glColor3f(0.5, 0.1, 0.1);
		glTranslatef(0, 30, 0);
		glScalef(2.5, 3, 2.5);
		glutSolidSphere(1, 10, 10);
		/*glava*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_g);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_g);
		glColor3f(0.7, 0.2, 0.2);
		glTranslatef(0, 1, 0);
		glScalef(0.5, 0.5, .5);
		glutSolidSphere(1, 10, 10);
		/*desno krilo*/
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_g);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_g);
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

/*boss kokoska iz lvl2 - kolajder*/
void drawBossChickenColider(){

	glColor3f(0, 0.5, 0.5);
	glBegin(GL_POLYGON);
		glVertex3f(bcx_curr - 8, -bcy_curr+33, 0);//gornja leva
		glVertex3f(bcx_curr +8,-bcy_curr+33, 0);
		glVertex3f(bcx_curr +8,-bcy_curr +26, 0);//donja desna
		glVertex3f(bcx_curr - 8,-bcy_curr + 26, 0);
	glEnd();
}

/*jaje boss kokoske iz lvl2*/
void drawBossChickenEgg(){

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_w);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, difuz_w);
		glColor3f(0.7, 0.7, 0.8);
		glScalef(0.14,0.4,0.1);
		glTranslatef(0,75,0);
		glutSolidSphere(7, 10, 10);
	glPopMatrix();
}

/*jaje boss kokoske iz lvl2 - kolajder*/
void drawBossChickenEggColider(){

	glColor3f(0, 0.5, 0.5);
	glBegin(GL_POLYGON);
		glVertex3f(bcx_curr - 1, -bey_curr-bcy_curr+33, 0);//gornja leva
		glVertex3f(bcx_curr +1,-bey_curr-bcy_curr+33, 0);
		glVertex3f(bcx_curr +1,-bey_curr-bcy_curr +27, 0);//donja desna
		glVertex3f(bcx_curr - 1,-bey_curr-bcy_curr + 27, 0);
	glEnd();
}

/*kretanje aviona*/
static void on_timer1(int value){

   	/*proverava se da li poziv dolazi od odgovarajuceg tajmera*/
	if (value == TIMER_ID1){

		//kretanje aviona pocetak
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
		napred=0;
		levo=0;
		//kretanje aviona kraj

		//kretanje kokoske pocetak
		cx_curr += v_x;

		if (cx_curr <= LEVO_MAX_K){ /*leva granica*/
			cy_curr=cy_curr-0.1; 
			v_x *= -1;
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
		//kretanje kokoske kraj

		//kretanje metka pocetak
		for (int i = 0; i < MAX_BULLET_NUMBER; i++)
        	if (bullet_ypos[i] >= 0) {
        		bullet_ypos[i] += BULLET_SPEED;
        		if (bullet_ypos[i] > 80)
        			bullet_ypos[i] = -1;
        	}
		//kretanje metka kraj


		
		//jaje kokoske pocetak
		if(eggy_curr<EGG_MAX_Y)
			eggy_curr +=1;
		else{
			eggy_curr =0;
			eggx_curr = cx_curr;
		}
		//jaje kokoske kraj

		//provera da li li je igrac stigao do 2. nivoa
		if(numOfDeathChickens!=30){
			kolizija_avion_kokoska();
			kolizija_jaje_avion();
		}
		else
			lvl2=true;
		//kraj provere

		if(lvl2){
			if(bcy_curr<20)//ako se velika kokoska nije pozicionirala na sredinu ekrana
				bcy_curr += 0.1;
			else{//velika kokoska se zaustavila (implementiramo kretanje jajeta)
				bey_curr += 1;

				int r=rand()%100 + 20;//random vreme ispaljivanja jajeta(duzina puta van ekrana)
				if(bey_curr>r)
					bey_curr=0;

				kolizija_avion_boss_jaja();
				kolizija_avion_boss_kokoska();
			}
		}

		/* Forsira se ponovno iscrtavanje prozora. */
		glutPostRedisplay();

		/* Po potrebi se ponovo postavlja tajmer. */
		if (animation_ongoing)
			glutTimerFunc(TIMER_INTERVAL1, on_timer1, TIMER_ID1);
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

void kolizija_avion_kokoska(){

	for(int i=0;i<10;i++){
		//printf("%d. kokoska: %lf %lf, %s\n",i, kokoske1[i].x, kokoske1[i].y, kokoske1[i].alive? "yes" : "no");
		for(int m=0;m<MAX_BULLET_NUMBER;m++){
			//printf("%d. metak: %lf %lf\n",m, x_curr, bullet_ypos[m]);
			if(kokoske1[i].y+5<=bullet_ypos[m]/2.5 && kokoske1[i].x<=x_curr && kokoske1[i].x + 3 >=x_curr
				&& kokoske1[i].y+8>=bullet_ypos[m]/2.5){
				if(kokoske1[i].alive){
					kokoske1[i].alive=false;
					bullet_ypos[m] = -1;
					numOfDeathChickens++;
					//printf("%d\n", numOfDeathChickens);
				}
				//printf("%d\n", i);
				//animation_ongoing = 0;
				//printf("%d %s\n", i, kokoske1[i].alive ? "true":"false");
			}
			else if(kokoske2[i].y+5<=bullet_ypos[m]/2.5 && kokoske2[i].x<=x_curr && kokoske2[i].x + 3 >=x_curr
				&& kokoske2[i].y+8>=bullet_ypos[m]/2.5){
				if(kokoske2[i].alive){
					kokoske2[i].alive=false;
					bullet_ypos[m] = -1;
					numOfDeathChickens++;
				}
			}
			else if(kokoske3[i].y+5<=bullet_ypos[m]/2.5 && kokoske3[i].x<=x_curr && kokoske3[i].x + 3 >=x_curr
				&& kokoske3[i].y+8>=bullet_ypos[m]/2.5){
				if(kokoske3[i].alive){
					kokoske3[i].alive=false;
					bullet_ypos[m] = -1;
					numOfDeathChickens++;
				}
			}
		}
	}
}

void kolizija_avion_boss_kokoska(){

	for(int i=0;i<MAX_BULLET_NUMBER;i++){
		if(x_curr>=(bcx_curr - 7) && x_curr <= (bcx_curr+7) && bullet_ypos[i]/2.5>=32-bcy_curr
		&& bullet_ypos[i]/2.5<=39-bcy_curr){
			bullet_ypos[i]=-1;
			if(bossChickenHealth>0)
				bossChickenHealth--;
		//printf("%d\n", bossChickenHealth);
		}
	}
}

void kolizija_avion_boss_jaja(){

	if(((bcx_curr - 1) < (x_curr + 1.5)) && ((bcx_curr + 1) >= (x_curr - 1.5)) && 
		((27-bey_curr-bcy_curr) < (y_curr - 11))){
		sleep(1);
		init();
	}
}

void kolizija_jaje_avion(){

	for(int i=0;i<10;i++){
		if(kokoske1[i].alive && jaje1[i].go){
			if(jaje1[i].x + 0.5 >= x_curr -1.5 && jaje1[i].x + 0.5 <= x_curr +1.5 && 
				jaje1[i].y-1 <= y_curr-10){
					init();
					sleep(1);

			}
		}
	}
}
