#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include "cube.h"

typedef struct MatrixStackNode {
    struct MatrixStackNode *next;
    float matrix[16];
} MatrixStackNode;

typedef struct MatrixStack {
    MatrixStackNode *top;
} MatrixStack;

static void push(MatrixStack *stack);
static void pop(MatrixStack *stack);
static void makeIdentity(float *mat);
static void makePerspective(float *mat, float width, float height, float near, float far);
static void translate(float *mat, float x, float y, float z);
static void scale(float *mat, float amount);
static void rotate(float *mat, float x, float y, float z, float angle);
static void multiply(float *mat1, float *mat2);
static void makeQuaternion(float *q, float x, float y, float z, float angle);
static void qMultiply(float *q1, float *q2);
static void qMultiplyL(float *q1, float *q2);
static void qToMat(float *mat, float *q);
static void qNormalize(float *q);
static void qRotate(float *q, float x, float y, float z, float angle);
static char *readFile(const char *filename);
static void die(const char *fmt, ...);

int main(int argc, char **argv) {
    GLenum err;
    int major, minor;
    SDL_Event e;
    int running;
    GLuint vao, vbo, vs, fs, program;
    GLint uModel, uView, uProjection;
    char *source;
    float view[16], projection[16];
    MatrixStack model = {0};
    double dt;
    int now, then;
    int x, y;
    int dx, dy;
    float cameraQ[4];
    float cameraX[4], cameraY[4];
    float temp[16];

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        die("couldn't initialize SDL: %s", SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if(!SDL_SetVideoMode(800, 600, 32, SDL_OPENGL))
        die("couldn't set SDL video mode: %s", SDL_GetError());

    SDL_WM_SetCaption("3D Camera", "3D Camera");

    err = glewInit();
    if(err != GLEW_OK)
        die("couldn't initialize GLEW: %s", glewGetErrorString(err));

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if(!(major == 4 && minor == 3))
        die("OpenGL version must be 4.3 (found: %d.%d)", major, minor);

    glViewport(0, 0, 800, 600);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 6 * 4 * 3 * sizeof(float),
        cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    source = readFile("vs.glsl");
    if(!source)
        die("couldn't read vertex shader");

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, (const char **) &source, NULL);
    free(source);

    source = readFile("fs.glsl");
    if(!source)
        die("couldn't read fragment shader");

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, (const char **) &source, NULL);
    free(source);

    glCompileShader(vs);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    uModel = glGetUniformLocation(program, "model");
    uView = glGetUniformLocation(program, "view");
    uProjection = glGetUniformLocation(program, "projection");

    makePerspective(projection, 800.0f, 600.0f, 1.0f, 100.0f);
    makeIdentity(view);
    push(&model);
    makeIdentity(model.top->matrix);

    glUniformMatrix4fv(uProjection, 1, GL_FALSE, projection);

    running = 1;
    dt = 0.0;
    then = SDL_GetTicks();
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_WarpMouse(400, 300);
    SDL_ShowCursor(0);
    makeQuaternion(cameraQ, 0.0f, 0.0f, 0.0f, 0.0f);
    makeQuaternion(cameraX, 0.0f, 0.0f, 0.0f, 0.0f);
    makeQuaternion(cameraY, 0.0f, 0.0f, 0.0f, 0.0f);
    while(running) {
        while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = 0;
                            break;

                        default:
                            break;
                    }
            }
        }

        SDL_GetMouseState(&x, &y);
        dx = x - 400;
        dy = y - 300;
        if(!(dx == 0 && dy == 0)) {
            /*rotate(view, dy / sqrt(dx * dx + dy * dy), dx / sqrt(dx * dx + dy * dy), 0, sqrt(dx * dx + dy * dy) * 0.005f);*/
            /*qRotate(cameraQ, dy / sqrt(dx * dx + dy * dy), dx / sqrt(dx * dx + dy * dy), 0, sqrt(dx * dx + dy * dy) * 0.005f);*/
            /*qRotate(cameraQ, 1.0f, 0.0f, 0.0f, (dy / sqrt(dx * dx + dy * dy)) * 0.05f);*/
            /*qRotate(cameraQ, 0.0f, 1.0f, 0.0f, (dx / sqrt(dx * dx + dy * dy)) * 0.05f);*/
            qRotate(cameraX, 0.0f, 1.0f, 0.0f, dx / sqrt(dx * dx + dy * dy) * 0.05f);
            qRotate(cameraY, 1.0f, 0.0f, 0.0f, dy / sqrt(dx * dx + dy * dy) * 0.05f);
            /*qNormalize(cameraQ);*/
            /*qToMat(view, cameraQ);*/
            qNormalize(cameraX);
            qNormalize(cameraY);
            qToMat(view, cameraX);
            qToMat(temp, cameraY);
            multiply(view, temp);
            SDL_WarpMouse(400, 300);
        }

        now = SDL_GetTicks();
        dt += (now - then) / 1000.0;
        then = now;

        while(dt > 1 / 60.0) {
            rotate(model.top->matrix, 0.0f, 1.0f, 0.0f, 0.01f);
            dt -= 1 / 60.0;
        }

        push(&model);
        translate(model.top->matrix, 0.0f, 0.0f, -5.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(uModel, 1, GL_FALSE, model.top->matrix);
        glUniformMatrix4fv(uView, 1, GL_FALSE, view);

        pop(&model);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        SDL_GL_SwapBuffers();
    }

    pop(&model);

    glUseProgram(0);
    glDeleteProgram(program);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    SDL_Quit();
    return 0;
}

