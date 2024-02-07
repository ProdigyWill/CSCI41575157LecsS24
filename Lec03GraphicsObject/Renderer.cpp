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
    }

    ~Renderer() {
        // Example cleanup of the VAO
        glDeleteVertexArrays(1, &vaoId);
    }

    void allocateVertexBuffers(const std::vector<std::shared_ptr<GraphicsObject>>& objects) {
        // Bind VAO before allocating vertex buffers
        glBindVertexArray(vaoId);

        // Perform static allocation of vertex buffers here
        for (const auto& object : objects) {
            object->StaticAllocateVertexBuffer();
        }

        // Unbind VAO after allocating vertex buffers
        glBindVertexArray(0);
    }
};

