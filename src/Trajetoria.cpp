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

// Limites da tela
const float SCREEN_BOUND_X = 8.0f;  // Limites horizontais da tela
const float SCREEN_BOUND_Y = 6.0f;  // Limites verticais da tela 
const float SCREEN_BOUND_Z_NEAR = -10.0f;  // Limite de profundidade (mais próximo)
const float SCREEN_BOUND_Z_FAR = -1.0f;    // Limite de profundidade (mais distante)

// Função para manter uma posição dentro dos limites da tela
vec3 keepInBounds(const vec3& position) {
    vec3 result = position;
    
    // Limita as coordenadas X
    if (result.x < -SCREEN_BOUND_X) result.x = -SCREEN_BOUND_X;
    if (result.x > SCREEN_BOUND_X) result.x = SCREEN_BOUND_X;
    
    // Limita as coordenadas Y
    if (result.y < -SCREEN_BOUND_Y) result.y = -SCREEN_BOUND_Y;
    if (result.y > SCREEN_BOUND_Y) result.y = SCREEN_BOUND_Y;
    
    // Limita as coordenadas Z
    if (result.z < SCREEN_BOUND_Z_NEAR) result.z = SCREEN_BOUND_Z_NEAR;
    if (result.z > SCREEN_BOUND_Z_FAR) result.z = SCREEN_BOUND_Z_FAR;
    
    return result;
}

// Flamingo 1
vector<vec3> trajectoryPoints1 = {
    vec3(0.0f, 0.0f, -5.0f),
    vec3(4.0f, 0.0f, -5.0f),
    vec3(4.0f, 4.0f, -5.0f),
    vec3(0.0f, 4.0f, -5.0f),
    vec3(-4.0f, 4.0f, -5.0f),
    vec3(-4.0f, 0.0f, -5.0f),
    vec3(0.0f, 0.0f, -5.0f)};
size_t currentTargetIndex1 = 0;
vec3 flamingoPosition1 = vec3(0.0f, 0.0f, -5.0f);

// Flamingo 2
vector<vec3> trajectoryPoints2 = {
    vec3(4.0f, 0.0f, -5.0f),
    vec3(4.0f, 4.0f, -5.0f),
    vec3(0.0f, 4.0f, -5.0f),
    vec3(-4.0f, 4.0f, -5.0f),
    vec3(-4.0f, 0.0f, -5.0f),
    vec3(0.0f, 0.0f, -5.0f),
    vec3(4.0f, 0.0f, -5.0f)};
size_t currentTargetIndex2 = 0;
vec3 flamingoPosition2 = vec3(4.0f, 0.0f, -5.0f);

// Flamingo 3
vector<vec3> trajectoryPoints3 = {
    vec3(-4.0f, 0.0f, -5.0f),
    vec3(-4.0f, -4.0f, -5.0f),
    vec3(0.0f, -4.0f, -5.0f),
    vec3(4.0f, -4.0f, -5.0f),
    vec3(4.0f, 0.0f, -5.0f),
    vec3(-4.0f, 0.0f, -5.0f)};
size_t currentTargetIndex3 = 0;
vec3 flamingoPosition3 = vec3(-4.0f, 0.0f, -5.0f);

// Velocidade global
float moveSpeed = 1.5f;

// Classe Camera
class Camera
{
public:
    vec3 position;
    float yaw;	 // ângulo de rotação em torno de Y
    float pitch; // ângulo de rotação em torno de X

    // Construtor da câmera
    Camera(vec3 startPosition = vec3(0.0f, 0.0f, 3.0f), float startYaw = -90.0f, float startPitch = 0.0f)
        : position(startPosition), yaw(startYaw), pitch(startPitch) {}

    mat4 getViewMatrix()
    {
        vec3 front;
        front.x = cos(radians(yaw)) * cos(radians(pitch));
        front.y = sin(radians(pitch));
        front.z = sin(radians(yaw)) * cos(radians(pitch));
        front = normalize(front);

        vec3 right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
        vec3 up = normalize(cross(right, front));

        return lookAt(position, position + front, up);
    }

