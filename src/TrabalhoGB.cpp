/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Esta aplicação demonstra uma simulação 3D do espaço usando OpenGL, com:
 * - Objetos 3D carregados de arquivos OBJ (lua, Marte e Flamingo)
 * - Iluminação de Phong com materiais e texturas
 * - Transformações interativas (translação, rotação, escala)
 * - Movimentação de câmera e animação de órbita
 * - Carregamento de configurações de cena a partir de arquivo txt
 *
 * Autora: Júlia Faccio Zanette
 * Data: 06/2025
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

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
const GLuint WIDTH = 1200, HEIGHT = 800;
GLFWwindow *window;

// Variáveis de shader
vector<vec3> positions;
vector<vec2> texCoords;

// --- Moon ---
GLuint moonVAO;
GLuint moonTextureID;
size_t moonVertexCount = 0;

// --- Mars ---
GLuint marsVAO;
GLuint marsTextureID;
size_t marsVertexCount = 0;

// --- Flamingo ---
GLuint flamingoVAO;
GLuint flamingoBodyTextureID;
GLuint flamingoEyeTextureID;
size_t flamingoVertexCount = 0;
vec3 flamingoPosition = vec3(0.0f, 0.0f, -5.0f);
vec3 flamingoScale = vec3(0.2f, 0.2f, 0.2f);
float flamingoRotationX = 0.0f;
float flamingoRotationY = 0.0f;
float flamingoRotationZ = 0.0f;
float flamingoOrbitRadius = 4.0f;
float flamingoOrbitSpeed = 0.3f;
float flamingoOrbitAngle = 0.0f;

GLuint textureID;

// --- Moon ---
vec3 moonPosition = vec3(0.0f, 0.0f, -5.0f);
vec3 moonScale = vec3(0.5f, 0.5f, 0.5f);
float moonRotationX = 0.0f;
float moonRotationY = 0.0f;
float moonRotationZ = 0.0f;

// --- Mars ---
vec3 marsPosition = vec3(0.0f, 0.0f, -5.0f);
vec3 marsScale = vec3(0.5f, 0.5f, 0.5f);
float marsRotationX = 0.0f;
float marsRotationY = 0.0f;
float marsRotationZ = 0.0f;

// Velocidade global
float moveSpeed = 1.5f;

// Variáveis para controle de seleção e transformação
int selectedObject = 0; // 0 = Moon, 1 = Mars, 2 = Flamingo
bool rotateX = false, rotateY = false, rotateZ = false;
bool isMovingForward = false, isMovingBackward = false;
bool isMovingLeft = false, isMovingRight = false;
bool isMovingUp = false, isMovingDown = false;
bool isScalingUp = false, isScalingDown = false;
const float translationSpeed = 1.5f;
const float rotationSpeed = 25.0f;
const float scalingSpeed = 1.0f;

// Estrutura para material
struct Material {
    vec3 ka; // Coeficiente ambiente
    vec3 kd; // Coeficiente difuso
    vec3 ks; // Coeficiente especular
    float shininess; // Brilho da especular
};

// Estrutura para configuração de objetos
struct ObjectConfig {
    string objFile;
    string mtlFile;
    string textureFile;      // Para objetos com uma textura
    string textureBodyFile;  // Para o flamingo - corpo
    string textureEyeFile;   // Para o flamingo - olhos
    vec3 position;
    vec3 rotation;
    vec3 scale;
    string animation;
    Material material;
    
    // Parâmetros de órbita (se animation == "orbit")
    string orbitTarget;
    float orbitRadius;
    float orbitSpeed;


};

// Mapa de configurações de objetos
map<string, ObjectConfig> objectConfigs;

// Estrutura para configuração da câmera
struct CameraConfig {
    vec3 position;
    float yaw;
    float pitch;
    float fov;
    float nearPlane;
    float farPlane;
};

// Estrutura para configuração da luz
struct LightConfig {
    vec3 position;
    vec3 color;
};

// Variável de cor do objeto
vec3 objectColor = vec3(1.0f, 1.0f, 1.0f); // Cor branca padrão

// Variáveis de configuração
CameraConfig cameraConfig;
LightConfig lightConfig;

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
uniform int useEyeTexture; // Novo parâmetro para distinguir texturas

