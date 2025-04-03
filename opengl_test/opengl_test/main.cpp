#define GL_SILENCE_DEPRECATION //не показываем предупреждения об устаревших функциях OpenGL
#define GLFW_INCLUDE_NONE //не включать заголовочные файлы автоматически

#include <iostream>

#include <GL/glew.h> //подключение glew
#include <GLFW/glfw3.h> //подключение GLFW
#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#endif //для MacOS
#include <glm/glm.hpp> //подключение glm

#include "shaders_loader.h" //заголовочный файл с методами обработки шейдеров

enum Primitives { SQUARE, TRIANGLE, POINTS, LINES }; //перечисление с типом примитива
Primitives currentPrimitive = TRIANGLE;//дефолтный примитив - треугольник
bool showMenu = false;
glm::vec3 objColor(0.6f, 0.0f, 1.0f);//дефолтный фиолетовый цвет

//прототипы функций
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);//обработчик кнопок
void Menu();//вывод консольного меню

int main(){
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
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
    
    // Дополнительная проверка для macOS
    #ifdef __APPLE__
        glfwHideWindow(window);  // Сначала скрываем
        glfwShowWindow(window);  // Потом показываем
    #endif
    
    glfwMakeContextCurrent(window); //делаем OpenGl контекст созданного окна активным (текущим) для вызывающего потока
    glfwSetKeyCallback(window, key_callback);//регестрируем функцию обработку клавиш в окне
    
    glewExperimental = GL_TRUE;
    //инициализация GLEW и проверка корректности
    if(glewInit() != GLEW_OK){
        std::cerr << "Failed with initialization GLEW\n";
        return -1;
    }
    
    //настройка фреймбуфера
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    //загружаем шейдеры из папки
    GLuint shaderProgram = createShaderProgram("opengl_test/shaders/vertex.glsl", "opengl_test/shaders/fragment.glsl");
    if (!shaderProgram) {
        std::cerr << "Failed to create shader program\n";
        glfwTerminate();
        return -1;
    }
    /*
    //массив с вершинами треугольника
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    */
    
    GLfloat vertices[] = {
        //точка (0)
        0.0f, 0.0f, 0.0f,
        
        //отрезок (1-2)
        -0.8f, 0.0f, 0.0f,
        0.8f, 0.0f, 0.0f,
        
        //треугольник (3-5)
        -0.5f, -0.5f, 0.0f,  //лево низ
        0.5f, -0.5f, 0.0f,   //право низ
        0.0f, 0.5f, 0.0f,    //верх
        
        //квадрат (6-9)
        0.5f,  0.5f, 0.0f,   //верх право
        0.5f, -0.5f, 0.0f,   //низ право
        -0.5f,-0.5f, 0.0f,   //низ лево
        -0.5f, 0.5f, 0.0f    //верх лево
    };

    //для квадрата
    GLuint indices[] = {
        6, 7, 9,  //первый треугольник
        7, 8, 9   //второй треугольник
    };
    
    GLuint VBO, VAO, EBO; //объект вершинного массива, вершинный буфер, буфер индексов
    glGenBuffers(1, &VBO); //генерация 1-го буфера, указатель на переменную
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO); //привязываем VAO
    
    //Копируем наш массив вершин в буфер для OpenGL
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //последующие операции с GL_ARRAY_BUFFER будут применяться к этому VBO
    //выделяем память в GPU и копируем туда наши значения VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //аналогично для EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //последующие операции с GL_ARRAY_BUFFER будут применяться к этому VBO
    //выделяем память в GPU и копируем туда наши значения VBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    
    //интерпритация вершинных данных
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); //location 0, кол-во компоненотов (x,y,z), тип данных, не нормализуем, шаг между атрибутами, смещение данных в начале буфера
    glEnableVertexAttribArray(0);//активируем вершинный атрибут с location = 0
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);//отвязываем VAO
    
    //цикл обработки
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();//обработка событий
        
        //очистка экрана
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);//серый
        glClear(GL_COLOR_BUFFER_BIT); //очищаем буфер цвета
        
        //для вывода меню один раз
        static bool firstRun = true;
        if (firstRun || showMenu) {
            Menu();
            firstRun = false;
        }
        
        
        
        //используем шейдеры
        glUseProgram(shaderProgram);
        
        //передаем цвет в шейдер
        GLint objColorLoc = glGetUniformLocation(shaderProgram, "objColor");
        glUniform3f(objColorLoc, objColor.r, objColor.g, objColor.b);
        
        glBindVertexArray(VAO);//привязываем VAO
        
        switch(currentPrimitive) {
            case POINTS:
                glPointSize(10.0f); //размер точки
                glDrawArrays(GL_POINTS, 0, 1); //отрисовка точки
                break;
            case LINES:
                glLineWidth(3.0f); //толщина линии
                glDrawArrays(GL_LINES, 1, 2); //отрисовка линии
                break;
            case TRIANGLE:
                glDrawArrays(GL_TRIANGLES, 3, 3); //отрисовка треугольника
                break;
            case SQUARE:
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //отрисовка квадрата
                break;
        }
        
        glBindVertexArray(0);
        //обмен буферов
        glfwSwapBuffers(window);
    }
    //освобождаем ресурсы
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}
/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    //когда пользователь нажимает ESC закрываем окно
    std::cout << key << std::endl;//вывод для проверки
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);//устанавливаем свойство WindowShouldClose в true
}
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_1:
                currentPrimitive = SQUARE;
                //std::cout << "Выбран квадрат (нажата кнопка 1)" << std::endl;
                break;
            case GLFW_KEY_2:
                currentPrimitive = TRIANGLE;
                //std::cout << "Выбран треугольник (нажата кнопка 2)" << std::endl;
                break;
            case GLFW_KEY_3:
                currentPrimitive = POINTS;
                //std::cout << "Выбрана точка (нажата кнопка 3)" << std::endl;
                break;
            case GLFW_KEY_4:
                currentPrimitive = LINES;
                //std::cout << "Выбран отрезок (нажата кнопка 4)" << std::endl;
                break;
            case GLFW_KEY_G:
                objColor.r = static_cast<float>(rand()) / RAND_MAX;
                objColor.g = static_cast<float>(rand()) / RAND_MAX;
                objColor.b = static_cast<float>(rand()) / RAND_MAX;
                //std::cout <<  << std::endl;
                break;
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
        }
    }
}

void Menu(){
    std::cout << "------------------------" << std::endl;
    std::cout << "Меню отображения примитивов" << std::endl;
    std::cout << "1 - Показать квадрат" << std::endl;
    std::cout << "2 - Показать треугольник" << std::endl;
    std::cout << "3 - Показать точки" << std::endl;
    std::cout << "4 - Показать отрезок" << std::endl;
    std::cout << "G - Изменить цвет примитива" << std::endl;
    std::cout << "ESC - завершить" << std::endl;
}