    // Métodos de movimentação da câmera
    void moveForward(float delta)
    {
        vec3 front = getFrontVector();
        position += delta * front;
    }

    void moveRight(float delta)
    {
        vec3 right = getRightVector();
        position += delta * right;
    }

    void moveUp(float delta)
    {
        position.y += delta;
    }

    void rotate(float deltaYaw, float deltaPitch)
    {
        yaw += deltaYaw;
        pitch += deltaPitch;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

private:
    vec3 getFrontVector()
    {
        vec3 front;
        front.x = cos(radians(yaw)) * cos(radians(pitch));
        front.y = sin(radians(pitch));
        front.z = sin(radians(yaw)) * cos(radians(pitch));
        return normalize(front);
    }

    vec3 getRightVector()
    {
        vec3 front = getFrontVector();
        vec3 right = normalize(cross(front, vec3(0.0f, 1.0f, 0.0f)));
        return right;
    }
};

//  Variáveis da câmera
Camera camera;
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

uniform sampler2D texture1;
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

    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(phong, 1.0) * texColor;
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
    std::string texturePath = "../assets/tex/pink.jpg";  

    std::string textureFile;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "M6 - Trajetoria de objetos", nullptr, nullptr);
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

    if (!loadOBJ(objPath, mtlPath, textureFile))
    {
        cout << "Erro ao carregar Flamingo.obj" << endl;
        return -1;
    }

    textureID = loadTexture(texturePath);
    if (textureID == 0)
    {
        cout << "Erro ao carregar textura: " << texturePath << endl;
        return -1;
    }

    GLuint VAO = setupGeometry();

    mat4 projection = perspective(radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    vec3 lightPos = vec3(3.0f, 3.0f, 3.0f);
    vec3 lightColor = vec3(1.0f);
    vec3 objectColor = vec3(1.0f);
    float lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));

        mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));

        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, value_ptr(camera.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, value_ptr(objectColor));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        // Atualiza posição do Flamingo 1
        vec3 targetPos1 = trajectoryPoints1[currentTargetIndex1];
        vec3 direction1 = normalize(targetPos1 - flamingoPosition1);
        float distance1 = length(targetPos1 - flamingoPosition1);

        if (distance1 < 0.05f)
        {
            currentTargetIndex1 = (currentTargetIndex1 + 1) % trajectoryPoints1.size();
        }
        else
        {
            flamingoPosition1 += direction1 * moveSpeed * deltaTime;
            flamingoPosition1 = keepInBounds(flamingoPosition1);
        }

        // Atualiza posição do Flamingo 2
        vec3 targetPos2 = trajectoryPoints2[currentTargetIndex2];
        vec3 direction2 = normalize(targetPos2 - flamingoPosition2);
        float distance2 = length(targetPos2 - flamingoPosition2);

        if (distance2 < 0.05f)
        {
            currentTargetIndex2 = (currentTargetIndex2 + 1) % trajectoryPoints2.size();
        }
        else
        {
            flamingoPosition2 += direction2 * moveSpeed * deltaTime;
            flamingoPosition2 = keepInBounds(flamingoPosition2);
        }

        // Atualiza posição do Flamingo 3
        vec3 targetPos3 = trajectoryPoints3[currentTargetIndex3];
        vec3 direction3 = normalize(targetPos3 - flamingoPosition3);
        float distance3 = length(targetPos3 - flamingoPosition3);

        if (distance3 < 0.05f)
        {
            currentTargetIndex3 = (currentTargetIndex3 + 1) % trajectoryPoints3.size();
        }
        else
        {
            flamingoPosition3 += direction3 * moveSpeed * deltaTime;
            flamingoPosition3 = keepInBounds(flamingoPosition3);
        }

        // Desenha os 3 flamingos
        drawFlamingo(shaderProgram, VAO, flamingoPosition1, flamingoScale, vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ));
        drawFlamingo(shaderProgram, VAO, flamingoPosition2, flamingoScale, vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ));
        drawFlamingo(shaderProgram, VAO, flamingoPosition3, flamingoScale, vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ));

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
            tex.y = 1.0f - tex.y;
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

    cout << "Objeto carregado com " << positions.size() << " vertices.\n";
    cout << "Textura: " << textureFileOut << endl;
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

