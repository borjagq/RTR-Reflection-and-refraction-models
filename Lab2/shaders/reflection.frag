#version 330 core

in vec3 vertexPosition;             // Position from the VS.
in vec3 vertexNormal;	            // Normal from the VS.
in vec3 vertexColor;	            // Color from the VS.
in vec2 vertexUV;		            // UV coordinates from the VS.

uniform vec4 lightColor;		    // Light color.
uniform vec3 lightPos;			    // Light position.
uniform vec3 cameraPosition;	    // Position of the camera.
uniform sampler2D baseColor;        // The color texture.
uniform samplerCube skybox;         // The skybox.

out vec4 outColor; // Outputs color in RGBA.

void main() {
    
    vec3 I = normalize(vertexPosition - cameraPosition);
    vec3 R = reflect(I, normalize(vertexNormal));
    
    outColor = vec4(texture(skybox, R).rgb, 1.0);

}
