//
//  main.cpp
//  OpenGLTutorial
//
//  Created by Alivia Dewi Parahita on 3/20/18.
//  Copyright © 2018 Alivia Dewi Parahita. All rights reserved.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define pi 3.14159

void drawCircle( GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides );
void rotateVertices(float* vertices, int numberOfVertices, float centerX, float centerY, float angle);
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius);

int main( void )
{
    GLFWwindow *window;
    
    // Initialize the library
    if ( !glfwInit( ) )
    {
        return -1;
    }
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL );
    
    if ( !window )
    {
        glfwTerminate( );
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent( window );
    
    float kotak_atas[] =
    {
        250, 280, 0.0, // top left corner
        350, 280, 0.0, // top right corner
        350, 250, 0.0, // bottom right corner
        250, 250, 0.0 // bottom left corner
    };
    
    float kotak_bawah[] =
    {
        420, 250, 0.0, // top right corner
        200, 250, 0.0, // top left corner
        200, 220, 0.0, // bottom left corner
        420, 220, 0.0 // bottom right corner
    };

    float roda_kiri[] =
    {
        240, 240, 0.0, // top left corner
        260, 240, 0.0, // top right corner
        260, 220, 0.0, // bottom right corner
        240, 220, 0.0 // bottom left corner
    };
    
    float roda_kanan[] =
    {
        350, 240, 0.0, // top right corner
        370, 240, 0.0, // top left corner
        370, 220, 0.0, // bottom left corner
        350, 220, 0.0 // bottom right corner
    };
    
    GLfloat color_kotak[] =
    {
        255,0,0,
        255,0,0,
        0,0,0,
        0,0,0
    };
    
    float segitiga_kanan [] =
    {
        350, 280, 0.0,
        350, 250, 0.0,
        385, 250, 0.0
    };
    
    float segitiga_kiri [] =
    {
        250, 280, 0.0,
        250, 250, 0.0,
        215, 250, 0.0
        
    };
    
    GLfloat color_segitiga[] =
    {
        255,0,0,
        0,0,0,
        0,0,0
    };

    GLfloat color_roda[] =
    {
        0,255,255,
        0,0,255,
        0,0,255,
        0,0,0
    };

    float x = 250;
    float y = 220;
    float z = 0;
    float radius = 20;
    //Circle
    int numberOfSides = 20;
    int numberOfVertices = numberOfSides + 2;
    
    float twicePi = 2.0f * pi;
    
    float circleVerticesX[numberOfVertices];
    float circleVerticesY[numberOfVertices];
    float circleVerticesZ[numberOfVertices];
    
    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = z;
    
    for ( int i = 1; i < numberOfVertices; i++ )
    {
        circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
        circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
        circleVerticesZ[i] = z;
    }
    
    GLfloat allCircleVertices[numberOfVertices * 3];
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
        allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
    }


    
    glViewport( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity( ); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1 ); // essentially set coordinate system
    glMatrixMode( GL_MODELVIEW ); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity( ); // same as above comment
    
    //loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        // Render OpenGL here
        //rotateVertices(kotak_atas, 4, 300, 265, 1);
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, kotak_atas ); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_kotak);
        glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
        
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, kotak_bawah ); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_kotak);
        glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
        
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, segitiga_kanan ); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_segitiga);
        glDrawArrays( GL_TRIANGLES, 0, 3 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
        
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, segitiga_kiri ); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_segitiga);
        glDrawArrays( GL_TRIANGLES, 0, 3 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
        
        rotateVertices(roda_kiri, 4, 250, 230, -0.01);
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, roda_kiri); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_roda);
        glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
        
        rotateVertices(roda_kanan, 4, 360, 230, -0.01);
        glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer( 3, GL_FLOAT, 0, roda_kanan); // point to the vertices to be used
        glColorPointer(3, GL_FLOAT, 0, color_roda);
        glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute

        //Roda
        // // drawCircle( 250, 220, 0, 20, 360 );
        // // drawCircle( 360, 220, 0, 20, 360 );
        // // drawFilledCircle(250, 220, 20);
        // // drawFilledCircle(360, 220, 20);
        // rotateVertices(allCircleVertices, numberOfVertices, 250, 220, 1);
        // glEnableClientState( GL_VERTEX_ARRAY );
        // glVertexPointer( 3, GL_FLOAT, 0, allCircleVertices );
        // glColorPointer(3, GL_FLOAT, 0, color_segitiga);
        // glDrawArrays( GL_TRIANGLE_FAN, 0, numberOfVertices);
        // glDisableClientState( GL_VERTEX_ARRAY );

        //swap front and back buffers
        glfwSwapBuffers(window);
        
        //pool for and process
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
    
}

void drawCircle( GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides )
{
    GLint numberOfVertices = numberOfSides + 2;
    
    GLfloat twicePi = 2.0f * pi;
    
    GLfloat circleVerticesX[362];
    GLfloat circleVerticesY[362];
    GLfloat circleVerticesZ[362];
    
    circleVerticesX[0] = x;
    circleVerticesY[0] = y;
    circleVerticesZ[0] = z;

    GLfloat color_segitiga[] =
    {
        255,0,0,
        255,0,0,
        255,0,0
    };
    
    for ( int i = 1; i < numberOfVertices; i++ )
    {
        circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
        circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
        circleVerticesZ[i] = z;
    }
    
    GLfloat allCircleVertices[362 * 3];
    
    for ( int i = 0; i < numberOfVertices; i++ )
    {
        allCircleVertices[i * 3] = circleVerticesX[i];
        allCircleVertices[( i * 3 ) + 1] = circleVerticesY[i];
        allCircleVertices[( i * 3 ) + 2] = circleVerticesZ[i];
    }
    //rotateVertices(kotak_atas, 4, 300, 265, 0.01);

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, allCircleVertices );
    glColorPointer(3, GL_FLOAT, 0, color_segitiga);
    glDrawArrays( GL_TRIANGLE_FAN, 0, numberOfVertices);
    glDisableClientState( GL_VERTEX_ARRAY );
}

void rotateVertices(float* vertices, int numberOfVertices, float centerX, float centerY, float angle) {
 float angleSin = sin(angle);
 float angleCos = cos(angle);

 for (int i = 0; i < numberOfVertices*3; i += 3) {
  vertices[i] -= centerX;
  vertices[i+1] -= centerY;
  // rotate point
  float xnew = vertices[i] * angleCos - vertices[i+1] * angleSin;
  float ynew = vertices[i] * angleSin + vertices[i+1] * angleCos;

  vertices[i] = xnew + centerX;
  vertices[i+1] = ynew + centerY;
 }

}


void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle
    
    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * pi;
    
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount;i++) { 
            glVertex2f(
                    x + (radius * cos(i *  twicePi / triangleAmount)), 
                    y + (radius * sin(i * twicePi / triangleAmount))
            );
            if(i%6 == 0)
                glColor3f(1.0f, 0.0f, 0.0f);

            if (i%6 == 1)
                glColor3f(1.0f, 0.5f, 0.0f);

            if (i%6 == 2)
                glColor3f(1.0f, 1.0f, 0.0f);

            if (i%6 == 3)
                glColor3f(0.0f, 1.0f, 0.0f);

            if (i%6 == 4)
                glColor3f(0.0f, 0.0f, 1.0f);

            if (i%6 == 5)
                glColor3f(1.0f, 0.0f, 1.0f);
        }
    glEnd();
}