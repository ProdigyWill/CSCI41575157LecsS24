#include "Shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "GraphicsObject.h"
#include "Scene.h"

class Renderer {
private:
    std::shared_ptr<Shader> shader;
    GLuint vaoId;

    void RenderObject(const GraphicsObject& object)
    {
        shader->SendMat4Uniform("world", object.GetReferenceFrame());

        auto& buffer = object.GetVertexBuffer();
        buffer->Select();
        buffer->SetUpAttributeInterpretration();
        glDrawArrays(buffer->GetPrimitiveType(), 0, buffer->GetNumberOfVertices());

        // Recursively render the children
        auto& children = object.GetChildren();
        for (auto& child : children) {
            RenderObject(*child);
        }
    }

public:
    Renderer(const std::shared_ptr<Shader>& shader) : shader(shader) {
        // Generate VAO
        glGenVertexArrays(1, &vaoId);
    }

    ~Renderer() {
        glDeleteVertexArrays(1, &vaoId);
    }

    void allocateVertexBuffers(const std::vector<std::shared_ptr<GraphicsObject>>& objects) {
        // Bind VAO before allocating vertex buffers
        glBindVertexArray(vaoId);

        // static allocation of vertex buffers
        for (const auto& object : objects) {
            object->StaticAllocateVertexBuffer();
        }

        // Unbind VAO after allocating vertex buffers
        glBindVertexArray(0);
    }

    void RenderScene(const std::shared_ptr<Scene> scene, const glm::mat4& view) {
        if (shader->IsCreated()) {
            glUseProgram(shader->GetShaderProgram());
            glBindVertexArray(vaoId);
            shader->SendMat4Uniform("view", view);

            // Get the objects from the scene
            const std::vector<std::shared_ptr<GraphicsObject>>& objects = scene->GetObjects();

            // Render the objects in the scene
            for (auto& object : objects) {
                RenderObject(*object);
            }

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glUseProgram(0);
            glBindVertexArray(0);
        }
    }
};

