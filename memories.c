/* memories -- Visualize the contents of main memory.
 * Copyright 2021-2023 Justin J. Meza
 */

#include <GL/glut.h>
#include <math.h>
#include <sys/mman.h>
#include <png.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#define __USE_GNU
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <unistd.h>

#undef DEBUG

void debug(char* msg, ...)
{
#ifdef DEBUG
    va_list list;
    va_start(list, msg);
    vfprintf(stderr, msg, list);
    fflush(stderr);
    va_end(list);
#else
#endif
}

/* Segfault handler. */
void handler(int num, siginfo_t* info, void* context)
{
    ucontext_t* c = (ucontext_t*)context;

    /* Increment the instruction pointer to get past the load. */
    c->uc_mcontext.__gregs[REG_RIP]++;

    /* Provide a default value for the inaccessible location. */
    c->uc_mcontext.__gregs[REG_RAX] = 0x0;
}

const int data_size = 1 << 20;
static int* data;
void init_data() {
    /* Zero out pre-allocated data. */
    for (int n = 0; n < data_size; n++) {
        data[n] = 0;
    }

    /* /1* Random out un-allocated data. *1/ */
    /* data = malloc(sizeof(int) * data_size); */
    /* for (int n = 0; n < data_size; n++) { */
    /*     data[n] = rand(); */
    /*     debug("n = %d, data[n] = %d\n", n, data[n]); */
    /* } */
}

int* base;

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    srand(time(NULL));
    base = data;

    /* set up the signal handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    sigaction(SIGSEGV, &action, NULL);
}

/* Axis orientation:
 * - x points rightward, out of the screen,
 * - y points leftward, out of the screen,
 * - z points upward, parallel to the screen.
 */
float cube_size = 2.0;
float cube_padding = 0.0;
void draw_cube(int x, int y, int z, float r, float g, float b, float a)
{
    double dist = sqrt(1 / 3.0);
    glPushMatrix();
    glTranslated(z * cube_size, y * cube_size, x * cube_size);
    glColor4d(r, g, b, a);
    glutSolidCube(cube_size - cube_padding);
    /* glutWireCube(1);  // wireframe */
    glPopMatrix();
}

int x_size = (1 << 4) - 1;
int y_size = (1 << 4) - 1;
int z_size = (1 << 4) - 1;
int x_offset = 0;
int y_offset = 0;
int z_offset = 0;
int z_toggle = 0xffffffff;

int* compute_addr(int* base, int x, int y, int z) {
    int offset = (z + z_offset) * (x_size * y_size) \
                 + (y + y_offset) * y_size + (x + x_offset);
    unsigned int* addr = (unsigned int*)base + offset;
    /* printf("offset = %d, data[offset] = %d\n", offset, *addr);  // debug */
    /* fflush(stdout);  // debug */
    return addr;
}

