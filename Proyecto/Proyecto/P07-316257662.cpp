//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Implementación de cámaras
Camera camera;
Camera camera2;


//Modelos proyecto
Model resorte_M;
Model maquina_M;
Model flipper_M;
Model obstaculoFlor_M;
Model canica1_M;
Model canica2_M;
Model hongo1_M;

Model baseVanta;
Model vanta;
Model puertaVanta1;
Model puertaVanta2;

Model luna1;
Model luna2;

//Texturas proyecto
Texture resorte_T;
Texture maquina_T;
Texture flipper_T;
Texture canica1_T;
Texture canica2_T;
Texture hongo1_T1;
Texture hongo1_T2;

Texture vantaBase_T;
Texture vantaCuerpo_T;
Texture vantapuerta_T;
Texture luna_T;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
DirectionalLight mainLight2;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Variables proyecto

//Cambio de tamaño del resorte
bool cambioTam_1;
bool cambioTam_2;

//Animación básica canica1
float movCanica1_X;
float movCanica1_Z;
float movCanicaOffset;
float rotAspas = 0.0f;

bool animCanica1;
bool recorrido1;
bool recorrido2;
bool recorrido3;
bool recorrido4;

// Variables aux ilumnación cuarto
bool dia = true;
int ciclos = 0;

//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f, //La normal de la superficie debe de apuntar a la luz
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f, //Movimos la normal en z para que se ilumine el agave
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		1.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);
	camera2 = Camera(glm::vec3(200.0f, 400.0f, 300.0f), glm::vec3(0.0f, 1.0f, 0.0f), -120.0f, -30.0f, 25.0f, 0.5f);


	//Modelos proyecto
		//Resorte
	resorte_M = Model();
	resorte_M.LoadModel("Models/Resorte/Resorte.obj");
	//Maquina
	maquina_M = Model();
	maquina_M.LoadModel("Models/MaquinaPinball/Maquina.obj");
	//Flipper
	flipper_M = Model();
	flipper_M.LoadModel("Models/Flipper/Flipper.obj");
	//Obstaculo flor
	obstaculoFlor_M = Model();
	obstaculoFlor_M.LoadModel("Models/Flower/cartoon_flower.obj");
	//Canica1
	canica1_M = Model();
	canica1_M.LoadModel("Models/Canicas/Canica1.obj");
	//Canica2
	canica2_M = Model();
	canica2_M.LoadModel("Models/Canicas/Canica2.obj");
	//Obstaculo iluminado hongo1
	hongo1_M = Model();
	hongo1_M.LoadModel("Models/Mushroom/Mushroom_1.obj");


	//Obtaculo jerarquico base
	baseVanta = Model();
	baseVanta.LoadModel("Models/Vanta/baseVanta.obj");
	//Obtaculo jerarquico cuerpo
	vanta = Model();
	vanta.LoadModel("Models/Vanta/vanta.obj");
	//Obtaculo jerarquico puertas
	puertaVanta1 = Model();
	puertaVanta1.LoadModel("Models/Vanta/puertaVanta.obj");

	puertaVanta2 = Model();
	puertaVanta2.LoadModel("Models/Vanta/puertaVanta.obj");

	luna1 = Model();
	luna1.LoadModel("Models/Luna/Luna.obj");
	
	luna2 = Model();
	luna2.LoadModel("Models/Luna/Luna.obj");

	//Texturas proyecto
		//Resorte
	resorte_T = Texture("Textures/Resorte/Metal.jpg");
	resorte_T.LoadTextureA();
	//Maquina
	maquina_T = Texture("Textures/Pinball/Azul_Rosa.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Pinball/Fondo.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Pinball/Melody.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Resorte/Metal.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Pinball/Negro.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Pinball/RosaOscuro.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Pinball/RosaPastel.jpg");
	maquina_T.LoadTextureA();
	//Flipper
	flipper_T = Texture("Textures/Flipper/MoradoOscuro.jpg");
	flipper_T.LoadTextureA();
	//Canica1
	canica1_T = Texture("Textures/Canicas/Holo.jpg");
	canica1_T.LoadTextureA();
	//Canica1
	canica2_T = Texture("Textures/Canicas/Metal1.jpg");
	canica2_T.LoadTextureA();
	//Obstaculo iluminado hongo1
	hongo1_T1 = Texture("Textures/Mushroom/Mushroom_1_BaseColor.jpg");
	hongo1_T1.LoadTextureA();

	//Texturas objeto jerarquico
	vantaBase_T = Texture("Textures/Vanta/base_Base_Color.jpg");
	vantaBase_T.LoadTextureA();

	vantaCuerpo_T = Texture("Textures/Vanta/vanta_Base_Color.jpg");
	vantaCuerpo_T.LoadTextureA();

	//vantapuerta_T = Texture("Textures/Vanta/puerta_Base_Color.jpg");
	//vantapuerta_T.LoadTextureA();
	

	//Textura lunitas
	luna_T = Texture("Textures/Luna/Luna_Base_Color.jpg");
	luna_T.LoadTextureA();


	std::vector<std::string> skyboxFaces;

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		//+/- (Todo el esceanrio)    //+/- (En donde ilumina que tan fuerte se ve)
		0.0f, 0.0f, -1.0f); //Vector de dirección de la luz - Eje de la luz
	mainLight2 = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);

	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual - Luz roja
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		//+/-Intensidad de color   //+/- Radiante de color
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