// Coeficientes de material
uniform vec3 Ka; // Coeficiente ambiente
uniform vec3 Kd; // Coeficiente difuso
uniform vec3 Ks; // Coeficiente especular
uniform float shininess; // Brilho da especular

void main()
{
    // Componente ambiente
    vec3 ambient = Ka * lightColor;

    // Componente difusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Kd * diff * lightColor;

    // Componente especular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = Ks * spec * lightColor;

    // Iluminação final de Phong
    vec3 phong = ambient + diffuse + specular;

    // Combinar com a textura
    vec4 texColor = texture(texture1, TexCoord);
    
    // Se for a textura do olho e o pixel for transparente, descartá-lo
    if (useEyeTexture == 1 && texColor.a < 0.1) {
        discard;
    }
    
    FragColor = vec4(phong, 1.0) * texColor;
}
)";

const char* bgVertexShaderSource = R"(
#version 400 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* bgFragmentShaderSource = R"(
#version 400 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D background;
void main() {
    FragColor = texture(background, TexCoord);
}
)";

// Funções auxiliares - protótipos
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
GLuint setupShader();
GLuint setupGeometry();
GLuint setupGeometry(const vector<vec3>& positions, const vector<vec2>& texCoords);
GLuint setupGeometry(const vector<vec3>& positions, const vector<vec2>& texCoords, const vector<vec3>& normals);
GLuint loadTexture(const string &filePath);
bool loadOBJ(const string &objPath, const string &mtlPath, string &textureFileOut,
             vector<vec3>& outPositions, vector<vec2>& outTexCoords);
bool loadOBJ(const string &objPath, const string &mtlPath, string &textureFileOut,
             vector<vec3>& outPositions, vector<vec2>& outTexCoords, vector<vec3>& outNormals,
             Material& outMaterial);
void drawObject(GLuint shaderProgram, GLuint VAO, size_t vertexCount, vec3 position, vec3 scaleVec, vec3 rotation, Material material);
bool loadSceneConfig(const string &configFile);
vec3 keepInBounds(const vec3& position);
void loadTrajectoryPoints(vector<vec3> &points, const string &filename);
void saveTrajectoryPoints(const vector<vec3> &points, const string &filename);

