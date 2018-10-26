/*
Quadrado

Neste exemplo, especificamos um quadrado, formado por 2 tri�ngulos
*/

#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Dimensões da janela
GLuint WIDTH = 800, HEIGHT = 600;
GLFWwindow *window;
GLuint shader_programm, vao;

void redimensionaCallback(GLFWwindow *_window, int width, int height) {
	WIDTH = width;
	HEIGHT = height;
}

int inicializaOpenGL() {
	// Inicializa GLFW
	glfwInit();

	// Configura algumas op�ões da GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/*
	Cria��o da janela
	*/
	window = glfwCreateWindow(WIDTH, HEIGHT, "Exemplo 5", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "ERRO: n�o foi poss�vel abrir a janela com GLFW\n" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwSetWindowSizeCallback(window, redimensionaCallback);

	glfwMakeContextCurrent(window);

	// Configura��o para que a GLEW saiba que estamos trabalhando com OpenGL moderno
	glewExperimental = GL_TRUE;
	// Inicializa a GLEW
	if (GLEW_OK != glewInit())
	{
		std::cout << "Falha na inicializa��o da GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << glGetString(GL_RENDERER) << std::endl; //placa de video
	std::cout << glGetString(GL_VERSION) << std::endl; //vers�o do opengl

}

void inicializaObjetos() {

	/*
	VAO do tri�ngulo
	*/

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/*
	VBO dos v�rtices
	*/
	GLfloat points[] = {
		0.5f, 0.5f, 0.0f,//vertice superior direito
		0.5f, -0.5f, 0.0f,//vertice inferior direito
		-0.5f, -0.5f, 0.0f,//vertice inferior esquerdo
		-0.5f, 0.5f, 0.0f//vertice superior esquerdo
	};

	GLuint pvbo = 0;
	glGenBuffers(1, &pvbo);
	glBindBuffer(GL_ARRAY_BUFFER, pvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*
	VBO das cores
	*/
	GLfloat cores[] = {
		1.0f, 1.0f, 0.0f,//amarelo
		0.0f, 1.0f, 1.0f,//ciano
		1.0f, 0.0f, 1.0f,//magenta
		1.0f, 1.0f, 1.0f //branco
	};
	GLuint cvbo = 0;
	glGenBuffers(1, &cvbo);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cores), cores, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*
	VBO dos �ndices. Lembre-se que todo objeto deve ser desenhado com tri�ngulos.
	O quadrado em quest�o ser� formado por 2 tr�ngulos:
	- um triangulo inferior direito:
	0
	/|
	/_|
	2  1

	- um triangulo superior esquerdo:
	3 __0
	| /
	|/
	2
	que juntos formam um quadrado:
	__
	| //|
	|//_|

	*/
	GLuint indices[] = {
		0, 1, 2,//triangulo inferior direito
		2, 3, 0 //triangulo superior esquerdo
	};

	GLuint ivbo = 0;
	glGenBuffers(1, &ivbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void inicializaShaders() {
	/*
	Especifica��o do Vertex Shader:
	*/
	const GLchar* vertex_shader = "#version 400\n"
		"layout(location = 0) in vec3 vertex_posicao;"
		"layout(location = 1) in vec3 vertex_cores;"
		"out vec3 cores;"
		"void main () {"
		" cores = vertex_cores;"
		" gl_Position = vec4 (vertex_posicao, 1.0);"
		"}";
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	GLint successo;
	GLchar infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &successo);
	if (!successo) {
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "Erro no vertex shader:\n" << infoLog << std::endl;
	}
	/*
	Especifica��o do Fragment Shader:
	*/
	const GLchar* fragment_shader = "#version 400\n"
		"in vec3 cores;"
		"out vec4 frag_colour;"
		"void main () {"
		" frag_colour = vec4 (cores, 1.0);"
		"}";
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &successo);
	if (!successo) {
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "Erro no fragment shader:\n" << infoLog << std::endl;
	}

	/*
	Especifica��o do Shader Programm:
	*/
	shader_programm = glCreateProgram();
	glAttachShader(shader_programm, fs);
	glAttachShader(shader_programm, vs);
	glLinkProgram(shader_programm);
	glGetProgramiv(shader_programm, GL_LINK_STATUS, &successo);

	if (!successo)
	{
		glGetProgramInfoLog(shader_programm, 512, NULL, infoLog);
		std::cout << "Erro na linkagem do shader:\n" << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void inicializaRenderizacao() {

	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, WIDTH, HEIGHT);

		glUseProgram(shader_programm);

		glBindVertexArray(vao);
		/*
		Observe que, neste exemplo, passamos o valor 6 como par�metro, pois o quadrado � formado por 2 tri�ngulos,
		cada um com 3 v�rtices... 2x3 = 6 :)
		*/
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwPollEvents();

		glfwSwapBuffers(window);

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}
	}
}
// Fun��o principal
int main()
{

	inicializaOpenGL();
	inicializaObjetos();
	inicializaShaders();
	inicializaRenderizacao();


	// Toda aplica��o OpenGL termina aqui.
	glfwTerminate();

	//segura a tela
	std::cin.get();

	return EXIT_SUCCESS;
}
