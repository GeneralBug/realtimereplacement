/*
 * Simple 3D sine wave animation example using glm
 * $Id: sinewave3D-glm.cpp,v 1.8 2017/08/23 12:56:02 gl Exp gl $
 */


//TODO: REPLACE ALL THE GLUT STUFF
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <list>

#if _WIN32
#   include <Windows.h>
#	include <GL/glew.h>
#endif
#if __APPLE__

#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
//#   include <GLUT/glut.h>


#else
#   include <GL/gl.h>
#   include <GL/glu.h>
#   include <GL/glut.h>
#endif


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <SDL.h>
#include "shaders.h"

#define vertecies 0
#define normals 1
#define offsetBuffer 2

typedef enum { line, fill } polygonMode_t;
typedef enum { grid, wave } shape_t;

//SHADERS!
GLuint shaderProgram;
const char *vert = "shader.vert";
const char *frag = "shader.frag";


typedef struct {
    shape_t shape;
    bool animate;
    float t, lastT;
    polygonMode_t polygonMode;
    bool lighting;
    bool fixed;
    bool twoside;
    bool drawNormals;
    int width, height;
    int tess;
    int waveDim;
    int frameCount;
    float frameRate;
    float displayStatsInterval;
    int lastStatsDisplayT;
    bool displayOSD;
    bool consolePM;
    bool multiView;
    bool vbo;
} Global;
Global g = { grid, false, 0.0, 0.0, line, true, false, false, false, 0, 0, 8, 2, 0, 0.0, 1.0, 0, false, false, false, false };

typedef enum { inactive, rotate, pan, zoom } CameraControl;

SDL_Window* window;

struct camera_t {
    int lastX, lastY;
    float rotateX, rotateY;
    float scale;
    CameraControl control;
} camera = { 0, 0, 30.0, -30.0, 1.0, inactive };

glm::vec3 cyan(0.0, 1.0, 1.0);
glm::vec3 magenta(1.0, 0.0, 1.0);
glm::vec3 yellow(1.0, 1.0, 0.0);
glm::vec3 white(1.0, 1.0, 1.0);
glm::vec3 grey(0.8, 0.8, 0.8);
glm::vec3 black(0.0, 0.0, 0.0);
const float shininess = 50.0;

const float milli = 1000.0;

glm::mat4 modelViewMatrix;
glm::mat3 normalMatrix;

void (*displayFunc)();
void display();
void displayMultiView();

int err;

GLuint buffers[3];

static glm::vec3 *vertex = nullptr;
static glm::vec3 *normal = nullptr;
static int *indicies = nullptr;
static GLvoid **offset = nullptr;

//static std::list<glm::vec3> vertex;
//static std::list<glm::vec3> normal;
//static std::list<int> indicies;

//static glm::vec3 vertex[(g.tess + 1) g.tess * 2];
//static glm::vec3 normal[g.tess * 2];
//static int indices[g.tess * 2];

int prevDim = 0;
int prevTess = 0;
bool prevLighting = false;

int numLights = 8;


void quit(int code) //done
{
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(code);
}

int wantRedisplay = 1;
void postRedisplay() //done
{
    wantRedisplay = 1;
}

void printVec(float* v, int n)
{
    int i;

    for (i = 0; i < n; i++)
        printf("%5.3f ", v[i]);
    printf("\n");
}

void printMatrixLinear(float* m, int n)
{
    int i;

    for (i = 0; i < n; i++)
        printf("%5.3f ", m[i]);
    printf("\n");
}

void printMatrixColumnMajor(float* m, int n)
{
    int i, j;

    for (j = 0; j < n; j++) {
        for (i = 0; i < n; i++) {
            printf("%5.3f ", m[i * 4 + j]);
        }
        printf("\n");
    }
    printf("\n");
}

void init(void) //done
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    if (g.twoside)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glColor3f(1.0, 1.0, 1.0);
    displayFunc = &display;

    GLenum err = glewInit();

    //shaders
    shaderProgram = getShader(vert, frag);
    glUseProgram(shaderProgram); //need glew to run this? not recognised by SDL
}

