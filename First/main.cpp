#define _CRT_SECURE_NO_WARNINGS

#include "glut.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
using namespace std;

const double PI = 4 * atan(1.0);
const int W = 1300;
const int H = 600;
const int GSZ = 100;

// texture
const int TW = 256;  // must be a power of 2
const int TH = 256;

// texture definition
unsigned char tx1[1024][1024][3]; // wall
unsigned char tx2[512][512][3]; // roof
unsigned char tx3[1024][1024][3]; // grass
unsigned char tx4[1024][1024][3]; // road
unsigned char tx5[512][512][3]; // door
unsigned char tx6[512][512][3]; // facade floor
unsigned char tx7[512][512][3]; // tree
unsigned char tx8[512][512][3]; // fountain water
unsigned char tx9[512][512][3]; // fountain body
unsigned char tx11[512][512][3]; // flowers
unsigned char tx12[1024][2048][3]; // sky
unsigned char tx13[512][512][3]; // window

double ground[GSZ][GSZ] = { 0 };

double fountainWaterOffset = 0;

double horizonSphereOffset = 0;

double eyex = 15, eyey = 25, eyez = 15; // eye coordinates
double angle=0;
double dx = 0, dy = 0, dz = 0;
double dir[3] = {0,0,-1}; // x,y,z
double speed = 0;
double angular_speed = 0;
double sight_angle = PI;
double random = 2.0;
double WaterHeight = 0.035;
bool stop = true;

// pixels of texture image
unsigned char* pixels;

struct Vertex
{
	float x, y, z;
};

class Droplet
{
private:
	float time;
	Vertex constantSpeed;
	float scaler;
public:
	void SetConstantSpeed(Vertex NewSpeed);
	void SetScaler(float newScaler);
	void SetTime(float NewTime);
	void GetNewPosition(Vertex* PositionVertex);
};

void Droplet::SetConstantSpeed(Vertex NewSpeed)
{
	constantSpeed = NewSpeed;
}

void Droplet::SetScaler(float newScaler)
{
	scaler = newScaler;
}

void Droplet::SetTime(float newTime)
{
	time = newTime;
}

void Droplet::GetNewPosition(Vertex* PositionVertex)
{
	Vertex Position;
	time += 0.15;
	Position.x = constantSpeed.x * time;
	Position.y = constantSpeed.y * time - scaler * time * time;
	Position.z = constantSpeed.z * time;
	PositionVertex->x = Position.x;
	PositionVertex->y = Position.y + WaterHeight;
	PositionVertex->z = Position.z;
	if (Position.y < 0.0)
	{
		time = time - int(time);
		if (time > 0.0) time -= 1.0;
	}
}

Droplet* Droplets;
Vertex* DropletsVertices;
int levels = 8;
int raysPerLevel = 18;
int dropsPerRay = 80;
float totalDrops = levels * raysPerLevel * dropsPerRay;
float angleModifier = 60; 
float scaler = 0.01; 

void ReadImageFromFile(char* fname) {
	FILE* pf = fopen(fname,"rb");  // "rb" stands for read binary
	// in bitmap file the order of data is: bf  ,   bi ,  b,g,r,b,g,r,b,g,r ....
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	
	fread(&bf, sizeof(bf), 1, pf);
	fread(&bi, sizeof(bi), 1, pf);
	pixels = (unsigned char*)malloc(bi.biWidth*bi.biHeight * 3);
	// copy all pixels
	fread(pixels, 1, bi.biWidth*bi.biHeight * 3, pf);
}

