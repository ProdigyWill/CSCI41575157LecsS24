#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "BaseObject.h"
#include <glad/glad.h>
#include "GraphicsObject.h"
#include "Scene.h"
#include "Shader.h"
class Renderer {
private:
    std::shared_ptr<Shader> shader;
    GLuint vaoId;

public:
    Renderer(std::shared_ptr<Shader>& shader);

    inline const std::shared_ptr<Shader>& getShader() const {
        return shader;
    }
};