//Luz entre los flippers
	pointLights[1] = PointLight(0.9176470588235294f, 0.792156862745098f, 0.09411764705882353f, //Incrementra índice
		20.0f, 10.0f,
		//+/-Intensidad de color   //+/- Radiante de color
		45.0f, 130.0f, 120.0f, //Ubicación de la luz
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.8f, 8.0f,
		0.0f, 0.0f, 0.0f, //Posición de la luz
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//Spotlight del tablero completo
	spotLights[1] = SpotLight(0.0f, 0.8549019607843137f, 0.0f, //Color de luz VERDE
		0.0f, 1.0f,
		40.0f, 120.0f, -130.0f, //Posición
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		50.0f); //Ángulo - Ampliación de diametro
	spotLightCount++;

	/*
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f, //Color de luz AZUL
		1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f); //Ángulo - Ampliación de diametro
	spotLightCount++;
	*/

	/*//luz fija - Luz verde
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f, 
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //SE DEBE DE MOVER LA EL PREVIO
		15.0f); //Ángulo - Ampliación de diametro
	spotLightCount++;
	
//se crean mas luces puntuales y spotlight 
	//luz fija - Luz verde
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //SE DEBE DE MOVER LA EL PREVIO
		15.0f); //Ángulo - Ampliación de diametro
	spotLightCount++;*/



/*
//Spotlight del faro del carro
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f, //Color de luz AZUL
		1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f); //Ángulo - Ampliación de diametro
	spotLightCount++; */

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana

//Variables animación básica canica1
movCanica1_X = 125.0f;
movCanica1_Z = 100.0f;
movCanicaOffset = 1.0f;

recorrido1 = true;
recorrido2 = false;
recorrido3 = false;
recorrido4 = false;



	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (ciclos >= 10) {
			dia = !(dia);
			ciclos = 0;
			rotAspas += 10.0f;
		}
		//SkyBox
		if (dia) {
			skyboxFaces.clear();
			skyboxFaces.push_back("Textures/Skybox/rightD.jpg");
			skyboxFaces.push_back("Textures/Skybox/leftD.jpg");
			skyboxFaces.push_back("Textures/Skybox/bottomD.jpg");
			skyboxFaces.push_back("Textures/Skybox/topD.jpg");
			skyboxFaces.push_back("Textures/Skybox/backD.jpg");
			skyboxFaces.push_back("Textures/Skybox/frontD.jpg");
			skybox = Skybox(skyboxFaces);
		}
		else {
			skyboxFaces.clear();
			skyboxFaces.push_back("Textures/Skybox/rightN.jpg");
			skyboxFaces.push_back("Textures/Skybox/leftN.jpg");
			skyboxFaces.push_back("Textures/Skybox/bottomN.jpg");
			skyboxFaces.push_back("Textures/Skybox/topN.jpg");
			skyboxFaces.push_back("Textures/Skybox/backN.jpg");
			skyboxFaces.push_back("Textures/Skybox/frontN.jpg");
			skybox = Skybox(skyboxFaces);
		}
		ciclos++;

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		if (dia) {
			shaderList[0].SetDirectionalLight(&mainLight);
		}
		else {
			shaderList[0].SetDirectionalLight(&mainLight2);
		}

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

//Función para prender y apagar luces

	////Prende y apagar luz puntual entre flippers
	//	if (mainWindow.getLuzFlippers() == true) {
	//		shaderList[0].SetPointLights(pointLights, pointLightCount);
	//	}
	//	else {
	//		shaderList[0].SetPointLights(pointLights, pointLightCount - 2);
	//	}

	////Prende y apagar luz puntual entre flippers
	//	if (mainWindow.getLuzTablero() == true) {
	//		shaderList[0].SetSpotLights(spotLights, spotLightCount);
	//	}
	//	else {
	//		shaderList[0].SetSpotLights(spotLights, spotLightCount - 2);
	//	}

//Se implemeta el cambio de la cámara
		if (mainWindow.getcambiaCamara()) { //La tecla Z para la cámara isométrica
			camera2.keyControl(mainWindow.getsKeys(), deltaTime * 0.5f);
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		else {//La tecla X para la cámara normal
			camera.keyControl(mainWindow.getsKeys(), deltaTime * 5);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		}

//Animación básica de la canica1
		if (mainWindow.getAnimCanica1() == true) {
			if (recorrido1) {
				movCanica1_Z -= movCanicaOffset * deltaTime;
				//printf("movCanica1_Z %f\n", movCanica1_Z);
				if (movCanica1_Z < -70.0f) {
					recorrido1 = false;
					recorrido2 = true;
				}
			}
			if (recorrido2) { 
				movCanica1_X -= movCanicaOffset * deltaTime;
				if (movCanica1_X < -15.0f) {
					recorrido2 = false;
					recorrido3 = true;
				}
			}
			if (recorrido3) { 
				movCanica1_Z += movCanicaOffset * deltaTime;
				if (movCanica1_Z > 90.0f) {
					recorrido3 = false;
					recorrido3 = false;
					recorrido4 = true;
				}
			}
			if (recorrido4) {
				movCanica1_X += movCanicaOffset * deltaTime;
				if (movCanica1_X > 125.0f) {
					recorrido4 = false;
				}
			}		
		}
		if (mainWindow.getAnimCanica1() == false) {
			recorrido1 = true;
		}

		//
			

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));