// set matrix of texture
void SetTexture(int kind) // kind of texture
{
	int i, j,q;
	int tileSize = TH / 8;
	int k = 0;

	switch (kind) {

	case 1:
		k = 0;
		for (i = 0; i<1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx1[i][j][0] = pixels[k + 2]; // red
				tx1[i][j][1] = pixels[k + 1];   // green
				tx1[i][j][2] = pixels[k];   // blue
			}
		break;
	case 2:
		k = 0;
		for (i = 0; i<512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx2[i][j][0] = pixels[k + 2]; // red
				tx2[i][j][1] = pixels[k + 1];   // green
				tx2[i][j][2] = pixels[k];   // blue
			}
		break;
	case 3:
		k = 0;
		for (i = 0; i < 1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx3[i][j][0] = pixels[k + 2]; // red
				tx3[i][j][1] = pixels[k + 1];   // green
				tx3[i][j][2] = pixels[k];   // blue
			}
		break;
	case 4:
		k = 0;
		for (i = 0; i < 1024; i++)
			for (j = 0; j < 1024; j++, k += 3)
			{
				tx4[i][j][0] = pixels[k + 2]; // red
				tx4[i][j][1] = pixels[k + 1];   // green
				tx4[i][j][2] = pixels[k];   // blue
			}
		break;
	case 5:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx5[i][j][0] = pixels[k + 2]; // red
				tx5[i][j][1] = pixels[k + 1];   // green
				tx5[i][j][2] = pixels[k];   // blue
			}
		break;
	case 6:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx6[i][j][0] = pixels[k + 2]; // red
				tx6[i][j][1] = pixels[k + 1];   // green
				tx6[i][j][2] = pixels[k];   // blue
			}
		break;
	case 7:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx7[i][j][0] = pixels[k + 2]; // red
				tx7[i][j][1] = pixels[k + 1];   // green
				tx7[i][j][2] = pixels[k];   // blue
			}
		break;
	case 8:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx8[i][j][0] = pixels[k + 2]; // red
				tx8[i][j][1] = pixels[k + 1];   // green
				tx8[i][j][2] = pixels[k];   // blue
			}
		break;
	case 9:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx9[i][j][0] = pixels[k + 2]; // red
				tx9[i][j][1] = pixels[k + 1];   // green
				tx9[i][j][2] = pixels[k];   // blue
			}
		break;
	case 11:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx11[i][j][0] = pixels[k + 2]; // red
				tx11[i][j][1] = pixels[k + 1];   // green
				tx11[i][j][2] = pixels[k];   // blue
			}
		break;
	case 12:
		k = 0;
		for (i = 0; i < 1024; i++)
			for (j = 0; j < 2048; j++, k += 3)
			{
				tx12[i][j][0] = pixels[k + 2]; // red
				tx12[i][j][1] = pixels[k + 1];   // green
				tx12[i][j][2] = pixels[k];   // blue
			}
		break;
	case 13:
		k = 0;
		for (i = 0; i < 512; i++)
			for (j = 0; j < 512; j++, k += 3)
			{
				tx13[i][j][0] = pixels[k + 2]; // red
				tx13[i][j][1] = pixels[k + 1];   // green
				tx13[i][j][2] = pixels[k];   // blue
			}
		break;
	}
}

// level the road along x-axis
void PrepareRoad() {
	int i,j;

	for (i = 0; i < GSZ; i++)
		ground[GSZ / 2-1][i] = ground[GSZ / 2+1][i] = ground[GSZ / 2][i];

	for (j = 0; j < GSZ; j++)
	{
		ground[j][GSZ / 2 + 1] = ground[j][GSZ / 2 - 1] = ground[j][GSZ / 2];
	}
}

// rectify the area
void PrepareArea(double x, double z, double w, double h) {
	// set y-level of all area as in itst mid poit (if it is abobe 0) or to 1 (if it is below 0)
	double level = ground[(int)( z+GSZ/2+h/2)][(int)(x+GSZ/2+w/2)];
	if (level < 0) level = 1;

	for (int i = z + GSZ / 2; i <= z + GSZ / 2 + h; i++)
		for (int j = x + GSZ / 2; j <= x + GSZ / 2 + w; j++)
			ground[i][j] = level;
}

