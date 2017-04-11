// Chess animation starter kit.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;
#include "glut.h"
#include "graphics.h"

enum PIECES { PAWN=10, KNIGHT, BISHOP, ROOK, KING, QUEEN };


// Global Variables
// Some colors you can use, or make your own and add them
// here and in graphics.h
GLdouble redMaterial[] = {0.7, 0.1, 0.2, 1.0};
GLdouble greenMaterial[] = {0.1, 0.7, 0.4, 1.0};
GLdouble brightGreenMaterial[] = {0.1, 0.9, 0.1, 1.0};
GLdouble blueMaterial[] = {0.1, 0.2, 0.7, 1.0};
GLdouble whiteMaterial[] = {1.0, 1.0, 1.0, 1.0};
int myCount = 0;
double screen_x = 600;
double screen_y = 500;


// Outputs a string of text at the specified location.
void text_output(double x, double y, char *string)
{
	void *font = GLUT_BITMAP_9_BY_15;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
	
	int len, i;
	glRasterPos2d(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) 
	{
		glutBitmapCharacter(font, string[i]);
	}

    glDisable(GL_BLEND);
}

// Given the three triangle points x[0],y[0],z[0],
//		x[1],y[1],z[1], and x[2],y[2],z[2],
//		Finds the normal vector n[0], n[1], n[2].
void FindTriangleNormal(double x[], double y[], double z[], double n[])
{
	// Convert the 3 input points to 2 vectors, v1 and v2.
	double v1[3], v2[3];
	v1[0] = x[1] - x[0];
	v1[1] = y[1] - y[0];
	v1[2] = z[1] - z[0];
	v2[0] = x[2] - x[0];
	v2[1] = y[2] - y[0];
	v2[2] = z[2] - z[0];
	
	// Take the cross product of v1 and v2, to find the vector perpendicular to both.
	n[0] = v1[1]*v2[2] - v1[2]*v2[1];
	n[1] = -(v1[0]*v2[2] - v1[2]*v2[0]);
	n[2] = v1[0]*v2[1] - v1[1]*v2[0];

	double size = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	n[0] /= -size;
	n[1] /= -size;
	n[2] /= -size;
}

// Loads the given data file and draws it at its default position.
// Call glTranslate before calling this to get it in the right place.
void DrawPiece(char filename[])
{
	// Try to open the given file.
	char buffer[200];
	ifstream in(filename);
	if(!in)
	{
		cerr << "Error. Could not open " << filename << endl;
		exit(1);
	}

	double x[100], y[100], z[100]; // stores a single polygon up to 100 vertices.
	int done = false;
	int verts = 0; // vertices in the current polygon
	int polygons = 0; // total polygons in this file.
	do
	{
		in.getline(buffer, 200); // get one line (point) from the file.
		int count = sscanf(buffer, "%lf, %lf, %lf", &(x[verts]), &(y[verts]), &(z[verts]));
		done = in.eof();
		if(!done)
		{
			if(count == 3) // if this line had an x,y,z point.
			{
				verts++;
			}
			else // the line was empty. Finish current polygon and start a new one.
			{
				if(verts>=3)
				{
					glBegin(GL_POLYGON);
					double n[3];
					FindTriangleNormal(x, y, z, n);
					glNormal3dv(n);
					for(int i=0; i<verts; i++)
					{
						glVertex3d(x[i], y[i], z[i]);
					}
					glEnd(); // end previous polygon
					polygons++;
					verts = 0;
				}
			}
		}
	}
	while(!done);

	if(verts>0)
	{
		cerr << "Error. Extra vertices in file " << filename << endl;
		exit(1);
	}

}

//
// GLUT callback functions
//

void Interpolate(double t, double t1, double t2, 
	double &v, double v1, double v2)
{
	if(t<t1)
		t = t1;
	if(t>t2)
		t = t2;
	double ratio = (t-t1)/(t2-t1);
	v = v1 + ratio*(v2-v1);
}