void reshape(int w, int h) //done
{
    g.width = w;
    g.height = h;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawAxes(float length)
{
    glm::vec4 v;

    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_LINES);

    /* x axis */
    glColor3f(1.0, 0.0, 0.0);
    v = modelViewMatrix * glm::vec4(-length, 0.0, 0.0, 1.0);
    glVertex3fv(&v[0]);
    v = modelViewMatrix * glm::vec4(length, 0.0, 0.0, 1.0);
    glVertex3fv(&v[0]);

    /* y axis */
    glColor3f(0.0, 1.0, 0.0);
    v = modelViewMatrix * glm::vec4(0.0, -length, 0.0, 1.0);
    glVertex3fv(&v[0]);
    v = modelViewMatrix * glm::vec4(0.0, length, 0.0, 1.0);
    glVertex3fv(&v[0]);

    /* z axis */
    glColor3f(0.0, 0.0, 1.0);
    v = modelViewMatrix * glm::vec4(0.0, 0.0, -length, 1.0);
    glVertex3fv(&v[0]);
    v = modelViewMatrix * glm::vec4(0.0, 0.0, length, 1.0);
    glVertex3fv(&v[0]);

    glEnd();
    glPopAttrib();
}

void drawVector(glm::vec3& o, glm::vec3& v, float s, bool normalize, glm::vec3& c)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor3fv(&c[0]);
    glBegin(GL_LINES);
    if (normalize)
        v = glm::normalize(v);

    glVertex3fv(&o[0]);
    glm::vec3 e(o + s * v);
    glVertex3fv(&e[0]);
    glEnd();
    glPopAttrib();
}

// Console performance meter
void consolePM()
{
    printf("frame rate (f/s):  %5.0f\n", g.frameRate);
    printf("frame time (ms/f): %5.0f\n", 1.0 / g.frameRate * 1000.0);
    printf("tesselation:       %5d\n", g.tess);
}

// On screen display 
//void displayOSD()
//{
//    //TODO: glut? just ui text
//    char buffer[30];
//    char* bufp;
//    int w, h;
//
//    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
//    glDisable(GL_DEPTH_TEST);
//    glDisable(GL_LIGHTING);
//
//    glMatrixMode(GL_PROJECTION);
//    glPushMatrix();
//    glLoadIdentity();
//
//    /* Set up orthographic coordinate system to match the window,
//       i.e. (0,0)-(w,h) */
//    w = glutGet(GLUT_WINDOW_WIDTH);
//    h = glutGet(GLUT_WINDOW_HEIGHT);
//    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
//
//    glMatrixMode(GL_MODELVIEW);
//    glPushMatrix();
//    glLoadIdentity();
//
//    /* Frame rate */
//    glColor3f(1.0, 1.0, 0.0);
//    glRasterPos2i(10, 60);
//    snprintf(buffer, sizeof buffer, "frame rate (f/s):  %5.0f", g.frameRate);
//    for (bufp = buffer; *bufp; bufp++)
//        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
//
//    /* Frame time */
//    glColor3f(1.0, 1.0, 0.0);
//    glRasterPos2i(10, 40);
//    snprintf(buffer, sizeof buffer, "frame time (ms/f): %5.0f", 1.0 / g.frameRate * milli);
//    for (bufp = buffer; *bufp; bufp++)
//        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
//
//    /* Tesselation */
//    glColor3f(1.0, 1.0, 0.0);
//    glRasterPos2i(10, 20);
//    snprintf(buffer, sizeof buffer, "tesselation:       %5d", g.tess);
//    for (bufp = buffer; *bufp; bufp++)
//        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
//
//    glPopMatrix();  /* Pop modelview */
//    glMatrixMode(GL_PROJECTION);
//
//    glPopMatrix();  /* Pop projection */
//    glMatrixMode(GL_MODELVIEW);
//
//    glPopAttrib();
//}


/* Perform ADS - ambient, diffuse and specular - lighting calculation
 * in eye coordinates (EC).
 */
