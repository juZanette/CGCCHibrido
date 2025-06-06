/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 13/08/2024
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <algorithm>

using namespace glm;

// Estrutura para armazenar um vértice completo
struct Vertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

// Função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
GLuint setupShader();
GLuint loadTexture(string filePath, int &width, int &height);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, std::string& outTextureFile);
GLuint setupModelBuffers(const std::vector<Vertex>& vertices);

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Vertex Shader
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texc;
uniform mat4 projection;
uniform mat4 model;
out vec2 texCoord;
void main()
{
    gl_Position = projection * model * vec4(position, 1.0);
    texCoord = texc;
})";

// Fragment Shader
const GLchar *fragmentShaderSource = R"(
#version 400
in vec2 texCoord;
uniform sampler2D texBuff;
out vec4 color;
void main()
{
    color = texture(texBuff, texCoord);
})";

int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Criação da janela GLFW
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Suzanne Texturizada", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Callback de teclado
    glfwSetKeyCallback(window, key_callback);

    // GLAD: carrega ponteiros de funções OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Habilita depth test e modo preenchido
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Informações de versão
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilar shaders
    GLuint shaderID = setupShader();

    // Carregar modelo OBJ e textura do MTL
    std::vector<Vertex> modelVertices;
    std::string textureFile;
    if (!loadOBJ("../assets/Modelos3D/Suzanne.obj", modelVertices, textureFile)) {
        std::cout << "Erro ao carregar OBJ!" << std::endl;
        return -1;
    }

    // Montar buffers do modelo
    GLuint modelVAO = setupModelBuffers(modelVertices);

    // Carregar textura do arquivo referenciado no MTL
    int imgWidth, imgHeight;
    GLuint texID = loadTexture("../assets/Modelos3D/" + textureFile, imgWidth, imgHeight);

    glUseProgram(shaderID);

    // Uniforms
    glUniform1i(glGetUniformLocation(shaderID, "texBuff"), 0);
    glActiveTexture(GL_TEXTURE0);

    // Matriz de projeção ortográfica
    mat4 projection = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -10.0f, 10.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(modelVAO);
        glBindTexture(GL_TEXTURE_2D, texID);

        // Matriz de modelo para centralizar e escalar a Suzanne
        mat4 model = mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size());

        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &modelVAO);
    glfwTerminate();
    return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Compila shaders
GLuint setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Carrega textura usando stb_image
GLuint loadTexture(string filePath, int &width, int &height)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int nrChannels;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture " << filePath << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

// Carrega OBJ e MTL (apenas o necessário para textura)
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, std::string& outTextureFile)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::string mtlFile;

    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string dir = path.substr(0, path.find_last_of("/\\") + 1);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        } else if (prefix == "vt") {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            tex.y = 1.0f - tex.y; // Inverte o eixo Y para OpenGL
            texcoords.push_back(tex);
        } else if (prefix == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            normals.push_back(norm);
        } else if (prefix == "f") {
            // Lê todos os vértices da face
            std::vector<std::string> faceVerts;
            std::string vertStr;
            while (iss >> vertStr) {
                faceVerts.push_back(vertStr);
            }
            // Triangula a face (fan triangulation)
            for (size_t i = 1; i + 1 < faceVerts.size(); ++i) {
                int idx[3][3] = {0};
                std::string vStrs[3] = {faceVerts[0], faceVerts[i], faceVerts[i+1]};
                for (int k = 0; k < 3; ++k) {
                    sscanf(vStrs[k].c_str(), "%d/%d/%d", &idx[k][0], &idx[k][1], &idx[k][2]);
                }
                for (int k = 0; k < 3; ++k) {
                    Vertex vert;
                    vert.position = positions[idx[k][0] - 1];
                    vert.texcoord = texcoords[idx[k][1] - 1];
                    vert.normal   = normals[idx[k][2] - 1];
                    outVertices.push_back(vert);
                }
            }
        } else if (prefix == "mtllib") {
            iss >> mtlFile;
        }
    }
    file.close();

    // Carregar nome da textura do MTL
    if (!mtlFile.empty()) {
        std::ifstream mtl(dir + mtlFile);
        if (mtl.is_open()) {
            while (std::getline(mtl, line)) {
                std::istringstream miss(line);
                std::string mtlPrefix;
                miss >> mtlPrefix;
                if (mtlPrefix == "map_Kd") {
                    miss >> outTextureFile;
                    break;
                }
            }
            mtl.close();
        }
    }
    return !outVertices.empty() && !outTextureFile.empty();
}

// Cria VAO/VBO do modelo
GLuint setupModelBuffers(const std::vector<Vertex>& vertices)
{
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    // Texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}