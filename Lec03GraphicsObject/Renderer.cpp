#include "Renderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Shader.h"
#include "GraphicsObject.h"
#include "Scene.h"

class Renderer {
private:
    std::shared_ptr<Shader> shader;
    GLuint vaoId;

public:
    Renderer(const std::shared_ptr<Shader>& shader) : shader(shader) {
        // Generate VAO
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
    }

    ~Renderer() {
        // Delete VAO when done
        glDeleteVertexArrays(1, &vaoId);
    }
    
    void staticAllocateVertexBuffers(const std::vector<GraphicsObject>& objects) {

    }
};

