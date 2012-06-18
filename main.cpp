#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "imageloader.h"
#include "vec3f.h"
#endif



static GLfloat spin, spin2 = 0.0;
float angle = 0;
using namespace std;

float lastx, lasty;
GLint stencilBits;
static int viewx = 160;
static int viewy = 80;
static int viewz = 200;

float rot = 0;

GLuint texture[1]; //array untuk texture

struct Images {//tempat image
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Images Images;

//class untuk terain 2D
class Terrain {
private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	Vec3f** normals;
	bool computedNormals; //Whether normals is up-to-date
public:
	Terrain(int w2, int l2) {
		w = w2;
		l = l2;

		hs = new float*[l];
		for (int i = 0; i < l; i++) {
			hs[i] = new float[w];
		}

		normals = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals[i] = new Vec3f[w];
		}

		computedNormals = false;
	}

	Terrain() {
		for (int i = 0; i < l; i++) {
			delete[] hs[i];
		}
		delete[] hs;

		for (int i = 0; i < l; i++) {
			delete[] normals[i];
		}
		delete[] normals;
	}

	int width() {
		return w;
	}

	int length() {
		return l;
	}

	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y) {
		hs[z][x] = y;
		computedNormals = false;
	}

	//Returns the height at (x, z)
	float getHeight(int x, int z) {
		return hs[z][x];
	}

	//Computes the normals, if they haven't been computed yet
	void computeNormals() {
		if (computedNormals) {
			return;
		}

		//Compute the rough version of the normals
		Vec3f** normals2 = new Vec3f*[l];
		for (int i = 0; i < l; i++) {
			normals2[i] = new Vec3f[w];
		}

		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum(0.0f, 0.0f, 0.0f);

				Vec3f out;
				if (z > 0) {
					out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
				}
				Vec3f in;
				if (z < l - 1) {
					in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
				}
				Vec3f left;
				if (x > 0) {
					left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
				}
				Vec3f right;
				if (x < w - 1) {
					right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
				}

				if (x > 0 && z > 0) {
					sum += out.cross(left).normalize();
				}
				if (x > 0 && z < l - 1) {
					sum += left.cross(in).normalize();
				}
				if (x < w - 1 && z < l - 1) {
					sum += in.cross(right).normalize();
				}
				if (x < w - 1 && z > 0) {
					sum += right.cross(out).normalize();
				}

				normals2[z][x] = sum;
			}
		}

		//Smooth out the normals
		const float FALLOUT_RATIO = 0.5f;//memperhalus
		for (int z = 0; z < l; z++) {
			for (int x = 0; x < w; x++) {
				Vec3f sum = normals2[z][x];

				if (x > 0) {
					sum += normals2[z][x - 1] * FALLOUT_RATIO;
				}
				if (x < w - 1) {
					sum += normals2[z][x + 1] * FALLOUT_RATIO;
				}
				if (z > 0) {
					sum += normals2[z - 1][x] * FALLOUT_RATIO;
				}
				if (z < l - 1) {
					sum += normals2[z + 1][x] * FALLOUT_RATIO;
				}

				if (sum.magnitude() == 0) {
					sum = Vec3f(0.0f, 1.0f, 0.0f);
				}
				normals[z][x] = sum;
			}
		}

		for (int i = 0; i < l; i++) {
			delete[] normals2[i];
		}
		delete[] normals2;

		computedNormals = true;
	}

	//Returns the normal at (x, z)
	Vec3f getNormal(int x, int z) {
		if (!computedNormals) {
			computeNormals();
		}
		return normals[z][x];
	}
};
//end class
//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
//load terain di procedure inisialisasi
Terrain* loadTerrain(const char* filename, float height) {//ngambil file
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			unsigned char color = (unsigned char) image->pixels[3 * (y
					* image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
//buat tipe data terain
Terrain* _terrain;//pariable terain
Terrain* _terrainTanah;
//Terrain* _terrainAir;

void cleanup() {//untuk mehilangin file image
	delete _terrain;
	delete _terrainTanah;
	//delete _terrainAir;
}
//mengambil gambar BMP
int ImageLoad(char *filename, Images *image) {
	FILE *file;
	unsigned long size; // ukuran image dalam bytes
	unsigned long i; // standard counter.
	unsigned short int plane; // number of planes in image

	unsigned short int bpp; // jumlah bits per pixel
	char temp; // temporary color storage for var warna sementara untuk memastikan filenya ada


	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}
	// mencari file header bmp
	fseek(file, 18, SEEK_CUR);
	// read the width
	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {//lebar beda
		printf("Error reading width from %s.\n", filename);
		return 0;
	}
	//printf("Width of %s: %lu\n", filename, image->sizeX);
	// membaca nilai height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {//tingginya beda
		printf("Error reading height from %s.\n", filename);
		return 0;
	}
	//printf("Height of %s: %lu\n", filename, image->sizeY);
	//menghitung ukuran image(asumsi 24 bits or 3 bytes per pixel).

	size = image->sizeX * image->sizeY * 3;
	// read the planes
	if ((fread(&plane, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);//bukan file bmp
		return 0;
	}
	if (plane != 1) {
		printf("Planes from %s is not 1: %u\n", filename, plane);//
		return 0;
	}
	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);

		return 0;
	}
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);//bukan 24 pixel
		return 0;
	}
	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);
	// read the data.
	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");//gagal ambil memory
		return 0;
	}
	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}
	for (i = 0; i < size; i += 3) { // membalikan semuan nilai warna (gbr - > rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	// we're done.
	return 1;
}