// Função MAIN
int main()
{
    // Inicialização do GLFW, janela, etc.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "GB", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Carregar configuração da cena
    if (!loadSceneConfig("../src/scene_init.txt")) {
        cout << "ERRO CRÍTICO: Arquivo de configuração scene_init.txt não encontrado." << endl;
        cout << "O programa não pode continuar sem o arquivo de configuração." << endl;
        glfwTerminate();
        return -1;
    }

    // Aplicar configurações
    
    // Configurar câmera
    camera = Camera(cameraConfig.position, cameraConfig.yaw, cameraConfig.pitch);
    
    // Configurar projeção
    mat4 projection = perspective(radians(cameraConfig.fov), 
                                 (float)WIDTH / (float)HEIGHT, 
                                 cameraConfig.nearPlane, 
                                 cameraConfig.farPlane);
    
    // Configurar luz
    vec3 lightPos = lightConfig.position;
    vec3 lightColor = lightConfig.color;
    
    // Carregar objetos da configuração
    GLuint shaderProgram = setupShader();
    
    // --- MOON ---
    auto& moonCfg = objectConfigs["moon"];
    vector<vec3> moonPositions;
    vector<vec2> moonTexCoords;
    positions.clear();
    texCoords.clear();
    string moonTextureFile;
    vector<vec3> moonNormals;
    Material moonMaterial;
    if (!loadOBJ(moonCfg.objFile, moonCfg.mtlFile, moonTextureFile, moonPositions, moonTexCoords, moonNormals, moonMaterial)) {
        cout << "Erro ao carregar " << moonCfg.objFile << endl;
        return -1;
    }
    moonTextureID = loadTexture(moonCfg.textureFile);
    if (moonTextureID == 0) {
        cout << "Erro ao carregar textura: " << moonCfg.textureFile << endl;
        return -1;
    }
    moonVAO = setupGeometry(moonPositions, moonTexCoords, moonNormals);
    moonVertexCount = moonPositions.size();
    
    // Aplicar configurações da lua
    moonPosition = moonCfg.position;
    moonScale = moonCfg.scale;
    moonRotationX = moonCfg.rotation.x;
    moonRotationY = moonCfg.rotation.y;
    moonRotationZ = moonCfg.rotation.z;
    objectConfigs["moon"].material = moonMaterial;
    
    // --- MARS ---
    auto& marsCfg = objectConfigs["mars"];
    vector<vec3> marsPositions;
    vector<vec2> marsTexCoords;
    positions.clear();
    texCoords.clear();
    string marsTextureFile;
    vector<vec3> marsNormals;
    Material marsMaterial;
    if (!loadOBJ(marsCfg.objFile, marsCfg.mtlFile, marsTextureFile, marsPositions, marsTexCoords, marsNormals, marsMaterial)) {
        cout << "Erro ao carregar " << marsCfg.objFile << endl;
        return -1;
    }
    marsTextureID = loadTexture(marsCfg.textureFile);
    if (marsTextureID == 0) {
        cout << "Erro ao carregar textura: " << marsCfg.textureFile << endl;
        return -1;
    }
    marsVAO = setupGeometry(marsPositions, marsTexCoords, marsNormals);
    marsVertexCount = marsPositions.size();
    
    // Aplicar configurações de Marte
    marsPosition = marsCfg.position;
    marsScale = marsCfg.scale;
    marsRotationX = marsCfg.rotation.x;
    marsRotationY = marsCfg.rotation.y;
    marsRotationZ = marsCfg.rotation.z;
    objectConfigs["mars"].material = marsMaterial;
    
    // --- FLAMINGO ---
    auto& flamingoCfg = objectConfigs["flamingo"];
    vector<vec3> flamingoPositions;
    vector<vec2> flamingoTexCoords;
    positions.clear();
    texCoords.clear();
    string flamingoTextureFile;
    vector<vec3> flamingoNormals;
    Material flamingoMaterial;
    if (!loadOBJ(flamingoCfg.objFile, flamingoCfg.mtlFile, flamingoTextureFile, flamingoPositions, flamingoTexCoords, flamingoNormals, flamingoMaterial)) {
        cout << "Erro ao carregar " << flamingoCfg.objFile << endl;
        return -1;
    }

    // Carregar texturas do flamingo
    flamingoBodyTextureID = loadTexture(flamingoCfg.textureBodyFile);
    flamingoEyeTextureID = loadTexture(flamingoCfg.textureEyeFile);
    if (flamingoBodyTextureID == 0 || flamingoEyeTextureID == 0) {
        cout << "Erro ao carregar texturas do flamingo." << endl;
        return -1;
    }

    flamingoVAO = setupGeometry(flamingoPositions, flamingoTexCoords, flamingoNormals);
    flamingoVertexCount = flamingoPositions.size();
    
    // Aplicar configurações do flamingo
    flamingoPosition = flamingoCfg.position;
    flamingoScale = flamingoCfg.scale;
    flamingoRotationX = flamingoCfg.rotation.x;
    flamingoRotationY = flamingoCfg.rotation.y;
    flamingoRotationZ = flamingoCfg.rotation.z;
    objectConfigs["flamingo"].material = flamingoMaterial;
    
    // Continuar com o resto do código...
    float lastFrameTime = glfwGetTime();

    GLuint bgShaderProgram;
    {
        GLuint bgVertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(bgVertex, 1, &bgVertexShaderSource, nullptr);
        glCompileShader(bgVertex);

        GLuint bgFragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(bgFragment, 1, &bgFragmentShaderSource, nullptr);
        glCompileShader(bgFragment);

        bgShaderProgram = glCreateProgram();
        glAttachShader(bgShaderProgram, bgVertex);
        glAttachShader(bgShaderProgram, bgFragment);
        glLinkProgram(bgShaderProgram);

        glDeleteShader(bgVertex);
        glDeleteShader(bgFragment);
    }

    GLuint backgroundTexID = loadTexture("../assets/tex/fundo-estrelas.jpg");
    if (backgroundTexID == 0) {
        cout << "Erro ao carregar textura de fundo." << endl;
        return -1;
    }

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    GLuint bgVAO, bgVBO;
    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Instruções de uso
    cout << "\n=== CONTROLES DE SELECAO E TRANSFORMACAO ===" << endl;
    cout << "1, 2, 3: Selecionar Lua, Marte ou Flamingo" << endl;
    cout << "X, Y, Z: Ativar rotacao no eixo correspondente" << endl;
    cout << "R: Desativar rotacao" << endl;
    cout << "WASD: Mover objeto no plano XZ" << endl;
    cout << "I/K: Mover objeto para cima/baixo" << endl;
    cout << "+ / -: Aumentar/diminuir escala" << endl; 
    cout << "Setas: Rotacionar camera" << endl;
    cout << "Q/E: Mover camera para cima/baixo" << endl;
    cout << "================================================\n" << endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        // Aplicar transformações ao objeto selecionado
        if (selectedObject == 0) {  // Moon
            // Translação
            if (isMovingForward) moonPosition.z -= translationSpeed * deltaTime;
            if (isMovingBackward) moonPosition.z += translationSpeed * deltaTime;
            if (isMovingLeft) moonPosition.x -= translationSpeed * deltaTime;
            if (isMovingRight) moonPosition.x += translationSpeed * deltaTime;
            if (isMovingUp) moonPosition.y += translationSpeed * deltaTime;
            if (isMovingDown) moonPosition.y -= translationSpeed * deltaTime;
            
            // Escala
            if (isScalingUp) moonScale *= (1.0f + scalingSpeed * deltaTime);
            if (isScalingDown) moonScale *= (1.0f - scalingSpeed * deltaTime);
            
            // Rotação
            if (rotateX) moonRotationX += rotationSpeed * deltaTime;
            if (rotateY) moonRotationY += rotationSpeed * deltaTime;
            if (rotateZ) moonRotationZ += rotationSpeed * deltaTime;
        }
        else if (selectedObject == 1) {  // Mars
            // Translação
            if (isMovingForward) marsPosition.z -= translationSpeed * deltaTime;
            if (isMovingBackward) marsPosition.z += translationSpeed * deltaTime;
            if (isMovingLeft) marsPosition.x -= translationSpeed * deltaTime;
            if (isMovingRight) marsPosition.x += translationSpeed * deltaTime;
            if (isMovingUp) marsPosition.y += translationSpeed * deltaTime;
            if (isMovingDown) marsPosition.y -= translationSpeed * deltaTime;
            
            // Escala
            if (isScalingUp) marsScale *= (1.0f + scalingSpeed * deltaTime);
            if (isScalingDown) marsScale *= (1.0f - scalingSpeed * deltaTime);
            
            // Rotação
            if (rotateX) marsRotationX += rotationSpeed * deltaTime;
            if (rotateY) marsRotationY += rotationSpeed * deltaTime;
            if (rotateZ) marsRotationZ += rotationSpeed * deltaTime;
        }
        else if (selectedObject == 2) {  // Flamingo
            // Translação
            if (isMovingForward) flamingoPosition.z -= translationSpeed * deltaTime;
            if (isMovingBackward) flamingoPosition.z += translationSpeed * deltaTime;
            if (isMovingLeft) flamingoPosition.x -= translationSpeed * deltaTime;
            if (isMovingRight) flamingoPosition.x += translationSpeed * deltaTime;
            if (isMovingUp) flamingoPosition.y += translationSpeed * deltaTime;
            if (isMovingDown) flamingoPosition.y -= translationSpeed * deltaTime;
            
            // Escala
            if (isScalingUp) flamingoScale *= (1.0f + scalingSpeed * deltaTime);
            if (isScalingDown) flamingoScale *= (1.0f - scalingSpeed * deltaTime);
            
            // Rotação
            if (rotateX) flamingoRotationX += rotationSpeed * deltaTime;
            if (rotateY) flamingoRotationY += rotationSpeed * deltaTime;
            if (rotateZ) flamingoRotationZ += rotationSpeed * deltaTime;
        }

        // Atualiza a órbita do flamingo apenas se não estiver selecionado
        if (selectedObject != 2 && objectConfigs["flamingo"].animation == "orbit") {
            flamingoOrbitAngle += flamingoOrbitSpeed * deltaTime;
            
            // Determinar o alvo da órbita
            vec3 targetPosition;
            if (objectConfigs["flamingo"].orbitTarget == "mars") {
                targetPosition = marsPosition;
            } else if (objectConfigs["flamingo"].orbitTarget == "moon") {
                targetPosition = moonPosition;
            } else {
                targetPosition = vec3(0.0f, 0.0f, -5.0f); // Posição padrão
            }
            
            flamingoPosition.x = targetPosition.x + flamingoOrbitRadius * cos(flamingoOrbitAngle);
            flamingoPosition.z = targetPosition.z + flamingoOrbitRadius * sin(flamingoOrbitAngle);
            
            // Ajustar rotação para o flamingo apontar para a direção do movimento
            flamingoRotationY = glm::degrees(flamingoOrbitAngle) + 90.0f;
        }

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Desenha o fundo
        glDepthMask(GL_FALSE);
        glUseProgram(bgShaderProgram);
        glBindVertexArray(bgVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexID);
        glUniform1i(glGetUniformLocation(bgShaderProgram, "background"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

        // Configuração do shader principal
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
        mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, value_ptr(camera.position));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, value_ptr(objectColor));

        // Desenhe a lua
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moonTextureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        drawObject(shaderProgram, moonVAO, moonVertexCount, moonPosition, moonScale, 
            vec3(moonRotationX, moonRotationY, moonRotationZ), objectConfigs["moon"].material);

        // Desenhe Marte
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, marsTextureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        drawObject(shaderProgram, marsVAO, marsVertexCount, marsPosition, marsScale, 
            vec3(marsRotationX, marsRotationY, marsRotationZ), objectConfigs["mars"].material);

        // Desenhe o flamingo
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flamingoBodyTextureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "useEyeTexture"), 0); // Parâmetro adicional para distinguir texturas
        drawObject(shaderProgram, flamingoVAO, flamingoVertexCount, flamingoPosition, flamingoScale, 
            vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ), objectConfigs["flamingo"].material);

        // Desenhe o olho do flamingo
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, flamingoEyeTextureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniform1i(glGetUniformLocation(shaderProgram, "useEyeTexture"), 1); // Parâmetro para usar a textura do olho
        drawObject(shaderProgram, flamingoVAO, flamingoVertexCount, flamingoPosition, flamingoScale, 
            vec3(flamingoRotationX, flamingoRotationY, flamingoRotationZ), objectConfigs["flamingo"].material);

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