void init()
{
	int i, j;

	srand(time(0));
	glClearColor(0.0, 0.0, 0.5, 0);// color of window background
	glEnable(GL_DEPTH_TEST); // add capability to check what object is the closest

	PrepareRoad();
	PrepareArea(5,-15,10,10);

	// wall
	ReadImageFromFile("wall.bmp");
	SetTexture(1);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 1); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx1);

	// roof
	ReadImageFromFile("roof.bmp");
	SetTexture(2);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 2); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx2);

	// grass
	ReadImageFromFile("grass.bmp");
	SetTexture(3);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 3); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx3);

	// road
	ReadImageFromFile("stone_road.bmp");
	SetTexture(4);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 4); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx4);

	// door
	ReadImageFromFile("door.bmp");
	SetTexture(5);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 5); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx5);

	// facade floor
	ReadImageFromFile("facade_flor.bmp");
	SetTexture(6);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 6); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx6);

	// tree
	ReadImageFromFile("tree.bmp");
	SetTexture(7);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 7); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx7);

	// water
	ReadImageFromFile("water.bmp");
	SetTexture(8);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 8); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx8);

	// body
	ReadImageFromFile("body.bmp");
	SetTexture(9);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 9);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx9);

	// flowers
	ReadImageFromFile("flowers.bmp");
	SetTexture(11);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 11);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx11);

	// sky
	ReadImageFromFile("sky.bmp");
	SetTexture(12);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 12); // clouds
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2048, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, tx12);

	//window
	ReadImageFromFile("window.bmp");
	SetTexture(13);
	free(pixels);
	glBindTexture(GL_TEXTURE_2D, 13); // clouds
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // can be GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, tx13);
}

float GetRandomFloat(float range)
{
	return (float)rand() / (float)RAND_MAX * range * random;
}

void InitWaterDroplets(void)
{
	Droplets = new Droplet[(int)totalDrops];
	DropletsVertices = new Vertex[(int)totalDrops];
	Vertex NewSpeed;
	float scaleModifier;
	float t;
	float levelAngle;
	float rayAngle;
	int i, j, k;
	for (k = 0; k < levels; k++)
	{
		for (j = 0; j < raysPerLevel; j++)
		{
			for (i = 0; i < dropsPerRay; i++)
			{
				scaleModifier = scaler + GetRandomFloat(0.0005);
				levelAngle = angleModifier + (90.0 - angleModifier)
					* float(k) / (levels - 1) + GetRandomFloat(0.2 + 0.8 * (levels - k - 1) / (levels - 1));

				NewSpeed.x = cos(levelAngle * PI / 180.0) * (0.2 + 0.04 * k);
				NewSpeed.y = sin(levelAngle * PI / 180.0) * (0.2 + 0.04 * k);


				rayAngle = (float)j / (float)raysPerLevel * 360.0;


				NewSpeed.z = NewSpeed.x * sin(rayAngle * PI / 180.0);
				NewSpeed.x = NewSpeed.x * cos(rayAngle * PI / 180.0);

				t = NewSpeed.y / scaleModifier;
				Droplets[i + j * dropsPerRay + k * dropsPerRay * raysPerLevel].SetConstantSpeed(NewSpeed);
				Droplets[i + j * dropsPerRay + k * dropsPerRay * raysPerLevel].SetScaler(scaleModifier);
				Droplets[i + j * dropsPerRay + k * dropsPerRay * raysPerLevel].SetTime(t * i / dropsPerRay);
			}
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, DropletsVertices);

}

void DrawCube() {
	// top side
	glBegin(GL_POLYGON);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, 1, 1);
	glEnd();

	// bottom side
	glBegin(GL_POLYGON);
	glVertex3d(1, -1, 1);
	glVertex3d(1, -1, -1);
	glVertex3d(-1, -1, -1);
	glVertex3d(-1, -1, 1);
	glEnd();

	// front side
	glBegin(GL_POLYGON);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(1, -1, -1);
	glVertex3d(1, -1, 1);
	glEnd();

	// left side
	glBegin(GL_POLYGON);
	glVertex3d(1, 1, 1);
	glVertex3d(1, -1, 1);
	glVertex3d(-1, -1, 1);
	glVertex3d(-1, 1, 1);
	glEnd();

	// right side
	glBegin(GL_POLYGON);
	glVertex3d(1, 1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, -1, -1);
	glVertex3d(1, -1, -1);
	glEnd();

	// rear side
	glBegin(GL_POLYGON);
	glVertex3d(-1, -1, 1);
	glVertex3d(-1, -1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, 1, 1);
	glEnd();

}

