#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//ifstream
#include <fstream>
#include <sstream>

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Camera
#include "Camera.h"

//Shader
#include "Shader.h"

//Model
#include "Model.h"




//GLOBAL VARIABLES
//Camera variables
Camera camera(glm::vec3(0.f, 0.f, 3.f));
//Time variables
float deltaTime = 0.f;
float lastFrame = 0.f;
//Cursor variables
float lastX = 400.f;
float lastY = 300.f;
bool firstmouse = true;
//Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
//Shader variables
GLuint VBO, VAO, lightVAO;
GLuint EBO;
//glm::vec3 lightPos(1.2f, 1.f, 2.f);
glm::vec3 pointLightPosition[] = {
	glm::vec3(0.f,0.f,0.f),
	glm::vec3(0.f,2.5f,0.f),
	glm::vec3(0.f,5.f,0.f),
};


unsigned int LoadTextures(char* name)
{
	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(name, &width, &height, &nrChannels, STBI_rgb);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Failed to load texture!");
	}
	stbi_image_free(data);
	return texture;
}

//Load Vertexshader or Fragmentshader
std::string LoadShader(const char* ShaderPath)
{
	std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shaderFile.open(ShaderPath);
		std::stringstream vShaderStream;
		vShaderStream << shaderFile.rdbuf();
		shaderFile.close();
		shaderCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	return shaderCode;
}

void RenderBox()
{

    //Smaller box
    GLfloat vertices[] =
    {
             //Position            //Color         //Texture coords
         /*0.2f,  0.2f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,     //top right
         0.2f, -0.2f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,     //bottom right
        -0.2f, -0.2f, 0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,     //bottom left
        -0.2f,  0.2f, 0.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f      //top left*/

		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };



    //GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Bind vertex array object, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    //Position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
    glEnableVertexAttribArray(0);

	//Normal attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

    
    //Color attributes
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);

    //Texture attributes
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    

	//Lamp
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}