glm::vec3 computeLighting(glm::vec3& rEC, glm::vec3& nEC)
{

    // Used to accumulate ambient, diffuse and specular contributions
    // Note: it is a vec3 being constructed with a single value which
    // is used for all 3 components
    glm::vec3 color(0.0);

    // Ambient contribution: A=La×Ma
    // Default light ambient color and default ambient material color
    // are both (0.2, 0.2, 0.2)
    glm::vec3 La(0.2);
    glm::vec3 Ma(0.2);
    glm::vec3 ambient(La * Ma);
    color += ambient;

    // Light direction vector. Default for LIGHT0 is a directional light
    // along z axis for all vertices, i.e. <0, 0, 1>
    glm::vec3 lEC(0.0, 0.0, 1.0);

    // Test if normal points towards light source, i.e. if polygon
    // faces toward the light - if not then no diffuse or specular
    // contribution
    float dp = glm::dot(nEC, lEC);
    if (dp > 0.0) {
        // Calculate diffuse and specular contribution

        // Lambert diffuse: D=Ld×Md×cosθ
        // Ld: default diffuse light color for GL_LIGHT0 is white (1.0, 1.0, 1.0).
        // Md: default diffuse material color is grey (0.8, 0.8, 0.8).
        glm::vec3 Ld(1.0);
        glm::vec3 Md(0.8);
        // Need normalized normal to calculate cosθ,
        // light vector <0, 0, 1> is already normalized 
        nEC = glm::normalize(nEC);
        float NdotL = glm::dot(nEC, lEC);
        glm::vec3 diffuse(Ld * Md * NdotL);
        color += diffuse;

        // Blinn-Phong specular: S=Ls×Ms×cosⁿα
        // Ls: default specular light color for LIGHT0 is white (1.0, 1.0, 1.0)
        // Ms: specular material color, also set to white (1.0, 1.0, 1.0),
        // but default for fixed pipeline is black, which means can't see
        // specular reflection. Need to set it to same value for fixed
        // pipeline lighting otherwise will look different.
        glm::vec3 Ls(1.0);
        glm::vec3 Ms(1.0);
        // Default viewer is at infinity along z axis <0, 0, 1> i.e. a
        // non local viewer (see glLightModel and GL_LIGHT_MODEL_LOCAL_VIEWER)
        glm::vec3 vEC(0.0, 0.0, 1.0);
        // Blinn-Phong half vector (using a single capital letter for
        // variable name!). Need normalized H (and nEC, above) to calculate cosα.
        glm::vec3 H(lEC + vEC);
        H = glm::normalize(H);
        float NdotH = glm::dot(nEC, H);
        if (NdotH < 0.0)
            NdotH = 0.0;
        glm::vec3 specular(Ls * Ms * powf(NdotH, shininess));
        color += specular;
    }

    return color;
}

void drawNormals() {
    const float A1 = 0.25, k1 = 2.0 * 3.14, w1 = 0.25;
    const float A2 = 0.25, k2 = 2.0 * 3.14, w2 = 0.25;
    float stepSize = 2.0 / g.tess;
    glm::vec3 r, n, rEC, nEC;
    float t = g.t;

    for (int j = 0; j <= g.tess; j++) {
        for (int i = 0; i <= g.tess; i++) {
            r.x = -1.0 + i * stepSize;
            r.z = -1.0 + j * stepSize;

            n.y = 1.0;
            n.x = -A1 * k1 * cosf(k1 * r.x + w1 * t);
            if (g.waveDim == 2) {
                r.y = A1 * sinf(k1 * r.x + w1 * t);
                n.z = 0.0;
            }
            else {
                r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
                n.z = -A2 * k2 * cosf(k2 * r.z + w2 * t);
            }

            rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
            nEC = normalMatrix * glm::normalize(n);
            drawVector(rEC, nEC, 0.05, true, yellow);
        }
    }
}

