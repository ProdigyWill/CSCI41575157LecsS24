#include "Renderer.h"
#include "Shader.h"

Renderer::Renderer()
{
	vaoId = 0;
	shader = std::make_shared<Shader>();
}

