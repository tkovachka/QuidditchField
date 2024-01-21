#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Camera.hpp>
#include <Shader.hpp>
#include <Object.hpp>

#include <iostream>
#include <string>

const std::string program_name = ("QuidditchField");

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
static Camera camera(glm::vec3(0.0f, 1.5f, 3.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f; // time between current frame and last frame
static float lastFrame = 0.0f;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(
      GLFW_OPENGL_FORWARD_COMPAT,
      GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          program_name.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile our shader program
    // ------------------------------------
    const char *vert_shader = "../res/shaders/shader.vert";
    const char *frag_shader = "../res/shaders/shader.frag";

    Shader ourShader(vert_shader, frag_shader);
    ourShader.use();

    unsigned int view_matrix_loc = glGetUniformLocation(ourShader.ID, "view");
    unsigned int projection_matrix_loc = glGetUniformLocation(ourShader.ID, "projection");
    unsigned int camera_position_loc = glGetUniformLocation(ourShader.ID, "camera_position");

    glm::vec3 camera_position(0.0f, 0.0f, 3.0f); // -Z is into the screen.
    glm::vec3 camera_target(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

    glUniform3f(camera_position_loc, camera_position.x, camera_position.y, camera_position.z);

    glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);

    glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float) 800 / (float) 800, 0.1f, 100.0f);
    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection));

    //does nothing atm
    glm::mat4 spinning_mat(1.0f);
    unsigned int animate_loc = glGetUniformLocation(ourShader.ID, "animate");

    //todo fix .mtl file with correct materials
    Model object("../res/objects/field_better.obj");
    glActiveTexture(GL_TEXTURE0); // Reusing the same texture unit for each model mesh.
    unsigned int image_sampler_loc = glGetUniformLocation(ourShader.ID, "image");
    glUniform1i(image_sampler_loc, 0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        glUniformMatrix4fv(animate_loc, 1, GL_FALSE, glm::value_ptr(spinning_mat));

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE,
                           glm::value_ptr(view)); // Uniform: Transfer view matrix to vertex shader.

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (unsigned int i = 0; i < object.num_meshes; ++i) {
            glBindTexture(GL_TEXTURE_2D, object.mesh_list[i].tex_handle); // Bind texture for the current mesh.

            glBindVertexArray(object.mesh_list[i].VAO);
            glDrawElements(GL_TRIANGLES, (GLsizei) object.mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // pass projection matrix to shader (note that in this case it could change
        // every frame)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(ourShader.ID);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(camera.mode, FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(camera.mode,BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(camera.mode,LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(camera.mode,RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        camera.ToggleFlightMode();
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}