void DrawBox(double x1, double y1, double z1, 
			double x2, double y2, double z2)
{
	glBegin(GL_QUADS);

	// front face
	glNormal3d(0,0,-1);
	glVertex3d(x1,y1,z1);
	glVertex3d(x2,y1,z1);
	glVertex3d(x2,y2,z1);
	glVertex3d(x1,y2,z1);
	// right face
	glNormal3d(1,0,0);
	glVertex3d(x2,y1,z1);
	glVertex3d(x2,y1,z2);
	glVertex3d(x2,y2,z2);
	glVertex3d(x2,y2,z1);
	// top face
	glNormal3d(0,1,0);
	glVertex3d(x1,y2,z1);
	glVertex3d(x2,y2,z1);
	glVertex3d(x2,y2,z2);
	glVertex3d(x1,y2,z2);
	// bottom face
	glNormal3d(0,-1,0);
	glVertex3d(x1,y1,z1);
	glVertex3d(x1,y1,z2);
	glVertex3d(x2,y1,z2);
	glVertex3d(x2,y1,z1);
	// back face
	glNormal3d(0,0,1);
	glVertex3d(x1,y1,z2);
	glVertex3d(x1,y2,z2);
	glVertex3d(x2,y2,z2);
	glVertex3d(x2,y1,z2);
	// left face
	glNormal3d(-1,0,0);
	glVertex3d(x1,y1,z1);
	glVertex3d(x1,y2,z1);
	glVertex3d(x1,y2,z2);
	glVertex3d(x1,y1,z2);

	glEnd();
}

