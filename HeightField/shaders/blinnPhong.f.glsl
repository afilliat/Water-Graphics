/*
 *  CSCI 444, Advanced Computer Graphics, Spring 2017
 *
 *  Project: jpaone_a1
 *  File: blinnPhong.f.glsl
 *
 *  Description:
 *      A1 Solution
 *
 *      Blinn-Phong Fragment Shader
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
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

// group our material information
struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};

// ***** UNIFORMS *****
uniform LightInfo light;        // all light related info
uniform MaterialInfo material;  // all material related info

// ***** FRAGMENT SHADER INPUT *****
in vec3 lightVec;               // interpolated light vector in eye space
in vec3 cameraVec;              // interpolated camera vector in eye space
in vec3 normalVec;              // interpolated normal vector in eye space

// ***** FRAGMENT SHADER OUTPUT *****
layout(location=0) out vec4 fragColorOut;

void main() {
    // ambient component of Phong Shading
    vec3 ambient = light.La * material.Ka;
    
    // to allow our object to be front and back sided, if back facing flip our normal
    vec3 normal = normalize(normalVec);
    if( !gl_FrontFacing )
        normal = -normal;
    
    // compute the halfway vector - BlinnPhong Shading!
    vec3 halfwayVec = normalize( lightVec + cameraVec );
    
    // diffuse component of Phong Shading
    float diffuseTerm = max( dot(lightVec, normal), 0.0 );
    vec3 diffuse = light.Ld * material.Kd * diffuseTerm;
    
    // specular component of Phong Shading
    // for Blinn-Phong Shading we apply 4*alpha for shininess
    vec3 spec = light.Ls * material.Ks * pow( max( dot(normal, halfwayVec), 0.0 ), 4*material.shininess );
    
    // combine all three components of Phong Shading
    fragColorOut = vec4(ambient + diffuse + spec,1);
    
    // if back facing, reverse our colors as a visual cue
    //if( !gl_FrontFacing )
        //fragColorOut.rgb = fragColorOut.bgr;
}