void rgba(int* addr, int* r, int* g, int* b, int* a)
{
    unsigned char* color = (unsigned char*)addr;
    *r = color[0];
    *g = color[1];
    *b = color[2];
    *a = color[3];
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* /1* Draw axes. *1/ */
    /* glBegin(GL_LINES); */
    /* glColor4d(1.0, 0.0, 0.0, 0.5); */
    /* glVertex3d(0.0, 0.0, 0.0); */
    /* glVertex3d(1.0, 0.0, 0.0); */
    /* glColor4d(0.0, 1.0, 0.0, 0.5); */
    /* glVertex3d(0.0, 0.0, 0.0); */
    /* glVertex3d(0.0, 1.0, 0.0); */
    /* glColor4d(0.0, 0.0, 1.0, 0.5); */
    /* glVertex3d(0.0, 0.0, 0.0); */
    /* glVertex3d(0.0, 0.0, 1.0); */
    /* glEnd(); */

    /* Draw a range of memory. */
    for (int z = 0; z < z_size; z++) {
        if (((z_toggle >> z) & 0x1) == 0) continue;
        for (int y = 0; y < y_size; y++) {
            for (int x = 0; x < z_size; x++) {
                unsigned int* addr = compute_addr(base, x, y, z);
                int r, g, b, a;
                rgba(addr, &r, &g, &b, &a);

                draw_cube(x, y, z, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
            }
        }
    }

    glDisable(GL_BLEND);

    glFlush();

    glutSwapBuffers();
}

float zoom = 1.0;

void reshape(GLsizei width, GLsizei height) {
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    /* Render on entire window. */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* Place the camera at a distance to reduce near clipping. */
    float ortho = 50.0 * (1.0 / zoom);

    /* Ensure unit axes are equal length on the screen. */
    glOrtho(-ortho * aspect, ortho * aspect, -ortho, ortho, -ortho, ortho);

    /* Use this length so that camera is 1 unit away from origin. */
    double dist = sqrt(1 / 3.0);

    /* Set up an isometric projection matrix. */
    gluLookAt(dist, dist, dist,  /* Position of camera. */
              0.0,  0.0,  0.0,   /* Where camera is pointing at. */
              1.0,  0.0,  0.0);  /* Which direction is up. */
}

extern char etext, edata, end;

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        /* Quit. */
        case 'q':
            exit(0);
        /* Scroll up by one block. */
        case 'j':
            z_offset--;
            break;
        /* Scroll down by one block. */
        case 'k':
            z_offset++;
            break;
        /* Scroll up by one chunk. */
        case 'b':
            z_offset = z_offset - z_size;
            break;
        /* Scroll down by one chunk. */
        case 'f':
            z_offset = z_offset + z_size;
            break;
        /* Zoom out by one block. */
        case '+':
            x_size--;
            y_size--;
            z_size--;
            break;
        /* Zoom in by one block. */
        case '-':
            x_size++;
            y_size++;
            z_size++;
            break;
        /* Toggle z layers. */
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
                int index = key - '0';
                z_toggle ^= (1 << index);
                break;
            }
        /* Zoom in. */
        case 'h':
            {
                zoom -= 0.1;
                int screen[4];
                glGetIntegerv(GL_VIEWPORT, screen);
                int width = screen[2];
                int height = screen[3];
                reshape(width, height);
                break;
            }
        case 'l':
            {
                zoom += 0.1;
                int screen[4];
                glGetIntegerv(GL_VIEWPORT, screen);
                int width = screen[2];
                int height = screen[3];
                reshape(width, height);
                break;
            }
        /* Jump to text segment. */
        case 't':
            base = (int *)&etext;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* Jump to data segment. */
        case 'd':
            base = (int *)&edata;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* Jump to end segment. */
        case 'e':
            base = (int *)&end;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* Jump to address. */
        case 'a':
            base = data;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* Take a snapshot. */
        case 's':
            {
                int screen[4];
                glGetIntegerv(GL_VIEWPORT, screen);

                /* Read the image data. */
                int width = screen[2];
                int height = screen[3];
                unsigned char* pixels = malloc(sizeof(unsigned char) * width * height * 4);
                glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

                /* Write the image data. */
                FILE* f;

                char title[100];
                int* base = data;
                sprintf(title, "memories_%d_%p.png", time(NULL), compute_addr(base, 0, 0, 0));
                debug("title is %s\n", title);

                f = fopen (title, "wb");
                if (!f) {
                    printf("failed to open file\n");
                    /* break; */
                }

                png_structp png = NULL;
                png_infop info = NULL;
                png_byte** rows = NULL;

                png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
                info = png_create_info_struct(png);
                png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA,
                        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                        PNG_FILTER_TYPE_DEFAULT);
                rows = png_malloc(png, height * sizeof (png_byte *));

                for (y = 0; y < height; y++) {
                    png_byte* row =
                        png_malloc(png, sizeof(uint8_t) * width * 4);
                    rows[height - y - 1] = row;
                    for (x = 0; x < width; x++) {
                        unsigned char* pixel = pixels + ((y * width) + x) * 4;
                        *row++ = pixel[0];
                        *row++ = pixel[1];
                        *row++ = pixel[2];
                        *row++ = pixel[3];
                    }
                }

                png_init_io(png, f);
                png_set_rows(png, info, rows);
                png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

                for (y = 0; y < height; y++) {
                    png_free(png, rows[y]);
                }
                png_free(png, rows);

                fclose(f);
            }
        break;
    }

    glutPostRedisplay();
}

/* Co-routine definitions. */

double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 - x); }
double init_weight() { return ((double)rand())/((double)RAND_MAX); }
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int *map(int *data, int *offset, int size, int num)
{
    int *addr = data + *offset;
    *offset = *offset + (size * num);
    return addr;
}

/* Algorithm implementation based on:
 * https://towardsdatascience.com/simple-neural-network-implementation-in-c-663f51447547.
 */