void drawGrid(int tess)
{
    float stepSize = 2.0 / tess;
    glm::vec3 r, n, rEC, nEC;
    int i, j;

    if (g.lighting && g.fixed) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);
        if (g.twoside)
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glMaterialfv(GL_FRONT, GL_SPECULAR, &white[0]);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    else {
        glDisable(GL_LIGHTING);
        glColor3fv(&cyan[0]);
    }

    if (g.polygonMode == line)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for (j = 0; j < tess; j++) {
        glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= tess; i++) {
            r.x = -1.0 + i * stepSize;
            r.y = 0.0;
            r.z = -1.0 + j * stepSize;

            if (g.lighting) {
                n.x = 0.0;
                n.y = 1.0;
                n.z = 0.0;
            }

            rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
            if (g.lighting) {
                nEC = normalMatrix * glm::normalize(n);
                if (g.fixed) {
                    glNormal3fv(&nEC[0]);
                }
                else {
                    glm::vec3 c = computeLighting(rEC, nEC);
                    glColor3fv(&c[0]);
                }
            }
            glVertex3fv(&rEC[0]);

            r.z += stepSize;

            rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
            if (g.lighting) {
                nEC = normalMatrix * glm::normalize(n);
                if (g.fixed) {
                    glNormal3fv(&nEC[0]);
                }
                else {
                    glm::vec3 c = computeLighting(rEC, nEC);
                    glColor3fv(&c[0]);
                }
            }
            glVertex3fv(&rEC[0]);
        }

        glEnd();

    }

    if (g.lighting) {
        glDisable(GL_LIGHTING);
    }

    // Normals
    if (g.drawNormals) {
        for (j = 0; j <= tess; j++) {
            for (i = 0; i <= tess; i++) {
                r.x = -1.0 + i * stepSize;
                r.y = 0.0;
                r.z = -1.0 + j * stepSize;

                n.y = 1.0;
                n.x = 0.0;
                n.z = 0.0;
                rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
                nEC = normalMatrix * glm::normalize(n);
                drawVector(rEC, nEC, 0.05, true, yellow);
            }
        }
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("%s %d\n", __FILE__, __LINE__);
        printf("displaySineWave: %s\n", gluErrorString(err));
    }
}

void updateOffset() {
    for (int i = 0; i < g.tess + 1; i++) {
        offset[i] = (GLvoid *) (i * (g.tess+1) * 2 * sizeof(GLuint));
    }
}

void updateIndicies() {
    int index = 0;

    for (int i = 0; i < g.tess; i++) {
        for (int j = 0; j < (g.tess + 1); j++) {
            indicies[index] = (i * (g.tess + 1)) + j;
            index++;
            indicies[index] = ((i + 1) * (g.tess + 1)) + j;
            index++;
        }

        i++;

        for (int j = g.tess; j >= 0; j--) {
            indicies[index] = ((i + 1) * (g.tess + 1)) + j;
            index++;
            indicies[index] = (i * (g.tess + 1)) + j;
            index++;
        }
    }
}

void updateNormals() {
    int index = 0;
    float t = g.t;
    const float A2 = 0.25, k2 = 2.0 * M_PI, w2 = 0.25;
    const float A1 = 0.25, k1 = 2.0 * M_PI, w1 = 0.25;

    if (g.waveDim == 2) {
        for (int i = 0; i <= g.tess; i++) {
            for (int j = 0; j <= g.tess; j++) {
                normal[index].x = -A1 * k1 * cosf(k1 * vertex[index].x + w1 * t);
                normal[index].y = 1.0f;
                normal[index].z = 0.0f;

                glm::vec3 normalMath = normalMatrix * glm::normalize(normal[index]);
                if (g.fixed) {
                    normal[index] = normalMath;
                } else {
                    glm::vec3 compute = computeLighting(vertex[index], normalMath);
                    normal[index] = compute;
                }

                index++;
            }
        }
    } else if (g.waveDim == 3) {
        for (int i = 0; i <= g.tess; i++) {
            for (int j = 0; j <= g.tess; j++) {
                normal[index].x = -A1 * k1 * cosf(k1 * vertex[index].x + w1 * t);
                normal[index].y = 1.0f;
                normal[index].z = -A2 * k2 * cosf(k2 * vertex[index].z + w2 * t);

                glm::vec3 normalMath = normalMatrix * glm::normalize(normal[index]);
                if (g.fixed) {
                    normal[index] = normalMath;
                } else {
                    glm::vec3 compute = computeLighting(vertex[index], normalMath);
                    normal[index] = compute;
                }

                index++;
            }
        }
    }
}

