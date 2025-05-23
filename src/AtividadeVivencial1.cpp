#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

using namespace std;

// Função para carregar OBJ (com cor fixa por vértice, conforme diretrizes)
GLuint loadSimpleOBJ(string filePath, int &nVertices) {
    vector<float> vertices;
    vector<float> vBuffer;

    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Erro ao tentar ler o arquivo " << filePath << endl;
        return -1;
    }

    string line;
    vector<int> indices;
    while (getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            istringstream s(line.substr(2));
            float x, y, z;
            s >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (line.substr(0, 2) == "f ") {
            istringstream s(line.substr(2));
            string v_str;
            for (int i = 0; i < 3; ++i) {
                s >> v_str;
                int vIdx = 0;
                size_t pos = v_str.find('/');
                if (pos != string::npos)
                    vIdx = stoi(v_str.substr(0, pos));
                else
                    vIdx = stoi(v_str);
                indices.push_back(vIdx - 1);
            }
        }
    }
    file.close();

    // Monta o vBuffer (x, y, z, r, g, b)
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i] * 3;
        vBuffer.push_back(vertices[idx]);
        vBuffer.push_back(vertices[idx + 1]);
        vBuffer.push_back(vertices[idx + 2]);
        // Cor fixa (vermelho)
        vBuffer.push_back(1.0f);
        vBuffer.push_back(0.0f);
        vBuffer.push_back(0.0f);
    }

    nVertices = vBuffer.size() / 6;

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(float), vBuffer.data(), GL_STATIC_DRAW);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

// Estrutura para armazenar cada modelo
struct Model3D {
    GLuint vao;
    int nVertices;
    float pos[3] = {0, 0, 0};
    float rot[3] = {0, 0, 0};
    float scale = 1.0f;
};

vector<Model3D> modelos;
int selecionado = 0;

// Callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_TAB) selecionado = (selecionado + 1) % modelos.size();
        if (key == GLFW_KEY_R) modelos[selecionado].rot[1] += 10.0f;
        if (key == GLFW_KEY_W) modelos[selecionado].pos[1] += 0.1f;
        if (key == GLFW_KEY_S) modelos[selecionado].pos[1] -= 0.1f;
        if (key == GLFW_KEY_A) modelos[selecionado].pos[0] -= 0.1f;
        if (key == GLFW_KEY_D) modelos[selecionado].pos[0] += 0.1f;
        if (key == GLFW_KEY_E) modelos[selecionado].scale *= 1.1f;
        if (key == GLFW_KEY_Q) modelos[selecionado].scale *= 0.9f;
        if (key == GLFW_KEY_O) {
            Model3D novo;
            novo.vao = loadSimpleOBJ("../assets/Modelos3D/suzanne.obj", novo.nVertices);
            if (novo.vao != (GLuint)-1 && novo.nVertices > 0) {
                novo.pos[0] = modelos.size();
                modelos.push_back(novo);
                std::cout << "Nova Suzanne adicionada! Total: " << modelos.size() << std::endl;
            } else {
                std::cout << "Falha ao carregar nova Suzanne!" << std::endl;
            }
        }
    }
}

void drawModel(const Model3D& model, bool wireframe) {
    glPushMatrix();
    glTranslatef(model.pos[0], model.pos[1], model.pos[2]);
    glRotatef(model.rot[0], 1, 0, 0);
    glRotatef(model.rot[1], 0, 1, 0);
    glRotatef(model.rot[2], 0, 0, 1);
    glScalef(model.scale, model.scale, model.scale);
    glBindVertexArray(model.vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, model.nVertices);
    glBindVertexArray(0);
    glPopMatrix();
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1600, 1200, "Atividade Vivencial 1", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro ao inicializar GLAD!" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, 1600, 1200);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float fovy = 60.0f;
    float aspect = 1600.0f / 1200.0f;
    float zNear = 0.1f;
    float zFar = 100.0f;
    float top = zNear * tanf((fovy * 3.14159265f / 180.0f) / 2.0f);
    float right = top * aspect;
    glFrustum(-right, right, -top, top, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -5);

    glEnable(GL_DEPTH_TEST);

    {
        Model3D m1, m2;
        m1.vao = loadSimpleOBJ("../assets/Modelos3D/suzanne.obj", m1.nVertices);
        m2.vao = loadSimpleOBJ("../assets/Modelos3D/cube.obj", m2.nVertices);
        modelos.push_back(m1);
        modelos.push_back(m2);
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < modelos.size(); ++i) {
            drawModel(modelos[i], false);
            if (i == selecionado) drawModel(modelos[i], true);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}