/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Computação Gráfica - Unisinos
 * Versão inicial: 04/07/2017
 * Última atualização: 07/03/2025
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Variáveis globais
const GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow *window;

// Variáveis de shader
vector<vec3> positions;
vector<vec2> texCoords;

GLuint textureID;
GLuint textureBodyID;
GLuint textureEyeID;
GLuint backgroundTexID;

// Classe Camera
class Camera
{
public:
    float radius; // distância ao objeto
    float yaw;    // ângulo em torno do eixo Y 
    float pitch;  // ângulo em torno do eixo X 
    float roll;   // ângulo em torno do eixo Z 
    vec3 target;  // centro do objeto

    Camera(vec3 target = vec3(0.0f, 0.0f, 0.0f), float radius = 5.0f, float yaw = -90.0f, float pitch = 0.0f, float roll = 0.0f)
        : target(target), radius(radius), yaw(yaw), pitch(pitch), roll(roll) {}

    mat4 getViewMatrix()
    {
        // Calcula a posição da câmera em coordenadas esféricas
        float camX = target.x + radius * cos(radians(pitch)) * cos(radians(yaw));
        float camY = target.y + radius * sin(radians(pitch));
        float camZ = target.z + radius * cos(radians(pitch)) * sin(radians(yaw));
        vec3 position = vec3(camX, camY, camZ);
        // Cria a matriz de visualização
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
        if (roll != 0.0f) {
            mat4 rollMat = glm::rotate(mat4(1.0f), radians(roll), normalize(position - target));
            up = vec3(rollMat * vec4(up, 0.0f));
        }

        return lookAt(position, target, up);
    }
};

//  Variáveis da câmera
Camera camera(vec3(0.0f, 0.0f, -5.0f), 5.0f, 0.0f, 0.0f, 0.0f);
vec3 flamingoPosition = vec3(0.0f, 0.0f, -5.0f);
vec3 flamingoScale = vec3(0.2f, 0.2f, 0.2f);
float flamingoRotationX = 0.0f;
float flamingoRotationY = 0.0f;
float flamingoRotationZ = 0.0f;

// Vertex Shader
const char *vertexShaderSource = R"(
#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}
)";

// Fragment Shader
const char *fragmentShaderSource = R"(
#version 400 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D textureBody;
uniform sampler2D textureEye;
uniform int useEyeTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    vec3 ambientColor = 0.2 * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseColor = diff * lightColor;

    float shininess = 32.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specularColor = spec * lightColor;

    vec3 phong = (ambientColor + diffuseColor + specularColor);

    vec4 texColor = useEyeTexture == 1 ? texture(textureEye, TexCoord) : texture(textureBody, TexCoord);
    FragColor = vec4(phong, 1.0) * texColor;
}
)";

// Shaders para o fundo
const char* bgVertexShader = R"(
#version 400 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

// Fragment Shader para o fundo
const char* bgFragmentShader = R"(
#version 400 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D background;
void main() {
    FragColor = texture(background, TexCoord);
}
)";

//  Funções auxiliares
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

GLuint setupShader();
GLuint setupGeometry();

bool loadOBJ(const string &objPath, const string &mtlPath, string &textureFileOut);

GLuint loadTexture(const string &filePath);

void drawFlamingo(GLuint shaderProgram, GLuint VAO, vec3 position, vec3 scale, vec3 rotation);

