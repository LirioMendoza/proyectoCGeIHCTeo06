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

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

//Implementaci�n de c�maras
Camera camera;
Camera camera2;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
//Nueva textura
Texture lamparaTextura;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
//Nuevo modelo
Model lampara;

//Modelos proyecto
Model resorte_M;
Model maquina_M;
Model flipper_M;
Model obstaculoFlor_M;
Model canica1_M;
Model canica2_M;
Model hongo1_M;

//Texturas proyecto
Texture resorte_T;
Texture maquina_T;
Texture flipper_T;
Texture canica1_T;
Texture canica2_T;
Texture hongo1_T1;
Texture hongo1_T2;

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
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Variables proyecto

//Funci�n para interactuar con teclado para animaci�n por Key frames
void inputKeyframes(bool* keys);

//Cambio de tama�o del resorte
bool cambioTam_1;
bool cambioTam_2;

//Animaci�n b�sica canica1
float movCanica1_X;
float movCanica1_Z;
float movCanicaOffset;

bool animCanica1;
bool recorrido1;
bool recorrido2;
bool recorrido3;
bool recorrido4;

//Variables animaci�n por Key frames
float iniciaAnimacion;
float activaAnimacion;
bool animacion = false;

//Variables animaci�n canica2
float pos_X_Canica2 = 70.0f; //Hay que inicializar en la posici�n inicial
float pos_Y_Canica2 = 119.0f; 
float pos_Z_Canica2 = 4.0f;

float movCanica2_X = 0;
float movCanica2_Z = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 7;