void updateVertex() {
    int index = 0;
    float t = g.t;
    float stepSize = 2.0f / (float) g.tess;
    const float A2 = 0.25, k2 = 2.0 * M_PI, w2 = 0.25;
    const float A1 = 0.25, k1 = 2.0 * M_PI, w1 = 0.25;

    if (g.waveDim == 2) {
        for (int i = 0; i <= g.tess; i++) {
            for (int j = 0; j <= g.tess; j++) {
                vertex[index].x = -1.0f + j * stepSize;
                vertex[index].y = A1 * sinf(k1 * vertex[index].x + w1 * t);
                vertex[index].z = -1.0f + i * stepSize;

                vertex[index] = glm::vec3(modelViewMatrix * glm::vec4(vertex[index], 1.0));
                index++;
            }
        }
    } else if (g.waveDim == 3) {
        for (int i = 0; i <= g.tess; i++) {
            for (int j = 0; j <= g.tess; j++) {
                vertex[index].x = -1.0f + j * stepSize;
                vertex[index].z = -1.0f + i * stepSize;
                vertex[index].y = A1 * sinf(k1 * vertex[index].x + w1 * t) + A2 * sinf(k2 * vertex[index].z + w2 * t);

                vertex[index] = glm::vec3(modelViewMatrix * glm::vec4(vertex[index], 1.0));
                index++;
            }
        }
    }
}

void allocateVertex() {
    auto *tempVertex = (glm::vec3 * ) realloc(vertex, (g.tess+1) * (g.tess+1) * sizeof(glm::vec3));
    vertex = tempVertex;
}

void allocateNormals() {
    auto *tempNormal = (glm::vec3 *) realloc(normal, (g.tess+1) * (g.tess+1) * sizeof(glm::vec3));
    normal = tempNormal;
}

void allocateIndicies() {
    auto *tempindicies = (int *) realloc(indicies, (g.tess+1) * g.tess * 2 * sizeof(int));
    indicies = tempindicies;
}

void allocateOffset() {
    auto **tempOffset = (GLvoid **) realloc(offset, g.tess * 2 * sizeof(GLvoid *));
    offset = tempOffset;
}

void updateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, buffers[vertecies]);
    glBufferData(GL_ARRAY_BUFFER, (g.tess+1) * (g.tess+1) * sizeof(glm::vec3), vertex, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[normals]);
    glBufferData(GL_ARRAY_BUFFER, (g.tess+1) * (g.tess+1) * sizeof(glm::vec3), normal, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[offsetBuffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (g.tess+1) * (g.tess+1) * sizeof(int), indicies, GL_STATIC_DRAW);
    return;
}

void updateSineWave() {
    // Allocate memory
    allocateVertex();
    allocateNormals();
    allocateIndicies();
    allocateOffset();

    // Update all VBO data
    updateIndicies();
    updateOffset();
    updateVertex();

    if (g.lighting) {
        updateNormals();
    }

    // Update all buffers
    updateBuffers();
}

void drawSineWaveVBO(int tess) {
    glPushAttrib(GL_CURRENT_BIT);
    glPushMatrix();

    if (g.lighting) {
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH);
        glMaterialfv(GL_FRONT, GL_SPECULAR, &white[0]);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    } else {
        glDisable(GL_LIGHTING);
        glColor3fv(&cyan[0]);
    }

    if (g.polygonMode == line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // VBO Sine wave?

    glBindBuffer(GL_ARRAY_BUFFER, buffers[vertecies]);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[normals]);
    glNormalPointer(GL_FLOAT, 0, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[offsetBuffer]);

    for(int i = 0; i < g.tess; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, (g.tess+1)*2 , GL_UNSIGNED_INT, offset[i]);
    }


    if (g.lighting) {
        glDisable(GL_LIGHTING);
    }

    if (g.drawNormals) {
        drawNormals();
    }

    glPopAttrib();
    glPopMatrix();
}