void DrawBoard()
{
	//GLfloat board1[] = {0.01, 0.7, 0.4, 1.0};
	GLfloat board1[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat board2[] = {-1.0, -1.0, -1.0, 1.0};
	glBegin(GL_QUADS);
	for(int i=0; i<8; i++)
		for(int j=0; j<8; j++)
		{
			if( (i+j)%2==0 )
				glMaterialfv(GL_FRONT, GL_DIFFUSE, board1);
			else
				glMaterialfv(GL_FRONT, GL_DIFFUSE, board2);
			glNormal3d(0,1,0);
			double offset_x = 500;
			double offset_z = 500;
			glVertex3d(i*1000+offset_x, 0, j*1000+offset_z);
			glVertex3d(i*1000+offset_x, 0, (j+1)*1000+offset_z);
			glVertex3d((i+1)*1000+offset_x, 0, (j+1)*1000+offset_z);
			glVertex3d((i+1)*1000+offset_x, 0, j*1000+offset_z);
			
		
		}
	glEnd();

}

// NOTE: Y is the UP direction for the chess pieces.
double eye[3] = {-4500, 12000, 4500}; // pick a nice vantage point.
double at[3]  = {4500, 0,     4500};

// This callback function gets called by the Glut
// system whenever it decides things need to be redrawn.
void display(void)
{
	static clock_t start = clock();
	clock_t finish = clock();
	double t = (double)(finish - start) / CLOCKS_PER_SEC;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	gluLookAt(eye[0], eye[1], eye[2],  at[0], at[1], at[2],  0,1,0); // Y is up!

//bottom
	glPushMatrix();
	GLfloat myBrown[] = {0.63, 0.32, 0.18, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, myBrown);
	
	DrawBox(500,-1,500,  8500,-499,8500);
	glPopMatrix();

// Set the color for one side (white), and draw its 16 pieces.
	GLfloat mat_amb_diff1[] = {0.9, 0.9, 0.9, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_amb_diff1);

//example bishop rotating
	//double bish_rot = 0;
	//Interpolate(t, 0, 1000, bish_rot, 0, 360*1000);
	//glPushMatrix();
	//glTranslatef(4000, 0, 3000);
	//glRotated(bish_rot, 0,1,0);
	//glRotated(10, 0,0,1);
	//glCallList(BISHOP);
	//DrawBox(-300,0,-300,  300,1000,300);
	//glPopMatrix();

//white bishop right side
	double bishop_x3 = 3000;
	double bishop_z3 = 1000;
	double bishop_rot3 = 0;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(bishop_x3, 0, bishop_z3);
	glTranslatef(0,700,0);
	glRotated(bishop_rot3, 0,0,1);
	glTranslatef(0,-700,0);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

//white king
	glPushMatrix();
	glTranslatef(4000, 0, 1000);
	double king_y_height = 1;
	//Interpolate(t, 5, 7, king_y_height, 1, 0);
	glScaled(1, king_y_height, 1);	
	//DrawPiece("KING.POL");
	glCallList(KING);
	glPopMatrix();

//white queen
	double queen_z = 1000;
	//Interpolate(t, 2, 4, queen_z, 1000, 4000);
	glPushMatrix();
	glTranslatef(5000, 0, queen_z);
	//DrawPiece("QUEEN.POL");
	glCallList(QUEEN);
	glPopMatrix();

//white bishop left side
	if (myCount < 15000){
		double bishop_x = 6000;
		double bishop_z = 4000;
		//double bishop_rot = 0;
		//(,,,,start, stop)
		Interpolate(t, 3, 6, bishop_x, 6000, 0);
		//(,,,,forward)
		Interpolate(t, 3, 6, bishop_z, 1000, 7000);
		//rotates certain amount of times
		//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
		glPushMatrix();
		glTranslatef(bishop_x, 0, bishop_z);
		//glTranslatef(0,700,0);
		//glRotated(bishop_rot, 0,0,1);
		//glTranslatef(0,-700,0);
		//DrawPiece("BISHOP.POL");
		glCallList(BISHOP);
		glPopMatrix();
	}
	
	if (myCount > 15000){
		double bishop_x = 000;
		double bishop_z = 2000;
		double bishop_rot = 0;
		//(,,,,start, stop)
		Interpolate(t, 17.5, 20, bishop_x, 000, 2000);
		//(,,,,forward)
		Interpolate(t, 17.5, 20, bishop_z, 7000, 5000);
		glPushMatrix();
		glTranslatef(bishop_x, 0, bishop_z);
		glCallList(BISHOP);
		glPopMatrix();
	}

//white pawns all
	for(int x=1000; x<=8000; x+=1000)
	{
		if (x == 5000){
			double pawn_z = 1000;
			Interpolate(t, 0, 1, pawn_z, 2000, 4000);
			glPushMatrix();
			glTranslatef(5000, 0, pawn_z);
			//DrawPiece("PAWN.POL");
			glCallList(PAWN);
			glPopMatrix();
			
		} else if (x==1000){
			glPushMatrix();
			glTranslatef(1000, 0, 2000);
			double pawn_y_height = 1;
			//DrawPiece("PAWN.POL");
			Interpolate(t, 15.7, 16, pawn_y_height, 1, 0);
			glScaled(1, pawn_y_height, 1);
			glCallList(PAWN);
			glPopMatrix();
		} else if (x==8000){
			glPushMatrix();
			glTranslatef(8000, 0, 2000);
			double pawn_y_height2 = 1;
			Interpolate(t, 15.7, 16, pawn_y_height2, 1, 0);
			glScaled(1, pawn_y_height2, 1);
			//DrawPiece("PAWN.POL");
			glCallList(PAWN);
			glPopMatrix();
			
			/*glPushMatrix();
	glTranslatef(4000, 0, 1000);
	double king_y_height = 1;
	//Interpolate(t, 5, 7, king_y_height, 1, 0);
	glScaled(1, king_y_height, 1);	
	//DrawPiece("KING.POL");
	glCallList(KING);
	glPopMatrix();*/

		} else {
			glPushMatrix();
			glTranslatef(x, 0, 2000);
			//DrawPiece("PAWN.POL");
			glCallList(PAWN);
			glPopMatrix();
		}
	}

//white rook left side
	double rook_x1 = 8000;
	double rook_z1 = 1000;
	double rook_rot1 = 0;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(rook_x1, 0, rook_z1);
	glTranslatef(0,700,0);
	glRotated(rook_rot1, 0,0,1);
	glTranslatef(0,-700,0);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();


//white rook right side
	double rook_x3 = 1000;
	double rook_z3 = 1000;
	double rook_rot3 = 0;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(rook_x3, 0, rook_z3);
	glTranslatef(0,700,0);
	glRotated(rook_rot3, 0,0,1);
	glTranslatef(0,-700,0);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

//white knight left side
	double knight_x1 = 7000;
	double knight_z1 = 1000;
	double knight_rot1 = 0;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(knight_x1, 0, knight_z1);
	glTranslatef(0,700,0);
	glRotated(knight_rot1, 0,0,1);
	glTranslatef(0,-700,0);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();
 
	

//white knight right side
	double knight_x3 = 2000;
	double knight_z3 = 1000;
	double knight_rot3 = 0;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(knight_x3, 0, knight_z3);
	glTranslatef(0,700,0);
	glRotated(knight_rot3, 0,0,1);
	glTranslatef(0,-700,0);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

// Set the color for one side (black), and draw its 16 pieces.
	GLfloat mat_amb_diff2[] = {0.8, 0.2, 0.2, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_amb_diff2);

//black king
	glPushMatrix();
	glTranslatef(4000, 0, 8000);
	double deg = 0;
	//Interpolate(t, 3, 6, deg, 0, -90);
	glRotated(deg, 0,0,1);
	glCallList(KING);
	glPopMatrix();

//black pawns
	for(int x=1000; x<=8000; x+=1000)
	{
		glPushMatrix();
		double deg = 0;
		Interpolate(t, 10, 11, deg, 0, -90);
		glRotated(deg, 0,0,1);
		glTranslatef(x, 0, 7000);
		//DrawPiece("PAWN.POL");
		glCallList(PAWN);
		glPopMatrix();
	}

//black queen
	double queen_z2 = 8000;
	//Interpolate(t, 2, 4, queen_z, 1000, 4000);
	glPushMatrix();
	glTranslatef(5000, 0, queen_z2);
	//DrawPiece("QUEEN.POL");
	glCallList(QUEEN);
	glPopMatrix();

//black bishop left side
	double bishop_x2 = 6000;
	double bishop_z2 = 8000;
	double bishop_rot2 = 180;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(bishop_x2, 0, bishop_z2);
	glTranslatef(0,700,0);
	glRotated(bishop_rot2, 0,1,0);
	glTranslatef(0,-700,0);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

//black bishop right side
	double bishop_x4 = 3000;
	double bishop_z4 = 8000;
	double bishop_rot4 = 180;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(bishop_x4, 0, bishop_z4);
	glTranslatef(0,700,0);
	glRotated(bishop_rot4, 0,1,0);
	glTranslatef(0,-700,0);
	//DrawPiece("BISHOP.POL");
	glCallList(BISHOP);
	glPopMatrix();

//black rook left side
	double rook_x2 = 1000;
	Interpolate(t, 14, 16, rook_x2, 8000, 2000);
	glPushMatrix();
	glTranslatef(8000, 0, rook_x2);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

//black rook right side
	double rook_x4 = 1000;
	Interpolate(t, 14, 16, rook_x4, 8000, 2000);
	glPushMatrix();
	glTranslatef(1000, 0, rook_x4);
	//DrawPiece("ROOK.POL");
	glCallList(ROOK);
	glPopMatrix();

//black knight left side
	double knight_x2 = 6000;
	double knight_z2 = 5000;
	double knight_rot2 = 180;
	Interpolate(t, 1, 2, knight_x2, 7000, 6000);
	Interpolate(t, 1, 2, knight_z2, 8000, 6000);
	glPushMatrix();
	glTranslatef(knight_x2, 0, knight_z2);
	glRotated(knight_rot2, 0,1,0);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

	/*double bishop_x = 6000;
	double bishop_z = 4000;
	double bishop_rot = 0;
	//(,,,,start, stop)
	Interpolate(t, 2, 6, bishop_x, 6000, 0);
	//(,,,,forward)
	Interpolate(t, 2, 6, bishop_z, 1000, 7000);
	//rotates certain amount of times
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(bishop_x, 0, bishop_z);*/

//black knight right side
	double knight_x4 = 2000;
	double knight_z4 = 8000;
	double knight_rot4 = 180;
	//Interpolate(t, 0, 6, bishop_x, 6000, -4000);
	//Interpolate(t, 0, 6, bishop_z, 1000, 4000);
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	glPushMatrix();
	glTranslatef(knight_x4, 0, knight_z4);
	glTranslatef(0,700,0);
	glRotated(knight_rot4, 0,1,0);
	glTranslatef(0,-700,0);
	//DrawPiece("KNIGHT.POL");
	glCallList(KNIGHT);
	glPopMatrix();

	DrawBoard();

//ammo spawn
	glPushMatrix();
	GLfloat base2[] = {0.0, 0.5, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, base2);
	glPopMatrix();

	double balltestX = 6000;
	double balltestZ = 4000;
	Interpolate(t, 9, 10, balltestX, 0, 15000);
	Interpolate(t, 9, 10, balltestZ, 7000, 7000);
	glPushMatrix();
	glTranslatef(balltestX, 800, balltestZ);
	glutSolidSphere(160, 50, 50);
	
	glPopMatrix();

//black box for base
	glPushMatrix();
	GLfloat base[] = {-1.0, -1.0, -1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, base);
	DrawBox(-15000,-500,4500,  15000,-2000,15000);
	glPopMatrix();

	double balltestX2 = 7000;
	Interpolate(t, 8, 9, balltestX2, 7000, 5000);
	glPushMatrix();
	glTranslatef(0, 800, balltestX2);
	glutSolidSphere(165, 50, 50);
	
	glPopMatrix();

//spikey gold hair
	if(myCount > 5700 && myCount < 14950){
		glPushMatrix();
		GLfloat hair1[] = {1.0, 0.84, 0.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, hair1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0,1500,7000);
		glRotated(180, 0, 1, 1);
		glutSolidCone(100, 200, 10,10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0,1500,7000);
		glRotated(165, 1, 1, 1);
		glutSolidCone(100, 200, 10,10);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0,1500,7000);
		glRotated(200, 1, 1, 0);
		glutSolidCone(100, 200, 10,10);
		glPopMatrix();
	}
	myCount++;
	//cout << myCount << endl;

	
	if (myCount > 4200 && myCount < 5800)
	{
		glPushMatrix();
		GLfloat hair1[] = {0.1, 0.1, 0.1, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, hair1);
		glPopMatrix();
		glPushMatrix();
		text_output(0, 0, "Fools! you are no match for a ");
		glPopMatrix();
	}

	if (myCount > 5850 && myCount < 8000)
	{
		glPushMatrix();
		GLfloat hair1[] = {1.0, 0.84, 0.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, hair1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0,10,0);
		text_output(0, 0, "                               Super Saiyan!!!");
		glPopMatrix();
	}

	if(myCount > 8500 && myCount < 11000)
	{
		glPushMatrix();
		text_output(0, 0, "Mua hahahaha hahah haha");
		glPopMatrix();
	}

	if(myCount > 11000 && myCount < 11900)
	{
		glPushMatrix();
		GLfloat base2[] = {0.8, 0.2, 0.2, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, base2);
		glPopMatrix();
		glPushMatrix();
		text_output(9000, 1000, "Noo...");
		glPopMatrix();
	}

	if(myCount > 11900 && myCount < 14500)
	{
		glPushMatrix();
		GLfloat base2[] = {0.8, 0.2, 0.2, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, base2);
		glPopMatrix();
		glPushMatrix();
		text_output(9000, 1000, "Rooks take them out!");
		glPopMatrix();
	}

	if(myCount > 14900 && myCount < 17300)
	{
		glPushMatrix();
		GLfloat base2[] = {1.0, 0.84, 0.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, base2);
		glPopMatrix();
		glPushMatrix();
		text_output(0, 0, "Ha! You'll never win this way!");
		glPopMatrix();
	}

	if(myCount > 17300 && myCount < 18000)
	{
		glPushMatrix();
		GLfloat base2[] = {.1, .1, .1, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, base2);
		glPopMatrix();
		glPushMatrix();
		text_output(0, 0, "Cause now you're in check!");
		glPopMatrix();
	}
	
	if(myCount > 18050 && myCount < 19350){

		glPushMatrix();
		GLfloat blueB[] = {0.1, 0.5, 1.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blueB);
		glPopMatrix();
		
		glPushMatrix();
		text_output(9000, 1000, "For more adventures");
		glPopMatrix();
	}

	if(myCount > 19400 && myCount < 20450){

		glPushMatrix();
		GLfloat blueB[] = {0.1, 0.5, 1.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blueB);
		glPopMatrix();
		
		glPushMatrix();
		text_output(9000, 1000, "Tune in next time to");
		glPopMatrix();
	}

	if(myCount > 20500){

		glPushMatrix();
		GLfloat blueB[] = {0.1, 0.5, 1.0, 1.0};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blueB);
		glPopMatrix();
		
		glPushMatrix();
		text_output(9000, 1000, "DF Tv!");
		glPopMatrix();
	}

	//white bishop left side
	//double bishop_x = 6000;
	//double bishop_z = 4000;
	//double bishop_rot = 0;
	//(,,,,start, stop)
	//Interpolate(t, 0, 6, bishop_x, 6000, 0);
	//(,,,,forward)
	//Interpolate(t, 0, 6, bishop_z, 1000, 7000);
	//rotates certain amount of times
	//Interpolate(t, 0, 6, bishop_rot, 0, 360*5);
	//glPushMatrix();
	//glTranslatef(bishop_x, 0, bishop_z);
	//glTranslatef(0,700,0);
	//glRotated(bishop_rot, 0,0,1);
	//glTranslatef(0,-700,0);
	//DrawPiece("BISHOP.POL");
	//glCallList(BISHOP);
	//glPopMatrix();

	//double queen_z = 1000;
	//Interpolate(t, 2, 4, queen_z, 1000, 4000);
	//glPushMatrix();
	//glTranslatef(5000, 0, queen_z);
	//DrawPiece("QUEEN.POL");
	//glCallList(QUEEN);
	//glPushMatrix();

	
	//glutSolidTeapot(800);
	//glPopMatrix();

	GLfloat light_position[] = {1,2,-.1, 0}; // light comes FROM this vector direction.
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light

	glutSwapBuffers();
	glutPostRedisplay();
}