typedef struct _frame
{
	//Variables para guardar Key Frames canica2
	float movCanica2_X;
	float movCanica2_Z;
	float movCanica2_X_Inc;
	float movCanica2_Z_Inc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 7; //Cantidad de Keyframes
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{
	printf("frameindex %d\n", FrameIndex); //Imprimiendo key frames en pantalla

	KeyFrame[FrameIndex].movCanica2_X = movCanica2_X;
	KeyFrame[FrameIndex].movCanica2_Z = movCanica2_Z;

	FrameIndex++;
}

void resetElements(void) //Tecla 0
{
	movCanica2_X = KeyFrame[0].movCanica2_X;
	movCanica2_Z = KeyFrame[0].movCanica2_Z;
}

void interpolation(void)
{
	KeyFrame[playIndex].movCanica2_X_Inc = (KeyFrame[playIndex + 1].movCanica2_X - KeyFrame[playIndex].movCanica2_X) / i_max_steps;
	KeyFrame[playIndex].movCanica2_Z_Inc = (KeyFrame[playIndex + 1].movCanica2_Z - KeyFrame[playIndex].movCanica2_Z) / i_max_steps;
}


void animate(void) //Barra espacio
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animaci�n entre frames?
		{
			playIndex++;
			printf("\nReproduciendo cuadro no. =  %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//Fin de toda la animaci�n con �ltimo frame?
			{
				printf("\nIndice / Total de cuadros = %d\n", FrameIndex);
				printf("\nTermino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else //Interpolaci�n del pr�ximo cuadro
			{
				i_curr_steps = 0; //Resetea contador
				//Interpolar
				interpolation();
			}
		}
		else
		{
			movCanica2_X += KeyFrame[playIndex].movCanica2_X_Inc;
			movCanica2_Z += KeyFrame[playIndex].movCanica2_Z_Inc;
			i_curr_steps++;

		}

	}
}

//funci�n de calculo de normales por promedio de v�rtices 
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

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

//Modelos proyecto
	//Resorte
	resorte_M = Model();
	resorte_M.LoadModel("Models/Proyecto/Resorte.obj");
	//Maquina
	maquina_M = Model();
	maquina_M.LoadModel("Models/Proyecto/Maquina.obj");
	//Flipper
	flipper_M = Model();
	flipper_M.LoadModel("Models/Proyecto/Flipper.obj");
	//Obstaculo flor
	obstaculoFlor_M = Model();
	obstaculoFlor_M.LoadModel("Models/Proyecto/cartoon_flower.obj");
	//Canica1
	canica1_M = Model();
	canica1_M.LoadModel("Models/Proyecto/Canica1.obj");
	//Canica2
	canica2_M = Model();
	canica2_M.LoadModel("Models/Proyecto/Canica2.obj");
	//Obstaculo iluminado hongo1
	hongo1_M = Model();
	hongo1_M.LoadModel("Models/Proyecto/Mushroom_1.obj");


//Texturas proyecto
	//Resorte
	resorte_T = Texture("Textures/Metal.jpg");
	resorte_T.LoadTextureA();
	//Maquina
	maquina_T = Texture("Textures/Azul_Rosa.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Fondo.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Melody.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Metal.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/Negro.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/RosaOscuro.jpg");
	maquina_T.LoadTextureA();
	maquina_T = Texture("Textures/RosaPastel.jpg");
	maquina_T.LoadTextureA();
	//Flipper
	flipper_T = Texture("Textures/MoradoOscuro.jpg");
	flipper_T.LoadTextureA();
	//Canica1
	canica1_T = Texture("Textures/Holo.jpg");
	canica1_T.LoadTextureA();
	//Canica1
	canica2_T = Texture("Textures/Metal1.jpg");
	canica2_T.LoadTextureA();
	//Obstaculo iluminado hongo1
	hongo1_T1 = Texture("Textures/Mushroom_1_BaseColor.jpg");
	hongo1_T1.LoadTextureA();

	hongo1_T2 = Texture("Textures/Mushroom_1_BaseColor.jpg");
	hongo1_T2.LoadTextureA();


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		//+/- (Todo el esceanrio)    //+/- (En donde ilumina que tan fuerte se ve)
		0.0f, 0.0f, -1.0f); //Vector de direcci�n de la luz - Eje de la luz
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual - Luz roja
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		//+/-Intensidad de color   //+/- Radiante de color
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

//Luz entre los flippers
	pointLights[1] = PointLight(0.9176470588235294f, 0.792156862745098f, 0.09411764705882353f, //Incrementra �ndice
		20.0f, 10.0f,
		//+/-Intensidad de color   //+/- Radiante de color
		45.0f, 130.0f, 120.0f, //Ubicaci�n de la luz
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.8f, 4.0f,
		0.0f, 0.0f, 0.0f, //Posici�n de la luz
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	//Spotlight del tablero completo
	spotLights[1] = SpotLight(0.0f, 0.8549019607843137f, 0.0f, //Color de luz VERDE
		0.0f, 1.0f,
		40.0f, 120.0f, -130.0f, //Posici�n
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		50.0f); //�ngulo - Ampliaci�n de diametro
	spotLightCount++;

	/*
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f, //Color de luz AZUL
		1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f); //�ngulo - Ampliaci�n de diametro
	spotLightCount++;
	*/

	/*//luz fija - Luz verde
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f, 
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //SE DEBE DE MOVER LA EL PREVIO
		15.0f); //�ngulo - Ampliaci�n de diametro
	spotLightCount++;
	
//se crean mas luces puntuales y spotlight 
	//luz fija - Luz verde
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f, //SE DEBE DE MOVER LA EL PREVIO
		15.0f); //�ngulo - Ampliaci�n de diametro
	spotLightCount++;*/



/*
//Spotlight del faro del carro
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f, //Color de luz AZUL
		1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f); //�ngulo - Ampliaci�n de diametro
	spotLightCount++; */

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana

//Variables animaci�n b�sica canica1 
	movCanica1_X = 125.0f;
	movCanica1_Z = 100.0f;
	movCanicaOffset = 1.0f;

	recorrido1 = true;
	recorrido2 = false;
	recorrido3 = false;
	recorrido4 = false;

//Variables animaci�n por key frames canica2 
	glm::vec3 posicionCanica2 = glm::vec3(0.0f, 0.0f, 0.0f);

	KeyFrame[0].movCanica2_X = 0.0f;
	KeyFrame[0].movCanica2_Z = 0.0f;

	KeyFrame[1].movCanica2_X = -60.0f; //Se desplaza a la flor morada
	KeyFrame[1].movCanica2_Z = -40.0f; 

	KeyFrame[2].movCanica2_X = -10.0f; //Se desplaza a la flor azul
	KeyFrame[2].movCanica2_Z = -82.0f; 

	KeyFrame[3].movCanica2_X = -60.0f; //Se desplaza al hongo
	KeyFrame[3].movCanica2_Z = -125.0f; 

	KeyFrame[4].movCanica2_X = -10.0f; //Se desplaza a la flor azul de regreso
	KeyFrame[4].movCanica2_Z = -82.0f;

	KeyFrame[5].movCanica2_X = -60.0f; //Se desplaza a la flor morada de regreso
	KeyFrame[5].movCanica2_Z = -40.0f;

	KeyFrame[6].movCanica2_X = 0.0f; //Regresa a su posici�n inicial
	KeyFrame[6].movCanica2_Z = 0.0f;

	printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");

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
		
		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

//Se implemeta el cambio de la c�mara
		if (mainWindow.getcambiaCamara()) { //La tecla Z para la c�mara isom�trica
			camera2.keyControl(mainWindow.getsKeys(), deltaTime * 0.5f);
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		else {//La tecla X para la c�mara normal
			camera.keyControl(mainWindow.getsKeys(), deltaTime * 5);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		}

//Animaci�n b�sica de la canica1
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
		 
		//Animaci�n key frames
		inputKeyframes(mainWindow.getsKeys());
		animate();

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();
		/*
		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.5f, -3.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();
		//Fijamos la luz al automovil para que este se mueva cuando el lo haga
		spotLights[2].SetFlash(glm::vec3(mainWindow.getmuevex(), 0.5f, 0.0f), glm::vec3(-1.0, 0.0f, 0.0f));

		//Llanta delantera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);//llanta con color gris
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta delantera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(7.0f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(15.5f, -0.5f, 1.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();
	
//Helicoptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevexH(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		//Fijamos luz del helicoptero
		spotLights[1].SetFlash(glm::vec3(mainWindow.getmuevexH(), 0.5f, 6.0f), glm::vec3(0.0, -1.0f, 0.0f));

//Dibujamos la lampara 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, -1.0f, 30.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara.RenderModel();
		lamparaTextura.UseTexture();
*/
//Resorte
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(125.0f, 132.0f, 145.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(15.05f, 15.05f, 15.05f));

//Animaci�n resorte
		if (mainWindow.getAnimResorte() == true) {
			cambioTam_1 = true;
			cambioTam_2 = false; 
			if (cambioTam_1) {
				model = glm::scale(model, glm::vec3(0.05f, 0.005f, 0.05f));
			}
		}

		if (mainWindow.getAnimResorte() == false) {
			animCanica1 = true; //Activando animaci�n cuando se compacte resorte
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

		//Dibujamos filper sup der
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(85.0f, 123.0f, -40.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getMovFlipIzq2() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos hongo1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 120.0f, -40.0f));
		model = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hongo1_T1.UseTexture();
		hongo1_T2.UseTexture();
		hongo1_M.RenderModel();


		//Dibujamos obstaculo flor 1 (Morada)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 115.0f, 50.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		obstaculoFlor_M.RenderModel();

		//Dibujamos obstaculo flor 2 (Azul)
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60.0f, 115.0f, 0.0f));
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		obstaculoFlor_M.RenderModel();

		//Dibujamos canica 1 - Animaci�n b�sica
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCanica1_X, 122.0f, movCanica1_Z));
		//model = glm::translate(model, glm::vec3(125.0f, 122.0f, 100.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica1_T.UseTexture();
		canica1_M.RenderModel();

		//Dibujamos canica 2 - Animaci�n Key frames
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 85.0f));
		posicionCanica2 = glm::vec3(pos_X_Canica2 + movCanica2_X, pos_Y_Canica2, pos_Z_Canica2 + movCanica2_Z);
		//model = glm::translate(model, glm::vec3(80.0f, 123.0f, 100.0f));
		model = glm::translate(model, posicionCanica2);
		modelaux = model;
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica2_T.UseTexture();
		canica2_M.RenderModel();

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

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (iniciaAnimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				iniciaAnimacion++;
				printf("\nPresiona la tecla 0 para habilitar reproducir de nuevo la animacion\n");
				activaAnimacion = 0;

			}
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (activaAnimacion < 1 && iniciaAnimacion > 0)
		{
			printf("\nYa puedes reproducir de nuevo la animacion con la tecla de barra espaciadora\n");
			iniciaAnimacion = 0;

		}
	}
}
