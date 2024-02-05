#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/glm.hpp>
#include "BaseObject.h"
class Renderer : public BaseObject
{
public:
	Renderer();
	~Renderer() = default;

	inline const std::shared_ptr<Shader>& GetShader() const { return shader; }
private:
	std::shared_ptr<Shader> shader;
	unsigned int vaoId;
};

