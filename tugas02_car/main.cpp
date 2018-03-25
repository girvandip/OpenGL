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
        
        drawCircle( 250, 220, 0, 20, 360 );
        drawCircle( 360, 220, 0, 20, 360 );
        
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
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, allCircleVertices );
    glDrawArrays( GL_TRIANGLE_FAN, 0, numberOfVertices);
    glDisableClientState( GL_VERTEX_ARRAY );
}
