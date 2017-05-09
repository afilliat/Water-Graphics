#version 410 core

layout(std140) uniform LightInfo {
    vec3 position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
} light;

layout(std140) uniform MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};

in Vectors {
    vec3 light;
    vec3 camera;
    vec3 normal;
    vec3 altitudes;
    vec4 patchDistance;
} vectorsIn;

out vec4 fragColorOut;

uniform int useOutline;

float amplify(float d, float scale, float offset) {
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main() {
    vec3 ambient = light.La * Ka;
    
    vec3 normal = vectorsIn.normal;
    if( !gl_FrontFacing )
        normal = -normal;
    
    vec3 halfwayVec = normalize( vectorsIn.light + vectorsIn.camera );
    
    float sDotN = max( dot(vectorsIn.light, normal), 0.0 );
    vec3 diffuse = light.Ld * Kd * sDotN;
    
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = light.Ls * Ks * pow( max( dot(normal, halfwayVec), 0.0 ), 4*shininess );
    
    // distance from triangle edge
    float d1 = min( min( vectorsIn.altitudes.x, vectorsIn.altitudes.y ), vectorsIn.altitudes.z );
    // distance from patch edge
    float d2 = min( min( min( vectorsIn.patchDistance.x, vectorsIn.patchDistance.y ), vectorsIn.patchDistance.z ), vectorsIn.patchDistance.w );
    
    vec4 phongColor = vec4(ambient+diffuse+spec, 1);
    
    d1 = 1 - amplify(d1, 250, -0.5);
    d2 = amplify(d2, 100, -0.5);
    fragColorOut = d2 * phongColor + d1 * d2 * vec4(1,1,1,1);
    fragColorOut.a = 1;
    
    if( useOutline == 0 )
        fragColorOut = phongColor;
    
    if( !gl_FrontFacing )
        fragColorOut.rgb = fragColorOut.bgr;
    
}
