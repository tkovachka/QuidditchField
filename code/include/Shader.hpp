#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <fstream>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader{
public:
    GLuint ID;

    Shader(const char* vert_path, const char* frag_path);
    void use();

private:
    void Check_Shaders_Program(GLuint type, std::string name);
};
#endif // SHADER_HPP
