/* memories visualizes the contents of main memory.
*/

#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

GLfloat ASPECT;

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /* glClearDepth(1.0f); */
    glEnable(GL_DEPTH_TEST);
    /* glDepthFunc(GL_LEQUAL); */
    /* glShadeModel(GL_SMOOTH); */
    /* glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); */
}

/* x points rightward, out of the screen
 * y points leftward, out of the screen
 * z points upward, parallel to the screen
 */
void draw_cube(int x, int y, int z, float r, float g, float b)
{
    double dist = sqrt(1 / 3.0);
    glPushMatrix();
    glTranslated(z, y, x);
    glColor4d(r, g, b, 0.25);
    glutSolidCube(1);
    /* glutWireCube(1); */
    glPopMatrix();
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

    draw_cube(0, 0, 0, 0, 1, 0);
    draw_cube(0, 0, 1, 1, 0, 0);

    glDisable(GL_BLEND);

    glFlush();

    glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height) {
    ASPECT = (GLfloat)width / (GLfloat)height;

    /* render on entire window */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /* ensure unit axes are equal length on the screen */
    glOrtho(-10.0f * ASPECT, 10.0f * ASPECT, -10.0f, 10.0f, -10.0f, 10.0f);

    /* use this length so that camera is 1 unit away from origin */
    double dist = sqrt(1 / 3.0);

    /* set up an isometric projection matrix */
    gluLookAt(dist, dist, dist,  /* position of camera */
            0.0,  0.0,  0.0,   /* where camera is pointing at */
            1.0,  0.0,  0.0);  /* which direction is up */
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("memories");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    init();
    glutMainLoop();

    return 0;
}
