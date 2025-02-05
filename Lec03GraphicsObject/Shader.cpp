#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TextFile.h"

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
{
    this->vertexSource = vertexSource;
    this->fragmentSource = fragmentSource;
    Init();
    CreateShaderProgram();
}

Shader::Shader()
{
    SetDefaultSource();
    Init();
    CreateShaderProgram();
}

void Shader::AddUniform(const std::string& uniformName)
{
    if (uniformMap.contains(uniformName)) {
        return;
    }

    // If it doesn't exist, get the uniform location
    int uniformLocation = glGetUniformLocation(shaderProgram, uniformName.c_str());

    // Store the location in the uniformMap
    uniformMap[uniformName] = uniformLocation;
}

void Shader::SendMat4Uniform(const std::string& uniformName, const glm::mat4& mat)
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(uniformMap[uniformName], 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetDefaultSource()
{
    vertexSource =
        "#version 430\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec3 color;\n"
        "out vec4 fragColor;\n"
        "uniform mat4 world;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * world * vec4(position, 1.0);\n"
        "   fragColor = vec4(color, 1.0);\n"
        "}\n";

    fragmentSource =
        "#version 430\n"
        "in vec4 fragColor;\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = fragColor;\n"
        "}\n";
}

//void Shader::SetDefaultSource()
//{
//    const std::string vertexFilePath = "basic.vert.glsl";
//    const std::string fragmentFilePath = "basic.frag.glsl";
//
//    TextFile vertexFile(vertexFilePath);
//    TextFile fragmentFile(fragmentFilePath);
//
//    vertexSource = vertexFile.getData();
//    fragmentSource = fragmentFile.getData();
//}

void Shader::Init()
{
	shaderProgram = 0;
}

unsigned int Shader::CompileShaderSource(int type, const std::string& shaderSource)
{
    unsigned shaderId = glCreateShader(type);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const char* source = (const char*)shaderSource.c_str();
    glShaderSource(shaderId, 1, &source, 0);

    glCompileShader(shaderId);

    int isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<char> infoLog(maxLength);
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(shaderId);

        Log(infoLog);
        return -1;
    }
    Log("Success!");
    return shaderId;
}

void Shader::CreateShaderProgram()
{
    unsigned int vertexShader =
        CompileShaderSource(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == -1) { return; }

    unsigned int fragmentShader =
        CompileShaderSource(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == -1) { return; }

    // Time to link the shaders together into a program.

    shaderProgram = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link our program
    glLinkProgram(shaderProgram);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    int isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(shaderProgram);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        Log(infoLog);
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    Log("Successfully created the shader!");
}