//mengambil tekstur
Images * loadTexture() {
	Images *image1;
	// alokasi memmory untuk tekstur
	image1 = (Images *) malloc(sizeof(Images));
	if (image1 == NULL) {
		printf("Error allocating space for image");//memory tidak cukup
		exit(0);
	}
	//pic.bmp is a 64x64 picture
	if (!ImageLoad("piramid.bmp", image1)) {
		exit(1);
	}
	return image1;
}



void initRendering() {//inisialisasi
	glEnable(GL_DEPTH_TEST);//kedalaman
	glEnable(GL_COLOR_MATERIAL);//warna
	glEnable(GL_LIGHTING);//cahaya
	glEnable(GL_LIGHT0);//lampu
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void drawScene() {//buat terain
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glTranslatef(0.0f, 0.0f, -10.0f);
	 glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	 glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

	 GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	 GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	 GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	 glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	 */
	float scale = 500.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float) (_terrain->width() - 1) / 2, 0.0f,
			-(float) (_terrain->length() - 1) / 2);

	glColor3f(0.3f, 0.9f, 0.0f);
	for (int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

}


//untuk di display
void drawSceneTanah(Terrain *terrain, GLfloat r, GLfloat g, GLfloat b) {
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
	 glTranslatef(0.0f, 0.0f, -10.0f);
	 glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	 glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

	 GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	 glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	 GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	 GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	 glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	 glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	 */
	float scale = 500.0f / max(terrain->width() - 1, terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float) (terrain->width() - 1) / 2, 0.0f,
			-(float) (terrain->length() - 1) / 2);

	glColor3f(r, g, b);
	for (int z = 0; z < terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for (int x = 0; x < terrain->width(); x++) {
			Vec3f normal = terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z), z);
			normal = terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

}

