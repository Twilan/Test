#include <cassert> //Debugging, turns off at Release
#include <iostream>

#include <fstream>
#include <string>
#include <sstream>

#include <cmath>

///----------///

#include "src/Glm_CommonIncludes.h"

///----------///

#include <GL/glew.h>
#include <GLFW/glfw3.h>

///----------///

/*Error Handling*/
static bool GLCheckError() {
	while (GLenum error = glGetError() != GL_NO_ERROR) {
		std::cout << "[OpenGL_Error_Code: " << error << "]"
			//Can add more details if neccesary later
			<< "\n";
		return false;
	}
	return true;
}

/*Shader Handling*/
struct ShaderSourceCodes {
	std::string vertex;
	std::string fragment;
};

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	/*Error Handling*/
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char * message = new char[length];

		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader!" << "\n";
		std::cout << message << "\n";
		glDeleteShader(id);

		delete[] message;
		return 0;
	}

	return id;
}

static int CreateShader(const std::string& filepath) {
	/*Parse Shader*/
	std::ifstream stream(filepath);
	ShaderSourceCodes sources;

	std::string line;
	std::stringstream ss[2];

	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
			ss[(int)type] << line << "\n";
	}

	sources.vertex = ss[0].str();
	sources.fragment = ss[1].str();

	/*Create&Compile Shader*/
	unsigned int programID = glCreateProgram();
	unsigned int verS = CompileShader(GL_VERTEX_SHADER, sources.vertex);
	unsigned int fragS = CompileShader(GL_FRAGMENT_SHADER, sources.fragment);

	/*Attach to OpenGL*/
	glAttachShader(programID, verS);
	glAttachShader(programID, fragS);
	glLinkProgram(programID);
	glValidateProgram(programID);
	assert(GLCheckError());

	glDeleteShader(verS);
	glDeleteShader(fragS);
	assert(GLCheckError());

	return programID;
}

/*Window Handling*/
static void window_resize_callback(GLFWwindow * window, int width, int height){


	glViewport(0, 0, width, height);
}

/*Input Handling*/
int InputMouseOldX;
int InputMouseOldY;
int InputMouseNewX;
int InputMouseNewY;
int InputMouseDiffX;
int InputMouseDiffY;
bool InputMouseIsMoving;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

//vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
vec3 cameraRight = glm::normalize(glm::cross(cameraUp  , cameraDirection));

//vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

float pitch = 0.0f; //X (Side to Side)
float yaw = 0.0f;	//Y (Up and Down)
float roll = 0.0f;	//Z (Cartwheel Rotations)

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	InputMouseNewX = xpos;
	InputMouseNewY = ypos;

	InputMouseDiffX = InputMouseNewX - InputMouseOldX;
	InputMouseDiffY = InputMouseNewY - InputMouseOldY;


	//if (firstMouse)
	//{
		//InputMouseOldX = xpos;
		//InputMouseOldY = ypos;
		//firstMouse = false;
	//}

	float xoffset = xpos - InputMouseOldX;
	float yoffset = InputMouseOldY - ypos;
	//InputMouseOldX = xpos;
	//InputMouseOldY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//vec4 cameraFront2 = vec4(front.x, front.y, front.z, 0.0f);
	//mat4 frontToUp = glm::rotate(mat4(), glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
	//cameraUp = frontToUp * cameraFront2;
	 
	/*
	glm::vec3 up;
	up.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	up.y = sin(glm::radians(pitch));
	up.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraUp = glm::normalize(up);
	*/
	/*
	glm::vec3 right;
	right.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	right.y = sin(glm::radians(pitch));
	right.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));*/
	cameraRight = glm::normalize(glm::cross(cameraUp, cameraDirection));
}
/*
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	InputMouseNewX = xpos;
	InputMouseNewY = ypos;

	InputMouseDiffX = InputMouseNewX - InputMouseOldX;
	InputMouseDiffY = InputMouseNewY - InputMouseOldY;
}
*/

bool InputW = false;
bool InputS = false;
bool InputA = false;
bool InputD = false;
bool InputShift = false;
bool InputSpace = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W){
		if (action == GLFW_PRESS)
			InputW = true;
		else if (action == GLFW_RELEASE)
			InputW = false;
	}else if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS)
			InputS = true;
		if (action == GLFW_RELEASE)
			InputS = false;
	}else if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS)
			InputA = true;
		if (action == GLFW_RELEASE)
			InputA = false;
	}else if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS)
			InputD = true;
		if (action == GLFW_RELEASE)
			InputD = false;
	}
	
	else if (key == GLFW_KEY_LEFT_SHIFT) {
		if (action == GLFW_PRESS)
			InputShift = true;
		if (action == GLFW_RELEASE)
			InputShift = false;
	}else if (key == GLFW_KEY_SPACE) {
		if (action == GLFW_PRESS)
			InputSpace = true;
		if (action == GLFW_RELEASE)
			InputSpace = false;
	}

	else if (key == GLFW_KEY_TAB) {
		if (action == GLFW_PRESS) {
			if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
	}

	else if (key == GLFW_KEY_MINUS) {
		if (action == GLFW_PRESS)
			std::cout << "--------------------\n";
	}
}

