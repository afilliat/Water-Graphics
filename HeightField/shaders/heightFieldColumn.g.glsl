/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: HeightFields
 *  File: main.cpp
 *
 *  Description:
 *      Height Field simulation
 *		Geometry shader draws columns for heightField and calculates lighting vectors
 *
 *  Author:
 *      Joseph Wilson
 *		Jacob Davis
 *		Arnaud Filliat
 */

#version 410

layout(points) in;

layout(triangle_strip, max_vertices=20) out;

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

//output
out vec3 lightVec;               // interpolated light vector in eye space
out vec3 cameraVec;              // interpolated camera vector in eye space
out vec3 normalVec;              // interpolated normal vector in eye space


void main() {
	vec4 originPosition = gl_in[0].gl_Position;
	vec4 bottomPosition = originPosition;
	bottomPosition.y = 0;
	
	//Simple Shifts for boxes
	vec4 shiftX = vec4(0.9, 0, 0, 0);
	vec4 shiftZ = vec4(0, 0, 0.9, 0);
	
	//Emits each side for box with shifts defined above applied to points and boxes starting at y=0
	//Top
	vec4 position = originPosition;
	gl_Position = MVP_Matrix * position;
	vec4 eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 1, 0));
	EmitVertex();
	
	position = originPosition + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 1, 0));
	EmitVertex();
	
	position =  originPosition + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 1, 0));
	EmitVertex();
		
	position = originPosition + shiftX + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 1, 0));
	EmitVertex();
	
	EndPrimitive();
	
	//Back
	position = originPosition;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, -1));
	EmitVertex();
	
	position = originPosition + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, -1));
	EmitVertex();
	
	position =  bottomPosition;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, -1));
	EmitVertex();
		
	position = bottomPosition + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, -1));
	EmitVertex();
	
	EndPrimitive();
	
	//Right
	position = originPosition;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(-1, 0, 0));
	EmitVertex();
	
	position = bottomPosition;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(-1, 0, 0));
	EmitVertex();
	
	position =  originPosition + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(-1, 0, 0));
	EmitVertex();
		
	position = bottomPosition + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(-1, 0, 0));
	EmitVertex();
	
	EndPrimitive();
	
	//Left
	position = originPosition + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(1, 0, 0));
	EmitVertex();
	
	position = originPosition + shiftZ + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(1, 0, 0));
	EmitVertex();
	
	position =  bottomPosition + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(1, 0, 0));
	EmitVertex();
		
	position = bottomPosition + shiftZ + shiftX;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(1, 0, 0));
	EmitVertex();
	
	EndPrimitive();
	
	//Front
	position = originPosition + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, 1));
	EmitVertex();
	
	position = bottomPosition + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, 1));
	EmitVertex();
	
	position =  originPosition + shiftX + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, 1));
	EmitVertex();
		
	position = bottomPosition + shiftX + shiftZ;
	gl_Position = MVP_Matrix * position;
	eyeCoords = modelviewMatrix * position;
	lightVec = normalize( light.position - eyeCoords.xyz );
	cameraVec = normalize( -eyeCoords.xyz );
	normalVec = normalize( normalMatrix * vec3(0, 0, 1));
	EmitVertex();
	
	EndPrimitive();
	
}