void DrawCylinder(int sides)
{
	double alpha, teta = 2 * PI / sides;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		glBegin(GL_LINE_LOOP);
		glColor3d(fabs(sin(alpha)), fabs(sin(alpha)), fabs(sin(alpha)));
		glVertex3d(sin(alpha+3*PI/4), 1, cos(alpha + 3 * PI / 4)); // point 1
		glVertex3d(sin(alpha + 3 * PI / 4 + teta), 1, cos(alpha + 3 * PI / 4 + teta)); // point 2
		glColor3d(fabs(sin(alpha - PI / 4)) / 2, fabs(sin(alpha)) / 2, fabs(sin(alpha)) / 2);
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // point 3
		glVertex3d(sin(alpha), 0, cos(alpha)); // point 4
		glEnd();
	}
}

void DrawCylinder1(int sides, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / sides;

	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glVertex3d(topr * sin(alpha), 4, topr * cos(alpha)); // point 1
		glVertex3d(topr * sin(alpha + teta), 4, topr * cos(alpha + teta)); // point 2
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // point 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // point 4
		glEnd();
	}
}

void DrawTexCylinder(int sides, int tnum,int hrepeat, int vrepeat)
{
	double alpha, teta = 2 * PI / sides;
	double part = hrepeat / (double)sides; 
	int counter;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tnum); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_MODULATE

	for (alpha = 0,counter = 0; alpha <= 2 * PI; alpha += teta,counter++)
	{
		glColor3d(0.3+0.7*fabs(sin(alpha)), 0.3 + 0.7*fabs(sin(alpha)), 0.3 + 0.7*fabs(sin(alpha)));
		glBegin(GL_POLYGON);
		glTexCoord2d(counter*part, vrepeat);
		glVertex3d(sin(alpha), 1, cos(alpha)); // point 1
		glTexCoord2d((1+counter)*part, vrepeat);
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta)); // point 2
		glTexCoord2d((1 + counter)*part, 0);
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // point 3
		glTexCoord2d(counter*part, 0);
		glVertex3d(sin(alpha), 0, cos(alpha)); // point 4
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

}

void DrawTexCylinder1(int sides, int tnum, int hrepeat, int vrepeat, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / sides;
	double part = hrepeat / (double)sides;
	int counter;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tnum);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	if (topr < bottomr)
	{
		for (alpha = 0, counter = 0; alpha <= 2 * PI; alpha += teta, counter++)
		{
			glColor3d(0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)));
			glBegin(GL_POLYGON);
			glTexCoord2d((counter + 0.5 - topr / bottomr) * part, vrepeat);
			glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // point 1
			glTexCoord2d((counter + 0.5 + topr / bottomr) * part, vrepeat);
			glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // point 2
			glTexCoord2d((1 + counter) * part, 0);
			glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // point 3
			glTexCoord2d(counter * part, 0);
			glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // point 4
			glEnd();
		}
	}
	else
	{
		for (alpha = 0, counter = 0; alpha <= 2 * PI; alpha += teta, counter++)
		{
			glColor3d(0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)));
			glBegin(GL_POLYGON);
			glTexCoord2d(counter * part, vrepeat);
			glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // point 1
			glTexCoord2d((counter + 1) * part, vrepeat);
			glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // point 2
			glTexCoord2d((counter + 0.5 + bottomr / topr) * part, 0);
			glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // point 3
			glTexCoord2d((counter + 0.5 - bottomr / topr) * part, 0);
			glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // point 4
			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);

}

