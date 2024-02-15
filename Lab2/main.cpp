/**
 * @file main.cpp
 * @brief Main aladdin 3d file.
 * @version 1.0.0 (2022-10-21)
 * @date 2022-10-21
 * @author Borja García Quiroga <garcaqub@tcd.ie>
 *
 *
 * Copyright (c) Borja García Quiroga, All Rights Reserved.
 *
 * The information and material provided below was developed as partial
 * requirements for the MSc in Computer Science at Trinity College Dublin,
 * Ireland.
 */

#define GLM_ENABLE_EXPERIMENTAL

#include "main.h"

#include <math.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/common.hpp"
#include "glm/gtx/string_cast.hpp"

#include "classes/camera/camera.h"
#include "classes/cubemap/cubemap.h"
#include "classes/light/light.h"
#include "classes/object/object.h"
#include "classes/shader/shader.h"
#include "classes/skybox/skybox.h"
#include "structs/bounding_box/bounding_box.h"

void clean() {

    // Delete all the shaders.
    for (size_t i = 0; i < shaders.size(); i++)
        shaders[i].remove();
    
    // Terminate ImGUI.
    ImGui_ImplGlfwGL3_Shutdown();
    
    // Close GL context and any other GLFW resources.
    glfwTerminate();

}

void clear() {
    
    // Specify the color of the background
    glClearColor(background.x, background.y, background.z, background.w);

    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void displayElements() {
    
    // Get the program internal time.
    auto current_time = std::chrono::system_clock::now();
    double real_time = std::chrono::duration<double>(current_time.time_since_epoch()).count();
    internal_time = real_time - time_start;
    
    for (int i = 0; i < shaders.size(); i++) {
        
        // Pass the parameters to the shaders.
        shaders[i].activate();
        shaders[i].passLight(scene_light);
        shaders[i].passCubemap(skyboxes[current_skybox].getCubemap());
        
        // Pass some parameters to the shaders.
        shaders[i].passFloat("eta", refraction_eta);
        shaders[i].passFloat("fresnelPower", fresnel_power);
        
        shaders[i].passFloat("etaR", refraction_eta_r);
        shaders[i].passFloat("etaG", refraction_eta_g);
        shaders[i].passFloat("etaB", refraction_eta_b);
        
    }
    
    // Updates the camera matrixes.
    cameras[current_camera].update();
    
    if (current_scene == 0) {
        
        // Get info from the model.
        bgq_opengl::BoundingBox bb = objects[current_object].getBoundingBox();
        glm::vec3 centre = (bb.min + bb.max) / 2.0f;
        glm::vec3 size = bb.max - bb.min;
        float max_dim = std::max(size.x, std::max(size.y, size.z));
        float scale_rat = NORM_SIZE / max_dim;
        
        for (int i = 0; i < 4; i++) {
            
            objects[current_object].resetTransforms();
            
            // Rotate
            float rotating = fmod(internal_time * 10, 360);
            objects[current_object].rotate(0.0, 1.0, 0.0, rotating);
            
            // Rotate
            rotating = -90 * i;
            objects[current_object].rotate(0.0, 1.0, 0.0, rotating);
            
            objects[current_object].translate(0.0, 0.0, 1.0);
            
            // Resize it to normalize it.
            objects[current_object].scale(scale_rat, scale_rat, scale_rat);
            
            // Center the object and get it in the right position.
            objects[current_object].translate(-centre.x, -centre.y, -centre.z);
            
            // Draw the object.
            objects[current_object].draw(shaders[i % shaders.size()], cameras[current_camera]);
            
        }
        
    } else if (current_scene == 1) {
        
        // Get info from the model.
        bgq_opengl::BoundingBox bb = (*tray).getBoundingBox();
        glm::vec3 centre = (bb.min + bb.max) / 2.0f;
        glm::vec3 size = bb.max - bb.min;
        float max_dim = std::max(size.x, std::max(size.y, size.z));
        float scale_rat = NORM_SIZE / max_dim;
        
        (*tray).resetTransforms();

        // Rotate
        float rotating = fmod(internal_time * 10, 360);
        (*tray).rotate(0.0, 1.0, 0.0, rotating);
        
        // Resize it to normalize it.
        (*tray).scale(scale_rat, scale_rat, scale_rat);
        
        // Center the object and get it in the right position.
        (*tray).translate(-centre.x, -centre.y, -centre.z);
        
        // Draw the object.
        (*fancy_shader).activate();
        (*fancy_shader).passFloat("mixColor", tray_interpol_color);
        (*fancy_shader).passVec("objectColor", tray_color);
        (*fancy_shader).passFloat("fresnelPower", tray_fresnel_power);
        (*fancy_shader).passFloat("etaR", tray_eta_r);
        (*fancy_shader).passFloat("etaG", tray_eta_g);
        (*fancy_shader).passFloat("etaB", tray_eta_b);
        (*tray).draw(*fancy_shader, cameras[current_camera]);
        
        // Get info from the model.
        bb = (*bottle).getBoundingBox();
        centre = (bb.min + bb.max) / 2.0f;
        size = bb.max - bb.min;
        max_dim = std::max(size.x, std::max(size.y, size.z));
        scale_rat = NORM_SIZE / max_dim;
        
        (*bottle).resetTransforms();

        // Rotate
        rotating = fmod(internal_time * 10, 360);
        (*bottle).rotate(0.0, 1.0, 0.0, rotating);
        
        (*bottle).translate(0.2, 0.0, 0.2);
        
        // Resize it to normalize it.
        (*bottle).scale(scale_rat / 1.2, scale_rat / 1.2, scale_rat / 1.2);
        
        // Center the object and get it in the right position.
        (*bottle).translate(-centre.x, 0.0, -centre.z);
        
        // Draw the object.
        (*fancy_shader).activate();
        (*fancy_shader).passFloat("mixColor", bottle_interpol_color);
        (*fancy_shader).passVec("objectColor", bottle_color);
        (*fancy_shader).passFloat("fresnelPower", bottle_fresnel_power);
        (*fancy_shader).passFloat("etaR", bottle_eta_r);
        (*fancy_shader).passFloat("etaG", bottle_eta_g);
        (*fancy_shader).passFloat("etaB", bottle_eta_b);
        (*bottle).draw(*fancy_shader, cameras[current_camera]);
        
        // Get info from the model.
        bb = (*ico).getBoundingBox();
        centre = (bb.min + bb.max) / 2.0f;
        size = bb.max - bb.min;
        max_dim = std::max(size.x, std::max(size.y, size.z));
        scale_rat = NORM_SIZE / max_dim;
        
        (*ico).resetTransforms();

        // Rotate
        rotating = fmod(internal_time * 10, 360);
        (*ico).rotate(0.0, 1.0, 0.0, rotating);
        
        (*ico).translate(-0.15, 0.0, 0.2);
        
        // Resize it to normalize it.
        (*ico).scale(scale_rat / 3.0, scale_rat / 3.0, scale_rat / 3.0);
        
        // Center the object and get it in the right position.
        (*ico).translate(-centre.x, 0.0, -centre.z);
        
        // Draw the object.
        (*fancy_shader).activate();
        (*fancy_shader).passFloat("mixColor", ico_interpol_color);
        (*fancy_shader).passVec("objectColor", ico_color);
        (*fancy_shader).passFloat("fresnelPower", ico_fresnel_power);
        (*fancy_shader).passFloat("etaR", ico_eta_r);
        (*fancy_shader).passFloat("etaG", ico_eta_g);
        (*fancy_shader).passFloat("etaB", ico_eta_b);
        (*ico).draw(*fancy_shader, cameras[current_camera]);
        
        // Get info from the model.
        bb = (*mirror).getBoundingBox();
        centre = (bb.min + bb.max) / 2.0f;
        size = bb.max - bb.min;
        max_dim = std::max(size.x, std::max(size.y, size.z));
        scale_rat = NORM_SIZE / max_dim;
        
        (*mirror).resetTransforms();

        // Rotate
        rotating = fmod(internal_time * 10, 360);
        (*mirror).rotate(0.0, 1.0, 0.0, rotating);
        
        (*mirror).translate(0.0, 0.0, -0.15);
        
        // Rotate it because i want it in the other direction.
        (*mirror).rotate(0.0, 1.0, 0.0, -90.0);
        
        // Resize it to normalize it.
        (*mirror).scale(scale_rat / 1.5, scale_rat / 1.5, scale_rat / 1.5);
        
        // Center the object and get it in the right position.
        (*mirror).translate(-centre.x, 0.0, -centre.z);
        
        // Draw the object.
        (*fancy_shader).activate();
        (*fancy_shader).passFloat("mixColor", mirror_interpol_color);
        (*fancy_shader).passVec("objectColor", mirror_color);
        (*fancy_shader).passFloat("fresnelPower", mirror_fresnel_power);
        (*fancy_shader).passFloat("etaR", mirror_eta_r);
        (*fancy_shader).passFloat("etaG", mirror_eta_g);
        (*fancy_shader).passFloat("etaB", mirror_eta_b);
        (*mirror).getGeometries()[0].draw(*fancy_shader, cameras[current_camera]);
        
        // Draw the object.
        (*fancy_shader).passFloat("mixColor", 1.0);
        (*mirror).getGeometries()[1].draw(*fancy_shader, cameras[current_camera]);
        
    }
    
    // Print the skybox.
    skyboxes[current_skybox].draw(*sky_shader, cameras[current_camera]);
        
}

void displayGUI() {
    
    // Init ImGUI for rendering.
    ImGui_ImplGlfwGL3_NewFrame();
    
    // Begin the new widget.
    ImGui::Begin("Parameters");
    
    ImGui::Text("Scene");
    ImGui::RadioButton("Basic", &current_scene, 0);
    ImGui::RadioButton("Fancy", &current_scene, 1);
    
    // Pass these values to the shaders.
    ImGui::Text("Model");
    ImGui::RadioButton("Teapot", &current_object, 0);
    ImGui::RadioButton("Chair", &current_object, 1);
    ImGui::RadioButton("Van", &current_object, 2);

    ImGui::Text("Skybox");
    ImGui::RadioButton("Sky", &current_skybox, 0);
    ImGui::RadioButton("Hotel room", &current_skybox, 1);
    ImGui::RadioButton("Chapel", &current_skybox, 2);

    ImGui::Text("Parameters");
    ImGui::SliderFloat("Ratio", &refraction_eta, 0.1, 1.0);
    ImGui::SliderFloat("FresnelPower", &fresnel_power, 0.1, 10.0);
    ImGui::SliderFloat("ETA R", &refraction_eta_r, 0.1, 1.0);
    ImGui::SliderFloat("ETA G", &refraction_eta_g, 0.1, 1.0);
    ImGui::SliderFloat("ETA B", &refraction_eta_b, 0.1, 1.0);
    
    ImGui::End();
    
    // Render ImGUI.
    ImGui::Render();
    
}

void handleKeyEvents() {
    
    // Key W will move camera 0 forward.
    if (ImGui::IsKeyPressed('W')) {

        // Move forward.
        cameras[current_camera].moveFront();
        
    }

    // Key S will move camera 0 backwards.
    if (ImGui::IsKeyPressed('S')) {

        // Move backwards.
        cameras[current_camera].moveBack();

    }

    // Key D will move camera 0 to the right.
    if (ImGui::IsKeyPressed('D')) {

        // Move right.
        cameras[current_camera].moveRight();

    }

    // Key A will move camera 0 to the right.
    if (ImGui::IsKeyPressed('A')) {

        // Move to the left.
        cameras[current_camera].moveLeft();

    }

    // Key R will move camera up.
    if (ImGui::IsKeyPressed('R')) {

        // Move up.
        cameras[current_camera].moveUp();

    }

    // Key F will move camera down.
    if (ImGui::IsKeyPressed('F')) {

        // Move up.
        cameras[current_camera].moveDown();

    }

    // Key E will rotate camera left.
    if (ImGui::IsKeyPressed('E')) {

        // Move up.
        cameras[current_camera].rotateLeft();

    }

    // Key Q will rotate the camera right.
    if (ImGui::IsKeyPressed('Q')) {

        // Move up.
        cameras[current_camera].rotateRight();

    }

    // Key T will bring the camera up.
    if (ImGui::IsKeyPressed('T')) {

        // Move up.
        cameras[current_camera].rotateUp();

    }

    // Key G will bring the camera down.
    if (ImGui::IsKeyPressed('G')) {

        // Move up.
        cameras[current_camera].rotateDown();

    }
    
}

void initElements() {

    // Create a white light in the center of the world.
    scene_light = bgq_opengl::Light(glm::vec3(2.0f, 8.0, 2.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Get the shaders.
    shaders.push_back(bgq_opengl::Shader("reflection.vert", "reflection.frag"));
    shaders.push_back(bgq_opengl::Shader("refraction.vert", "refraction.frag"));
    shaders.push_back(bgq_opengl::Shader("fresnel.vert", "fresnel.frag"));
    shaders.push_back(bgq_opengl::Shader("fresnelChromatic.vert", "fresnelChromatic.frag"));

    // Creates the first camera object
    bgq_opengl::Camera camera(glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(-1.0f, -0.5f, -1.0f), 45.0f, 0.1f, 300.0f, WINDOW_WIDTH, WINDOW_HEIGHT);
    cameras.push_back(camera);
    
    // Load the skyboxes.
    std::vector<std::string> faces {
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    bgq_opengl::Cubemap cubemap(faces, "sky", 0);
    bgq_opengl::Skybox skybox_1(cubemap);
    skyboxes.push_back(skybox_1);
    
    // Load the skyboxes.
    faces = std::vector<std::string> {
        "dallas_posx.jpg",
        "dallas_negx.jpg",
        "dallas_posy.jpg",
        "dallas_negy.jpg",
        "dallas_posz.jpg",
        "dallas_negz.jpg"
    };
    cubemap = bgq_opengl::Cubemap(faces, "dallas", 0);
    bgq_opengl::Skybox dallas(cubemap);
    skyboxes.push_back(dallas);
    
    // Load the skyboxes.
    faces = std::vector<std::string> {
        "posx.jpg",
        "negx.jpg",
        "posy.jpg",
        "negy.jpg",
        "posz.jpg",
        "negz.jpg"
    };
    cubemap = bgq_opengl::Cubemap(faces, "chapel", 0);
    bgq_opengl::Skybox chapel(cubemap);
    skyboxes.push_back(chapel);
    
    // Init the shader.
    sky_shader = new bgq_opengl::Shader("skybox.vert", "skybox.frag");
    
    // Init the fancy shader.
    fancy_shader = new bgq_opengl::Shader("fancyFresnelChromatic.vert", "fancyFresnelChromatic.frag");

    // Load the objects.
    objects.push_back(bgq_opengl::Object("teapot.obj", "Assimp"));
    objects.push_back(bgq_opengl::Object("eames_chair.fbx", "Assimp"));
    objects.push_back(bgq_opengl::Object("volkswagen.fbx", "Assimp"));
    
    // Load the objects of the fancy scene.
    tray = new bgq_opengl::Object("tray.obj", "Assimp");
    bottle = new bgq_opengl::Object("bottle.obj", "Assimp");
    ico = new bgq_opengl::Object("icosahedron.obj", "Assimp");
    mirror = new bgq_opengl::Object("mirror.obj", "Assimp");

}

void initEnvironment(int argc, char** argv) {
    
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        
        std::cerr << "ERROR: could not start GLFW3" << std::endl;
        exit(1);

    }
    
    // MacOS initializations.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create the window.
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, GAME_NAME, NULL, NULL);
    if (!window) {
        
        std::cerr << "Error 121-1001 - Failed to create the window." << std::endl;
        glfwTerminate();
        exit(1);

    }
    glfwMakeContextCurrent(window);
    
    // Tell what function handles the events.
    // glfwSetKeyCallback(window, handleKeyEvents);
    
    // Initialize GLEW and OpenGL.
    GLenum res = glewInit();

    // Check for any errors.
    if (res != GLEW_OK) {

        std::cerr << "Error 121-1002 - GLEW could not be initialized:" << glewGetErrorString(res) << std::endl;
        exit(1);

    }
    
    // Get version info.
    std::cerr << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "OpenGL version supported " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "GLSL version supported " << (char *) glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    
    // Init the initial time.
    auto current_time = std::chrono::system_clock::now();
    time_start = std::chrono::duration<double>(current_time.time_since_epoch()).count();

}

int main(int argc, char** argv) {

    // Initialise the environment.
    initEnvironment(argc, argv);
    
    // Initialise the objects and elements.
    initElements();

    // Main loop.
    while(!glfwWindowShouldClose(window)) {
        
        // Clear the scene.
        clear();
        
        // Handle key events.
        handleKeyEvents();
        
        // Display the scene.
        displayElements();
        
        // Make the things to print everything.
        displayGUI();
        
        // Update other events like input handling.
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }

    // Clean everything and terminate.
    clean();

}