//  Função para configurar a geometria
GLuint setupGeometry(const vector<vec3>& positions, const vector<vec2>& texCoords)
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

// Função para configurar a geometria com normais
GLuint setupGeometry(const vector<vec3>& positions, const vector<vec2>& texCoords, const vector<vec3>& normals)
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
        v.normal = normals[i]; // Usar a normal calculada
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

// Função para carregar a textura
GLuint loadTexture(const string &filePath)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4); // Forçar 4 canais
    GLenum format = GL_RGBA;
    if (!data)
    {
        cout << "Falha ao carregar imagem: " << filePath << endl;
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texID;
}
void drawObject(GLuint shaderProgram, GLuint VAO, size_t vertexCount, vec3 position, vec3 scaleVec, vec3 rotation, Material material)
{
    mat4 model = translate(mat4(1.0f), position);
    model = rotate(model, radians(rotation.x), vec3(1.0f, 0.0f, 0.0f));
    model = rotate(model, radians(rotation.y), vec3(0.0f, 1.0f, 0.0f));
    model = rotate(model, radians(rotation.z), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scaleVec);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
    
    // Passar os coeficientes de material para o shader
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ka"), 1, value_ptr(material.ka));
    glUniform3fv(glGetUniformLocation(shaderProgram, "Kd"), 1, value_ptr(material.kd));
    glUniform3fv(glGetUniformLocation(shaderProgram, "Ks"), 1, value_ptr(material.ks));
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), material.shininess);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

// Callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    const float moveSpeed = 0.1f;
    const float rotateSpeed = 2.0f;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);

        // Seleção de objetos
        if (key == GLFW_KEY_1) {
            selectedObject = 0; // Moon
            cout << "Lua selecionada" << endl;
        }
        if (key == GLFW_KEY_2) {
            selectedObject = 1; // Mars
            cout << "Marte selecionado" << endl;
        }
        if (key == GLFW_KEY_3) {
            selectedObject = 2; // Flamingo
            cout << "Flamingo selecionado" << endl;
        }

        // Rotação (X, Y, Z)
        if (key == GLFW_KEY_X) {
            rotateX = true; rotateY = false; rotateZ = false;
            cout << "Modo de rotacao em X ativado" << endl;
        }
        if (key == GLFW_KEY_Y) {
            rotateX = false; rotateY = true; rotateZ = false;
            cout << "Modo de rotacao em Y ativado" << endl;
        }
        if (key == GLFW_KEY_Z) {
            rotateX = false; rotateY = false; rotateZ = true;
            cout << "Modo de rotacao em Z ativado" << endl;
        }
        
        // Desativa rotação
        if (key == GLFW_KEY_R) {
            rotateX = false; rotateY = false; rotateZ = false;
            cout << "Rotacao desativada" << endl;
        }

        // Translação (WASD)
        if (key == GLFW_KEY_W) {
            isMovingForward = true;
        }
        if (key == GLFW_KEY_S) {
            isMovingBackward = true;
        }
        if (key == GLFW_KEY_A) {
            isMovingLeft = true;
        }
        if (key == GLFW_KEY_D) {
            isMovingRight = true;
        }

        // Translação Y (I/K)
        if (key == GLFW_KEY_I) {
            isMovingUp = true;
        }
        if (key == GLFW_KEY_K) {
            isMovingDown = true;
        }

        // Escala (+ e -)
        if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
            isScalingDown = true;
            cout << "Diminuindo escala" << endl;
        }
        if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) { 
            isScalingUp = true;
            cout << "Aumentando escala" << endl;
        }

        // Movimento da camera (com teclas de seta)
        if (key == GLFW_KEY_LEFT)
            camera.rotate(-rotateSpeed, 0.0f);
        if (key == GLFW_KEY_RIGHT)
            camera.rotate(rotateSpeed, 0.0f);
        if (key == GLFW_KEY_UP)
            camera.rotate(0.0f, rotateSpeed);
        if (key == GLFW_KEY_DOWN)
            camera.rotate(0.0f, -rotateSpeed);
        
        // Movimento da câmera (com Q/E)
        if (key == GLFW_KEY_Q)
            camera.moveUp(moveSpeed);
        if (key == GLFW_KEY_E)
            camera.moveUp(-moveSpeed);
    }
    else if (action == GLFW_RELEASE) {
        // Liberar teclas de movimento
        if (key == GLFW_KEY_W) isMovingForward = false;
        if (key == GLFW_KEY_S) isMovingBackward = false;
        if (key == GLFW_KEY_A) isMovingLeft = false;
        if (key == GLFW_KEY_D) isMovingRight = false;
        if (key == GLFW_KEY_I) isMovingUp = false;
        if (key == GLFW_KEY_K) isMovingDown = false;
        
        // Novas teclas para escala
        if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) isScalingDown = false;
        if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) isScalingUp = false;
    }
}

