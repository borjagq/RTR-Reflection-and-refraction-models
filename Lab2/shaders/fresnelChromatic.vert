#version 330 core

// This shader has been adapted from the code found on https://www.scss.tcd.ie/Michael.Manzke/CS7055/GLSL/GLSL-3rdEd-refraction.pdf.

layout (location = 0) in vec3 inVertex;	// Vertex.
layout (location = 1) in vec3 inNormal;	// Normal (not necessarily normalized).
layout (location = 2) in vec3 inColor;	// Color (not necessarily normalized).
layout (location = 3) in vec2 inUV;		// UV coordinates.

uniform mat4 Model;			// Imports the model matrix.
uniform mat4 View;			// Imports the View matrix.
uniform mat4 Projection;	// Imports the projection matrix.
uniform mat4 modelView;		// Imports the modelView already multiplied.
uniform mat4 normalMatrix;	// Imports the normal matrix.
uniform vec3 lightPos;		// Light position.
uniform float time;			// Time in seconds.
uniform vec3 cameraPosition;        // Position of the camera.
uniform float etaR;                  // The ratio.
uniform float etaG;                  // The ratio.
uniform float etaB;                  // The ratio.
uniform float fresnelPower;

out vec3 vertexNormal;		// Passes the normal to the fragment shader.
out vec3 vertexColor;		// Passes the color to the fragment shader.
out vec2 vertexUV;			// Passes the UV coordinates to the fragment shader.
out vec3 vertexPosition;    // Passes the current vertex to the fragment shader.
out vec3 Reflect;
out vec3 RefractR;
out vec3 RefractG;
out vec3 RefractB;
out float Ratio;

void main() {
    
    // Assigns the direct passes.
    vertexNormal = vec3(normalMatrix * vec4(inNormal, 0.0));
    vertexColor = inColor;
    vertexUV = mat2(0.0, -1.0, 1.0, 0.0) * inUV;
    vertexPosition = vec3(modelView * vec4(inVertex, 1.0));
    
    // Get the F component of the fresnel.
    float F = ((1.0 - etaG) * (1.0 - etaG)) / ((1.0 + etaG) * (1.0 + etaG));

    // Get the transformed position and normalize it.
    vec4 ecPosition  = modelView * vec4(inVertex, 1.0);
    vec3 ecPosition3 = ecPosition.xyz / ecPosition.w;
    
    // Get the other components of the fresnel.
    vec3 i = normalize(ecPosition3);
    vec3 n = normalize(vertexNormal);

    // Compute the fresnet equation.
    Ratio   = F + (1.0 - F) * pow((1.0 - dot(-i, n)), fresnelPower);
    
    RefractR = refract(i, n, etaR);
    //RefractR = vec3(TextureMatrix * vec4(RefractR, 1.0));
    RefractG = refract(i, n, etaG);
    //RefractG = vec3(TextureMatrix * vec4(RefractG, 1.0));
    RefractB = refract(i, n, etaB);
    //RefractB = vec3(TextureMatrix * vec4(RefractB, 1.0));
    
    Reflect = reflect(i, n);
    //Reflect = vec3(TextureMatrix * vec4(Reflect, 1.0));
    
    // Sets the visualized position by applying the camera matrix.
    gl_Position = Projection * vec4(vertexPosition, 1.0);
    
}
