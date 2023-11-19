#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();

//Animación resorte
	GLboolean getAnimResorte() { return animResorte;  }

//Movimiento Flipper der
	GLfloat getMovFlipDer() { return angFlip_Der; }
//Movimiento Flipper izq
	GLfloat getMovFlipIzq() { return angFlip_Izq; }
//Movimiento Flipper izq sup
	GLfloat getMovFlipIzq2() { return angFlip_Izq2; }

//Cambio cámara
	GLint getcambiaCamara() { return cambiaCamara; }

//Animación básica canica1
	GLboolean getAnimCanica1() { return animCanica1; }

//Prendido y apagado luz entre flippers
	GLboolean getLuzFlippers() { return estadoLuzFlipper; }

//Prendido y apagado luz tablero
	GLboolean getLuzTablero() { return estadoLuzTablero; }

	//Prendido y apagado luz linterna
	GLboolean getLuzLinterna() { return estadoLuzLinterna; }

//Movimiento personaje
	GLfloat getMovX() { return movX; }
	GLfloat getMovZ() { return movZ; }

	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevexH;

//Animación resorte
	GLboolean animResorte;

//Movimiento Flipper der
	GLfloat angFlip_Der = 0.0f;
//Movimiento Flipper izq
	GLfloat angFlip_Izq = 0.0f;
//Movimiento Flipper izq
	GLfloat angFlip_Izq2 = 0.0f;

//Cambio de cámara
	GLint cambiaCamara;

//Animación básica canica1
	GLboolean animCanica1;

//Prendido y apagado luz entre flippers
	GLboolean estadoLuzFlipper;

//Prendido y apagado luz tablero
	GLboolean estadoLuzTablero; 

	//Prendido y apagado luz linterna
	GLboolean estadoLuzLinterna;

//Variables movimiento personaje
	GLfloat movX;
	GLfloat movZ;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

//Función para usar los botones del mouse
	static void ManejaBotonesMouse(GLFWwindow* window, int boton, int action, int mod);
};