// Função para carregar o arquivo de configuração
bool loadSceneConfig(const string &configFile) {
    ifstream file(configFile);
    if (!file.is_open()) {
        cout << "Erro ao abrir arquivo de configuração: " << configFile << endl;
        return false;
    }
    
    string line;
    string currentObject = "";
    
    while (getline(file, line)) {
        // Ignora linhas vazias e comentários
        if (line.empty() || line[0] == '#') continue;
        
        // Remove espaços em branco no início e fim
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Divide a linha em chave e valor
        size_t separatorPos = line.find('=');
        if (separatorPos == string::npos) continue;
        
        string key = line.substr(0, separatorPos);
        string value = line.substr(separatorPos + 1);
        
        // Remove espaços em branco
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Processa valores baseado na chave
        if (key.substr(0, 7) == "object.") {
            // Extrai o nome do objeto e a propriedade
            string objKey = key.substr(7);
            size_t dotPos = objKey.find('.');
            
            if (dotPos != string::npos) {
                string objName = objKey.substr(0, dotPos);
                string objProp = objKey.substr(dotPos + 1);
                
                // Certifique-se de que o objeto existe no mapa
                if (objectConfigs.find(objName) == objectConfigs.end()) {
                    objectConfigs[objName] = ObjectConfig();
                }
                
                // Configura propriedades do objeto
                if (objProp == "file") {
                    objectConfigs[objName].objFile = value;
                }
                else if (objProp == "mtl") {
                    objectConfigs[objName].mtlFile = value;
                }
                else if (objProp == "texture") {
                    objectConfigs[objName].textureFile = value;
                }
                else if (objProp == "position") {
                    istringstream iss(value);
                    iss >> objectConfigs[objName].position.x >> objectConfigs[objName].position.y >> objectConfigs[objName].position.z;
                }
                else if (objProp == "rotation") {
                    istringstream iss(value);
                    iss >> objectConfigs[objName].rotation.x >> objectConfigs[objName].rotation.y >> objectConfigs[objName].rotation.z;
                }
                else if (objProp == "scale") {
                    istringstream iss(value);
                    iss >> objectConfigs[objName].scale.x >> objectConfigs[objName].scale.y >> objectConfigs[objName].scale.z;
                }
                else if (objProp == "animation") {
                    objectConfigs[objName].animation = value;
                }
                else if (objProp == "orbit.target") {
                    objectConfigs[objName].orbitTarget = value;
                }
                else if (objProp == "orbit.radius") {
                    objectConfigs[objName].orbitRadius = stof(value);
                }
                else if (objProp == "orbit.speed") {
                    objectConfigs[objName].orbitSpeed = stof(value);
                }
                else if (objProp == "texture.body") {
                    objectConfigs[objName].textureBodyFile = value;
                }
                else if (objProp == "texture.eye") {
                    objectConfigs[objName].textureEyeFile = value;
                }
            }
        }
        else if (key.substr(0, 7) == "camera.") {
            string camProp = key.substr(7);
            
            if (camProp == "position") {
                istringstream iss(value);
                iss >> cameraConfig.position.x >> cameraConfig.position.y >> cameraConfig.position.z;
            }
            else if (camProp == "yaw") {
                cameraConfig.yaw = stof(value);
            }
            else if (camProp == "pitch") {
                cameraConfig.pitch = stof(value);
            }
            else if (camProp == "fov") {
                cameraConfig.fov = stof(value);
            }
            else if (camProp == "near") {
                cameraConfig.nearPlane = stof(value);
            }
            else if (camProp == "far") {
                cameraConfig.farPlane = stof(value);
            }
        }
        else if (key.substr(0, 6) == "light.") {
            string lightProp = key.substr(6);
            
            if (lightProp == "position") {
                istringstream iss(value);
                iss >> lightConfig.position.x >> lightConfig.position.y >> lightConfig.position.z;
            }
            else if (lightProp == "color") {
                istringstream iss(value);
                iss >> lightConfig.color.x >> lightConfig.color.y >> lightConfig.color.z;
            }
        }
    }
    
    file.close();
    return true;
}

