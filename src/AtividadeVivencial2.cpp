/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Computação Gráfica - Unisinos
 * Versão inicial: 04/07/2017
 * Última atualização: 07/03/2025
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector> 
#include <random> 
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Protótipo da função de callback do teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Protótipos de funções
int setupShader();
int setupGeometry();

// Dimensões da janela (podem ser alteradas em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;

// Variáveis globais para controle de luzes
bool lightEnabled[3] = { true, true, true };

// Código-fonte do Vertex Shader
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"layout (location = 2) in vec3 normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 scaledNormal;\n"
"out vec3 fragPos;\n"
"out vec3 finalColor;\n"
"void main()\n"
"{\n"
"    vec4 worldPos = model * vec4(position, 1.0);\n"
"    gl_Position = projection * view * worldPos;\n"
"    fragPos = vec3(worldPos);\n"
"    scaledNormal = mat3(transpose(inverse(model))) * normal;\n"
"    finalColor = color;\n"
"}\0";

// Código fonte Fragment Shader
const GLchar* fragmentShaderSource = "#version 450\n"
"struct Light {\n"
"    vec3 position;\n"
"    vec3 color;\n"
"    float intensity;\n"
"};\n"
"uniform Light lights[3];\n"
"uniform bool lightEnabled[3];\n"
"uniform vec3 camPos;\n"
"uniform float ka;\n"
"uniform float kd;\n"
"uniform float ks;\n"
"uniform float q;\n"
"in vec3 scaledNormal;\n"
"in vec3 fragPos;\n"
"in vec3 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"    vec3 N = normalize(scaledNormal);\n"
"    vec3 V = normalize(camPos - fragPos);\n"
"    vec3 result = vec3(0.0);\n"
"    for (int i = 0; i < 3; ++i) {\n"
"        if (!lightEnabled[i]) continue;\n"
"        vec3 L = normalize(lights[i].position - fragPos);\n"
"        vec3 lightColor = lights[i].color * lights[i].intensity;\n"
"        // //Cálculo da parcela de iluminação ambiente\n"
"        vec3 ambient = ka * lightColor;\n"
"        // Luz difusa\n"
"        float diff = max(dot(N, L), 0.0);\n"
"        vec3 diffuse = kd * diff * lightColor;\n"
"        // Luz especular\n"
"        vec3 R = reflect(-L, N);\n"
"        float spec = pow(max(dot(R, V), 0.0), q);\n"
"        vec3 specular = ks * spec * lightColor;\n"
"        result += (ambient + diffuse) * finalColor + specular;\n"
"    }\n"
"    color = vec4(result,1.0);\n"
"}\0";

// Estrutura para armazenar dados do modelo OBJ e transformações
struct OBJModel {
    GLuint VAO;
    int numVertices;
    glm::vec3 position;
    glm::vec3 rotation; 
    glm::vec3 scale;
    std::vector<glm::vec3> vertices;

    OBJModel(GLuint vao, int vertices) : VAO(vao), numVertices(vertices), position(0.0f), rotation(0.0f), scale(1.0f) {}
};