void drawSineWave(int tess)
{
    const float A1 = 0.25, k1 = 2.0 * M_PI, w1 = 0.25;
    const float A2 = 0.25, k2 = 2.0 * M_PI, w2 = 0.25;
    float stepSize = 2.0 / tess;
    glm::vec3 r, n, rEC, nEC;
    int i, j;
    float t = g.t;

    if (g.lighting && g.fixed) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        glShadeModel(GL_SMOOTH);
        if (g.twoside)
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glMaterialfv(GL_FRONT, GL_SPECULAR, &white[0]);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    else {
        glDisable(GL_LIGHTING);
        glColor3fv(&cyan[0]);
    }

    if (g.polygonMode == line)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Sine wave
    for (j = 0; j < tess; j++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (i = 0; i <= tess; i++) {
            r.x = -1.0 + i * stepSize;
            r.z = -1.0 + j * stepSize;

            if (g.waveDim == 2) {
                r.y = A1 * sinf(k1 * r.x + w1 * t);
                if (g.lighting) {
                    n.x = -A1 * k1 * cosf(k1 * r.x + w1 * t);
                    n.y = 1.0;
                    n.z = 0.0;
                }
            }
            else if (g.waveDim == 3) {
                r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
                if (g.lighting) {
                    n.x = -A1 * k1 * cosf(k1 * r.x + w1 * t);
                    n.y = 1.0;
                    n.z = -A2 * k2 * cosf(k2 * r.z + w2 * t);
                }
            }

            rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
            if (g.lighting) {
                nEC = normalMatrix * glm::normalize(n);
                if (g.fixed) {
                    glNormal3fv(&nEC[0]);
                }
                else {

                    glm::vec3 c = computeLighting(rEC, nEC);
                    glColor3fv(&c[0]);
                }
            }
            glVertex3fv(&rEC[0]);

            r.z += stepSize;

            if (g.waveDim == 3) {
                r.y = A1 * sinf(k1 * r.x + w1 * t) + A2 * sinf(k2 * r.z + w2 * t);
                if (g.lighting) {
                    n.z = -A2 * k2 * cosf(k2 * r.z + w2 * t);
                }
            }

            rEC = glm::vec3(modelViewMatrix * glm::vec4(r, 1.0));
            if (g.lighting) {
                nEC = normalMatrix * glm::normalize(n);
                if (g.fixed) {
                    glNormal3fv(&nEC[0]);
                }
                else {
                    glm::vec3 c = computeLighting(rEC, nEC);
                    glColor3fv(&c[0]);
                }
            }
            glVertex3fv(&rEC[0]);
        }

        glEnd();

    }

    if (g.lighting) {
        glDisable(GL_LIGHTING);
    }

    // Normals
    if (g.drawNormals) {
        drawNormals();
    }

    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("%s %d\n", __FILE__, __LINE__);
        printf("displaySineWave: %s\n", gluErrorString(err));
    }
}


void idle()
{
    float t, dt;

    t = SDL_GetTicks() / milli;

    // Accumulate time if animation enabled
    if (g.animate) {
        dt = t - g.lastT;
        g.t += dt;
        g.lastT = t;
    }

    // Update stats, although could make conditional on a flag set interactively
    dt = (t - g.lastStatsDisplayT);
    if (dt > g.displayStatsInterval) {
        g.frameRate = g.frameCount / dt;
        g.lastStatsDisplayT = t;
        g.frameCount = 0;
    }

    postRedisplay();
}

