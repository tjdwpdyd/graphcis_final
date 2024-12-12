#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>

GLuint tableTexture, vaseTexture;
float cameraAngleX = 0.0f, cameraAngleY = 0.0f;
float zoom = -10.0f;  // 초기 카메라 거리 설정

// 카메라 위치 및 방향
float cameraPosX = 0.0f, cameraPosY = 0.0f, cameraPosZ = 10.0f;
float cameraLookAtX = 0.0f, cameraLookAtY = 0.0f, cameraLookAtZ = 0.0f;
float cameraUpX = 0.0f, cameraUpY = 1.0f, cameraUpZ = 0.0f;

// BMP 파일 로드 함수
GLuint loadBMP(const char* filename) {
    FILE* file;
    if (fopen_s(&file, filename, "rb") != 0) return 0;

    unsigned char header[54];
    fread(header, 1, 54, file);

    unsigned int dataPos = *(int*)&header[0x0A];
    unsigned int imageSize = *(int*)&header[0x22];
    unsigned int width = *(int*)&header[0x12];
    unsigned int height = *(int*)&header[0x16];

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char* data = new unsigned char[imageSize];
    fread(data, 1, imageSize, file);
    fclose(file);

    for (unsigned int i = 0; i < imageSize; i += 3) {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}

// 텍스처 초기화
void initTextures() {
    tableTexture = loadBMP("table");
    vaseTexture = loadBMP("glass");
}

// 원 그리기 함수
void drawCircle(float radius, float height) {
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.1415926f / 20.0f;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glTexCoord2f(0.5f + 0.5f * cos(angle), 0.5f + 0.5f * sin(angle));
        glVertex3f(x, height, z);
    }
    glEnd();
}

// 원통 그리기 함수
void drawCylinder(float radius, float height) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 20; i++) {
        float angle = i * 2.0f * 3.1415926f / 20.0f;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glTexCoord2f((float)i / 20, 1.0f);
        glVertex3f(x, height, z);
        glTexCoord2f((float)i / 20, 0.0f);
        glVertex3f(x, -height, z);
    }
    glEnd();
}

// 테이블 그리기
void drawTable() {
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, tableTexture);

    // 테이블 상판
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);
    glScalef(5.0f, 0.2f, 5.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // 테이블 다리
    glPushMatrix();
    glTranslatef(-2.5f, 0.75f, -2.5f);
    glScalef(0.1f, 1.5f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 0.75f, -2.5f);
    glScalef(0.1f, 1.5f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.5f, 0.75f, 2.5f);
    glScalef(0.1f, 1.5f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5f, 0.75f, 2.5f);
    glScalef(0.1f, 1.5f, 0.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

// 꽃병 그리기
void drawVase() {
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, vaseTexture);

    glTranslatef(0.0f, 2.2f, 0.0f);
    drawCircle(0.3f, 0.0f);
    drawCylinder(0.3f, 1.0f);

    glTranslatef(0.0f, -1.0f, 0.0f);
    drawCircle(0.3f, 0.0f);

    glPopMatrix();
}

// 디스플레이 콜백
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // 카메라 위치 설정 (시점 변환)
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ + zoom, cameraLookAtX, cameraLookAtY, cameraLookAtZ, cameraUpX, cameraUpY, cameraUpZ);

    // 모델 변환과 물체 그리기
    glRotatef(cameraAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraAngleY, 0.0f, 1.0f, 0.0f);

    drawTable();
    drawVase();

    glPopMatrix();

    glutSwapBuffers();
}

// 키보드 입력 처리
void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') {  // 카메라 줌인 (앞으로 이동)
        zoom += 0.5f;
    }
    else if (key == 's') {  // 카메라 줌아웃 (뒤로 이동)
        zoom -= 0.5f;
    }
    else if (key == 'a') {  // 좌우 회전
        cameraAngleY -= 0.05f;
    }
    else if (key == 'd') {
        cameraAngleY += 0.05f;
    }
    else if (key == 'q') {  // 위/아래 회전
        cameraAngleX -= 0.05f;
    }
    else if (key == 'e') {
        cameraAngleX += 0.05f;
    }

    glutPostRedisplay();
}

// 마우스 움직임에 의한 카메라 회전
void motion(int x, int y) {
    cameraAngleY = (x - 400) * 0.1f;  // x 좌표에 따라 Y축 회전
    cameraAngleX = (y - 300) * 0.1f;  // y 좌표에 따라 X축 회전

    glutPostRedisplay();
}

// 초기화
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    initTextures();

    // 투상 변환 (원근 투영)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);  // 원근 투영 설정

    glMatrixMode(GL_MODELVIEW);  // 모델뷰 행렬로 돌아가기
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Table and Vase");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
