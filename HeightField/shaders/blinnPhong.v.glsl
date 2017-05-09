/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: HeightFields
 *  File: main.cpp
 *
 *  Description:
 *      Blinn-Phong shader for drawing objects
 *
 *  Author:
 *      Joseph Wilson
 *		Jacob Davis
 *		Arnaud Filliat
 */

// we are using OpenGL 4.1 Core profile
#version 410 core

// group our light information
struct LightInfo {
    vec3 position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};

// ***** UNIFORMS *****
uniform LightInfo light;            // all light related info

uniform mat4 modelviewMatrix;
uniform mat4 MVP_Matrix;
uniform mat3 normalMatrix;

// ***** ATTRIBUTES *****
layout(location=0) in vec3 vPos;    // vertex position at location 0
layout(location=1) in vec3 vNorm;   // vertex normal at location 1

// ***** VERTEX SHADER OUTPUT *****
out vec3 lightVec;                  // vector from our vertex to the light source in eye space
out vec3 cameraVec;                 // vector from our vertex to the camera in eye space
out vec3 normalVec;                 // normal vector in eye space

void main() {
    // transform our vertex into clip space
    gl_Position = MVP_Matrix * vec4(vPos, 1);
    
    // compute the vertex position in eye space
    vec4 eyeCoords = modelviewMatrix * vec4(vPos, 1);
    
    // compute our light vector in eye space
    lightVec = normalize( light.position - eyeCoords.xyz );
    // compute our camera vector in eye space
    cameraVec = normalize( -eyeCoords.xyz );
    // transform our normal vector into eye space
    normalVec = normalize( normalMatrix * vNorm );
}
