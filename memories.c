/* memories visualizes the contents of main memory.
*/

#include <GL/glut.h>
#include <math.h>
#include <png.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/ucontext.h>
#include <unistd.h>

#define DEBUG

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

/* TODO: debug how to include these definitions from /usr/include/x86_64-linux-gnu/sys/ucontext.h */
enum
{
  REG_R8 = 0,
# define REG_R8		REG_R8
  REG_R9,
# define REG_R9		REG_R9
  REG_R10,
# define REG_R10	REG_R10
  REG_R11,
# define REG_R11	REG_R11
  REG_R12,
# define REG_R12	REG_R12
  REG_R13,
# define REG_R13	REG_R13
  REG_R14,
# define REG_R14	REG_R14
  REG_R15,
# define REG_R15	REG_R15
  REG_RDI,
# define REG_RDI	REG_RDI
  REG_RSI,
# define REG_RSI	REG_RSI
  REG_RBP,
# define REG_RBP	REG_RBP
  REG_RBX,
# define REG_RBX	REG_RBX
  REG_RDX,
# define REG_RDX	REG_RDX
  REG_RAX,
# define REG_RAX	REG_RAX
  REG_RCX,
# define REG_RCX	REG_RCX
  REG_RSP,
# define REG_RSP	REG_RSP
  REG_RIP,
# define REG_RIP	REG_RIP
  REG_EFL,
# define REG_EFL	REG_EFL
  REG_CSGSFS,		/* Actually short cs, gs, fs, __pad0.  */
# define REG_CSGSFS	REG_CSGSFS
  REG_ERR,
# define REG_ERR	REG_ERR
  REG_TRAPNO,
# define REG_TRAPNO	REG_TRAPNO
  REG_OLDMASK,
# define REG_OLDMASK	REG_OLDMASK
  REG_CR2
# define REG_CR2	REG_CR2
};

/* segfault handler */
void handler(int num, siginfo_t* info, void* context)
{
    ucontext_t* c = (ucontext_t*)context;

    /* increment the instruction pointer to get past the load */
    c->uc_mcontext.__gregs[REG_RIP]++;

    /* provide a default value for the inaccessible location */
    c->uc_mcontext.__gregs[REG_RAX] = 0x0;
}

const int data_size = 1 << 8;
int* data;
void create_data() {
    data = malloc(sizeof(int) * data_size);
    for (int n = 0; n < data_size; n++) {
        data[n] = rand();
        /* printf("n = %d, data[n] = %d\n", n, data[n]); */
    }
}

int* base;

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /* glClearDepth(1.0f); */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    srand(time(NULL));
    create_data();
    base = data;

    /* set up the signal handler */
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    sigaction(SIGSEGV, &action, NULL);
}

/* x points rightward, out of the screen
 * y points leftward, out of the screen
 * z points upward, parallel to the screen
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
    /* glutWireCube(1); */
    glPopMatrix();
}

int x_size = 1 << 4;
int y_size = 1 << 4;
int z_size = 1 << 4;
int x_offset = 0;
int y_offset = 0;
int z_offset = 0;

int* compute_addr(int* base, int x, int y, int z) {
    int offset = (z + z_offset) * (x_size * y_size) \
                 + (y + y_offset) * y_size + (x + x_offset);
    unsigned int* addr = base + offset;
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

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw axes */
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

    /* draw a range of memory */

    for (int z = 0; z < z_size; z++) {
        for (int y = 0; y < y_size; y++) {
            for (int x = 0; x < z_size; x++) {
                unsigned int* addr = compute_addr(base, x, y, z);
                /* printf("offset = %d, data[offset] = %d\n", offset, *addr); */
                /* fflush(stdout); */
                /* unsigned char* color = (unsigned char*)addr; */
                int r, g, b, a;
                rgba(addr, &r, &g, &b, &a);
                /* float r = r / 255.0; */
                /* float g = g / 255.0; */
                /* float b = b / 255.0; */
                /* float a = a / 255.0; */
                /* float r = color[0] / 255.0; */
                /* float g = color[1] / 255.0; */
                /* float b = color[2] / 255.0; */
                /* float a = color[3] / 255.0; */

                draw_cube(x, y, z, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
            }
        }
    }

    glDisable(GL_BLEND);

    glFlush();

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    /* render on entire window */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* place the camera at a distance to reduce near clipping */
    /* float ortho = 10.0 * (y_size / 3.0); */
    float ortho = 50.0;

    /* ensure unit axes are equal length on the screen */
    glOrtho(-ortho * aspect, ortho * aspect, -ortho, ortho, -ortho, ortho);

    /* use this length so that camera is 1 unit away from origin */
    double dist = sqrt(1 / 3.0);

    /* set up an isometric projection matrix */
    gluLookAt(dist, dist, dist,  /* position of camera */
            0.0,  0.0,  0.0,   /* where camera is pointing at */
            1.0,  0.0,  0.0);  /* which direction is up */
    /* glMatrixMode(GL_MODELVIEW); */
}

extern char etext, edata, end;

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        /* quit */
        case 'q':
            exit(0);
        /* scroll up by one block */
        case 'j':
            z_offset--;
            break;
        /* scroll down by one block */
        case 'k':
            z_offset++;
            break;
        /* scroll up by one chunk */
        case 'b':
            z_offset = z_offset - z_size;
            break;
        /* scroll down by one chunk */
        case 'f':
            z_offset = z_offset + z_size;
            break;
        /* zoom out by one block */
        case '-':
            x_size--;
            y_size--;
            z_size--;
            break;
        /* zoom in by one block */
        case '+':
            x_size++;
            y_size++;
            z_size++;
            break;
        /* jump to text segment */
        case 't':
            base = (int *)&etext;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* jump to data segment */
        case 'd':
            base = (int *)&edata;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* jump to end segment */
        case 'e':
            base = (int *)&end;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* jump to address */
        case 'a':
            base = data;
            x_offset = 0;
            y_offset = 0;
            z_offset = 0;
            break;
        /* take a snapshot */
        case 's':
            {
                int screen[4];
                glGetIntegerv(GL_VIEWPORT, screen);

                /* read the image data */
                int width = screen[2];
                int height = screen[3];
                unsigned char* pixels = malloc(sizeof(unsigned char) * width * height * 4);
                glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

                /* write the image data */
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
                    rows[y] = row;
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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("memories");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