void DrawTexCylinder2(int sides, int tnum, int hrepeat, double vstart, double vend, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / sides;
	double part = hrepeat / (double)sides;
	int counter;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tnum);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	for (alpha = 0, counter = 0; alpha <= 2 * PI; alpha += teta, counter++)
	{
		glColor3d(0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)), 0.3 + 0.7 * fabs(sin(alpha)));
		glBegin(GL_POLYGON);
		glTexCoord2d(counter * part, vend);
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // point 1
		glTexCoord2d((counter + 1) * part, vend);
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // point 2
		glTexCoord2d((1 + counter) * part, vstart);
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // point 3
		glTexCoord2d(counter * part, vstart);
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // point 4
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

}

void DrawHalfCylinder(int sides, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / sides;

	for (alpha = 0; alpha <= PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // point 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // point 2
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // point 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // point 4
		glEnd();
	}
}

void DrawTexSphere(int slices, int sectors, int tnum, int hrepeat, int vrepeat)
{
	double beta, gamma = PI / slices;
	double vpart = vrepeat / (double)slices;
	int counter;

	
	for (beta = -PI / 2, counter = 0; beta < PI / 2; beta += gamma, counter++)
	{ 
	  
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, fabs(sin(beta) - sin(beta + gamma)), 1);
		DrawTexCylinder2(sectors, tnum, hrepeat, counter * vpart, (counter + 1) * vpart, cos(beta + gamma), cos(beta));
		glPopMatrix();
	}
}

void DrawGrass()
{
	int j, i;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 3); // grass
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE


	glColor3d(1, 1, 1);
	glPushMatrix();
	glTranslated(0, 0, -30);
	glScaled(1.5, 1, 1.5);
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(-30, 0.1, -30);
	glTexCoord2d(0, 2);
	glVertex3d(-30, 0.1, 30);
	glTexCoord2d(3, 2);
	glVertex3d(30, 0.1, 30);
	glTexCoord2d(3, 0);
	glVertex3d(30, 0.1, -30);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}

void DrawRoad()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 4); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	glColor3d(1, 1, 1);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(5, 0.15, 5);
	glTexCoord2d(0, 2);
	glVertex3d(5, 0.15, -10);
	glTexCoord2d(3, 2);
	glVertex3d(13, 0.15, -10);
	glTexCoord2d(3, 0);
	glVertex3d(13, 0.15, 5);
	glEnd();

	glDisable(GL_TEXTURE_2D);

}

void DrawHouse(int left, int far_point, int w, int h)
{
	double height = 16;
	double floors = 2;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3d(1, 1, 1);

	glPushMatrix();
	glTranslated(0, 0.1, 0);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], far_point + h);
	glTexCoord2d(0, floors);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point + h);
	glTexCoord2d(w / 2, floors);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point + h);
	glTexCoord2d(w / 2, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], far_point + h);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], far_point);
	glTexCoord2d(0, floors);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point);
	glTexCoord2d(w / 2, floors);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point);
	glTexCoord2d(w / 2, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], far_point);
	glEnd();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], far_point);
	glTexCoord2d(0, floors);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point);
	glTexCoord2d(h / 2, floors);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point + h);
	glTexCoord2d(h / 2, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], far_point + h);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], far_point);
	glTexCoord2d(0, floors);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point);
	glTexCoord2d(h / 2, floors);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, far_point + h);
	glTexCoord2d(h / 2, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], far_point + h);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

}

