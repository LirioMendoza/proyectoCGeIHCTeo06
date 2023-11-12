#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

//Variables animación resorte
	animResorte = false;

//Movimienfo flipper der
	angFlip_Der = 0.0f;
//Movimienfo flipper izq
	angFlip_Izq = 0.0f;
//Movimienfo flipper izq
	angFlip_Izq2 = 0.0f;

//Cambio de cámara
	cambiaCamara = 0;

//Animación básica canica 1
	animCanica1 = false;

//Variables movimiento personaje
	movX = 0.0f;
	movZ = 0.0f;

//Estado luz flippers
	estadoLuzFlipper = true;

//Estado luz flippers
	estadoLuzLinterna = true;

	//Estado luz tablero
	estadoLuzTablero = true;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica07: Iluminacion 1", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
	glfwSetMouseButtonCallback(mainWindow, ManejaBotonesMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}


void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

//Movimiento Flipper der
	if (key == GLFW_KEY_1)
	{
		if(theWindow->angFlip_Der > 45.0f){
		}
		else {
			theWindow->angFlip_Der += 10.0f;
		}
		
	}
	if (key == GLFW_KEY_2)
	{
		if (theWindow->angFlip_Der < -25.0f) {
		}
		else {
			theWindow->angFlip_Der -= 10.0f;
		}

	}

//Movimiento Flipper izq
	if (key == GLFW_KEY_3)
	{
		if (theWindow->angFlip_Izq > 45.0f) {
		}
		else {
			theWindow->angFlip_Izq += 10.0f;
		}

	}

	if (key == GLFW_KEY_4)
	{
		if (theWindow->angFlip_Izq < -25.0f) {
		}
		else {
			theWindow->angFlip_Izq -= 10.0f;
		}

	}

//Movimiento personaje (eje Z)
	if (key == GLFW_KEY_U)
	{
		theWindow->movZ -= 1.0f;
	}

	if (key == GLFW_KEY_J)
	{
		theWindow->movZ += 1.0f;
	}

//Movimiento personaje (eje x)
	if (key == GLFW_KEY_H)
	{
		theWindow->movX -= 1.0f;
	}

	if (key == GLFW_KEY_K)
	{
		theWindow->movX += 1.0f;
	}


	if (key == GLFW_KEY_6)
	{
		if (theWindow->angFlip_Izq2 < -25.0f) {
		}
		else {
			theWindow->angFlip_Izq2 -= 10.0f;
		}

	}

//Luz flippers
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		theWindow->estadoLuzFlipper = !(theWindow->estadoLuzFlipper);
	}

	//Luz linterna
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->estadoLuzLinterna = !(theWindow->estadoLuzLinterna);
	}

	//Luz tablero
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		theWindow->estadoLuzTablero = !(theWindow->estadoLuzTablero);
	}

//Cambio de cámara
	if (key == GLFW_KEY_Z)//Activación de la camara ISOMÉTRICA
	{
		theWindow->cambiaCamara = 1;
		printf("\nSe hizo el cambio para utilizar la camara de tipo Isometrica.\n");
	}
	if (key == GLFW_KEY_X)////Activación de la camara NORMAL
	{
		theWindow->cambiaCamara = 0;
		printf("\nSe hizo el cambio para utilizar la camara de tipo normal.\n");
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void Window::ManejaBotonesMouse(GLFWwindow* window, int boton, int action, int mod)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	//Animación resorte
	if (boton == GLFW_MOUSE_BUTTON_LEFT) //Clic izquierdo --> GLFW_MOUSE_BUTTON_LEFT
	{
		theWindow->animResorte = true;
		theWindow->animCanica1 = false;
	}
	if (boton == GLFW_MOUSE_BUTTON_RIGHT) //Clic derecho --> GLFW_MOUSE_BUTTON_RIGHT
	{
		theWindow->animResorte = false;
		theWindow->animCanica1 = true;
	}
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
