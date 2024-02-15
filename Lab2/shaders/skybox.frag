#version 330 core

in vec3 texCoords;

uniform samplerCube sky;

out vec4 outColor;

void main() {
    
    outColor = texture(sky, texCoords);
    
}