//Resorte
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, 132.0f, 145.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(13.05f, 13.05f, 13.05f));

//Animación resorte
		if (mainWindow.getAnimResorte() == true) {
			cambioTam_1 = true;
			cambioTam_2 = false; 
			if (cambioTam_1) {
				model = glm::scale(model, glm::vec3(0.05f, 0.005f, 0.05f));
			}
		}

		if (mainWindow.getAnimResorte() == false) {
			animCanica1 = true; //Activando animación cuando se compacte resorte
			cambioTam_2 = true;
			cambioTam_1 = false;
			if (cambioTam_2) {
				model = glm::scale(model, glm::vec3(0.05f, 0.1f, 0.05f));
			}
		}

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		resorte_M.RenderModel();
		resorte_T.UseTexture();

//Dibujamos tablero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 110.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		maquina_M.RenderModel();
		maquina_T.UseTexture();

//Dibujamos fipplers
	//Dibujamos filper inf izq
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 123.0f, 100.0f));
		modelaux = model;
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, mainWindow.getMovFlipDer() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos filper inf der
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, 123.0f, 100.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getMovFlipIzq() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos filper sup izq
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-57.0f, 123.0f, -30.0f));
		model = glm::rotate(model, toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getMovFlipIzq2() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos hongo1 con textura iluminada
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 120.0f, -40.0f));
		model = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hongo1_T1.UseTexture();
		//hongo1_T2.UseTexture();
		hongo1_M.RenderModel();


		//Dibujamos obstaculo flor 1
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 115.0f, 50.0f));
		//model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//obstaculoFlor_M.RenderModel();

		////Dibujamos obstaculo flor 2
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(20.0f, 115.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 1.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//obstaculoFlor_M.RenderModel();

		//Dibujamos canica 1 - Animación básica
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCanica1_X, 122.0f, movCanica1_Z));
		//model = glm::translate(model, glm::vec3(125.0f, 122.0f, 100.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica1_T.UseTexture();
		canica1_M.RenderModel();

		//Dibujamos canica 2 - Animación Key frames
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 85.0f));
		//posCanica = glm::vec3(pos_X_Canica + movCanica_X, pos_Y_Canica, pos_Z_Canica + movCanica_Z);
		//model = glm::translate(model, posCanica);
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica2_T.UseTexture();
		canica2_M.RenderModel();

//---------------------------------------------------------------//
		//INSTANCIA 1 DEL OBJETO JERARQUICO

		//Dibujamos obstaculo jerarquico base (hexagono)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 123.0f, 50.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vantaBase_T.UseTexture();
		baseVanta.RenderModel();
		
		

		modelaux = model;

		//Dibujamos obstaculo jerarquico aspas
		model = glm::scale(model, glm::vec3(0.8F,0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotAspas), glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess); 
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vantaCuerpo_T.UseTexture();
		vanta.RenderModel();
		
		


		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta1

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta1.RenderModel();
	/*	vantapuerta_T.UseTexture();*/

		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta2

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta2.RenderModel();
		//vantapuerta_T.UseTexture();


		//INSTANCIA 2 DEL OBJETO JERARQUICO
		
		//Dibujamos obstaculo jerarquico base (hexagono)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(75.0f, 123.0f, -15.0f));
		model = glm::scale(model, glm::vec3(25.0f, 25.0f, 25.0f));
		
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vantaBase_T.UseTexture();
		baseVanta.RenderModel();
		


		modelaux = model;

		//Dibujamos obstaculo jerarquico aspas
		model = glm::scale(model, glm::vec3(0.8F, 0.8f, 0.8f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotAspas), glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		vantaCuerpo_T.UseTexture();
		vanta.RenderModel();
		

		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta1

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta1.RenderModel();
		//vantapuerta_T.UseTexture();

		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta2

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta2.RenderModel();
		//vantapuerta_T.UseTexture();



		//Dibujamos luna
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 123.0f, -75.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		luna1.RenderModel();
		luna_T.UseTexture();

		//Dibujamos luna
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 123.0f, 20.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		luna1.RenderModel();
		luna_T.UseTexture();


		/*
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.2f, 5.4f, -1.7f));
		model = glm::rotate(model, glm::radians(mainWindow.getangulo_pdelizq()), glm::vec3(0.0f, 0.1f, 0.1f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		PDELIZQ_M.RenderModel();
		*/

		/*
		//Agave 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);
		*/

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