void push(MatrixStack *stack) {
    MatrixStackNode *new;

    new = malloc(sizeof(MatrixStackNode));
    if(!new)
        die("couldn't allocate new matrix stack node");

    if(stack->top)
        memcpy(new->matrix, stack->top->matrix, sizeof(float) * 16);

    new->next = stack->top;
    stack->top = new;
}

void pop(MatrixStack *stack) {
    MatrixStackNode *doomed;

    if(!stack->top)
        return;

    doomed = stack->top;
    stack->top = stack->top->next;
    free(doomed);
}

void makeIdentity(float *mat) {
    memset(mat, 0, sizeof(float) * 16);

    mat[0] = 1.0f;
    mat[5] = 1.0f;
    mat[10] = 1.0f;
    mat[15] = 1.0f;
}

void makePerspective(float *mat, float width, float height, float near, float far) {
    makeIdentity(mat);

    mat[0] = height / width;
    mat[10] = (far + near) / (near - far);
    mat[11] = -1.0f;
    mat[14] = 2 * far * near / (near - far);
}

void translate(float *mat, float x, float y, float z) {
    float translation[16];

    makeIdentity(translation);
    translation[12] = x;
    translation[13] = y;
    translation[14] = z;

    multiply(mat, translation);
}

void scale(float *mat, float amount) {
    float scaling[16];

    makeIdentity(scaling);
    scaling[0] = amount;
    scaling[5] = amount;
    scaling[10] = amount;

    multiply(mat, scaling);
}

void rotate(float *mat, float x, float y, float z, float amount) {
    float rotation[16];
    float s, c;

    makeIdentity(rotation);

    s = (float) sin(amount);
    c = (float) cos(amount);

    rotation[0] = x * x + (1 - x * x) * c;
    rotation[1] = (1 - c) * x * y + z * s;
    rotation[2] = (1 - c) * x * z - y * s;
    rotation[4] = (1 - c) * x * y - z * s;
    rotation[5] = y * y + (1 - y * y) * c;
    rotation[6] = (1 - c) * y * z + x * s;
    rotation[8] = (1 - c) * x * z + y * s;
    rotation[9] = (1 - c) * y * z - x * s;
    rotation[10] = z * z + (1 - z * z) * c;

    multiply(mat, rotation);
}

void multiply(float *mat1, float *mat2) {
    float result[16] = {0};
    int i, j, k;

    for(i = 0; i < 4; ++i)
        for(j = 0; j < 4; ++j)
            for(k = 0; k < 4; ++k)
                result[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];

    memcpy(mat1, result, sizeof(float) * 16);
}

void makeQuaternion(float *q, float x, float y, float z, float angle) {
    q[0] = x * sin(angle / 2);
    q[1] = y * sin(angle / 2);
    q[2] = z * sin(angle / 2);
    q[3] = cos(angle / 2);
}

void qMultiply(float *q1, float *q2) {
    float result[4];

    result[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
    result[1] = q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2];
    result[2] = q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0];
    result[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];

    memcpy(q1, result, sizeof(float) * 4);
}

void qMultiplyL(float *q1, float *q2) {
    float result[4];

    result[0] = q2[3] * q1[0] + q2[0] * q1[3] + q2[1] * q1[2] - q2[2] * q1[1];
    result[1] = q2[3] * q1[1] + q2[1] * q1[3] + q2[2] * q1[0] - q2[0] * q1[2];
    result[2] = q2[3] * q1[2] + q2[2] * q1[3] + q2[0] * q1[1] - q2[1] * q1[0];
    result[3] = q2[3] * q1[3] - q2[0] * q1[0] - q2[1] * q1[1] - q2[2] * q1[2];

    memcpy(q1, result, sizeof(float) * 4);
}

void qToMat(float *mat, float *q) {
    makeIdentity(mat);

    mat[0] = 1 - 2 * q[1] * q[1] - 2 * q[2] * q[2];
    mat[1] = 2 * q[0] * q[1] + 2 * q[3] * q[2];
    mat[2] = 2 * q[0] * q[2] - 2 * q[3] * q[1];

    mat[4] = 2 * q[0] * q[1] - 2 * q[3] * q[2];
    mat[5] = 1 - 2 * q[0] * q[0] - 2 * q[2] * q[2];
    mat[6] = 2 * q[1] * q[2] + 2 * q[3] * q[0];

    mat[8] = 2 * q[0] * q[2] + 2 * q[3] * q[1];
    mat[9] = 2 * q[1] * q[2] - 2 * q[3] * q[0];
    mat[10] = 1 - 2 * q[0] * q[0] - 2 * q[1] * q[1];
}

void qNormalize(float *q) {
    float d = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] /= d;
    q[1] /= d;
    q[2] /= d;
    q[3] /= d;
}

void qRotate(float *q, float x, float y, float z, float angle) {
    float rotation[4];
    makeQuaternion(rotation, x, y, z, angle);
    qMultiplyL(q, rotation);
}

char *readFile(const char *filename) {
    FILE *f;
    char *contents;
    long length;

    f = fopen(filename, "rb");
    if(!f)
        goto FAIL;

    if(fseek(f, 0, SEEK_END) < 0)
        goto CLOSE;

    length = ftell(f);
    if(length < 0)
        goto CLOSE;

    rewind(f);

    contents = malloc(sizeof(char) * (length + 1));
    if(!contents)
        goto FREE;

    if(fread(contents, sizeof(char), length, f) < length)
        goto FREE;

    contents[length] = 0;
    fclose(f);
    return contents;

FREE:
    free(contents);

CLOSE:
    fclose(f);

FAIL:
    return NULL;
}

void die(const char *fmt, ...) {
    va_list ap;

    fprintf(stderr, "error: ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}