void nn_sgd() {
    static const int numInputs = 2;
    static const int numHiddenNodes = 2;
    static const int numOutputs = 1;

    int offset = 0;
    double *hiddenLayer = (double *)map(data, &offset, sizeof(double), numHiddenNodes);
    double *outputLayer = (double *)map(data, &offset, sizeof(double), numOutputs);
    double *hiddenLayerBias = (double *)map(data, &offset, sizeof(double), numHiddenNodes);
    double *outputLayerBias = (double *)map(data, &offset, sizeof(double), numOutputs);

    double **hiddenWeights = (double **)map(data, &offset, sizeof(double *), numInputs);
    for (int n = 0; n < numInputs; n++) { hiddenWeights[n] = (double *)map(data, &offset, sizeof(double), numHiddenNodes); }

    double **outputWeights = (double **)map(data, &offset, sizeof(double *), numHiddenNodes);
    for (int n = 0; n < numHiddenNodes; n++) { outputWeights[n] = (double *)map(data, &offset, sizeof(double), numOutputs); }

    for (int n = 0; n < numInputs; n++) {
        for (int o = 0; o < numHiddenNodes; o++) {
            hiddenWeights[n][o] = init_weight();
        }
    }

    for (int n = 0; n < numHiddenNodes; n++) {
        for (int o = 0; o < numOutputs; o++) {
            outputWeights[n][o] = init_weight();
        }
    }

    static const int numTrainingSets = 4;
    double **training_inputs = (double **)map(data, &offset, sizeof(double *), numTrainingSets);
    for (int n = 0; n < numTrainingSets; n++) { training_inputs[n] = (double *)map(data, &offset, sizeof(double), numInputs); }
    double **training_outputs = (double **)map(data, &offset, sizeof(double *), numTrainingSets);
    for (int n = 0; n < numTrainingSets; n++) { training_outputs[n] = (double *)map(data, &offset, sizeof(double), numOutputs); }

    /* xor */
    training_inputs[0] = (double[2]){0.0f,0.0f};
    training_inputs[1] = (double[2]){1.0f,0.0f};
    training_inputs[2] = (double[2]){0.0f,1.0f};
    training_inputs[3] = (double[2]){1.0f,1.0f};
    training_outputs[0] = (double[1]){0.0f};
    training_outputs[1] = (double[1]){1.0f};
    training_outputs[2] = (double[1]){1.0f};
    training_outputs[3] = (double[1]){0.0f};

    int epochs = 10000;
    for (int n=0; n < epochs; n++) {
        int trainingSetOrder[] = {0,1,2,3};
        shuffle(trainingSetOrder,numTrainingSets);

        for (int x=0; x<numTrainingSets; x++) {
            int i = trainingSetOrder[x];

            for (int j=0; j<numHiddenNodes; j++) {
                double activation=hiddenLayerBias[j];
                for (int k=0; k<numInputs; k++) {
                    activation+=training_inputs[i][k]*hiddenWeights[k][j];
                }
                hiddenLayer[j] = sigmoid(activation);
            }

            for (int j=0; j<numOutputs; j++) {
                double activation=outputLayerBias[j];
                for (int k=0; k<numHiddenNodes; k++) {
                    activation+=hiddenLayer[k]*outputWeights[k][j];
                }
                outputLayer[j] = sigmoid(activation);
            }

            double deltaOutput[numOutputs];
            for (int j=0; j<numOutputs; j++) {
                double dError = (training_outputs[i][j]-outputLayer[j]);
                deltaOutput[j] = dError*dSigmoid(outputLayer[j]);
            }

            double deltaHidden[numHiddenNodes];
            for (int j=0; j<numHiddenNodes; j++) {
                double dError = 0.0f;
                for(int k=0; k<numOutputs; k++) {
                    dError+=deltaOutput[k]*outputWeights[j][k];
                }
                deltaHidden[j] = dError*dSigmoid(hiddenLayer[j]);
            }

            double lr = 0.1;
            for (int j=0; j<numOutputs; j++) {
                outputLayerBias[j] += deltaOutput[j]*lr;
                for (int k=0; k<numHiddenNodes; k++) {
                    outputWeights[k][j]+=hiddenLayer[k]*deltaOutput[j]*lr;
                }
            }
            for (int j=0; j<numHiddenNodes; j++) {
                hiddenLayerBias[j] += deltaHidden[j]*lr;
                for(int k=0; k<numInputs; k++) {
                    hiddenWeights[k][j]+=training_inputs[i][k]*deltaHidden[j]*lr;
                    debug("hiddenWeight = %f\n", hiddenWeights[k][j]);
                }
            }
        }

        sleep(1);
    }
}

void idle(void)
{
    glutPostRedisplay();
}

void coroutine(void)
{
    debug("Starting co-routine...\n");

    /* /1* Option 1: random values *1/ */
    /* while (1) { */
    /*     for (int n = 0; n < data_size; n++) { */
    /*         int val = rand(); */
    /*         /1* printf("setting data[%d] to %d\n", n, val); *1/ */
    /*         data[n] = val; */
    /*     } */
    /*     sleep(1); */
    /* } */

    /* Option 2: neural network. */
    nn_sgd();
}

int main(int argc, char** argv)
{
    int f = open("/dev/zero", O_RDWR);
    data = mmap(NULL, sizeof(int) * data_size, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
    init_data();
    close(f);

    if (fork() == 0) {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowSize(500, 500);
        glutInitWindowPosition(50, 50);
        glutCreateWindow("memories");
        init();
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keyboard);
        glutIdleFunc(idle);
        glutMainLoop();
    } else {
        coroutine();
    /* /1* Zero out pre-allocated data. *1/ */
    /* for (int n = 0; n < data_size; n++) { */
    /*     data[n] = 0; */
    /* } */
    }

    return 0;
}
