#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormals_in;
layout (location = 2) in vec2 uvCoords_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
uniform vec3 lightPosition;
uniform float t;
uniform float amplitude[8];
uniform float frequency[8];
uniform float phase[8];

// Specify the variables
float dU, dV, x, y, z;
vec3 currentVert;

// Specify the output of the vertex stage
out vec3 vertPosition;
out vec3 vertNormal;
out vec3 relativeLightPosition;
out vec2 uvCoords_out;

float waveHeight(int waveIDx, float uValue){

    float amp, freq, phas, height;

    amp = amplitude[waveIDx];
    freq = frequency[waveIDx];
    phas = phase[waveIDx];

    height =  amp * sin(2 * 3.141593 * (freq * vertCoordinates_in[0] + phas + t));

    return height;
}

float waveDU(int waveIDx, float uValue){
    float amp, freq, phas, newdU;

    amp = amplitude[waveIDx];
    freq = frequency[waveIDx];
    phas = phase[waveIDx];

    newdU =  2 * 3.141593 * freq * amp * cos(2 * 3.141593 * (freq * vertCoordinates_in[0] + phas + t));

    return newdU;
}

void main(){
    x = vertCoordinates_in[0];
    y = vertCoordinates_in[1];
    z = 0;

    dU = 0;
    dV = 0;

    for (int i = 0 ; i < 8 ; i++){
        z += waveHeight(i,x);
        dU += waveDU(i,x);
    }

    currentVert =  vec3(x, y, z);

    vertNormal            = normalTransform * normalize(vec3(-1 * dU, -1 * dV, 1.0));
    relativeLightPosition = vec3(modelViewTransform * vec4(lightPosition, 1));
    vertPosition          = vec3(modelViewTransform * vec4(currentVert,1));
    uvCoords_out          = uvCoords_in;

    gl_Position = projectionTransform * modelViewTransform * vec4(currentVert, 1.0);
}




