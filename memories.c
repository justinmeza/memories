/* memories visualizes the contents of main memory.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

const int data_size = 1 << 8;
int* data;
void create_data() {
    data = malloc(sizeof(int) * data_size);
    for (int n = 0; n < data_size; n++) {
        data[n] = rand();
        /* printf("n = %d, data[n] = %d\n", n, data[n]); */
    }
}

GLfloat aspect;

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /* glClearDepth(1.0f); */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);
    /* glDepthFunc(GL_LEQUAL); */
    /* glShadeModel(GL_SMOOTH); */
    /* glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); */

    srand(time(NULL));
    create_data();
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw axes */
    glBegin(GL_LINES);

    glColor4d(1.0, 0.0, 0.0, 0.5);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(1.0, 0.0, 0.0);

    glColor4d(0.0, 1.0, 0.0, 0.5);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 1.0, 0.0);

    glColor4d(0.0, 0.0, 1.0, 0.5);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 1.0);

    glEnd();

    /* draw a range of memory */
    int* base = data;

    for (int z = 0; z < z_size; z++) {
        for (int y = 0; y < y_size; y++) {
            for (int x = 0; x < z_size; x++) {
                int offset = (z + z_offset) * (x_size * y_size) \
                             + (y + y_offset) * y_size + (x + x_offset);
                unsigned int* addr = base + offset;
                /* printf("offset = %d, data[offset] = %d\n", offset, *addr); */
                /* fflush(stdout); */
                unsigned char* color = (unsigned char*)addr;

                float r = color[0] / 255.0;
                float g = color[1] / 255.0;
                float b = color[2] / 255.0;
                float a = color[3] / 255.0;

                draw_cube(x, y, z, r, g, b, a);
            }
        }
    }

    glDisable(GL_BLEND);

    glFlush();

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    aspect = (GLfloat)width / (GLfloat)height;

    /* render on entire window */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* ensure unit axes are equal length on the screen */
    float ortho = 10.0 * (y_size / 3.0);
    glOrtho(-ortho * aspect, ortho * aspect, -ortho, ortho, -ortho, ortho);

    /* use this length so that camera is 1 unit away from origin */
    double dist = sqrt(1 / 3.0);

    /* set up an isometric projection matrix */
    gluLookAt(dist, dist, dist,  /* position of camera */
            0.0,  0.0,  0.0,   /* where camera is pointing at */
            1.0,  0.0,  0.0);  /* which direction is up */
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'j':
            z_offset--;
            break;
        case 'k':
            z_offset++;
            break;
        case 'b':
            z_offset = z_offset - z_size;
            break;
        case 'f':
            z_offset = z_offset + z_size;
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
