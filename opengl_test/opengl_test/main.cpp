#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h> //подключение библиотеки GLFW
#include <OpenGL/gl3.h> //для MacOS

#define GL_SILENCE_DEPRECATION //не показываем предупреждения об устаревших функциях OpenGL
#define GLFW_INCLUDE_NONE //не включать заголовочные файлы автоматически

int main(){
    // Инициализируем GLFW
    //проверка на ошибку при инциализации библиотеки GLFW
    if(!glfwInit()) {
        std::cerr << "Failed with initialization GLFW\n"; //вывод ошибки
        return -1;
    }
    
    //создаем окошко OpenGL
    glfwWindowHint(GLFW_SAMPLES, 4); //сглаживание 4x MSAA
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //версия OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //версия OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //используем CoreProfile
    
    //forward-совместимость для MacOS
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Open GL ES Emulator", NULL, NULL); //указатель на структуру GLFWwindow(окно)
    
    //проверка успешности создания окна
    if(!window) {
        std::cerr << "Failed with create window\n";
        glfwTerminate();//завершаем работу GLFW в случае неудачи
        return -1;
    }
    
    glfwMakeContextCurrent(window); //делаем OpenGl контекст созданного окна активным (текущим) для вызывающего потока
    
    //инициализация GLEW и проверка корректности
    if(glewInit() != GLEW_OK){
        std::cerr << "Failed with initialization GLEW\n";
        return -1;
    }
    
    //цикл, чтобы окно не закрывалось
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}
