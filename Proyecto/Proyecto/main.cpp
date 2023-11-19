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

Model melody;

Model luna1;

//Texturas proyecto
Texture resorte_T;
Texture maquina_T;
Texture flipper_T;
Texture canica1_T;
Texture canica2_T;
Texture hongo1_T1;
Texture hongo1_T2;
Texture piramide_T;

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

//Luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
PointLight pointLights3[MAX_POINT_LIGHTS];

//Luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//Variables proyecto

// Variables aux iluminación cuarto
bool dia = true;
int ciclos = 0;

//Variables contro de luces Objetos jerárquicos
bool luzObJ1 = true;
bool luzObJ2 = true;

//Función para interactuar con teclado para animaci�n por Key frames
void inputKeyframes(bool* keys);

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

//Variables animación por Key frames
float iniciaAnimacion;
float activaAnimacion;
bool animacion = false;

//Variables animación canica2
float pos_X_Canica2 = 70.0f; //Hay que inicializar en la posici�n inicial
float pos_Y_Canica2 = 119.0f; 
float pos_Z_Canica2 = 4.0f;

float movCanica2_X = 0;
float movCanica2_Z = 0;

#define MAX_FRAMES 100
int i_max_steps = 90;
int i_curr_steps = 7;

//Variables animaci�n piramide
float rot_piramide;
float rot_piramide_Offset;

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

void Crear_piramide()
{
	unsigned int piramide_indices[] = {
	   0, 1, 2,
	   3, 4, 5,
	   6, 7, 8,

	};

	GLfloat vertices_piramide[] = {
		// Cara base			 S       T			NX       NY       NZ     
		 0.0f, 0.0f, 0.0f,		0.5f,  0.66f,		0.0f,	1.0f,	0.0f,
		1.0f, 0.0f, 0.0f,		0.33f,  0.33f,		0.0f,	1.0f,	0.0f,
		 0.5f, 1.0f, 0.0f,		0.66f,  0.33f,		0.0f,	1.0f,	0.0f,

		// Cara frontal
		 0.0f, 0.0f, 0.0f,		0.8f,  0.66f,		0.0f,   1.0f,   0.0f,
		 0.5f, 1.0f, 0.0f,		0.66f,  0.33f,		0.0f,   1.0f,   0.0f,
		0.5f, 0.0f, 1.0f,		1.0f,  0.33f,		0.0f,   1.0f,   0.0f,

		// Cara superior
		 1.0f, 0.0f, 0.0f,		0.2f,  0.66f,		0.0f,   0.0f,   1.0f,
		0.5f, 1.0f, 0.0f,		0.0f,  0.33f,		0.0f,   0.0f,   1.0f,
		 0.5f, 0.0f, 1.0f,		0.33f,  0.33f,		0.0f,   0.0f,   1.0f,
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(vertices_piramide, piramide_indices, 72, 9); //24 X 3 
	meshList.push_back(piramide);

}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	Crear_piramide();

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

	//Personaje
	melody = Model();
	melody.LoadModel("Models/Melody/melody.obj");

  //Obstáculos
	luna1 = Model();
	luna1.LoadModel("Models/Luna/Luna.obj");


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

	//Piramide por codigo
	piramide_T = Texture("Textures/text_piramide.jpg");
	piramide_T.LoadTextureA();

	//Texturas objeto jerarquico
	vantaBase_T = Texture("Textures/Vanta/base_Base_Color.jpg");
	vantaBase_T.LoadTextureA();

	vantaCuerpo_T = Texture("Textures/Vanta/vanta_Base_Color.jpg");
	vantaCuerpo_T.LoadTextureA();

	/*vantapuerta_T = Texture("Textures/Vanta/puerta_Base_Color.jpg");
	vantapuerta_T.LoadTextureA();*/
	

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
	
	//Luz entre los flippers
	pointLights[0] = PointLight(0.9176470588235294f, 0.792156862745098f, 0.09411764705882353f, 
		20.0f, 10.0f,
		//+/-Intensidad de color   //+/- Radiante de color
		45.0f, 130.0f, 120.0f, //Ubicación de la luz
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//Luz objeto jerárquico 1
	pointLights[1] = PointLight(0.0f, 1.0f, 1.5f,
		1.0f, 1.0f,
		45.0f, 130.0f, 50.0f,
		0.0f, 0.0f, 0.01f);
	pointLightCount++;

	//Luz objeto jerárquico 2
	pointLights[2] = PointLight(0.0f, 1.0f, 1.5f,
		1.0f, 1.0f,
		75.0f, 130.0f, -15.0f,
		0.0f, 0.0f, 0.01f);
	pointLightCount++;

	//Segundo arreglo luz puntual 
	pointLights2[0] = pointLights[1]; //Obj J1
	pointLights2[1] = pointLights[2]; //Obj J2
	pointLights2[2] = pointLights[0]; //flipper

	//Tercer arreglo luz puntual 
	pointLights3[0] = pointLights[2]; //Obj J2
	pointLights3[1] = pointLights[0]; //flipper
	pointLights3[2] = pointLights[1]; //Obj J1 

	unsigned int spotLightCount = 0;
	//Linterna personaje
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.2f,
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
  
	// Segundo arreglo luz spot
	spotLights2[0] = spotLights[1]; //Luz tablero
	spotLights2[1] = spotLights[0]; //Linterna personaje

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana

//Variables animación piramide
	rot_piramide = 0.0f;
	rot_piramide_Offset = 5.0f; //Velocidad de giro de la piramide

//Variables animación baica canica1 
	movCanica1_X = 125.0f;
	movCanica1_Z = 100.0f;
	movCanicaOffset = 1.0f;

	recorrido1 = true;
	recorrido2 = false;
	recorrido3 = false;
	recorrido4 = false;

//Variables animación por key frames canica2 
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
		
	//Funciones para prender y apagar luces
    
	//Point Lights
		if (mainWindow.getLuzFlippers() && luzObJ1 && luzObJ2) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}
		else if (mainWindow.getLuzFlippers() && luzObJ1 && !luzObJ2) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
		else if (mainWindow.getLuzFlippers() && !luzObJ1 && luzObJ2) {
			shaderList[0].SetPointLights(pointLights2, pointLightCount - 1);
		}
		else if (mainWindow.getLuzFlippers() && !luzObJ1 && !luzObJ2) {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 2);
		}
		else if (!mainWindow.getLuzFlippers() && luzObJ1 && luzObJ2) {
			shaderList[0].SetPointLights(pointLights2, pointLightCount - 1);
		}
		else if (!mainWindow.getLuzFlippers() && luzObJ1 && !luzObJ2) {
			shaderList[0].SetPointLights(pointLights2, pointLightCount - 2);
		}
		else if (!mainWindow.getLuzFlippers() && !luzObJ1 && luzObJ2) {
			shaderList[0].SetPointLights(pointLights3, pointLightCount - 1);
		}

	//Spot Lights
		if (mainWindow.getLuzTablero() && mainWindow.getLuzLinterna()) {
			spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
			shaderList[0].SetSpotLights(spotLights, spotLightCount);
		}else if (mainWindow.getLuzTablero() && !mainWindow.getLuzLinterna()) {
			spotLights2[1].SetFlash(lowerLight, camera.getCameraDirection());
			shaderList[0].SetSpotLights(spotLights2, spotLightCount - 1);
		}
		else if (!mainWindow.getLuzTablero() && mainWindow.getLuzLinterna()) {
			spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 2);
		}

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
		 
		//Animaci�n piramide
		rot_piramide += rot_piramide_Offset * deltaTime;

		//Animaci�n key frames
		inputKeyframes(mainWindow.getsKeys());
		animate();

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