// Função MAIN
int main()
{
    std::string objPath = "../assets/Modelos3D/Flamingo.obj"; 
    std::string mtlPath = "../assets/Modelos3D/Flamingo.mtl";  
    std::string textureBodyPath = "../assets/tex/FlamingoBody.png";
    std::string textureEyePath = "../assets/tex/flamingoEye.png";  

    std::string textureFile;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "M5 - Camera em primeira pessoa", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = setupShader();

    GLuint bgShaderProgram;

    // Compila e linka shaders para o fundo
    GLuint bgVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(bgVertex, 1, &bgVertexShader, nullptr);
    glCompileShader(bgVertex);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(bgVertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(bgVertex, 512, nullptr, infoLog);
        cout << "Erro no bgVertexShader:\n" << infoLog << endl;
    }

    GLuint bgFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(bgFragment, 1, &bgFragmentShader, nullptr);
    glCompileShader(bgFragment);
    glGetShaderiv(bgFragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(bgFragment, 512, nullptr, infoLog);
        cout << "Erro no bgFragmentShader:\n" << infoLog << endl;
    }

    bgShaderProgram = glCreateProgram();
    glAttachShader(bgShaderProgram, bgVertex);
    glAttachShader(bgShaderProgram, bgFragment);
    glLinkProgram(bgShaderProgram);
    glGetProgramiv(bgShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(bgShaderProgram, 512, nullptr, infoLog);
        cout << "Erro ao linkar bgShaderProgram:\n" << infoLog << endl;
    }
    glDeleteShader(bgVertex);
    glDeleteShader(bgFragment);

    if (!loadOBJ(objPath, mtlPath, textureFile))
    {
        cout << "Erro ao carregar Flamingo.obj" << endl;
        return -1;
    }

    textureBodyID = loadTexture(textureBodyPath);
    textureEyeID = loadTexture(textureEyePath);
    if (textureBodyID == 0 || textureEyeID == 0) {
        cout << "Erro ao carregar texturas." << endl;
        return -1;
}

    backgroundTexID = loadTexture("../assets/tex/fundo-estrelas.jpg");
    if (backgroundTexID == 0) {
        cout << "Erro ao carregar textura de fundo." << endl;
        return -1;
    }

    GLuint VAO = setupGeometry();

    mat4 projection = perspective(radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    vec3 lightPos = vec3(3.0f, 3.0f, 3.0f);
    vec3 lightColor = vec3(1.0f);
    vec3 objectColor = vec3(1.0f);
    float lastFrameTime = glfwGetTime();

    // Configuração fundo
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // Criação do VAO e VBO para o fundo
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Desenha fundo
        glDepthMask(GL_FALSE); // Desabilita escrita no depth buffer
        glUseProgram(bgShaderProgram);
        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexID);
        glUniform1i(glGetUniformLocation(bgShaderProgram, "background"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE); // Reabilita escrita no depth buffer

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));

        mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));

        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, value_ptr(lightPos));
        // Calcula a posição da câmera manualmente
        float camX = camera.target.x + camera.radius * cos(radians(camera.pitch)) * cos(radians(camera.yaw));
        float camY = camera.target.y + camera.radius * sin(radians(camera.pitch));
        float camZ = camera.target.z + camera.radius * cos(radians(camera.pitch)) * sin(radians(camera.yaw));
        glm::vec3 cameraPos = glm::vec3(camX, camY, camZ);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, value_ptr(cameraPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, value_ptr(objectColor));

        // Desenha a textura do corpo do flamingo
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBodyID);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureBody"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "useEyeTexture"), 0);
        drawFlamingo(shaderProgram, VAO, flamingoPosition, flamingoScale, vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ));

        // Desenha o olho do flamingo
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureEyeID);
        glUniform1i(glGetUniformLocation(shaderProgram, "textureEye"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "useEyeTexture"), 1);
        drawFlamingo(shaderProgram, VAO, flamingoPosition, flamingoScale, vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ));

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        cout << "Vertex Shader Error:\n"
             << infoLog << endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        cout << "Fragment Shader Error:\n"
             << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        cout << "Shader Linking Error:\n"
             << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLuint setupGeometry()
{
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    struct Vertex
    {
        vec3 pos;
        vec2 tex;
        vec3 normal;
    };

    vector<Vertex> vertices;
    for (size_t i = 0; i < positions.size(); i++)
    {
        Vertex v;
        v.pos = positions[i];
        v.tex = texCoords[i];
        v.normal = vec3(0.0f, 0.0f, 1.0f);
        vertices.push_back(v);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, tex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return VAO;
}

// Função para carregar o arquivo OBJ e MTL
bool loadOBJ(const string &objPath, const string &mtlPath, string &textureFileOut)
{
    ifstream objFile(objPath);
    if (!objFile.is_open())
    {
        cout << "Não foi possível abrir OBJ: " << objPath << endl;
        return false;
    }

    vector<vec3> tempPositions;
    vector<vec2> tempTexCoords;

    string line;
    while (getline(objFile, line))
    {
        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            tempPositions.push_back(pos);
        }
        else if (prefix == "vt")
        {
            vec2 tex;
            iss >> tex.x >> tex.y;
            tempTexCoords.push_back(tex);
        }
        else if (prefix == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                string v;
                iss >> v;

                size_t pos1 = v.find('/');
                size_t pos2 = v.find('/', pos1 + 1);

                int vi = stoi(v.substr(0, pos1)) - 1;
                int ti = stoi(v.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;

                positions.push_back(tempPositions[vi]);
                texCoords.push_back(tempTexCoords[ti]);
            }
        }
    }

    ifstream mtlFile(mtlPath);
    if (!mtlFile.is_open())
    {
        cout << "Não foi possível abrir MTL: " << mtlPath << endl;
        return false;
    }

    while (getline(mtlFile, line))
    {
        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "map_Kd")
        {
            string texFile;
            iss >> texFile;

            string mtlDir = mtlPath.substr(0, mtlPath.find_last_of("/\\"));
            textureFileOut = mtlDir + "/" + texFile;
            break;
        }
    }

    return true;
}

// Função para carregar a textura
GLuint loadTexture(const string &filePath)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        cout << "Falha ao carregar imagem: " << filePath << endl;
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texID;
}

//  Função para desenhar o Flamingo
void drawFlamingo(GLuint shaderProgram, GLuint VAO, vec3 position, vec3 scaleVec, vec3 rotation)
{
    mat4 model = translate(mat4(1.0f), position);
    model = rotate(model, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    model = rotate(model, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    model = rotate(model, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scaleVec);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, positions.size());
    glBindVertexArray(0);
}

// Callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    const float rotateSpeed = 2.0f;
    const float zoomSpeed = 0.2f;
    const float rollSpeed = 2.0f;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);

        // Rotação orbital
        if (key == GLFW_KEY_LEFT)
            camera.yaw -= rotateSpeed;
        if (key == GLFW_KEY_RIGHT)
            camera.yaw += rotateSpeed;
        if (key == GLFW_KEY_UP)
            camera.pitch += rotateSpeed;
        if (key == GLFW_KEY_DOWN)
            camera.pitch -= rotateSpeed;

        if (camera.pitch > 89.0f) camera.pitch = 89.0f;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;

        // Zoom in/out
        if (key == GLFW_KEY_W)
            camera.radius -= zoomSpeed;
        if (key == GLFW_KEY_S)
            camera.radius += zoomSpeed;
        if (camera.radius < 1.0f) camera.radius = 1.0f;

        // Roll 
        if (key == GLFW_KEY_Q)
            camera.roll -= rollSpeed;
        if (key == GLFW_KEY_E)
            camera.roll += rollSpeed;
    }
}