// Função para carregar um arquivo OBJ
int loadSimpleOBJ(string filePATH, int &nVertices, glm::vec3 color, std::vector<glm::vec3>& outVertices)
 {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<GLfloat> vBuffer;

    std::ifstream arqEntrada(filePATH.c_str());
    if (!arqEntrada.is_open())
    {
        std::cerr << "Erro ao tentar ler o arquivo " << filePATH << std::endl;
        return -1;
    }

    // Lê o arquivo OBJ (linha por linha)
    std::string line;
    while (std::getline(arqEntrada, line))
    {
        std::istringstream ssline(line);
        std::string word;
        ssline >> word;

        if (word == "v")
        {
            glm::vec3 vertice;
            ssline >> vertice.x >> vertice.y >> vertice.z;
            vertices.push_back(vertice);
        }
        else if (word == "vt")
        {
            glm::vec2 vt;
            ssline >> vt.s >> vt.t;
            texCoords.push_back(vt);
        }
        else if (word == "vn")
        {
            glm::vec3 normal;
            ssline >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (word == "f")
         {
            while (ssline >> word)
            {
                int vi = 0, ti = 0, ni = 0;
                std::istringstream ss(word);
                std::string index;

                if (std::getline(ss, index, '/')) vi = !index.empty() ? std::stoi(index) - 1 : 0;
                if (std::getline(ss, index, '/')) ti = !index.empty() ? std::stoi(index) - 1 : 0;
                if (std::getline(ss, index)) ni = !index.empty() ? std::stoi(index) - 1 : 0;

                vBuffer.push_back(vertices[vi].x);
                vBuffer.push_back(vertices[vi].y);
                vBuffer.push_back(vertices[vi].z);
                vBuffer.push_back(color.r);
                vBuffer.push_back(color.g);
                vBuffer.push_back(color.b);
                
                if (ni >= 0 && ni < normals.size()) {
                    vBuffer.push_back(normals[ni].x);
                    vBuffer.push_back(normals[ni].y);
                    vBuffer.push_back(normals[ni].z);
                } else {
                    vBuffer.push_back(0.0f);
                    vBuffer.push_back(0.0f);
                    vBuffer.push_back(1.0f);
                }
            }
        }
    }

    arqEntrada.close();
    outVertices = vertices;

    // Criação do Vertex Buffer Object (VBO) e Vertex Array Object (VAO)
    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    nVertices = vBuffer.size() / 9;  // x, y, z, r, g, b, nx, ny, nz

    return VAO;
}

// Variáveis globais para controle de transformações
bool rotateX=false, rotateY=false, rotateZ=false;
bool isMovingForward = false, isMovingBackward = false, isMovingLeft = false, isMovingRight = false, isMovingUp = false, isMovingDown = false;
bool isScalingUp = false, isScalingDown = false;
// Velocidades
const float translationSpeed = 1.0f;
const float rotationSpeed = 25.0f; 
const float scalingSpeed = 1.0f; 

// Vetor para armazenar os modelos carregados
std::vector<OBJModel> models; 
size_t selectedModelIndex = 0; 
// Matrizes de transformação
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

// MAIN
int main()
{
    // Inicialização do GLFW
    glfwInit();

    //Muita atenção aqui: alguns ambientes não aceitam essas configurações
    //Você deve adaptar para a versão do OpenGL suportada por sua placa
    //Sugestão: comente essas linhas de código para desobrir a versão e
    //depois atualize (por exemplo: 4.5 com 4 e 5)
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Essencial para computadores da Apple
    //#ifdef __APPLE__
    //	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //#endif

    // Criando GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Atividade Vivencial 2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Registra a função de callback do teclado
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega todos os ponteiros de função do OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

    }

    // Informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER); 
    const GLubyte* version = glGetString(GL_VERSION); 
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);


    // Compilação e construção do shader
    GLuint shaderID = setupShader();

    // Carregando OBJ
    int numVerticesSuzanne;
    std::vector<glm::vec3> verticesSuzanne;
    GLuint suzanneVAO = loadSimpleOBJ("../assets/Modelos3D/Suzanne.obj", numVerticesSuzanne, glm::vec3(1.0f, 1.0f, 1.0f), verticesSuzanne); // Red color
    if (suzanneVAO != -1) {
        models.push_back(OBJModel(suzanneVAO, numVerticesSuzanne));
        models.back().vertices = verticesSuzanne;
    }

    glUseProgram(shaderID);
    
    // Configuração do VAO para o modelo Suzanne
    GLint modelLoc = glGetUniformLocation(shaderID, "model");
    GLint viewLoc = glGetUniformLocation(shaderID, "view");
    GLint projLoc = glGetUniformLocation(shaderID, "projection");

    GLint camPosLoc = glGetUniformLocation(shaderID, "camPos");
    GLint kaLoc = glGetUniformLocation(shaderID, "ka");
    GLint kdLoc = glGetUniformLocation(shaderID, "kd");
    GLint ksLoc = glGetUniformLocation(shaderID, "ks");
    GLint qLoc = glGetUniformLocation(shaderID, "q");

    // Estrutura das luzes - uniforms
    GLint lightPosLoc[3], lightColorLoc[3], lightIntensityLoc[3];
    char nameBuf[64];
    for (int i = 0; i < 3; ++i) {
        sprintf(nameBuf, "lights[%d].position", i);
        lightPosLoc[i] = glGetUniformLocation(shaderID, nameBuf);
        sprintf(nameBuf, "lights[%d].color", i);
        lightColorLoc[i] = glGetUniformLocation(shaderID, nameBuf);
        sprintf(nameBuf, "lights[%d].intensity", i);
        lightIntensityLoc[i] = glGetUniformLocation(shaderID, nameBuf);
    }

    glEnable(GL_DEPTH_TEST);
    float lastFrame = 0.0f;

    // Variáveis de controle das luzes
    float ka = 0.1f, kd = 0.7f, ks = 0.5f, q = 32.0f;
    glm::vec3 lightColors[3] = {
        glm::vec3(1.0f, 0.95f, 0.8f),
        glm::vec3(0.6f, 0.7f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };
    // Intensidades das luzes
    float lightIntensities[3] = { 1.0f, 0.5f, 0.3f };


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        // Atualiza a matriz de visualização e projeção
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
        viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniform3fv(camPosLoc, 1, glm::value_ptr(cameraPos));
        glUniform1f(kaLoc, ka);
        glUniform1f(kdLoc, kd);
        glUniform1f(ksLoc, ks);
        glUniform1f(qLoc, q);

        glm::vec3 objPos = models[selectedModelIndex].position;
        float objScale = models[selectedModelIndex].scale.x;
        glm::vec3 lightPositions[3] = {
            objPos + glm::vec3(2.0f * objScale, 2.0f * objScale, 2.0f * objScale),
            objPos + glm::vec3(-2.0f * objScale, 1.0f * objScale, 2.0f * objScale),
            objPos + glm::vec3(0.0f, 3.0f * objScale, -2.0f * objScale)
        };
        for (int i = 0; i < 3; ++i) {
            glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPositions[i]));
            glUniform3fv(lightColorLoc[i], 1, glm::value_ptr(lightColors[i]));
            glUniform1f(lightIntensityLoc[i], lightIntensities[i]);
        }

        // Habilita/desabilita luzes
        GLint lightEnabledInt[3] = { lightEnabled[0] ? 1 : 0, lightEnabled[1] ? 1 : 0, lightEnabled[2] ? 1 : 0 };
        glUniform1iv(glGetUniformLocation(shaderID, "lightEnabled"), 3, lightEnabledInt);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLineWidth(2);
        glPointSize(5);

        // Desenha o modelo
        for (size_t i = 0; i < models.size(); i++) {
            glm::mat4 model = glm::mat4(1);
            if (i == selectedModelIndex) {
                if (isMovingForward) models[i].position.z -= translationSpeed * deltaTime;
                if (isMovingBackward) models[i].position.z += translationSpeed * deltaTime;
                if (isMovingLeft) models[i].position.x -= translationSpeed * deltaTime;
                if (isMovingRight) models[i].position.x += translationSpeed * deltaTime;
                if (isMovingUp) models[i].position.y += translationSpeed * deltaTime;
                if (isMovingDown) models[i].position.y -= translationSpeed * deltaTime;
                if (isScalingUp) models[i].scale *= (1.0f + (scalingSpeed - 1.0f) * deltaTime);
                if (isScalingDown) models[i].scale *= (1.0f - (scalingSpeed - 1.0f) * deltaTime);
                if (rotateX || rotateY || rotateZ) {
                    if (rotateX)
                        models[i].rotation.x += rotationSpeed * deltaTime;
                    else if (rotateY)
                        models[i].rotation.y += rotationSpeed * deltaTime;
                    else if (rotateZ)
                        models[i].rotation.z += rotationSpeed * deltaTime;
                }
            }

            // Aplica as transformações
            model = glm::translate(model, models[i].position);
            model = glm::rotate(model, glm::radians(models[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(models[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(models[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(models[i].scale));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(models[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, models[i].numVertices);
            glBindVertexArray(0);
        }
        glfwSwapBuffers(window);
    }
    for (const auto& model : models) {
        glDeleteVertexArrays(1, &model.VAO);
    }
    glfwTerminate();
    return 0;
}

// Função de callback do teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Rotação (X, Y, Z)
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        rotateX = true; rotateY = false; rotateZ = false;
    }
    if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        rotateX = false; rotateY = true; rotateZ = false;
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        rotateX = false; rotateY = false; rotateZ = true;
    }

    // Translação X/Z
    if (key == GLFW_KEY_W) isMovingForward = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S) isMovingBackward = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A) isMovingLeft = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D) isMovingRight = (action != GLFW_RELEASE);

    // Translação Y (I/J)
    if (key == GLFW_KEY_I) isMovingUp = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_J) isMovingDown = (action != GLFW_RELEASE);

    // Escala ([ e ])
    if (key == GLFW_KEY_LEFT_BRACKET) isScalingDown = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_RIGHT_BRACKET) isScalingUp = (action != GLFW_RELEASE);
    
    // Habilita/desabilita luzes (1, 2, 3)
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) lightEnabled[0] = !lightEnabled[0];
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) lightEnabled[1] = !lightEnabled[1];
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) lightEnabled[2] = !lightEnabled[2];

}

