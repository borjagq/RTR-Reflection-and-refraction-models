/**
 * @file cubemap.h
 * @brief Cubemap class implementation file.
 * @version 1.0.0 (2023-02-01)
 * @date 2023-02-01
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#include "cubemap.h"

#include <iostream>
#include <string>
#include <vector>

#include "GL/glew.h"
#include "stb/stb_image.h"

namespace bgq_opengl {

    Cubemap::Cubemap(GLuint id, std::string name, GLuint slot) {
        
        this->ID = id;
        this->slot = slot;
        this->name = name;
        
    }

    Cubemap::Cubemap(const std::vector<std::string> &textures_faces, const char* type, GLuint slot) {

        // Generate a texture in OpenGL and store the parameters in the attributes.
        glGenTextures(1, &this->ID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);
        
        this->name = std::string(name);
        this->slot = slot;
        
        int width, height, channels;
        
        // Loop through the images, load them and pass them to OpenGL.
        for (unsigned int i = 0; i < textures_faces.size(); i++) {
            
            unsigned char *data = stbi_load(textures_faces[i].c_str(), &width, &height, &channels, 0);
            if (data) {
                
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
                
            } else {
                
                std::cerr << "Cubemap error: cubemap " << textures_faces[i] << " could not be loaded." << std::endl;
                stbi_image_free(data);
                exit(1);
                
            }
            
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
    }

    GLuint Cubemap::getID() {
        
        return this->ID;
        
    }

    GLuint Cubemap::getSlot() {
        
        return this->slot;
        
    }

    std::string Cubemap::getName() {
        
        return this->name;
        
    }

    void Cubemap::bind() {
        
        // Activate the texture and bind it.
        glActiveTexture(GL_TEXTURE0 + this->slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);
        
    }

    void Cubemap::remove() {
        
        glDeleteTextures(1, &this->ID);
        
    }

    void Cubemap::unbind() {
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        
    }

}  // namespace bgq_opengl