void processInput(GLFWwindow * window)
{
    float cameraSpeed = 3.f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
        //cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        //cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
        //cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
        //cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstmouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstmouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

//MAIN
int main()
{
    //Init GLFW
    glfwInit();

    //Set options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //Create a GLFWwindow
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "GLWF Window", nullptr, nullptr);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glewExperimental = GL_TRUE;

	// Initialize GLEW
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);


    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
	Shader shaderlamp(LoadShader("lampvertexshader.vs.txt").c_str(), LoadShader("lampfragmentshader.fs.txt").c_str());
	

	//Shader shader(LoadShader("modelvertexshader.vs.txt").c_str(), LoadShader("modelfragmentshader.fs.txt").c_str());
	Shader shader(LoadShader("lightvertexshader.vs.txt").c_str(), LoadShader("lightfragmentshader.fs.txt").c_str());
    

    //Generate lamp box
    RenderBox();

	Model tdmodel("3dmodel/box.obj");

	Model tdmodel2("3dmodel/cone.obj");

	Model tdmodel3("3dmodel/monkey.obj");

	Model tdmodel4("3dmodel/boxN_S.obj");

	//Load texture and specular
    unsigned int texture, texture2, texture3;
	texture = LoadTextures("grey_and_white_tiles.png");
	texture2 = LoadTextures("grey_and_white_tiles_specular.png");


 
	shader.UseProgram();

	glUniform1i(glGetUniformLocation(shader.shaderProgram, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "material.specular"), 1);
	
                          // Game loop
    while (!glfwWindowShouldClose(window))
    {
        //Input
        processInput(window);

		//DeltaTime calculation
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Render
        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Bind Texture if objects don't have any
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
  
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        //models
        glm::mat4 transform;
        glm::mat4 view;
        glm::mat4 projection;

        
        view = camera.GetViewMatrix();
        
		projection = glm::perspective(glm::radians(camera.Fov), (float)(screenWidth / screenHeight), 0.1f, 100.f);

		//activate shader
		shader.UseProgram();

		GLint viewPosLocation = glGetUniformLocation(shader.shaderProgram, "viewPos");
		glUniform3f(viewPosLocation, camera.Position.x, camera.Position.y, camera.Position.z);

		glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.shine"), 64.f);

		// directional light
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		// point light 1
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].position"), pointLightPosition[0].x, pointLightPosition[0].y, pointLightPosition[0].z);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[0].quadratic"), 0.032);
		// point light 2
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].position"), pointLightPosition[1].x, pointLightPosition[1].y, pointLightPosition[1].z);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[1].quadratic"), 0.032);
		// point light 3
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].position"), pointLightPosition[2].x, pointLightPosition[2].y, pointLightPosition[2].z);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader.shaderProgram, "pointLights[2].quadratic"), 0.032);

		
        GLint viewLocation = glGetUniformLocation(shader.shaderProgram, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

        GLint projectionLocation = glGetUniformLocation(shader.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));



		GLint lightPosLocation = glGetUniformLocation(shader.shaderProgram, "lightPos");
		glUniform3f(lightPosLocation, pointLightPosition[0].x, pointLightPosition[0].y, pointLightPosition[0].z);

		glm::mat4 rmat;
		float angle = 40.f * glfwGetTime();
		rmat = glm::translate(rmat, glm::vec3(pointLightPosition[0].x, pointLightPosition[0].y, pointLightPosition[0].z));

		glm::mat4 model;
		float anglemodel = 90.f * glfwGetTime();
		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(0.0f, 0.f, 2.0f));
		model = glm::rotate(rmat, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
		model = glm::translate(model, glm::vec3(0.0f, 0.f, -2.0f));
		model = glm::rotate(model, glm::radians(anglemodel), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		tdmodel.Draw(shader);

		glm::mat4 model2;
		float anglemodel2 = -55.f * glfwGetTime();
		model2 = glm::translate(model2, glm::vec3(0.0f, 2.f, 2.0f));
		model2 = glm::rotate(rmat, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
		model2 = glm::translate(model2, glm::vec3(0.0f, 2.f, -2.0f));
		model2 = glm::rotate(model2, glm::radians(anglemodel2), glm::vec3(0.f, 1.f, 0.f));
		model2 = glm::scale(model2, glm::vec3(1.f, 1.f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

		tdmodel2.Draw(shader);

		glm::mat4 model3;
		float anglemodel3 = 120.f * glfwGetTime();
		model3 = glm::translate(model3, glm::vec3(0.f, 4.f, 2.f));
		model3 = glm::rotate(rmat, glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));
		model3 = glm::translate(model3, glm::vec3(0.0f, 4.f, -2.0f));
		model3 = glm::rotate(model3, glm::radians(anglemodel3), glm::vec3(0.f, 1.f, 0.f));
		model3 = glm::scale(model3, glm::vec3(1.f, 1.f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model3));

		tdmodel3.Draw(shader);


		glm::mat4 model4;
		model4 = glm::translate(model4, glm::vec3(0.f, 0.f, 0.f));
		model4 = glm::scale(model4, glm::vec3(8.f, 8.f, 8.f));
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model4));

		tdmodel4.Draw(shader);

		//Lamp
		glm::mat4 modelL;
		glm::mat4 viewL;
		glm::mat4 projectionL;

		viewL = camera.GetViewMatrix();
		projectionL = glm::perspective(glm::radians(camera.Fov), (float)(screenWidth / screenHeight), 0.1f, 100.f);

		shaderlamp.UseProgram();

		GLint viewLocationLamp = glGetUniformLocation(shaderlamp.shaderProgram, "view");
		glUniformMatrix4fv(viewLocationLamp, 1, GL_FALSE, glm::value_ptr(viewL));

		GLint projectionLocationLamp = glGetUniformLocation(shaderlamp.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLocationLamp, 1, GL_FALSE, glm::value_ptr(projectionL));

		for (int i = 0; i < 3; i++)
		{
			modelL = glm::mat4();
			modelL = glm::translate(modelL, pointLightPosition[i]);
			modelL = glm::scale(modelL, glm::vec3(0.2f));

			GLint modelLocationLamp = glGetUniformLocation(shaderlamp.shaderProgram, "model");
			glUniformMatrix4fv(modelLocationLamp, 1, GL_FALSE, glm::value_ptr(modelL));

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
        //Color
        //GLfloat colorValue = (sin(glfwGetTime()) / 2.0f) + 0.5;
        //GLint colorLocation = glGetUniformLocation(ownShader.shaderProgram, "ourColor");
        //glUniform4f(colorLocation, 0.0f, colorValue, colorLocation, 1.0f);
    
        glBindVertexArray(0);

        // Check any events
        glfwPollEvents();

        // Swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);


    //Terminate GLFW
    glfwTerminate();

    return EXIT_SUCCESS;
}