// Esta função é bastante hardcoded - o objetivo é compilar e construir um programa de shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e fragmentShaderSource no início deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Verifica erros de compilação (exibe via log no terminal)
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Verifica erros de compilação (exibe via log no terminal)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)	{
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Vincula os shaders e cria o identificador do programa de shader
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Verifica erros de link
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Esta função é bastante hardcoded - o objetivo é criar os buffers que armazenam a
// geometria do triângulo
// Apenas atributo de coordenadas nos vértices
// 1 VBO com coordenadas, VAO com apenas 1 ponteiro de atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
    // Aqui definimos as coordenadas x, y e z do triângulo e as armazenamos sequencialmente,
    // já visando enviá-las para o VBO (Vertex Buffer Objects)
    // Cada atributo de vértice (coordenada, cores, coordenadas de textura, normal, etc.)
    // Pode ser armazenado em um único VBO ou em VBOs separados
    GLfloat vertices[] = {
        // Face +X (red)
         0.5, -0.5, -0.5, 1,0,0,   0.5,  0.5, -0.5, 1,0,0,   0.5,  0.5,  0.5, 1,0,0,
         0.5, -0.5, -0.5, 1,0,0,   0.5,  0.5,  0.5, 1,0,0,   0.5, -0.5,  0.5, 1,0,0,
        // Face -X (green)
        -0.5, -0.5,  0.5, 0,1,0,  -0.5,  0.5,  0.5, 0,1,0,  -0.5,  0.5, -0.5, 0,1,0,
        -0.5, -0.5,  0.5, 0,1,0,  -0.5,  0.5, -0.5, 0,1,0,  -0.5, -0.5, -0.5, 0,1,0,
        // Face +Y (blue)
        -0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5,  0.5, 0,0,1,
        -0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5,  0.5, 0,0,1,  -0.5,  0.5,  0.5, 0,0,1,
        // Face -Y (yellow)
        -0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5, -0.5, 1,1,0,
        -0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5, -0.5, 1,1,0,  -0.5, -0.5, -0.5, 1,1,0,
        // Face +Z (magenta)
        -0.5, -0.5,  0.5, 1,0,1,  -0.5,  0.5,  0.5, 1,0,1,   0.5,  0.5,  0.5, 1,0,1,
        -0.5, -0.5,  0.5, 1,0,1,   0.5,  0.5,  0.5, 1,0,1,   0.5, -0.5,  0.5, 1,0,1,
        // Face -Z (cyan)
        -0.5,  0.5, -0.5, 0,1,1,  -0.5, -0.5, -0.5, 0,1,1,   0.5, -0.5, -0.5, 0,1,1,
        -0.5,  0.5, -0.5, 0,1,1,   0.5, -0.5, -0.5, 0,1,1,   0.5,  0.5, -0.5, 0,1,1,
    };

    GLuint VBO, VAO;

    // Gerar identificador do VBO
    glGenBuffers(1, &VBO);

    // Vincula o buffer como um array buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Envia o array de floats para o buffer do OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Gera identificador do VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);

    // Vincula o VAO
    glBindVertexArray(VAO);
  
    // Atributo de posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo de cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs indesejados)
    glBindVertexArray(0);

    return VAO;
}