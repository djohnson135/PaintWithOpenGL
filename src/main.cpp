#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
int STROKE_SIZE = 1;

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow *window;


// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; };
color PIXEL_COLOR = { 1,0,0 };
// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

// Set a particular pixel of the frameBuffer to the provided color
void SetFrameBufferPixel(int x, int y, struct color lc)
{
	// Origin is upper left corner. 
	// Changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	frameBuffer[y][x][0] = lc.r;
	frameBuffer[y][x][1] = lc.g;
	frameBuffer[y][x][2] = lc.b;

}

void BrushDraw(int xpos, int ypos) {
	for (int i = 0; i < STROKE_SIZE; i++) {
		for (int j = 0; j < STROKE_SIZE; j++) {
			SetFrameBufferPixel(CLAMP((xpos + STROKE_SIZE/2) - i, 0, WINDOW_WIDTH), CLAMP((ypos + STROKE_SIZE / 2)-j, 0, WINDOW_HEIGHT), PIXEL_COLOR);
		}
	}
}

void ClearFrameBuffer()
{
	memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

// Display frameBuffer on screen
void Display()
{	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		//SetFrameBufferPixel((int)xpos, (int)ypos, PIXEL_COLOR);
		BrushDraw((int)xpos, (int)ypos);
		std::cout << "Mouse position is: x - " << xpos << ", y - " << ypos << std::endl;
	}
}

// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
	{	
		std::cout << "Mouse left button is pressed." << std::endl;
	}
}

// You can use "switch" or "if" to compare key to a specific character.
// for example,
// if (key == '0')
//     DO SOMETHING

// Keyboard callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch ((char)key) {
		case '+':
			STROKE_SIZE = CLAMP(STROKE_SIZE*2, 1, 128);
			break;
		case '-':
			STROKE_SIZE = CLAMP(STROKE_SIZE/2, 1, 128);
			break;
	}
	
	std::cout << "Key " << (char)key << " is pressed." << std::endl;
	std::cout << "Stroke Size " << STROKE_SIZE << std::endl;

}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1 - Dathan Johnson", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();
}


int main()
{	
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}