// Funções para carregar e salvar pontos de trajetória
void loadTrajectoryPoints(vector<vec3> &points, const string &filename)
{
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cout << "Erro ao abrir arquivo: " << filename << endl;
        return;
    }

    points.clear();
    vec3 p;
    while (inFile >> p.x >> p.y >> p.z)
    {
        // Garante que os pontos carregados estão dentro dos limites
        p = keepInBounds(p);
        points.push_back(p);
    }

    inFile.close();
    cout << "Trajetoria carregada de " << filename << " com " << points.size() << " pontos." << endl;
}

// Função para salvar pontos de trajetória em um arquivo
void saveTrajectoryPoints(const vector<vec3> &points, const string &filename)
{
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cout << "Erro ao salvar arquivo: " << filename << endl;
        return;
    }

    for (const auto &p : points)
    {
        outFile << p.x << " " << p.y << " " << p.z << "\n";
    }

    outFile.close();
    cout << "Trajetoria armazenada em " << filename << endl;
}

// Callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    const float moveSpeed = 0.1f;
    const float rotateSpeed = 2.0f;
    static int currentFlamingo = 1; // Teclas 1, 2 ou 3 para selecionar o flamingo

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);

        // Movimento da camera
        if (key == GLFW_KEY_W)
            camera.moveForward(moveSpeed);
        if (key == GLFW_KEY_S)
            camera.moveForward(-moveSpeed);
        if (key == GLFW_KEY_A)
            camera.moveRight(-moveSpeed);
        if (key == GLFW_KEY_D)
            camera.moveRight(moveSpeed);
        if (key == GLFW_KEY_Q)
            camera.moveUp(moveSpeed);
        if (key == GLFW_KEY_E)
            camera.moveUp(-moveSpeed);

        // Rotação da camera
        if (key == GLFW_KEY_LEFT)
            camera.rotate(-rotateSpeed, 0.0f);
        if (key == GLFW_KEY_RIGHT)
            camera.rotate(rotateSpeed, 0.0f);
        if (key == GLFW_KEY_UP)
            camera.rotate(0.0f, rotateSpeed);
        if (key == GLFW_KEY_DOWN)
            camera.rotate(0.0f, -rotateSpeed);
        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
                currentFlamingo = 1;
            else if (key == GLFW_KEY_2)
                currentFlamingo = 2;
            else if (key == GLFW_KEY_3)
                currentFlamingo = 3;

            // Caminho base
            string basePath = "../M6/"; 

            if (key == GLFW_KEY_F) // "Tecla F" salva a trajetória
            {
                string filename = basePath + "trajetoriaFlamingo" + to_string(currentFlamingo) + ".txt";

                if (currentFlamingo == 1)
                    saveTrajectoryPoints(trajectoryPoints1, filename);
                else if (currentFlamingo == 2)
                    saveTrajectoryPoints(trajectoryPoints2, filename);
                else if (currentFlamingo == 3)
                    saveTrajectoryPoints(trajectoryPoints3, filename);
            }
            else if (key == GLFW_KEY_L) // "Tecla L" carregar a trajetória
            {
                string filename = basePath + "trajetoriaFlamingo" + to_string(currentFlamingo) + ".txt";

                if (currentFlamingo == 1)
                    loadTrajectoryPoints(trajectoryPoints1, filename);
                else if (currentFlamingo == 2)
                    loadTrajectoryPoints(trajectoryPoints2, filename);
                else if (currentFlamingo == 3)
                    loadTrajectoryPoints(trajectoryPoints3, filename);

                //  Reinicializa o índice do alvo para o primeiro ponto da trajetória
                if (currentFlamingo == 1)
                    currentTargetIndex1 = 0;
                else if (currentFlamingo == 2)
                    currentTargetIndex2 = 0;
                else if (currentFlamingo == 3)
                    currentTargetIndex3 = 0;
            }
        }
    }
}