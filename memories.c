/* memories visualizes the contents of main memory.
 */

#include <stdio.h>
#include <GL/glut.h>

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("memories");
    /* glutDisplayFunc(display); */
    /* glutReshapeFunc(reshape); */
    init();
    glutMainLoop();
    return 0;
}
