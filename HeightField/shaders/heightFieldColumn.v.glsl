/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: HeightFields
 *  File: main.cpp
 *
 *  Description:
 *      Height Field simulation
 *		Pass through vertex shader for drawing height field columns
 *
 *  Author:
 *      Joseph Wilson
 *		Jacob Davis
 *		Arnaud Filliat
 */

#version 410

uniform mat4 MVP_Matrix;

layout(location=0) in vec3 vPos;

void main() {
    gl_Position = vec4(vPos, 1);
}