//  Função para carregar um arquivo OBJ e MTL
bool loadOBJ(const string &objPath, const string &mtlPath, string &textureFileOut,
             vector<vec3>& outPositions, vector<vec2>& outTexCoords, vector<vec3>& outNormals,
             Material& outMaterial)
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
            // tex.y = 1.0f - tex.y;
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

                outPositions.push_back(tempPositions[vi]);
                outTexCoords.push_back(tempTexCoords[ti]);
            }
        }
    }

    // Inicializar os valores padrão do material
    outMaterial.ka = vec3(0.2f, 0.2f, 0.2f);
    outMaterial.kd = vec3(0.8f, 0.8f, 0.8f);
    outMaterial.ks = vec3(1.0f, 1.0f, 1.0f);
    outMaterial.shininess = 32.0f;

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
        }
        else if (prefix == "Ka")
        {
            iss >> outMaterial.ka.r >> outMaterial.ka.g >> outMaterial.ka.b;
            cout << "Ka: " << outMaterial.ka.r << " " << outMaterial.ka.g << " " << outMaterial.ka.b << endl;
        }
        else if (prefix == "Kd")
        {
            iss >> outMaterial.kd.r >> outMaterial.kd.g >> outMaterial.kd.b;
            cout << "Kd: " << outMaterial.kd.r << " " << outMaterial.kd.g << " " << outMaterial.kd.b << endl;
        }
        else if (prefix == "Ks")
        {
            iss >> outMaterial.ks.r >> outMaterial.ks.g >> outMaterial.ks.b;
            cout << "Ks: " << outMaterial.ks.r << " " << outMaterial.ks.g << " " << outMaterial.ks.b << endl;
        }
        else if (prefix == "Ns") // Coeficiente de brilho especular
        {
            iss >> outMaterial.shininess;
            cout << "Shininess: " << outMaterial.shininess << endl;
        }
    }

    // Calcular normais
    outNormals.clear();
    for (size_t i = 0; i < outPositions.size(); i++) {
        // Normais apontando para fora do centro aproximado do objeto
        vec3 normal = normalize(outPositions[i]);
        outNormals.push_back(normal);
    }
    
    return true;
}