void displayMultiView() //done
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 modelViewMatrixSave(modelViewMatrix);
    glm::mat3 normalMatrixSave(normalMatrix);

    if (g.animate || prevDim!= g.waveDim || prevLighting!= g.lighting || prevTess!= g.tess) {
        updateSineWave();
        prevDim = g.waveDim;
        prevLighting = g.lighting;
        prevTess = g.tess;
    }

    // Front view
    modelViewMatrix = glm::mat4(1.0);
    glViewport(g.width / 16.0, g.height * 9.0 / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);
    drawAxes(5.0);
    if (g.shape == grid)
        drawGrid(g.tess);
    else {
        if (!g.vbo) {
            drawSineWave(g.tess);
        } else {
            drawSineWaveVBO(g.tess);
        }
    }


    // Top view
    modelViewMatrix = glm::mat4(1.0);
    modelViewMatrix = glm::rotate(modelViewMatrix, glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0));
    glViewport(g.width / 16.0, g.height / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16);
    drawAxes(5.0);
    if (g.shape == grid)
        drawGrid(g.tess);
    else {
        if (!g.vbo) {
            drawSineWave(g.tess);
        } else {
            drawSineWaveVBO(g.tess);
        }
    }

    // Left view
    modelViewMatrix = glm::mat4(1.0);
    modelViewMatrix = glm::rotate(modelViewMatrix, glm::pi<float>() / 2.0f, glm::vec3(0.0, 1.0, 0.0));
    glViewport(g.width * 9.0 / 16.0, g.height * 9.0 / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);
    drawAxes(5.0);
    if (g.shape == grid)
        drawGrid(g.tess);
    else {
        if (!g.vbo) {
            drawSineWave(g.tess);
        } else {
            drawSineWaveVBO(g.tess);
        }
    }

    // General view
    modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateX * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
    modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateY * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
    modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(camera.scale));
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
    glViewport(g.width * 9.0 / 16.0, g.width / 16.0, g.width * 6.0 / 16.0, g.height * 6.0 / 16.0);
    drawAxes(5.0);
    if (g.shape == grid)
        drawGrid(g.tess);
    else {
        if (!g.vbo) {
            drawSineWave(g.tess);
        } else {
            drawSineWaveVBO(g.tess);
        }
    }


    //if (g.displayOSD)
    //    displayOSD();

    if (g.consolePM)
        consolePM();

    g.frameCount++;

    // Does the same thing as glutSwapBuffers
    SDL_GL_SwapWindow(window);

}

void display() //done
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glViewport(0, 0, g.width, g.height);

    // General view

    if (g.animate || prevDim!= g.waveDim || prevLighting!= g.lighting || prevTess!= g.tess) {
        updateSineWave();
        prevDim = g.waveDim;
        prevLighting = g.lighting;
        prevTess = g.tess;
    }

    modelViewMatrix = glm::mat4(1.0);
    normalMatrix = glm::mat3(1.0);

    modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateX * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
    modelViewMatrix = glm::rotate(modelViewMatrix, camera.rotateY * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
    modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(camera.scale));

    normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));


    drawAxes(5.0);
    if (g.shape == grid)
        drawGrid(g.tess);
    else {
        if (!g.vbo) {
            drawSineWave(g.tess);
        } else {
            drawSineWaveVBO(g.tess);
        }
    }

    //if (g.displayOSD)
    //    displayOSD();

    if (g.consolePM)
        consolePM();

    // Does the same thing as glutSwapBuffers
    SDL_GL_SwapWindow(window);

    g.frameCount++;

    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("%s %d\n", __FILE__, __LINE__);
        printf("display: %s\n", gluErrorString(err));
    }
}

void keyDown(SDL_KeyboardEvent *e)
{
    /*
        ESCAPE  : close
        A       : toggle animation
        L       : toggle lighting
        F       : no idea
        M       : toggle wireframe
        N       : toggle normals
        C       : print stats to console
        S       : switch between grid and wave
        4       : toggle multiview
        V       : toggle VBOs?
        F1      : increase tesselation
        F2      : decrease tesselation
        Z       : switch between wave and hill thing
    */
    switch (e->keysym.sym) {
    case SDLK_ESCAPE:
        printf("exit\n");
        exit(0);
        break;
    case SDLK_a:
        printf("a; toggling animation\n");
        g.animate = !g.animate;
        if (g.animate) {
            g.lastT = SDL_GetTicks() / milli;//TODO
        }
        break;
    case SDLK_l:
        printf("l; toggling lighting\n");
        g.lighting = !g.lighting;
        postRedisplay();
        break;
    case SDLK_f:
        printf("f; toggling \'fixed\'\n");
        g.fixed = !g.fixed;
        postRedisplay();
        break;
    case SDLK_m:
        printf("%d\n", g.polygonMode);
        if (g.polygonMode == line)
            g.polygonMode = fill;
        else
            g.polygonMode = line;
        postRedisplay();
        break;
    case SDLK_n:
        printf("n; toggling normals\n");
        g.drawNormals = !g.drawNormals;
        postRedisplay();
        break;
    case SDLK_c:
        g.consolePM = !g.consolePM;
        postRedisplay();
        break;
    case SDLK_s:
        printf("s; changing shape\n");
        g.shape = g.shape == grid ? wave : grid;
        g.animate = false;
        postRedisplay();
        break;
    case SDLK_4:
        printf("4; toggling multiview\n");
        g.multiView = !g.multiView;
        if (g.multiView)
            displayFunc = &displayMultiView;
        else
            displayFunc = &display;
        postRedisplay();
        break;
    case SDLK_v:
        printf("v; toggling VBO\n");
        g.vbo = !g.vbo;
        break;
    case SDLK_F1:
        printf("increasing tesselation\n");
        g.tess *= 2;
        postRedisplay();
        break;
    case SDLK_F2:
        printf("decreasing tesselation\n");
        g.tess /= 2;
        if (g.tess < 8)
            g.tess = 8;
        postRedisplay();
        break;
    case SDLK_z:
        printf("z; changing wave\n");
        g.waveDim++;
        if (g.waveDim > 3)
            g.waveDim = 2;
        postRedisplay();
        break;
    default:
        break;
    }
}