/*Main*/
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//A windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "PrV Here I come", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	//Is this really necessary?
	glfwSetWindowSizeLimits(window, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);

	//GLFW's window's context current
	glfwMakeContextCurrent(window);


	//Glew Init comes after Context
	if (glewInit() != GLEW_OK)
		std::cout << "Glew Init Error!\n";

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	std::cout << glGetString(GL_VERSION) << "\n";

	glfwSetWindowSizeCallback(window, window_resize_callback);
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float positions[] = {
		-2.0f + 10.0f,		-2.0f + 10.0f,		-2.0f + 10.0f,	0.0f,		1.0f,		1.0f,		1.0f,
		2.0f + 10.0f,		-2.0f + 10.0f,		-2.0f + 10.0f,	0.0f,		1.0f,		1.0f,		1.0f,
		2.0f + 10.0f,		 2.0f + 10.0f,		-2.0f + 10.0f,	1.0f,		1.0f,		1.0f,		1.0f,
		-2.0f + 10.0f,		 2.0f + 10.0f,		-2.0f + 10.0f,	1.0f,		1.0f,		1.0f,		1.0f,

		-2.0f + 10.0f,		-2.0f + 10.0f,		 2.0f + 10.0f,	0.0f,		1.0f,		1.0f,		1.0f,
		2.0f + 10.0f,		-2.0f + 10.0f,		 2.0f + 10.0f,	0.0f,		1.0f,		1.0f,		1.0f,
		2.0f + 10.0f,		 2.0f + 10.0f,		 2.0f + 10.0f,	1.0f,		1.0f,		1.0f,		1.0f,
		-2.0f + 10.0f,		 2.0f + 10.0f,		 2.0f + 10.0f,	1.0f,		1.0f,		1.0f,		1.0f
	};

	unsigned int colors[] = {
		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,

		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,
		1.0f,		1.0f,		1.0f,		1.0f,
		/*
		1.0f,		1.0f,		1.0f,
		0.0f,		0.0f,		1.0f,
		0.0f,		1.0f,		0.0f,
		1.0f,		0.0f,		0.0f,

		1.0f,		1.0f,		1.0f,
		0.0f,		0.0f,		1.0f,
		0.0f,		1.0f,		0.0f,
		1.0f,		0.0f,		0.0f
		*/
	};

	unsigned int indicies[] = {
		0, 1, 2,
		2, 3, 0, //Front (Negative Z)

		1 - 1, 2 - 1, 5 - 1,
		2 - 1, 5 - 1, 6 - 1,

		0 + 4, 1 + 4, 2 + 4,
		2 + 4, 3 + 4, 0 + 4 //Back (Positive Z)

	};

	for (int i = 2; i < sizeof(positions) / sizeof(float); i += 7) {
		//positions[i] += 5.0f;
		//std::cout << positions[i];
	}

	//Assigning Buffers
	unsigned int vertexArrayObj;
	glGenVertexArrays(1, &vertexArrayObj); assert(GLCheckError());
	glBindVertexArray(vertexArrayObj); assert(GLCheckError());

	unsigned int vertexBufferObj;
	glGenBuffers(1, &vertexBufferObj); assert(GLCheckError());
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj); assert(GLCheckError());
	glBufferData(GL_ARRAY_BUFFER, (8 * 3 + (8 * 4)) * sizeof(float), positions, GL_STATIC_DRAW);
	assert(GLCheckError()); //4 Verts in 3D +(Color)

	unsigned int indiciesBufferObj;
	glGenBuffers(1, &indiciesBufferObj); assert(GLCheckError());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBufferObj); assert(GLCheckError());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * 2 * 3 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);
	assert(GLCheckError()); //2 Tris

	unsigned int colorsBufferObj;
	glGenBuffers(1, &colorsBufferObj); assert(GLCheckError());
	glBindBuffer(GL_ARRAY_BUFFER, colorsBufferObj); assert(GLCheckError());
	glBufferData(GL_ARRAY_BUFFER, (8 * 4 + (8 * 3)) * sizeof(float), positions, GL_STATIC_DRAW);
	assert(GLCheckError()); //4 Verts 4 Attributes (RGBA)+(Verts)

	//Assigning Layout
	glEnableVertexAttribArray(0); assert(GLCheckError());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*)(0 * sizeof(float)));
	assert(GLCheckError()); //3Dimensionals of VertexBuffer, 3floats to the next Vertex

	glEnableVertexAttribArray(1); assert(GLCheckError());
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	assert(GLCheckError());//4 Verts 4 Attributes (RGBA)

	//Shader Stuffs
	unsigned int shader = CreateShader("src/shaders/Basic.shader");
	glUseProgram(shader); assert(GLCheckError());

	//Shader Uniforms
	{
		//Put Matricies here after ensuring they work
	}
	int location = glGetUniformLocation(shader, "uColor"); assert(GLCheckError());
	//glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f); assert(GLCheckError());

	float incrementX = 0.0f;
	float incrementY = 0.0f;
	float incrementZ = 0.0f;

	float rotInc = 0.0f;

	float rotX = 0.0f;
	float rotY = 0.0f;
	float rotZ = 0.0f;
	

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(window, cursor);

	vec4 ViewRotationVec4 = vec4(0.0f, 0.0f, 1.0f, 0.0f);//Defined outside loop... Make it look in 1Z directon somehow
	mat4 View = glm::lookAt(
		vec3(0.0f, 0.0f , -1.0f), // Camera is at (4,3,3), in World Space
		vec3(0.0f, 0.0f, 0.0f), // and looks at the origin

		vec3(0, -1, 0)  // (0, -1, 0) Makes up down and down up (0, 1, 0) is typical
						//Only the middle value is important on this variable??? Sure seems like it
	);
	/* Main Loop */
	while (!glfwWindowShouldClose(window))
	{
		//Input
		glfwPollEvents();

		float cameraSpeed = 0.5f; // adjust accordingly

		if (InputS)
			cameraPos -= cameraSpeed * cameraFront;
		if (InputW)
			cameraPos += cameraSpeed * cameraFront;
		if (InputA)
			cameraPos += cameraRight * cameraSpeed;
		if (InputD)
			cameraPos -= cameraRight * cameraSpeed;

		if (InputShift)
			cameraPos -= cameraSpeed * cameraUp;
		if (InputSpace)
			cameraPos += cameraSpeed * cameraUp;
		/*
		if (InputShift)
			incrementY += speed;
		if (InputSpace)
			incrementY -= speed;
			*/
		if (InputMouseNewX == InputMouseOldX && InputMouseNewY == InputMouseOldY)
			InputMouseIsMoving = false;
		else
			InputMouseIsMoving = true;
		/*
		std::cout << InputMouseIsMoving << "\n";
		std::cout << InputMouseNewX << ", " << InputMouseOldX << ", " << "\n";
		std::cout << InputMouseNewY << ", " << InputMouseOldY << ", " << "\n";
		*/
		InputMouseOldX = InputMouseNewX;
		InputMouseOldY = InputMouseNewY;

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int width=0, height=0;
		glfwGetWindowSize(window, &width, &height);
		for (float i = 0.0f; i < 10.0f; i += 1.0f) {
			//mat4 ViewRotation = //Defined outside loop... Make it look in 1Z directon somehow
			/*
			if (InputMouseIsMoving){
				float length = std::sqrt(std::pow(InputMouseDiffX, 2.0f) + std::pow(InputMouseDiffY, 2.0f));
				if (InputMouseDiffX != 0)
					float angle = std::tan(InputMouseDiffY/InputMouseDiffX)-1;
				
				vec2 mouseDiff(InputMouseDiffX, InputMouseDiffY);
				//mouseDiff = glm::normalize(mouseDiff);

				
				//ViewRotation = ViewRotation * glm::rotate(mat4(1), length, vec3(InputMouseDiffX, InputMouseDiffY, 0.0f));
				ViewRotationVec4 = ViewRotationVec4 * glm::rotate(mat4(), glm::radians(length)/100.0f, vec3(mouseDiff.x, mouseDiff.y, 0.0f));
				
				std::cout << ViewRotationVec4.x << ", " << ViewRotationVec4.y << "\n";

				
			}*/
			float radius = 10.0f;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;

			//cameraPos = vec3(incrementX, incrementY, incrementZ);

			glm::mat4 View;
			View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

			mat4 Translate = glm::translate(mat4(), vec3(i*10, 0.0f, 0.0f));
			mat4 Projection = glm::perspective(glm::radians(90.0f), ((float)width) / ((float)height), 0.1f, 500.0f);
			
			mat4 Model = mat4(1.0f);
			// Our ModelViewProjection : multiplication of our 3 matrices
			mat4 matAll = Projection * View * Model * Translate; // Remember, matrix multiplication is the other way around

			int matrixLocation = glGetUniformLocation(shader, "uModelViewProjectMatrix"); assert(GLCheckError());
			glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &matAll[0][0]); assert(GLCheckError());

			//Colors
			glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f); assert(GLCheckError());

			glDrawElements(GL_TRIANGLES, 6 * 3, GL_UNSIGNED_INT, nullptr); //Elements = Indicies
			assert(GLCheckError()); //2 Tris [Indicies]
		}
		//Display
		glfwSwapBuffers(window);
	}
	glfwDestroyCursor(cursor);
	glfwTerminate();
	return 0;
}