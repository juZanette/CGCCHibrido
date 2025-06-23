/* Olá Triângulo - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Computação Gráfica - Unisinos
 * Versão inicial: 07/04/2017
 * Última atualização: 07/03/2025
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector> 
#include <random> 

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Gerador de números aleatórios para posições dos cubos
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(-1.0, 1.0);

// Protótipo da função de callback do teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela (podem ser alteradas em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda codificado diretamente
const GLchar* vertexShaderSource = "#version 450\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"uniform mat4 model;\n"
"out vec4 finalColor;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position, 1.0);\n"
"finalColor = vec4(color, 1.0);\n"
"}\0";

// Código fonte do Fragment Shader (em GLSL)
const GLchar* fragmentShaderSource = "#version 450\n"
"in vec4 finalColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = finalColor;\n"
"}\n\0";

bool rotateX=false, rotateY=false, rotateZ=false;
float scale = 1.0f;

// Estrutura para armazenar transformações individuais de cada cubo
struct CubeTransform {
	glm::vec3 position;
	glm::vec3 translation = glm::vec3(0.0f);
	bool rotateX = false, rotateY = false, rotateZ = false;
	float scale = 1.0f;
};

// Vetor de cubos
std::vector<CubeTransform> cubes;

// Índice do cubo atualmente selecionado
size_t selectedCubeIndex = 0;

// Função MAIN
int main()
{
	// Inicialização do GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para descobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
	//#ifdef __APPLE__
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//#endif

	// Cria a janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D - Cubo - M2!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Registra a função de callback do teclado
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros de funções do OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Falha ao inicializar GLAD" << std::endl;

	}

	// Obtém informações da versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* obtém string do renderizador */
	const GLubyte* version = glGetString(GL_VERSION); /* versão como string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Define as dimensões do viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compila e constrói o programa de shader
	GLuint shaderID = setupShader();

	// Gera um buffer simples com a geometria do triângulo
	GLuint VAO = setupGeometry();

	// Offset inicial do cubo - começa com apenas um cubo na origem
	cubes.push_back({glm::vec3(0.0f, 0.0f, 0.0f)});
	selectedCubeIndex = 0;

	glUseProgram(shaderID);

	GLint modelLoc = glGetUniformLocation(shaderID, "model");
	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLineWidth(2);
		glPointSize(5);
		float angle = (GLfloat)glfwGetTime();
		glBindVertexArray(VAO);

		// Itera pelo vetor de cubos
		for (size_t i = 0; i < cubes.size(); i++) {
			glm::mat4 model = glm::mat4(1);

			// Aplica transformações individuais
			model = glm::translate(model, cubes[i].position + cubes[i].translation);

			// Só aplica rotação ao cubo selecionado
			if (i == selectedCubeIndex) {
				if (cubes[i].rotateX)
					model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
				else if (cubes[i].rotateY)
					model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
				else if (cubes[i].rotateZ)
					model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
			}

			// Escala individual
			model = glm::scale(model, glm::vec3(cubes[i].scale));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	// Solicita ao OpenGL desalocar buffers
	glDeleteVertexArrays(1, &VAO);
	// Encerra a execução do GLFW, limpando recursos alocados
	glfwTerminate();
	return 0;
}

// Função de callback do teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Alterna cubo selecionado pressionando TAB
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS && !cubes.empty()) {
		selectedCubeIndex = (selectedCubeIndex + 1) % cubes.size();
	}

	// Rotações (X, Y, Z)
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		cubes[selectedCubeIndex].rotateX = true;
		cubes[selectedCubeIndex].rotateY = false;
		cubes[selectedCubeIndex].rotateZ = false;
	}
	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		cubes[selectedCubeIndex].rotateX = false;
		cubes[selectedCubeIndex].rotateY = true;
		cubes[selectedCubeIndex].rotateZ = false;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		cubes[selectedCubeIndex].rotateX = false;
		cubes[selectedCubeIndex].rotateY = false;
		cubes[selectedCubeIndex].rotateZ = true;
	}

	// Translação X/Z (WASD)
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.z -= 0.1f;
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.z += 0.1f;
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.x -= 0.1f;
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.x += 0.1f;

	// Translação Y (I/J)
	if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.y += 0.1f;
	if (key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].translation.y -= 0.1f;

	// Escala (- e +)
	if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].scale *= 0.9f;
	if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT)) cubes[selectedCubeIndex].scale *= 1.1f;

	// Adiciona novo cubo ao pressionar 'N'
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		// Gera posição aleatória para o novo cubo
		glm::vec3 newPos(dis(gen), dis(gen), dis(gen));
		// Garante uma distância mínima dos outros cubos para evitar sobreposição
		bool validPosition = true;
		for (const auto& cube : cubes) {
			if (glm::length(newPos - cube.position) < 1.0f) {
				validPosition = false;
				break;
			}
		}
		if (validPosition || cubes.empty()) {
			cubes.push_back({newPos});
			selectedCubeIndex = cubes.size() - 1; // Seleciona o novo cubo automaticamente
		}
	}
}

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
		std::cout << "ERRO::SHADER::VERTEX::FALHA_COMPILACAO\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Verifica erros de compilação (exibe via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERRO::SHADER::FRAGMENT::FALHA_COMPILACAO\n" << infoLog << std::endl;
	}
	// Linka os shaders e cria o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Verifica erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERRO::SHADER::PROGRAMA::FALHA_LINKAGEM\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int setupGeometry()
{
	GLfloat vertices[] = {
		// Face +X (vermelho)
		 0.5, -0.5, -0.5, 1,0,0,   0.5,  0.5, -0.5, 1,0,0,   0.5,  0.5,  0.5, 1,0,0,
		 0.5, -0.5, -0.5, 1,0,0,   0.5,  0.5,  0.5, 1,0,0,   0.5, -0.5,  0.5, 1,0,0,
		// Face -X (verde)
		-0.5, -0.5,  0.5, 0,1,0,  -0.5,  0.5,  0.5, 0,1,0,  -0.5,  0.5, -0.5, 0,1,0,
		-0.5, -0.5,  0.5, 0,1,0,  -0.5,  0.5, -0.5, 0,1,0,  -0.5, -0.5, -0.5, 0,1,0,
		// Face +Y (azul)
		-0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5,  0.5, 0,0,1,
		-0.5,  0.5, -0.5, 0,0,1,   0.5,  0.5,  0.5, 0,0,1,  -0.5,  0.5,  0.5, 0,0,1,
		// Face -Y (amarelo)
		-0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5, -0.5, 1,1,0,
		-0.5, -0.5,  0.5, 1,1,0,   0.5, -0.5, -0.5, 1,1,0,  -0.5, -0.5, -0.5, 1,1,0,
		// Face +Z (magenta)
		-0.5, -0.5,  0.5, 1,0,1,  -0.5,  0.5,  0.5, 1,0,1,   0.5,  0.5,  0.5, 1,0,1,
		-0.5, -0.5,  0.5, 1,0,1,   0.5,  0.5,  0.5, 1,0,1,   0.5, -0.5,  0.5, 1,0,1,
		// Face -Z (ciano)
		-0.5,  0.5, -0.5, 0,1,1,  -0.5, -0.5, -0.5, 0,1,1,   0.5, -0.5, -0.5, 0,1,1,
		-0.5,  0.5, -0.5, 0,1,1,   0.5, -0.5, -0.5, 0,1,1,   0.5,  0.5, -0.5, 0,1,1,
	};

	GLuint VBO, VAO;

	// Gera identificador do VBO
	glGenBuffers(1, &VBO);

	// Faz o bind do buffer como array buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Envia os dados do array de floats para o buffer do OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Gera identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Faz o bind do VAO primeiro, depois conecta e define o(s) buffer(s) de vértices
	// e ponteiros de atributo
	glBindVertexArray(VAO);
	
	// Para cada atributo de vértice, cria um "AttribPointer" (ponteiro para o atributo), indicando:
	// Localização no shader * (as localizações dos atributos devem bater com o layout especificado no vertex shader)
	// Número de valores do atributo (ex: 3 coordenadas xyz)
	// Tipo de dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes
	// Offset a partir do byte zero
	
	// Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// Note que isso é permitido, a chamada ao glVertexAttribPointer registrou o VBO como o
	// buffer de vértices atualmente em uso - então podemos desassociar depois
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desassocia o VAO (é uma boa prática desassociar qualquer buffer ou array para evitar bugs)
	glBindVertexArray(0);

	return VAO;
}
