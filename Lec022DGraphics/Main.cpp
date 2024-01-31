#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void OnWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

struct Result {
    bool isSuccess;
    std::string message;

    Result() : isSuccess(true) {}
};

struct VertexData {
    glm::vec3 position, color;
};

static void Log(std::stringstream& log, const std::vector<char>& message)
{
    std::copy(message.begin(), message.end(), std::ostream_iterator<char>(log, ""));
}

static unsigned int CompileShaderSource(
    const std::string& shaderSource, int type, std::stringstream& logger, Result& result)
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

        Log(logger, infoLog);
        result.isSuccess = false;
        result.message = logger.str();
        return -1;
    }
    result.isSuccess = true;
    result.message = "Success!";
    return shaderId;
}

static Result CreateShaderProgram(
    std::string& vertexSource, std::string& fragmentSource, unsigned int& program)
{
    std::stringstream logger;
    Result result;

    unsigned int vertexShader =
        CompileShaderSource(vertexSource, GL_VERTEX_SHADER, logger, result);
    if (result.isSuccess == false) return result;

    unsigned int fragmentShader =
        CompileShaderSource(fragmentSource, GL_FRAGMENT_SHADER, logger, result);
    if (result.isSuccess == false) return result;

    // Time to link the shaders together into a program.

    program = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    int isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        Log(logger, infoLog);
        result.isSuccess = false;
        result.message = logger.str();
        return result;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    result.isSuccess = true;
    result.message = "Successfully created the shader!";
    return result;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "ETSU Computing Interactive Graphics", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1200, 800);
    glfwSetFramebufferSizeCallback(window, OnWindowSizeChanged);
    //glfwMaximizeWindow(window);

    std::string vertexSource =
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

    std::string fragmentSource =
        "#version 430\n"
        "in vec4 fragColor;\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "   color = fragColor;\n"
        "}\n";

    unsigned int shaderProgram;
    Result result = CreateShaderProgram(vertexSource, fragmentSource, shaderProgram);

    VertexData vertexData[7]{};
    vertexData[0] = { { 5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[1] = { { 5.0f, 20.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[2] = { { 10.0f, 20.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[3] = { { 0.0f, 35.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[4] = { {-10.0f, 20.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[5] = { {-5.0f, 20.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
    vertexData[6] = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspectRatio = width / (height * 1.0f);

    glm::mat4 referenceFrame(1.0f);
    // The 'camera' i.e., the view matrix
    glm::mat4 viewOriginal(1.0f), view(1.0);
    // The camera's x points down the -ve world x
    viewOriginal[0] = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
    // The camera's z points down the -ve world z
    viewOriginal[2] = glm::vec4( 0.0f, 0.0f, -1.0f, 0.0f);

    float left = -100.0f;
    float right = 100.0f;
    left *= aspectRatio;
    right *= aspectRatio;
    glm::mat4 projection = glm::ortho(left, right, -100.0f, 100.0f, -1.0f, 1.0f);

    unsigned int vaoId, vboId;
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    std::string message = result.message;

    glm::vec3 clearColor = { 0.2f, 0.3f, 0.3f };

    glUseProgram(shaderProgram);

    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int worldLoc = glGetUniformLocation(shaderProgram, "world");

    float angle = 0, oldAngle =  0.0f;
    float moveDelta = 0, oldDelta =0.0f;
    float xPos = 0, yPos = 0, oldX = 0.0f, oldY = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        

        
        if (moveDelta != oldDelta) {
            glm::vec4 delta = referenceFrame[1] * moveDelta;
            xPos = delta.x;
            yPos = delta.y;
            oldDelta = moveDelta;
        }

        if (xPos != oldX || yPos != oldY) {
            // Update reference frame based on changes in xPos and yPos
            referenceFrame[3][0] += (xPos - oldX) * aspectRatio;
            referenceFrame[3][1] += yPos - oldY;

            // Update oldX and oldY
            oldX = xPos;
            oldY = yPos;
        }

        if (angle != oldAngle) {
            // Update the rotation matrix
            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

            // Copy the rotation matrix to the reference frame
            referenceFrame[0] = rotationMatrix[0];
            referenceFrame[1] = rotationMatrix[1];
            referenceFrame[2] = rotationMatrix[2];

            // Update oldAngle
            oldAngle = angle;
        }
        //referenceFrame = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        //referenceFrame[3] = glm::vec4(xPos, yPos, 0.0f, 1.0f);
        

        // Render the object
        if (result.isSuccess)
        {
            glUseProgram(shaderProgram);
            
            glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(referenceFrame));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            glBindVertexArray(vaoId);
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            // Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,
                3,              // Each position has 3 components
                GL_FLOAT,       // Each component is a 32-bit floating point value
                GL_FALSE,
                sizeof(VertexData), // The number of bytes to the next position
                (void*)0        // Byte offset of the first position in the array
            );
            // Colors
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1,
                3,              // Each color has 3 components
                GL_FLOAT,       // Each component is a 32-bit float-ing point value
                GL_FALSE,
                sizeof(VertexData), // The number of bytes to the next color
                (void*)sizeof(glm::vec3) // Byte offset of the first color in the array
            );
            glDrawArrays(GL_LINE_LOOP, 0, 7);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
            glUseProgram(0);
            glBindVertexArray(0);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Computing Interactive Graphics");
        ImGui::Text(message.c_str());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / io.Framerate, io.Framerate);
        ImGui::ColorEdit3("Background color", (float*)&clearColor.r);
        ImGui::SliderFloat("Angle", &angle, 0, 360);
        ImGui::SliderFloat("Object X", &xPos, -100, 100);
        ImGui::SliderFloat("Object Y", &yPos, -100, 100);
        ImGui::SliderFloat("Move Delta", &moveDelta, -100, 100);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

