#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "BaseObject.h"
#include <glad/glad.h>
#include "GraphicsObject.h"
#include "Scene.h"
class Renderer {
private:
    std::shared_ptr<Shader> shader;
    GLuint vaoId;

public:
    Renderer(const std::shared_ptr<Shader>& shader);
    ~Renderer();

    inline const std::shared_ptr<Shader>& getShader() const {
        return shader;
    }

    void staticAllocateVertexBuffers(const std::vector<GraphicsObject>& objects);

};

