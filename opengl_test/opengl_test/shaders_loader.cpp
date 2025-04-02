#include <fstream>
#include <sstream>
#include <iostream>
#include "shaders_loader.h"

std::string loadShaderSource(const std::string &path){
    std::ifstream file(path);
    if(!file.is_open()){
        std::cerr << "Failed with open shaders dir - " << path << std::endl;
        return "";
    }
    
    std::stringstream buffer; //буффер
    buffer << file.rdbuf(); //записываем все содержимое файла в буффер
    return buffer.str(); //преобразуем содержимое буфера в строку
}

GLuint compileShader(GLenum type, const std::string& source){
    //привязываем исходный код шейдера к объекту шейдера и компилируем его
    GLuint Shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(Shader, 1, &src, NULL);
    glCompileShader(Shader);
    
    //проверка корректности сброки шейдера
    GLint success; //число для определения успешности сборки
    GLchar infoLog[512]; //контейнер для хранения ошибок
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success); //проверка успешности
    if(!success){
        glGetShaderInfoLog(Shader, 512, NULL, infoLog);
        std::cout << "Error with vertex shader\n" << infoLog << std::endl;
    }
    
    return Shader;
}

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath){
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);
    
    if (vertexSource.empty() || fragmentSource.empty()) {
            return 0;
    }
    
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    
    //соединяем два шейдера в один объект шейдерной программы
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    //присоединяем
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //связываем
    glLinkProgram(shaderProgram);
    //проверяем на корректность объединения
    GLint success;
    GLchar infoLog[512];
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error with linking shader program\n" << infoLog << std::endl;
    }
    //удаляем созданные шейдеры после связывания
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}