void DrawRoof(int left, int far_point, int w, int h, double level)
{
	double height = 5;
	double floors = 2;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 2); // roof
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // depends on color


	glPushMatrix();
	glTranslated(0, 6.1, -5);

	glColor3d(0.6, 0.6, 0.6);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point + h);
	glTexCoord2d(w / 2, floors); // top point
	glVertex3d(left + w / 2, ground[far_point + GSZ / 2][left + GSZ / 2] + level + height, far_point + h / 2);
	glTexCoord2d(w, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point + h);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point);
	glTexCoord2d(w / 2, floors); // top point
	glVertex3d(left + w / 2, ground[far_point + GSZ / 2][left + GSZ / 2] + level + height, far_point + h / 2);
	glTexCoord2d(w, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point);
	glEnd();


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point);
	glTexCoord2d(w / 2, floors); // top point
	glVertex3d(left + w / 2, ground[far_point + GSZ / 2][left + GSZ / 2] + level + height, far_point + h / 2);
	glTexCoord2d(w, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point + h);
	glEnd();


	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point);
	glTexCoord2d(w / 2, floors); // top point
	glVertex3d(left + w / 2, ground[far_point + GSZ / 2][left + GSZ / 2] + level + height, far_point + h / 2);
	glTexCoord2d(w, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + level, far_point + h);
	glEnd();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);


}

void DrawDoor(int left, int far_point, int w, int h) 
{
	double height = 25;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 5); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

	glColor3d(1, 1, 1);

	
	glPushMatrix();
	glTranslated(10, 5, -9.99);
	glScaled(0.3, 0.3, 0.3);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], 0);
	glTexCoord2d(0, 1);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, 0);
	glTexCoord2d(1, 1);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, 0);
	glTexCoord2d(1, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void DrawWindow(int left, int far_point, int w, int h) 
{
	double height = 15;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 13); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

	glColor3d(1, 1, 1);

	// left window
	glPushMatrix();
	glTranslated(2, 7, -9);
	glScaled(0.5, 0.5, 0.1);
	
	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2], 0);
	glTexCoord2d(0, 1);
	glVertex3d(left, ground[far_point + GSZ / 2][left + GSZ / 2] + height, 0);
	glTexCoord2d(1, 1);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2] + height, 0);
	glTexCoord2d(1, 0);
	glVertex3d(left + w, ground[far_point + GSZ / 2][left + GSZ / 2], 0);
	glEnd();



	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void DrawStairs() {
	int numOfStairs = 4;
	double yLevelOffset = 0.15;
	double zLevelOffset = 0;
	double colorOffset = 0.02;
	double r = 0.4;
	double g = 0.35;
	double b = 0.25;
	glColor3d(r, g, b);

	for (int i = 0; i < numOfStairs; i++) {
		yLevelOffset += 1;
		zLevelOffset -= 0.8;

		glPushMatrix();
		glTranslated(9, yLevelOffset, zLevelOffset-5);
		glScaled(2.5, 1, 2);
		DrawCube();
		glPopMatrix();

		colorOffset += 0.04;
		glColor3d(r - colorOffset, g - colorOffset, b - colorOffset);
	}	
}

void DrawTrees() {

	double y = 0;
	int treeNum = 2;
	int branchNum = 50;
	double xLevelOffset = 0;

	for (int i = 0; i < treeNum; i++) {
		xLevelOffset += 12;
		glPushMatrix();
		glTranslated(-8 + xLevelOffset, 0, -4);
		glScaled(0.2, 17, 0.2);
		DrawTexCylinder(40, 7, 1, 1);
		glPopMatrix();

		glColor3d(0.24, 0.7, 0.5);
		glLineWidth(1);
		double length = 5;
		double vertexes =50;
		double alpha = 0;
		double gamma = 0;
		double beta = PI / vertexes;

		for (int i = 0; i < branchNum; i++, gamma += 15)
		{
			glPushMatrix();
			glTranslated(-8 + xLevelOffset, 13.7, -4 );
			glRotated(gamma, 0, 1, 0);
			glBegin(GL_LINE_STRIP);

			for (alpha = 0; alpha < PI; alpha += beta)
			{
				glVertex3d(length*cos(alpha), length / 1.5*sin(alpha), 0);
			}
			glEnd();
			glPopMatrix();
		}
	}
	
}

