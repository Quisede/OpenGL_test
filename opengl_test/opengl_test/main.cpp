#define GL_SILENCE_DEPRECATION //не показываем предупреждения об устаревших функциях OpenGL
#define GLFW_INCLUDE_NONE //не включать заголовочные файлы автоматически

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h> //подключение библиотеки GLFW
#include <OpenGL/gl3.h> //для MacOS

//вершинный шейдер
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

//фрагментный шейдер
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.6f, 0.0f, 1.0f, 1.0f);\n"
"}\n\0";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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
    glfwSetKeyCallback(window, key_callback);
    
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
    
    //создаем и комплириуем шейдерную программу
    
    //вершинный шейдер
    //привязываем исходный код шейдера к объекту шейдера и компилируем его
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    //проверка корректности сброки шейдера
    GLint success; //число для определения успешности сборки
    GLchar infoLog[512]; //контейнер для хранения ошибок
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //проверка успешности
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error with vertex shader\n" << infoLog << std::endl;
    }
    
    //фрагментный шейдер
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    //проверка корректности сброки шейдера
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); //проверка успешности
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Error with fragment shader\n" << infoLog << std::endl;
    }
    
    //соединяем два шейдера в один объект шейдерной программы
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    //присоединяем
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //связываем
    glLinkProgram(shaderProgram);
    //проверяем на корректность объединения
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error with linking shader program\n" << infoLog << std::endl;
    }
    //удаляем созданные шейдеры после связывания
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    /*
    //массив с вершинами треугольника
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    */
    
    GLfloat vertices[] = {
        0.5f,  0.5f, 0.0f,  // нерхний правый угол
        0.5f, -0.5f, 0.0f,  // нижний правый угол
        -0.5f, -0.5f, 0.0f,  // нижний левый угол
        -0.5f,  0.5f, 0.0f   // верхний левый угол
    };
    
    GLuint indices[] = {
        0, 1, 3, //первый треуг
        1, 2, 3, //второй треуг
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
    
    //цикл, чтобы окно не закрывалось
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        //очистка экрана
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //используем шейдеры
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        //обмен буферов
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    //когда пользователь нажимает ESC закрываем окно
    std::cout << key << std::endl;//вывод для проверки
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);//устанавливаем свойство WindowShouldClose в true
}
