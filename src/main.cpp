#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
int BRUSH_SIZE = 1;

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH];
GLFWwindow *window;


// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; };
color BRUSH_COLOR = { 1,0,0 };
color BACKGROUND_COLOR = { 0,0,0 };
// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

enum Brush { square, circle };
int Brush = square;
bool isSprayPaint = false;


bool CheckMaskPixel(int x, int y) {
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);
	return mask[y][x];
}

void SetMaskPixel(int x, int y, bool isDrawn) {
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	mask[y][x] = isDrawn;
}
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

void PaintSpray(int x, int y, double chance) {
	double rn = rand() % 10000;
	rn /= 10000.0;
	if (chance > rn) {
		SetFrameBufferPixel(x, y, BRUSH_COLOR);
		SetMaskPixel(x, y, true);
	}
}

void PaintSquare(int xpos, int ypos) {
	int x = xpos - BRUSH_SIZE;
	int y = ypos - BRUSH_SIZE;

	int x_upperbound = CLAMP(xpos + BRUSH_SIZE, 0, WINDOW_WIDTH - 1);
	int y_upperbound = CLAMP(ypos + BRUSH_SIZE, 0, WINDOW_HEIGHT - 1);

	double chance = rand() % 100;
	chance /= 1000.0;

	for (x; x <= x_upperbound; x++) {
		for (y; y <= y_upperbound; y++) {
			if (isSprayPaint) {
				PaintSpray(x, y, chance);
			}
			else {
				SetFrameBufferPixel(x, y, BRUSH_COLOR);
				SetMaskPixel(x, y, true);
			}
		}
		y = ypos - BRUSH_SIZE;
	}
}

void PaintCircle(int xpos, int ypos) {
	double chance = rand() % 100;
	chance /= 1000.0;
	for (int x = -BRUSH_SIZE; x <= BRUSH_SIZE; x++) {
		for (int y = -BRUSH_SIZE; y <= BRUSH_SIZE; y++) {
			if (x * x + y * y < BRUSH_SIZE * BRUSH_SIZE + BRUSH_SIZE) {
				if (isSprayPaint) {
					PaintSpray(xpos + x, ypos + y, chance);
				}
				else {
					SetFrameBufferPixel(xpos + x, ypos + y, BRUSH_COLOR);
					SetMaskPixel(xpos + x, ypos + y, true);
				}	
			}
		}
	}
}




void Paint(int xpos, int ypos) {
	switch (Brush){
		case square:
			PaintSquare(xpos, ypos);
			break;
		case circle:
			PaintCircle(xpos, ypos);
			break;
		default:
			PaintSquare(xpos, ypos);
			break;
	}
}

void SetBackgroundColor() {
	for (int x = 0; x < WINDOW_WIDTH; x++) {
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			if (!CheckMaskPixel(x,y)) {
				SetFrameBufferPixel(x, y, BACKGROUND_COLOR);
			}			
		}
	}
}

void ClearFrameBuffer()
{
	memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

void ClearMask() {
	memset(mask, false, sizeof(bool) * WINDOW_WIDTH * WINDOW_HEIGHT);
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
		Paint((int)xpos, (int)ypos);
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
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		std::cout << "Mouse right button is pressed " << std::endl;
		ClearMask();
		SetBackgroundColor();
	}
}

color DigitToColor(int digit) {
	int binary[32];
	int iter = 0;
	while (iter < 3) {
		if (digit == 0) {
			binary[iter++] = 0;
		}
		if (digit % 2 == 0) binary[iter++]=0;
		else binary[iter++]=1;
		digit /= 2;
	}
	color Color = {
		(float)binary[2],
		(float)binary[1],
		(float)binary[0]
	};
	return Color;
}
// You can use "switch" or "if" to compare key to a specific character.
// for example,
// if (key == '0')
//     DO SOMETHING

// Keyboard callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	if (std::isdigit(char(key))) {
		int digit = char(key) - '0';
		if (digit >= 0 && digit <= 7) {
			BRUSH_COLOR = DigitToColor(digit);
		}
	}
	else {
		switch ((char)key) {
			case '+':
				BRUSH_SIZE = CLAMP(BRUSH_SIZE * 2, 1, 128);
				break;
			case '-':
				BRUSH_SIZE = CLAMP(BRUSH_SIZE / 2, 1, 128);
				break;
			case ')':
				BACKGROUND_COLOR = DigitToColor(0);
				SetBackgroundColor();
				break;
			case '!':
				BACKGROUND_COLOR = DigitToColor(1);
				SetBackgroundColor();
				break;
			case '@':
				BACKGROUND_COLOR = DigitToColor(2);
				SetBackgroundColor();
				break;
			case '#':
				BACKGROUND_COLOR = DigitToColor(3);
				SetBackgroundColor();
				break;
			case '$':
				BACKGROUND_COLOR = DigitToColor(4);
				SetBackgroundColor();
				break;
			case '%':
				BACKGROUND_COLOR = DigitToColor(5);
				SetBackgroundColor();
				break;
			case '^':
				BACKGROUND_COLOR = DigitToColor(6);
				SetBackgroundColor();
				break;
			case '&':
				BACKGROUND_COLOR = DigitToColor(7);
				SetBackgroundColor();
				break;
			case 'b':
				Brush = (Brush == square)? circle : square;
				break;
			case 's':
				isSprayPaint = !isSprayPaint;
				break;
			default:
				break;
		}
	}
	std::cout << "Key " << (char)key << " is pressed." << std::endl;
	std::cout << "Stroke Size " << BRUSH_SIZE << std::endl;
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
	ClearMask();
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