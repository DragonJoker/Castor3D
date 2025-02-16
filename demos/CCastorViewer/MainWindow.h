#ifndef ___CCV_MainWindow___
#define ___CCV_MainWindow___

#include "NodeState.h"

#include <stdbool.h>
#include <time.h>

#define MAX_PATH_SIZE 1024

typedef struct GLFWwindow GLFWwindow;

typedef struct MainWindow_ MainWindow;
typedef struct Timer_ Timer;
typedef void ( * TimerProc )( MainWindow * );

enum TIMER_ID_
{
	IDT_MOTION,
	IDT_LEFT,
	IDT_RIGHT,
	IDT_FORWARD,
	IDT_BACK,
	IDT_UP,
	IDT_DOWN,
	IDT_COUNT,
};
typedef enum TIMER_ID_ TIMER_ID;

struct Timer_
{
	clock_t startTime;
	bool started;
	TimerProc callback;
};

struct MainWindow_
{
	C3DRenderTarget * renderTarget;
	C3DRenderWindow * renderWindow;
	C3DScene * scene;
	C3DCamera * camera;
	C3DSceneNode * cameraNode;
	NodeState nodeState;
	C3DVec2 oldPosition;
	Timer timers[IDT_COUNT];

	bool isShiftDown;
	bool isCtrlDown;
	bool isAltDown;
};

bool windowCreate( GLFWwindow * glfwWindow, C3DEngine * engine, MainWindow * result );
void windowDestroy( MainWindow * window );

#endif
