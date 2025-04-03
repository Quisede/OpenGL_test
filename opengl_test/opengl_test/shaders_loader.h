#include <string>
#include <GL/glew.h>

//загрузка, компиляция и создание программы шейдеров
std::string loadShaderSource(const std::string& path);
GLuint compileShader(GLenum type, const std::string& source);
GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