// This callback function gets called by the Glut
// system whenever a key is pressed.
void keyboard(unsigned char c, int x, int y)
{
	switch (c) 
	{
		case 27: // escape character means to quit the program
			exit(0);
			break;
		default:
			return; // if we don't care, return without glutPostRedisplay()
	}

	glutPostRedisplay();
}



void SetPerspectiveView(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double aspectRatio = (GLdouble) w/(GLdouble) h;
	gluPerspective( 
	/* field of view in degree */ 38.0,
	/* aspect ratio */ aspectRatio,
	/* Z near */ 100, /* Z far */ 30000.0);
	glMatrixMode(GL_MODELVIEW);
}

// This callback function gets called by the Glut
// system whenever the window is resized by the user.
void reshape(int w, int h)
{
	screen_x = w;
	screen_y = h;

	// Set the pixel resolution of the final picture (Screen coordinates).
	glViewport(0, 0, w, h);

	SetPerspectiveView(w,h);

}

// This callback function gets called by the Glut
// system whenever any mouse button goes up or down.
void mouse(int mouse_button, int state, int x, int y)
{
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) 
	{
	}
	if (mouse_button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) 
	{
	}
	glutPostRedisplay();
}

// Your initialization code goes here.
void InitializeMyStuff()
{
	// set material's specular properties
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {50.0};
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// set light properties
	GLfloat light_position[] = {10000,0,0, 0};
	GLfloat white_light[] = {1,1,1,1};
	GLfloat low_light[] = {.3,.3,.3,1};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); // position first light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light); // specify first light's color
	glLightfv(GL_LIGHT0, GL_SPECULAR, low_light);

	glEnable(GL_DEPTH_TEST); // turn on depth buffering
	glEnable(GL_LIGHTING);	// enable general lighting
	glEnable(GL_LIGHT0);	// enable the first light.

	glNewList(PAWN, GL_COMPILE);
	DrawPiece("PAWN.POL");
	glEndList();

	glNewList(KING, GL_COMPILE);
	DrawPiece("KING.POL");
	glEndList();

	glNewList(QUEEN, GL_COMPILE);
	DrawPiece("QUEEN.POL");
	glEndList();

	glNewList(BISHOP, GL_COMPILE);
	DrawPiece("BISHOP.POL");
	glEndList();

	glNewList(ROOK, GL_COMPILE);
	DrawPiece("ROOK.POL");
	glEndList();

	glNewList(KNIGHT, GL_COMPILE);
	DrawPiece("KNIGHT.POL");
	glEndList();

}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_x, screen_y);
	glutInitWindowPosition(10, 10);

	int fullscreen = 0;
	if (fullscreen) 
	{
		glutGameModeString("800x600:32");
		glutEnterGameMode();
	} 
	else 
	{
		glutCreateWindow("Shapes");
	}

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);

	glClearColor(1,1,1,1);	
	InitializeMyStuff();

	glutMainLoop();

	return 0;
}