void mouse(int button, int x, int y)
{
    camera.lastX = x;
    camera.lastY = y;

    switch(button) {
        case SDL_BUTTON_LEFT:
            camera.control = rotate;
            break;
        case SDL_BUTTON_MIDDLE:
            camera.control = pan;
            break;
        case SDL_BUTTON_RIGHT:
            camera.control = zoom;
            break;
        default:
            break;
    }
}

void motion(int x, int y)
{
    float dx, dy;

    dx = x - camera.lastX;
    dy = y - camera.lastY;
    camera.lastX = x;
    camera.lastY = y;

    switch (camera.control) 
    {
        case inactive:
            break;
        case rotate:
            camera.rotateX += dy;
            camera.rotateY += dx;
            break;
        case pan:
            break;
        case zoom:
            camera.scale += dy / 100.0;
            break;
    }

    postRedisplay();
}

void eventDispatcher()
{
    SDL_Event e;

    // Handle events, stolen from robot example
    while (SDL_PollEvent(&e)) 
    {
        switch (e.type) 
        {
            case SDL_QUIT:
                quit(0);
                break;
            case SDL_MOUSEMOTION:
                motion(e.motion.x, e.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse(e.button.button, e.button.x, e.button.y);
                break;
            case SDL_MOUSEBUTTONUP:
                camera.control = inactive;
                break;
            case SDL_KEYDOWN:
                keyDown(&e.key);
                break;
            case SDL_WINDOWEVENT:
                switch (e.window.event)
                {
                    case SDL_WINDOWEVENT_SHOWN:
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        if (e.window.windowID == SDL_GetWindowID(window))
                        {
                            SDL_SetWindowSize(window, e.window.data1, e.window.data2);
                            reshape(e.window.data1, e.window.data2);
                            postRedisplay();
                        }
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void update()
{
    //physics and ai stuff
    idle();
}

void sys_shutdown()
{
    SDL_Quit();
}

void mainLoop() //done
{
    while (1) {
        eventDispatcher();
        if (wantRedisplay) {
            displayFunc();
            wantRedisplay = 0;
        }
        update();
    }
}

int initGraphics()
{
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window =
        SDL_CreateWindow("Robot Arm Using SDL2",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "%s:%d: create window failed: %s\n",
            __FILE__, __LINE__, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_GLContext mainGLContext = SDL_GL_CreateContext(window);
    if (mainGLContext == 0) {
        fprintf(stderr, "%s:%d: create context failed: %s\n",
            __FILE__, __LINE__, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    reshape(w, h);

    return 0;
}

void initVBO() {
    glGenBuffers(3, buffers);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

}

int main(int argc, char** argv) //done
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s:%d: unable to init SDL: %s\n",
            __FILE__, __LINE__, SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Set up the window and OpenGL rendering context 
    if (initGraphics()) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // OpenGL initialisation, must be done before any OpenGL calls
    init();
    initVBO();

    atexit(sys_shutdown);

    mainLoop();

    return 0;
}