//Piramide por código 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 119.f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, rot_piramide * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		piramide_T.UseTexture();
		meshList[4]->RenderMesh();

//Piramide por código 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(90.0f, 119.f, 50.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, rot_piramide * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		piramide_T.UseTexture();
		meshList[4]->RenderMesh();


//Resorte
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(128.0f, 132.0f, 150.0f));
		model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

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
		//resorte_T.UseTexture();

//Dibujamos tablero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, 110.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
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
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos filper inf der
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(80.0f, 123.0f, 100.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getMovFlipIzq() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

		//Dibujamos filper sup izq
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-57.0f, 123.0f, -30.0f));
		model = glm::rotate(model, toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, mainWindow.getMovFlipIzq2() * toRadians, glm::vec3(0.0f, 0.1f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		flipper_M.RenderModel();
		flipper_T.UseTexture();

//Dibujamos hongo1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 120.0f, -40.0f));
		model = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
		modelaux = model;
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		hongo1_T1.UseTexture();
		hongo1_M.RenderModel();

//Dibujamos canica 1 - Animación básica
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCanica1_X, 122.0f, movCanica1_Z));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.25f, 1.25f, 1.25f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica1_T.UseTexture();
		canica1_M.RenderModel();

//Dibujamos canica 2 - Animación Key frames
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 85.0f));
		posicionCanica2 = glm::vec3(pos_X_Canica2 + movCanica2_X, pos_Y_Canica2, pos_Z_Canica2 + movCanica2_Z);
		model = glm::translate(model, posicionCanica2);
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.25f, 1.25f, 1.25f));
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		canica2_T.UseTexture();
		canica2_M.RenderModel();

		//INSTANCIA PERSONAJE
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(110.0f+mainWindow.getMovX(), 121.5f, 100.0f+mainWindow.getMovZ()));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		melody.RenderModel();

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
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta1.RenderModel();

		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta2

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta2.RenderModel();


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


		model = modelaux;

		//Dibujamos obstaculo jerarquico puerta2

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.82f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		color = glm::vec3(0.9254901960784314f, 0.45098039215686275f, 0.9137254901960784f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puertaVanta2.RenderModel();

		//OBSTÁCULOS LUNA

		//Dibujamos luna 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(45.0f, 123.0f, -75.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		luna1.RenderModel();
		luna_T.UseTexture();

		//Dibujamos luna 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 123.0f, 20.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		luna1.RenderModel();
		luna_T.UseTexture();

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