GLuint loadtextures(const char *filename, int width, int height) {//buat ngambil dari file image untuk jadi texture
	GLuint texture;

	unsigned char *data;
	FILE *file;

	/*
	 if (filename == "water.bmp") {
	 Image *img = loadBMP(filename);
	 width = img->width;
	 height = img->height;
	 }
	 */

	file = fopen(filename, "rb");
	if (file == NULL)
		return 0;

	data = (unsigned char *) malloc(width * height * 3);
	fread(data, width * height * 3, 1, file);

	fclose(file);

	glGenTextures(1, &texture);//generet (dibentuk)
	glBindTexture(GL_TEXTURE_2D, texture);//binding (gambung)
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);//untuk membaca gambar jadi text dan dapat dibaca dengan pixel
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameterf(  GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB,
			GL_UNSIGNED_BYTE, data);

	data = NULL;

	return texture;
}

const GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 1.0f };

const GLfloat light_ambient2[] = { 0.3f, 0.3f, 0.3f, 0.0f };
const GLfloat light_diffuse2[] = { 0.3f, 0.3f, 0.3f, 0.0f };

const GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/*
unsigned int texture_piramid;
unsigned int LoadTextureFromBmpFile(char *filename);
*/


void piramid(void)/*(double putar)*/ {
   //lantai   
    glPushMatrix();
    glScaled(0.8, 0.03, 0.8);
    glTranslatef(0.0, -30.2, 0.0);
    glutSolidCube(7.0);
    glPopMatrix();
    
    //atap
    glPushMatrix();
    glScaled(0.8, 1.0, 0.8);
    glTranslatef(0.0, -0.8, 0.0);
    glRotated(45, 0, 1, 0);
    glRotated(-90, 1, 0, 0);
    glutSolidCone(4.5, 2, 4, 1);
    glPopMatrix();

}
void tambahan(void){
    //lantai kanan
    glPushMatrix();
    glScaled(0.3, 0.03, 0.8);
    glTranslatef(6.5, -30.2, 7.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    
    //lantai kiri
    glPushMatrix();
    glScaled(0.3, 0.03, 0.8);
    glTranslatef(-6.5, -30.2, 7.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    
    //badan kanan
    glPushMatrix();
    glScaled(0.05, 0.03, 0.08);
    glTranslatef(39.0, -23.2, 68.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(148, 123, 77);
    glutSolidCube(10.0);
    glPopMatrix();
    
    //kepala kanan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(19.5, -5.2, 58.2);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(148, 123, 77);
    glutSolidSphere(2.5,20,50);
    glPopMatrix();
    
    //kaki kanan objek kanan
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(21.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    //kaki kiri objek kanan 
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(18.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    
    
    //badan kiri
    glPushMatrix();
    glScaled(0.05, 0.03, 0.08);
    glTranslatef(-39.0, -23.2, 68.5);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(148, 123, 77);
    glutSolidCube(10.0);
    glPopMatrix();
    
    //kepala kiri
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-19.5, -5.2, 58.2);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(148, 123, 77);
    glutSolidSphere(2.5,20,50);
    glPopMatrix();
    
     //kaki kiri objek kiri
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(-21.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    //kaki kanan objek kiri 
    glPushMatrix();
    glScaled(0.1, 0.03, 0.15);
    glTranslatef(-18.3, -28.2, 40.7);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(182, 159, 118);
    glutSolidCube(2.0);
    glPopMatrix();
    
     //lantai tiang kanan depan bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(11.5, -10.0, 22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri depan bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(-11.5, -10.0, 22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri belakang bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(-11.5, -10.0, -22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    
    //lantai tiang kiri belakang bawah
    glPushMatrix();
    glScaled(0.3, 0.09, 0.3);
    glTranslatef(11.5, -10.0, -22.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kanan depan atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(17.25, 7.0, 33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri depan atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(-17.25, 7.0, 33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kiri belakang atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(-17.25, 7.0, -33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //lantai tiang kanan belakang atas
    glPushMatrix();
    glScaled(0.2, 0.06, 0.2);
    glTranslatef(17.25, 7.0, -33.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //batang tiang kanan depan
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(34.5, -0.2, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(96, 67, 11);
    glutSolidCube(1.0);
    glPopMatrix();
   
   //batang tiang kiri depan
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(-34.5, -0.2, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(42, 29, 3);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //batang tiang kiri belakang
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(-34.5, -0.2, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(42, 29, 3);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //batang tiang kanan belakang
    glPushMatrix();
    glScaled(0.1, 1.3, 0.1);
    glTranslatef(34.5, -0.2, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(42, 29, 3);
    glutSolidCube(1.0);
   glPopMatrix();
   
   //lampu kanan depan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(34.5, 6.4, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(252, 243, 169);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kiri depan
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-34.5, 6.4, 66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(252, 243, 169);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kiri belakang
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-34.5, 6.4, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(252, 243, 169);
    glutSolidSphere(2.0,20,50);
    glPopMatrix();
    
    //lampu kanan belakang
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(34.5, 6.4, -66.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3ub(252, 243, 169);
    glutSolidSphere(2.0,20,50);
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
    
    
    //matahari
    glPushMatrix();
    glScaled(0.1, 0.1, 0.1);
    glTranslatef(-80.5, 140.0, -100.0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(9.0, 8.0, 0.4222222);
    glutSolidSphere(9.0,20,50);
    glPopMatrix();
    
    
    
 }

void batu(void){
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(0.66, 0.508, 0.344);
    glutSolidIcosahedron();
  
}

void onta(void) {
    GLUquadricObj * pObj;
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
	//badan utama
	glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(248, 202, 78);
	glTranslatef(7.0, 10.0,70.3);
	glScalef(5.5, 3.5, 4.0);
	glutSolidSphere(0.5, 20, 30);
	glPopMatrix();
        
        // kaki 
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(8.2, 10.0,71.3);
        glRotated(90,1,0,0);
        glRotated(10,0,1,0);
	gluCylinder(pObj,0.8, 0.25, 4.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(8.2, 10.0,69.3);
        glRotated(90,1,0,0);
        glRotated(340,0,1,0);
	gluCylinder(pObj,0.8, 0.25, 4.0, 25.0, 25.0);
	glPopMatrix();        

        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(5.3, 10.0,69.3);
        glRotated(90,1,0,0);
        glRotated(10,0,1,0);
	gluCylinder(pObj,0.5, 0.25, 4.0, 25.0, 25.0);
	glPopMatrix();
        
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(5.3, 10.0,71.3);
        glRotated(90,1,0,0);
        glRotated(340,0,1,0);
	gluCylinder(pObj,0.5, 0.25, 4.0, 25.0, 25.0);
	glPopMatrix();
        
        //buntut
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(9.3, 10.0,70.3);
        glRotated(90,1,0,0);
        glRotated(30,0,1,0);
	gluCylinder(pObj,0.20, 0.20, 3.0, 25.0, 25.0);
	glPopMatrix();
        
        //leher
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(5.3, 10.5,70.3);
        glRotated(90,1,0,0);
        glRotated(300,0,1,0);
	gluCylinder(pObj,0.50, 0.50, 2.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(3.9, 9.5,70.3);
        glRotated(90,1,0,0);
        glRotated(220,0,1,0);
	gluCylinder(pObj,0.50, 0.50, 1.8, 25.0, 25.0);
	glPopMatrix();
        
        
        //kepala
	glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(248, 202, 78);
	glTranslatef(2.8, 11.0,70.3);
	glScalef(1.7, 1.5, 1.5);
	glutSolidSphere(0.5, 20, 30);
	glPopMatrix();
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(248,202,78);
        glTranslatef(2.5, 10.9,70.3);
        glRotated(90,1,0,0);
        glRotated(300,0,1,0);
	gluCylinder(pObj,0.70, 0.50, 0.7, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(248, 202, 78);
	glTranslatef(2.0, 10.6,70.3);
	glScalef(1.0, 1.0, 1.0);
	glutSolidSphere(0.5, 20, 30);
	glPopMatrix();
        
        //punggung
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(248, 202, 78);
	glTranslatef(6.0, 10.8,70.3);
	glScalef(2.0, 3.0, 3.0);
	glutSolidSphere(0.5, 20.0, 40.0);
	glPopMatrix();
        
        // mata
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(245, 250, 240);
	glTranslatef(2.5, 11.5,70.6);
	glScalef(0.5, 0.5, 0.5);
	glutSolidSphere(0.5, 20.0, 40.0);
	glPopMatrix();
        
        glPushMatrix();
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3ub(245, 250, 240);
	glTranslatef(2.5, 11.5,70.0);
	glScalef(0.5, 0.5, 0.5);
	glutSolidSphere(0.5, 20.0, 40.0);
	glPopMatrix();
}

//awan
void awan(void){
glPushMatrix(); 
glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
glColor3ub(153, 223, 255);
glutSolidSphere(10, 50, 50);
glPopMatrix();
glPushMatrix();
glTranslatef(10,0,1);
glutSolidSphere(5, 50, 50);
glPopMatrix();   
glPushMatrix();
glTranslatef(-2,6,-2);
glutSolidSphere(7, 50, 50);
glPopMatrix();   
glPushMatrix();
glTranslatef(-10,-3,0);
glutSolidSphere(7, 50, 50);
glPopMatrix();  
glPushMatrix();
glTranslatef(6,-2,2);
glutSolidSphere(7, 50, 50);
glPopMatrix();      
}

void pohon(void){
       GLUquadricObj * pObj;
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);
    //BATANG
        glPushMatrix();
        glTranslatef(0.0, 28.0,40.3);
        glRotated(90,1,0,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,1.0, 2.25, 25.0, 25.0, 25.0);
	glPopMatrix();
        
     //ranting   
        glPushMatrix();
        glTranslatef(0.0, 20.0,40.3);
        glRotated(90,1,0,0);
        glRotated(120,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.8, 0.5, 10.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0.0, 18.0,40.3);
        glRotated(90,1,0,0);
        glRotated(250,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.8, 0.5, 14.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0.0, 23.0,40.3);
        glRotated(90,1,0,0);
        glRotated(250,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.8, 0.5, 3.0, 25.0, 25.0);
	glPopMatrix();
        
        //ranting kecil kanan
        glPushMatrix();
        glTranslatef(6.3, 23.7,40.3);
        glRotated(90,1,0,0);
        glRotated(50,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.5, 0.3, 2.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(5.3, 23.0,40.3);
        glRotated(90,1,0,0);
        glRotated(160,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.5, 0.3, 4.0, 25.0, 25.0);
	glPopMatrix();
        
        //ranting kecil kiri
        glPushMatrix();
        glColor3ub(248,202,78);
        glRotated(90,1,0,0);
        glRotated(150,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.4, 0.3, 2.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-10.5, 22.0,40.3);
        glRotated(90,1,0,0);
        glRotated(190,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.5, 0.3, 4.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-5.5, 20.5,40.3);
        glRotated(90,1,0,0);
        glRotated(160,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.5, 0.3, 2.0, 25.0, 25.0);
	glPopMatrix();
        
        glPushMatrix();
        glTranslatef(-7.5, 20.5,40.3);
        glRotated(90,1,0,0);
        glRotated(300,0,1,0);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glColor3ub(242, 216, 166);
	gluCylinder(pObj,0.5, 0.3, 3.5, 25.0, 25.0);
	glPopMatrix();
        
}

void display(void){
//    glutSwapBuffers();
	glClearStencil(0); //clear the stencil buffer
	glClearDepth(1.0f);

	glClearColor(0.0, 0.6, 0.8, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();//reset posisi
	gluLookAt(viewx, viewy, viewz, 0.0, 0.0, 5.0, 0.0, 1.0, 0.0);


    glPushMatrix();
    drawScene();
    glPopMatrix();
    
    glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	drawSceneTanah(_terrain, 0.804f, 0.53999999f, 0.296f);
	glPopMatrix();

	
	/*glPushMatrix();
	drawSceneTanah(_terrainAir, 0.0f, 0.2f, 0.5f);
	glPopMatrix();
        */

	glPushMatrix();

//	glBindTexture(GL_TEXTURE_2D, texture[0]);
	drawSceneTanah(_terrainTanah, 0.7f, 0.2f, 0.1f);
	glPopMatrix();



//tambahan
glPushMatrix();
glTranslatef(0,8,0); 
glScalef(10, 10, 10);
tambahan();
glPopMatrix();

glPushMatrix();
glTranslatef(-30.5,0.0,90.0); 
glScalef(15.3, 1.8, 1.4);
batu();
glPopMatrix();

glPushMatrix();
glTranslatef(30.5,0.0,90.0); 
glScalef(15.3, 1.8, 1.4);
batu();
glPopMatrix();

glPushMatrix();
glTranslatef(17.5,0.0,111.2);
glRotated(90, 0, 1, 0); 
glScalef(25.3, 1.8, 1.4);
batu();
glPopMatrix();

glPushMatrix();
glTranslatef(-17.5,0.0,111.2);
glRotated(90, 0, 1, 0); 
glScalef(25.3, 1.8, 1.4);
batu();
glPopMatrix();

glPushMatrix();
glTranslatef(43.5,0.0,4.8);
glRotated(90, 0, 1, 0); 
glScalef(100.3, 1.8, 1.4);
batu();
glPopMatrix();

glPushMatrix();
glTranslatef(-43.5,0.0,4.8);
glRotated(90, 0, 1, 0); 
glScalef(100.3, 1.8, 1.4);
batu();
glPopMatrix();


glPushMatrix();
glTranslatef(0.0,0.0,-80.5); 
glScalef(51.2, 1.8, 1.4);
batu();
glPopMatrix();

//onta
glPushMatrix();
glTranslatef(-70.0,-7.0,100.3);
glRotated(90,0,1,0);
onta();
glPopMatrix();


glPushMatrix();
glTranslatef(-180.0,-7.0,30.0);
glRotated(90,0,1,0);
onta();
glPopMatrix();

glPushMatrix();
glTranslatef(80.0,-7.0,-50.0);
glRotated(10,0,1,0);
onta();
glPopMatrix();
//awan
glPushMatrix();
glTranslatef(-75, 110, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-45, 110, -115);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-50, 120, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-140, 90, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-155, 90, -115);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-130, 110, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-190, 110, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-175, 120, -115);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-200, 100, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-30, 110, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-35, 95, -115);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-20, 90, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();

glPushMatrix();
glTranslatef(-80, 90, -120);
glScalef(1.8, 1.0, 1.0);  
awan();
glPopMatrix();


//pohon
glPushMatrix();
glTranslated(-100.0,-5.0,0.0);
pohon();
glPopMatrix();

glPushMatrix();
glTranslated(100.0,-5.0,0.0);
pohon();
glPopMatrix();

//pirami
glPushMatrix();
glTranslatef(0,8,0); 
glScalef(10, 10, 10);
glBindTexture(GL_TEXTURE_2D, texture[0]);
piramid();
glPopMatrix();

glPushMatrix();
glTranslatef(-100,8,0); 
glScalef(5, 5, 5);
glBindTexture(GL_TEXTURE_2D, texture[0]);
piramid();
glPopMatrix();

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disable the color mask
	glDepthMask(GL_FALSE); //disable the depth mask

	glEnable(GL_STENCIL_TEST); //enable the stencil testing

	glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); //set the stencil buffer to replace our next lot of data

	//ground
	//tanah(); //set the data plane to be replaced
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //enable the color mask
	glDepthMask(GL_TRUE); //enable the depth mask

	glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); //set the stencil buffer to keep our next lot of data

	glDisable(GL_DEPTH_TEST); //disable depth testing of the reflection

	// glPopMatrix();  
	glEnable(GL_DEPTH_TEST); //enable the depth testing
	glDisable(GL_STENCIL_TEST); //disable the stencil testing
	//end of ground
	glEnable(GL_BLEND); //enable alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blending function
	glRotated(1, 0, 0, 0);
	
	glDisable(GL_BLEND);



    glutSwapBuffers();//buffeer ke memory
	glFlush();
	rot++;
	angle++;




//glDisable(GL_COLOR_MATERIAL);
}

void init(void){
/*GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light_position[] = {1.0, 1.0, 1.0, 1.0};
     
glClearColor(0.0,0.0,0.0,0.0);

//glShadeModel(GL_FLAT);

glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
glLightfv(GL_LIGHT0, GL_POSITION, light_position);
*/

glEnable(GL_DEPTH_TEST);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glDepthFunc(GL_LESS);
glEnable(GL_NORMALIZE);
glEnable(GL_COLOR_MATERIAL);
glDepthFunc(GL_LEQUAL);
glShadeModel(GL_SMOOTH);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glEnable(GL_CULL_FACE);

glEnable(GL_TEXTURE_2D);
glEnable(GL_TEXTURE_GEN_S);
glEnable(GL_TEXTURE_GEN_T);


    initRendering();
	_terrain = loadTerrain("heightmap.bmp", 20);
	_terrainTanah = loadTerrain("heightmapTanah.bmp", 20);
	//_terrainAir = loadTerrain("heightmapAir.bmp", 20);

	//binding texture

Images *image1 = loadTexture();

if (image1 == NULL) {
		printf("Image was not returned from loadTexture\n");
		exit(0);
	}
	
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	// Generate texture/ membuat texture
	glGenTextures(1, texture);


	
//------------tekstur piramid---------------

	//binding texture untuk membuat texture 2D
	glBindTexture(GL_TEXTURE_2D, texture[0]);


    
	//menyesuaikan ukuran textur ketika image lebih besar dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //
	//menyesuaikan ukuran textur ketika image lebih kecil dari texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //

	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image1->data);
 // texture
}

void reshape(int w, int h){
glViewport(0, 0 , (GLsizei) w,(GLsizei)h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();

//glFrustum(-1.0,1.0,-1.0,1.0,1.5,20.0);
gluPerspective(60, (GLfloat) w / (GLfloat) h, 0.1, 1000.0);
glMatrixMode(GL_MODELVIEW);
//glLoadIdentity();
//gluLookAt(100.0,0.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);
}

static void kibor(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_HOME:
		viewy++;
		break;
	case GLUT_KEY_END:
		viewy--;
		break;
	case GLUT_KEY_UP:
		viewz--;
		break;
	case GLUT_KEY_DOWN:
		viewz++;
		break;

	case GLUT_KEY_RIGHT:
		viewx++;
		break;
	case GLUT_KEY_LEFT:
		viewx--;
		break;

	case GLUT_KEY_F1: {
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}
		;
		break;
	case GLUT_KEY_F2: {
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient2);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse2);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	}
		;
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		viewz++;
	}
	if (key == 'e') {
		viewz--;
	}
	if (key == 's') {
		viewy--;
	}
	if (key == 'w') {
		viewy++;
	}
}

int main(int argc, char** argv){
glutInit(&argc, argv);
//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH); //add a stencil buffer to the window
glutInitWindowSize(800,600);
glutInitWindowPosition(100,100);
glutCreateWindow("kelompok piramid      ");
init();

glutDisplayFunc(display);
glutIdleFunc(display);
glutReshapeFunc(reshape);

glutKeyboardFunc (keyboard);
glutSpecialFunc(kibor);

glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
glLightfv(GL_LIGHT0, GL_POSITION, light_position);

glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
glColorMaterial(GL_FRONT, GL_DIFFUSE);

glutMainLoop();
return 0;
}
