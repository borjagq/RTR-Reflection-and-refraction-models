#version 330 core

// This shader has been adapted from the code found on https://www.scss.tcd.ie/Michael.Manzke/CS7055/GLSL/GLSL-3rdEd-refraction.pdf.


in vec3 vertexPosition;             // Position from the VS.
in vec3 vertexNormal;	            // Normal from the VS.
in vec3 vertexColor;	            // Color from the VS.
in vec2 vertexUV;                   // UV coordinates from the VS.
in vec3 Reflect;
in vec3  Refract;
in float Ratio;

uniform vec4 lightColor;		    // Light color.
uniform vec3 lightPos;			    // Light position.
uniform vec3 cameraPosition;	    // Position of the camera.
uniform sampler2D baseColor;        // The color texture.
uniform samplerCube skybox;         // The skybox.

out vec4 outColor; // Outputs color in RGBA.

void main() {
    
    vec3 refractColor = vec3(texture(skybox, Refract));
    vec3 reflectColor = vec3(texture(skybox, Reflect));
    vec3 color = mix(refractColor, reflectColor, Ratio);
    
    outColor = vec4(color, 1.0);
    
}