void DrawFlowers() {
	double zTranslateOffset = 0;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 11);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glColor3d(1, 1, 1);

	glPushMatrix();
	glTranslated(-7, 0.5, -7.5);
	glScaled(10, 0.5, -10.5);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(0, 0.2, 0);
	glTexCoord2d(0, 4);
	glVertex3d(0, 0.2, -0.5);
	glTexCoord2d(4, 4);
	glVertex3d(1, 0.2, -0.5);
	glTexCoord2d(4, 0);
	glVertex3d(1, 0.2, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void DrawFacade() {
	int numOfFrontlPoles = 10;
	int numOfSidelPoles = 3;
	double xTranslateOffset = 0;
	double zTranslateOffset = 0;
	glColor3d(0, 0, 0);
	
	for (int i = 0; i < numOfFrontlPoles; i++) {
		xTranslateOffset += 1.5;

		glPushMatrix();
		glTranslated(10.5+xTranslateOffset, 5, -5.5);
		glScaled(0.1, 1.0, 0.1);
		DrawCylinder1(40, 3, 3);
		glPopMatrix();

	}

	zTranslateOffset = 0;
	for (int i = 0; i < numOfSidelPoles; i++) {
		zTranslateOffset += 1.5;

		glPushMatrix();
		glTranslated(11.5 +xTranslateOffset, 5, -10.5 + zTranslateOffset);
		glScaled(0.1, 1.0, 0.1);
		DrawCylinder1(40, 3, 3);
		glPopMatrix();
	}

	glColor3d(0.3, 0.3, 0.25);
	glPushMatrix();
	glTranslated(19, 9, -5);
	glScaled(7.5, 0.3, 0.3);
	DrawCube();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(26.45, 9, -7);
	glRotated(90, 0, 1, 0);
	glScaled(2, 0.3, 0.3);
	DrawCube();
	glPopMatrix();
	
}

void DrawFacadeFloor() {
	int j, i;
	glPushMatrix();
	glTranslated(11.5, 5, -11);
	glScaled(4, 0, 2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 6); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 

	glColor3d(1, 1, 1);

	glBegin(GL_POLYGON);
	glTexCoord2d(0, 0);
	glVertex3d(0, 5.5, 3);
	glTexCoord2d(0, 1);
	glVertex3d(0, 5.5, 0);
	glTexCoord2d(1, 1);
	glVertex3d(4, 5.5, 0);
	glTexCoord2d(1, 0);
	glVertex3d(4, 5.5, 3);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void DrawMailbox() {


	glPushMatrix();
	glTranslated(6, 2, -2);
	glScaled(0.2, 2, 0.2);
	glColor3d(0, 0, 0);
	DrawCube();
	glTranslated(12.14, 0.8, 0);
	glPopMatrix();


	glPushMatrix();
	glTranslated(6.5, 4, -2);
	glRotated(90, 0, 0, 1);
	glScaled(0.05, 1.5, 0.05);
	glColor3d(0.03, 0.03, 0.83);
	DrawHalfCylinder(40, 9, 9);
	glPopMatrix();


	glPushMatrix();
	glTranslated(6, 3, -2);
	glScaled(0.5, 1, 0.4);
	glColor3d(0.03, 0.03, 0.83);
	glBegin(GL_POLYGON);
	glVertex3d(1, 1, 1);
	glVertex3d(1, 1, -1);
	glVertex3d(-1, 1, -1);
	glVertex3d(-1, 1, 1);
	glEnd();
	glPopMatrix();



	glPushMatrix();
	glTranslated(6.5, 4, -2);
	glRotated(-90, 0, 0, 1);
	glRotated(90, 1, 0, 0);
	glRotated(225, 0, 1, 0);
	glScaled(0.005, 0.005, 0.005);
	glColor3d(0.2, 0.3, 0.2);

	float radius = 70;
	float twoPI = 2 * PI;

	glBegin(GL_TRIANGLE_FAN);
	for (float i = PI; i <= twoPI; i += 0.001)
		glVertex2f((sin(i) * radius), (cos(i) * radius));
	glEnd();
	glPopMatrix();
}

void DrawWaterDroplets() {
	glColor3f(0.88, 0.89, 0.95);
	glPushMatrix();
	glTranslated(20, 2, 0);
	for (int i = 0; i < totalDrops; i++)
	{
		Droplets[i].GetNewPosition(&DropletsVertices[i]);
	}
	glDrawArrays(GL_POINTS, 0, totalDrops);
	glutPostRedisplay();
	glPopMatrix();
}

void DrawFountain() {
	
	glPushMatrix();
	glTranslated(20, 0.4, 0); 
	glScaled(4, 1.5, 4);
	DrawTexCylinder(150, 9, 20, 1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(20, 0.4, 0);
	glRotated(fountainWaterOffset, 0, 1, 0);
	glScaled(0.87, 1, 0.87);
	DrawTexCylinder1(40, 8, 4, 4, 4.5, 0);
	glPopMatrix();

	DrawWaterDroplets();
}


void display()
{// clean frame buffer and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	// add "Camera model" (instead of glOrtho)
	// definitions of camera: left, right, bottom ,top, near, far
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // start transformations from zero: M is identity
	glFrustum(-1, 1, -1, 1, 0.7, 300);
	// camera location and direction
	gluLookAt(eyex, eyey, eyez, // EYE coordinates
		eyex+dir[0], eyey-0.2, eyez+dir[2], // Point Of Interest
		0, 1, 0); // UP

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // start transformations from zero: M is identity
	
	// sky sphere
	glPushMatrix();
	glRotated(horizonSphereOffset, 0, 1, 0);
	glTranslated(0, 12, 0);
	glScaled(130, 130, 130);
	DrawTexSphere(240, 300, 12, 1, 1);
	glPopMatrix();

	DrawGrass();
	DrawRoad();
	DrawStairs();
	DrawHouse(-10, -25, 40, 15); 
	DrawRoof(-13, -20, 45, 15, 10); 
	DrawDoor(-8, 20, 8, 40);
	DrawWindow(-8, 20, 8, 40);
	DrawFacade();
	DrawFacadeFloor();
	DrawMailbox();
	DrawFlowers();
	DrawTrees();
	DrawFountain();

	glutSwapBuffers(); 
}

void idle()
{
	horizonSphereOffset += 0.05;
	fountainWaterOffset += 0.05;

	sight_angle += angular_speed;
	dir[0] = sin(sight_angle); // x
	dir[2] = cos(sight_angle); //z
	eyex += speed*dir[0];

	eyey += dy;
	eyez += speed*dir[2];

	glutPostRedisplay();
}

void special_keys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		angular_speed+=0.0001;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.0001;
		break;
	case GLUT_KEY_UP:
		speed += 0.001;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.001;
		break;
	case GLUT_KEY_PAGE_UP:
		dy += 0.01;
		break;
	case GLUT_KEY_PAGE_DOWN:
		dy -= 0.01;
		break;
	}
}

void keyboard(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 'w': // forward
		break;
	case 's': // backward
		break;
	case 'a':// turn left
		break;
	case 'd': // turn right
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE| GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Final Project");

	glutDisplayFunc(display);
	glutIdleFunc(idle); 
	glutSpecialFunc(special_keys);

	glutKeyboardFunc(keyboard);

	InitWaterDroplets();
	init();

	glutMainLoop();
}