//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//     - updated by FLL
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <crtdbg.h>
#include <Draw.h>
#include <iostream>
#include <nixLibrary\nixLibrary.h> // NIX
#include "NixHandler.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <gl/GL.h>
#include <SOIL2.h>
#include "ParticleSystem.h"
#include "Player.h"

//-----Audio/Sound-----//
#include <irrKlang.h>
using namespace irrklang;
ISoundEngine *SoundEngine = createIrrKlangDevice();
ISound* fireplaceAudio;
ISound* clockAudio;
ISound* backgroundMusic;
//-----3D Sound Positions-----//
vec3df fireplacePos3D(77.859f, 1.0f, 11.713f);
vec3df clockPos3D(-27.594f, 20.448f, 21.875f);

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#define WIDTH 1440
#define HEIGHT 900
GLFWwindow *gWindow;

void initWindow(unsigned int w, unsigned int h);

//-----full screen quad stuff-----//
GLuint gVertexBufferFS = 0;
GLuint gVertexAttributeFS = 0;
GLuint gShaderProgramFS = 0;

//-----Particles-----//
GLuint gVertexAttributePartical = 0;
GLuint gShaderProgramPartical = 0;
GLuint gShaderProgramDepth = 0;

GLuint gShaderProgramNix = 0;


//-----Particle-----//
unsigned int particelTextur;
unsigned int particelTextur2;
unsigned int interactTXT;

unsigned int fire1;
unsigned int smoke1;
unsigned int sparks1;
//-----??-----//
float gTx, gTy;
float gFloat = 0;
float gClearColour[3] {};

//-----Color From ImGui-----//
float gUniformColour[3] {};
GLint gUniformColourLoc = -1;

//-----Animation?-----//
float gOffsetX = 0.0f;
float gIncrement = 0.0f;
float gRotateZ = 0.0f;

//-----Collision-----//
glm::vec3 lastPos;

//-----light-----//
glm::vec3 lightPosR0[3] = { glm::vec3(-50.0f, 5.0f, 12.0f), glm::vec3(50.0f, 5.0f, 35.0f), glm::vec3(0.0f, 20.0f, 12.0f) };
glm::vec3 lightPos[2] = { glm::vec3(74.3f, 20.2f, 5.5f), glm::vec3(75.859, 2.0f, 11.713f)};

//-----Move everything in scene-----//
glm::vec3 position(0.0f, 0.0f, 0.0f);
glm::vec3 rotation(0.0f, 1.0f, 0.0f);
glm::vec3 rotation2(0.0f, 0.0f, 1.0f);
glm::vec3 testRotation(0.0f, 0.0f, 0.0f);
glm::vec3 updateScale(10.0f, 10.0f, 10.0f);

//-----Camera-----//
glm::vec3 camPos;
glm::vec3 upVec(0.f, 1.f, 0.f);
glm::vec3 lookAtVec(0.f, 0.f, -1.f);
glm::mat4 viewMatrix(1.f);

//-----Projection-----//
float FOV = 45.0f;
float aspectRatio = (16.0 / 14.0 * WIDTH) / HEIGHT;
float nearPlane = 0.1f;
float farPlane = 300.0f;

//-----Matrix-----//
glm::mat4 projectionMatrix(1.0f);
glm::mat4 worldMatrix(1.0f);
glm::mat4 gRotate2D;

//-----Mouse-----//
glm::vec3 directions(0, 0, 0);
glm::vec3 rightVec(0, 0, 0);
float positionTest;
float horizontalAngle = 3.14f*2;
float verticalAngle = -0.3f;
float initialFoV = 45.0f;
float mouseSpeed = 0.05;
float speed = 20.f;
float quitHorizontal = 6.28;
float quitVertical = -0.3f;

//-----Inventory-----//
bool inventoryDelay;
bool candleDelay;
bool inventoryON = false;
double mousePos[2];
bool mouseLeftClicked;
int inspecingObjektID;
int selectedItem = -1;
int ez4enceCount = 0;

bool clockMenyON = false;
int clockCode[4] = { -1, -1, -1, -1 };
float door1Rotation = 0, door2Rotation = 0, door3Rotation = 0, doorAllRotation = -170;

char debugRay[100];
glm::vec3 rayWorld = glm::vec3(0.0f, 0.0f, 0.0f);

bool rotateObjectBool = false;
bool playerMoveBool = true;
glm::vec3 tempRotation(0.0f);
float tempVerticalAngle = 0;
float tempHorizontalAngle = 0;
// 0 = mainMenu, 1 = game, 2 = inventory, 3 = chouse item from inventory
int gameState = 0;
float xPos = 0;
float yPos = 0;
bool ifQuit = false;
bool startGame = false;
bool pianoDone = false;
bool pianoBool = true;
bool renderFire = true;
bool firePlaceNotInteract = false;
int counter123 = 0;
bool resetItems = false;

bool pressLight1 = true;
bool pressLight2 = true;
bool pressLight3 = true;
bool pressLight4 = true;
bool pressLight5 = true;
bool renderGoldkey = false;
bool endKeyBool = true;

int candleArray[5] = { 0, 0, 0, 0, 0 };
int lightCounter = 0;

string combArrayText;
string combArray[7];

glm::vec3 tempPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 tempCamPos = glm::vec3(0.0f, 0.0f, 0.0f);

//-----Pausing-----//
bool pause = FALSE;

//-----Delta time-----//
float deltaTime;
float t = 0;

/// macro that returns "char*" with offset "i"
/// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
static void moveObjectIntersect(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		position.x += speed * 0.2f * deltaTime;
		if (position.x > -101.7f)
			position.x = -101.7f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		position.x -= speed * 0.2f * deltaTime;
		if (position.x < -110.0f)
			position.x = -110.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		position.z += speed * 0.2f * deltaTime;
		if (position.z > 1.3f)
			position.z = 1.3f;

	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		position.z -= speed * 0.2f * deltaTime;
		if (position.z < -1.6f)
			position.z = -1.6f;

	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		testRotation.x -= speed * 5 * deltaTime;
		if (testRotation.x < 0.0f)
			testRotation.x += 360.0f;

		if (testRotation.x < 90.0f)
			rotation2 = glm::vec3(-glm::sqrt(testRotation.x / 90.0f), 0.0f, glm::sqrt((90.0f - testRotation.x) / 90.0f));
		else if (testRotation.x < 180.0f)
			rotation2 = glm::vec3(-glm::sqrt((180.0f - testRotation.x) / 90.0f), 0.0f, -glm::sqrt((testRotation.x - 90.0f) / 90.0f));
		else if (testRotation.x < 270.0f)
			rotation2 = glm::vec3(glm::sqrt((testRotation.x - 180.0f) / 90.0f), 0.0f, -glm::sqrt((270.0f - testRotation.x) / 90.0f));
		else if (testRotation.x < 360.0f)
			rotation2 = glm::vec3(glm::sqrt((360.0f - testRotation.x) / 90.0f), 0.0f, glm::sqrt((testRotation.x - 270.0f) / 90.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		testRotation.x += speed * 5 * deltaTime;
		if (testRotation.x > 360.0f)
			testRotation.x -= 360.0f;

		if (testRotation.x < 90.0f)
			rotation2 = glm::vec3(-glm::sqrt(testRotation.x / 90.0f), 0.0f, glm::sqrt((90.0f - testRotation.x) / 90.0f));
		else if (testRotation.x < 180.0f)
			rotation2 = glm::vec3(-glm::sqrt((180.0f - testRotation.x) / 90.0f), 0.0f, -glm::sqrt((testRotation.x - 90.0f) / 90.0f));
		else if (testRotation.x < 270.0f)
			rotation2 = glm::vec3(glm::sqrt((testRotation.x - 180.0f) / 90.0f), 0.0f, -glm::sqrt((270.0f - testRotation.x) / 90.0f));
		else if (testRotation.x < 360.0f)
			rotation2 = glm::vec3(glm::sqrt((360.0f - testRotation.x) / 90.0f), 0.0f, glm::sqrt((testRotation.x - 270.0f) / 90.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		testRotation.y -= speed * 5 * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		testRotation.y += speed * 5 * deltaTime;
	}
}

static void mouseUpdate(GLFWwindow* window, double xpos, double ypos)
{	
	if (!inventoryON)
	{
		horizontalAngle += mouseSpeed * deltaTime * float(WIDTH / 2 - xpos);
		verticalAngle += mouseSpeed * deltaTime * float(HEIGHT / 2 - ypos);

		float x = (2.0f * xpos) / WIDTH - 1.0f;
		float y = 1.0f - (2.0f * ypos) / HEIGHT;
		float z = 1.0f;

		//-----Spaces-----//
		glm::vec3 rayNDC = glm::vec3(x, y, z);
		glm::vec4 rayClip = glm::vec4(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
		glm::vec4 rayEye = inverse(projectionMatrix) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
		glm::vec4 rayWorldTemp = glm::vec4(inverse(viewMatrix) * rayEye);
		rayWorld = glm::vec3(rayWorldTemp.x, rayWorldTemp.y, rayWorldTemp.z);
		rayWorld = normalize(rayWorld);

		//saves values for main meny
		xPos = xpos;
		yPos = ypos;
	}
}

bool intersect(glm::vec3 firstBoxMin, glm::vec3 firstBoxMax, glm::vec3 secondBoxMin, glm::vec3 secondBoxMax)
{
	bool intersect = false;
	if (firstBoxMin.x <= secondBoxMax.x && firstBoxMax.x >= secondBoxMin.x &&
		firstBoxMin.y <= secondBoxMax.y && firstBoxMax.y >= secondBoxMin.y &&
		firstBoxMin.z <= secondBoxMax.z && firstBoxMax.z >= secondBoxMin.z)
	{
		intersect = true;
	}
	return intersect;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 561 && xPos < 905 && yPos < 587 && yPos > 516)
	{
		camPos = glm::vec3(70.0f, 20.0f, -5.0f);
		gameState = 1;
		//-----Audio/Background Music-----//
		if (SoundEngine != nullptr)
		{
			backgroundMusic = SoundEngine->play2D("Audio/amnesia.mp3", GL_TRUE, GL_TRUE);
			backgroundMusic->setVolume(0.2f); 
				backgroundMusic->setIsPaused(false);

			fireplaceAudio = SoundEngine->play3D("Audio/FireLoop.mp3", vec3df(0, 0, 0), true, false, true);
			clockAudio = SoundEngine->play3D("Audio/ClockLoop.mp3", vec3df(0, 0, 0), true, false, true);

			fireplaceAudio->setMinDistance(2.0f);
			clockAudio->setMinDistance(3.0f);

			fireplaceAudio->setPosition(fireplacePos3D);
			clockAudio->setPosition(clockPos3D);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 561 && xPos < 905 && yPos < 807 && yPos > 739)
	{
		glfwSetWindowShouldClose(gWindow, 1);
	}
}

void mouse_button_callback_PauseMenu(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 561 && xPos < 905 && yPos < 587 && yPos > 516)
	{
		//reset
		gameState = 1;
		verticalAngle = tempVerticalAngle;
		horizontalAngle = tempHorizontalAngle;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 561 && xPos < 905 && yPos < 807 && yPos > 739)
	{
		gameState = 0;
		verticalAngle = quitVertical;
		horizontalAngle = quitHorizontal;
		resetItems = true;
	}
}

void mouse_button_callback_endScreen(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 561 && xPos < 905 && yPos < 807 && yPos > 739)
	{
		gameState = 0;
		verticalAngle = quitVertical;
		horizontalAngle = quitHorizontal;
		resetItems = true;
		SoundEngine->stopAllSounds();
	}
}

void mouse_button_callback_inventory(GLFWwindow* window, int button, int action, int mods)
{
	glfwGetCursorPos(window, &mousePos[0], &mousePos[1]);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouseLeftClicked = true;
	}

}
void mouse_button_callback_start(GLFWwindow* window, int button, int action, int mods)
{

}


void mouse_button_callback_piano(GLFWwindow* window, int button, int action, int mods)
{
	string tempString;
	//C
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 41 && xPos < 200 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("C");
		tempString = "C";
		SoundEngine->play2D("Audio/C.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//D
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 245 && xPos < 401 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("D");
		tempString = "D";
		SoundEngine->play2D("Audio/D.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//E
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 447 && xPos < 604 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("E");
		tempString = "E";
		SoundEngine->play2D("Audio/E.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//F
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 650 && xPos < 808 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("F");
		tempString = "F";
		SoundEngine->play2D("Audio/F.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//G
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 852 && xPos < 1009 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("G");
		tempString = "G";
		SoundEngine->play2D("Audio/G.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//A
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 1053 && xPos < 1211 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("A");
		tempString = "A";
		SoundEngine->play2D("Audio/A.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}
	//B
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && xPos > 1247 && xPos < 1402 && yPos < 690 && yPos > 255)
	{
		OutputDebugStringA("B");
		tempString = "B";
		SoundEngine->play2D("Audio/B.wav", GL_FALSE);
		counter123++;
		if (combArray[0] == "D" && tempString != "D")
		{
			combArray[counter123] = tempString;
		}
	}

	if (tempString == "D")
	{
		combArray[0] = "D";
		counter123 = 0;
	}
	if (counter123 == 7)
	{
		counter123 = 0;
	}
	
	
	if (combArray[0] == "D" && combArray[1] == "F" && combArray[2] == "E" && 
		combArray[3] == "G" && combArray[4] == "C" && combArray[5] == "A" && combArray[6] == "B")
	{
			OutputDebugStringA("nice key");
			pianoDone = true;
	}

	
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	initWindow(WIDTH, HEIGHT);
	glfwSetCursorPosCallback(gWindow, mouseUpdate);
	bool shutdown = false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui::StyleColorsDark();

	NixHandler loadingScreen("NixFolder/", "loading/loading.nix", glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 2, 2));
	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", loadingScreen.getShaderProgramForModel(0));
	loadingScreen.setUpAllModels();
	loadingScreen.render2DAllModels();
	glfwSwapBuffers(gWindow);

	Player player(camPos, 0);
	NixHandler room1("NixFolder/", "room1/room1.nix", glm::vec3(0, 0, 0));
	NixHandler room2("NixFolder/", "room2/room2.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler room3("NixFolder/", "room3/room3.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler room4("NixFolder/", "room4/room4.nix", glm::vec3(0, 0, 0));
	NixHandler room5("NixFolder/", "room5/room5.nix", glm::vec3(0, 0, 0));
	NixHandler room6("NixFolder/", "room6/room6.nix", glm::vec3(0, 0, 0));
	NixHandler room7("NixFolder/", "room7/room7.nix", glm::vec3(0, 0, 0));
	NixHandler room8("NixFolder/", "room8/room8.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler room3lights("NixFolder/", "lights/lights.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler doorLock("NixFolder/", "doorLock/doorLock.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler mansion("NixFolder/","newWalls/newWalls.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler mansionBox("NixFolder/", "boxtest1/boxtest1.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler piano("NixFolder/", "piano/piano.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler mansionStairs("NixFolder/", "stairsAndFence/stairsAndFence.nix", glm::vec3(0.f, 0.f, 0.f));
	
	NixHandler overDoor("NixFolder/", "overDoor/overDoor.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler doors("NixFolder/", "doors/doors.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler roof("NixFolder/", "roof/roof.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler kungen("NixFolder/", "kungen/kungen.nix", glm::vec3(0.f, 0.f, 0.f));
	NixHandler culling("NixFolder/", "cullingBoxes/cullingBoxes.nix", glm::vec3(0.f, 0.f, 0.f));

	//flyta till invent sen
	
	NixHandler tPlan("NixFolder/", "2plan00/2plan00.nix", glm::vec3(-100, -20, 0));
	NixHandler playerBox("NixFolder/", "player/player.nix", glm::vec3(0, 0, 0));
	
	NixHandler walkTerrain("NixFolder/", "FreezeTest/FreezeTest.nix", glm::vec3(0, 0, 0));
	NixHandler doorsAndWalls("NixFolder/", "arc/arc.nix", glm::vec3(0, 0, 0));
	
	NixHandler clock("NixFolder/", "clock/clock.nix", glm::vec3(0, 0, 0));
	NixHandler clockHandel("NixFolder/", "visare/visare.nix", glm::vec3(-27.594f, 20.448f, 21.875f), glm::vec3(1437.618f, 0.779f, -0.055f));
	NixHandler writingAndTabel("NixFolder/", "ritingTabel/ritingTabel.nix", glm::vec3(0, 0, 0));
	//writingAndTabel.updateByIndex(1, glm::vec3(5.76f, 0, 22.8), glm::vec3(0.0f, 180.0f, 0.0f));
	NixHandler bookShelves("NixFolder/", "bookShelves/bookShelves.nix", glm::vec3(0, 0, 0));
	NixHandler inventoryBacround("NixFolder/", "inventoryBackround/inventoryBackround.nix", glm::vec3(0, 0, 0), glm::vec3(90.0f, 90.0f, 0.0f), glm::vec3(2, 2, 2));// , camPos + glm::vec3(0.101f, 0.101f, 0.101f));// , glm::vec3(10.f, 3.f, 20.f));
	NixHandler pianoBackground("NixFolder/", "pianoTextureModel/pianoTextureModel.nix", glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 2, 2));
	NixHandler mainMenu("NixFolder/", "mainMenu/mainMenu.nix", glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 2, 2));
	NixHandler pauseMenu("NixFolder/", "pauseMenu/pauseMenu.nix", glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 2, 2));
	NixHandler endScreen("NixFolder/", "endScreen/endScreen.nix", glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2, 2, 2));
	NixHandler marker("NixFolder/", "marker/marker.nix", glm::vec3(0.0f, 0.0f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.13f, 1.0f));
	// UV 5x5 viktigt att veta !
	NixHandler clockNumbers("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.138f - 0.477f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.092f - 0.277f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.046f - 0.077f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.046f + 0.077f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.092f + 0.277f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.138f + 0.477f, -0.065f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.138f - 0.477f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.092f - 0.277f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(-0.046f - 0.077f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.046f + 0.077f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.092f + 0.277f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	clockNumbers.addModels("NixFolder/", "clockNumbers/clockNumbers.nix", glm::vec3(0.138f + 0.477f, -0.435f - 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	for (int i = 0; i < 12; i++)
	{
		clockNumbers.moveUVByIndex(i, ((i) / 5) * 0.2f, ((i) % 5) * 0.2f);
	}

	NixHandler inventorySlot("NixFolder/", "inventorySlot/inventorySlot.nix", glm::vec3(-0.138f - 0.6f, 0.435f + 0.12f, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
	float height, width;
	for (int i = 0; i < 4; i++) // höjd
	{
		switch (i)
		{
		case 0:
			height = 0.435f + 0.12f;
			break;
		case 1:
			height = 0.065f + 0.12f;
			break;
		case 2:
			height = -0.065f - 0.12f;
			break;
		case 3:
			height = -0.435f - 0.12f;
			break;
		}
		for (int j = 0; j < 7; j++) //bredd
		{
			switch (j)
			{
			case 0:
				width = -0.138f - 0.6f;
				break;
			case 1:
				width = -0.092f - 0.4f;
				break;
			case 2:
				width = -0.046f - 0.2f;
				break;
			case 3:
				width = 0;
				break;
			case 4:
				width = 0.046f + 0.2f;
				break;
			case 5:
				width = 0.092f + 0.4f;
				break;
			case 6:
				width = 0.138f + 0.6f;
				break;
			}
			if (i != 0 || j != 0)
				inventorySlot.addModels("NixFolder/", "inventorySlot/inventorySlot.nix", glm::vec3(width, height, -0.1f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.25f, 1.0f));
		}

	}

	NixHandler interactable("NixFolder/", "firePlace/firePlace.nix", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	interactable.addModels("NixFolder/", "finalDoor/finalDoor.nix", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	//interactable.addModels("NixFolder/", "firePlace/firePlace.nix", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

	NixHandler pickableObjets("NixFolder/ItemsTempFile/", "key1/key1.nix", glm::vec3(69.672f, 14.086f, 2.838f), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5)); 
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "rightNote/rightNote.nix", glm::vec3(-54.305f, 19.217f, -5.796f), glm::vec3(-60, 0, 0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "card/card.nix", glm::vec3(77.314f, 18.619f, 26.133f), glm::vec3(0, 0, -90));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "sword/sword.nix", glm::vec3(21.268f, 20.116f, -18.99f), glm::vec3(90, 0, 90));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "skull/skull.nix", glm::vec3(-49.45f, 19.186f, -5.355f), glm::vec3(-6.469, 60.93, 5.56));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "holyGrale/holyGrale.nix", glm::vec3(-47.461f, 4.055f, 5.26f), glm::vec3(0, 0, 0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "leftNote/leftNote.nix", glm::vec3(-28.754, 14.108f, 21.788f), glm::vec3(-89.166, 3.126, 29.852));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "bigNote/bigNote.nix", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 0, 0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "pianoKey/pianoKey.nix", glm::vec3(49.133f, 3.265f, 3.323f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "bucket/bucket.nix", glm::vec3(31.222f, 14.667f, -6.197f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "redKey/redKey.nix", glm::vec3(76.976f, 1.383f, 11.79f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
	pickableObjets.addModels("NixFolder/ItemsTempFile/", "finalKey/finalKey.nix", glm::vec3(41.922f, 14.302f, 28.354f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));

	
	//-----Check for memory leak-----//
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand(static_cast<unsigned int>(time(0)));

	ParticleSystem fireParticle1(100, glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0.5f), glm::vec3(0, -0.0001, 0), 1, 0);
	ParticleSystem smokeParticle1(100, glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0), glm::vec3(0, -0.00001, 0), 0.3f, 0);
	ParticleSystem sparkParticle1(50, glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), glm::vec4(1, 1, 1, 0.5f), glm::vec3(0, -0.001, 0), 1.0f, 0.5f);

	ParticleSystem candle1(1000, glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 0.5f), glm::vec3(0, -0.0001, 0), 0.1, 0);
	ParticleSystem candle2(1000, glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 0.5f), glm::vec3(0, -0.0001, 0), 0.1, 0);
	ParticleSystem candle3(1000, glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 0.5f), glm::vec3(0, -0.0001, 0), 0.1, 0);
	ParticleSystem candle4(1000, glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 0.5f), glm::vec3(0, -0.0001, 0), 0.1, 0);
	ParticleSystem candle5(1000, glm::vec3(0, 0, 0), glm::vec4(0, 1, 0, 1), glm::vec4(0, 1, 0, 0.5f), glm::vec3(0, -0.0001, 0), 0.1, 0);

	
	//-----Creating all Shaders-----//
	CreateShaders("VertexShaderPartical.glsl", "GeometryShaderPartical.glsl", "FragmentPartical.glsl", &gShaderProgramPartical);
	CreateShaders("VertexShaderDepth.glsl", "", "FragmentDepth.glsl", &gShaderProgramDepth);
	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", &gShaderProgramNix);
	room5.setgShaderProgramAllModels(gShaderProgramNix);
	doorsAndWalls.setgShaderProgramAllModels(gShaderProgramNix);
	mansionBox.setgShaderProgramAllModels(gShaderProgramNix);
	doors.setgShaderProgramAllModels(gShaderProgramNix);
	pickableObjets.setgShaderProgramAllModels(gShaderProgramNix);
	interactable.setgShaderProgramAllModels(gShaderProgramNix);
	mansionStairs.setgShaderProgramAllModels(gShaderProgramNix);
	room8.setgShaderProgramAllModels(gShaderProgramNix);
	room2.setgShaderProgramAllModels(gShaderProgramNix);
	clock.setgShaderProgramAllModels(gShaderProgramNix);
	clockHandel.setgShaderProgramAllModels(gShaderProgramNix);
	writingAndTabel.setgShaderProgramAllModels(gShaderProgramNix);
	bookShelves.setgShaderProgramAllModels(gShaderProgramNix);
	room3.setgShaderProgramAllModels(gShaderProgramNix);
	room6.setgShaderProgramAllModels(gShaderProgramNix);
	room7.setgShaderProgramAllModels(gShaderProgramNix);
	kungen.setgShaderProgramAllModels(gShaderProgramNix);
	doorLock.setgShaderProgramAllModels(gShaderProgramNix);
	room3lights.setgShaderProgramAllModels(gShaderProgramNix);

	for (int i = 0; i < mansion.nrOfModels(); i++)
		CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", mansion.getShaderProgramForModel(0));

	for (int i = 0; i < culling.nrOfModels(); i++)
		CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", culling.getShaderProgramForModel(i));

	for (int i = 0; i < overDoor.nrOfModels(); i++)
		CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", overDoor.getShaderProgramForModel(i));

	for (int i = 0; i < room4.nrOfModels(); i++)
		CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", room4.getShaderProgramForModel(i));

	for (int i = 0; i < inventorySlot.nrOfModels(); i++)
		CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", inventorySlot.getShaderProgramForModel(i));

	for (int i = 0; i < room1.nrOfModels(); i++)
		CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", room1.getShaderProgramForModel(i));

	for (int i = 0; i < clockNumbers.nrOfModels(); i++)
		CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", clockNumbers.getShaderProgramForModel(i));

	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", marker.getShaderProgramForModel(0));

	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", inventoryBacround.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", pianoBackground.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", mainMenu.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", pauseMenu.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix2D.glsl", "GeometryShaderNix2D.glsl", "FragmentNix2D.glsl", endScreen.getShaderProgramForModel(0));

	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", tPlan.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", playerBox.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", walkTerrain.getShaderProgramForModel(0));
	
	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", piano.getShaderProgramForModel(0));
	CreateShaders("VertexShaderNix.glsl", "GeometryShaderNix.glsl", "FragmentNix.glsl", roof.getShaderProgramForModel(0));


	//-----Create all textures-----//
	glGenVertexArrays(1, &gVertexAttributePartical);
	CrateTexture("images/pusheen.png", &particelTextur);
	CrateTexture("images/particel.png", &particelTextur2);
	CrateTexture("images/interactEv3.png", &interactTXT);

	CrateTexture("images/fire1.png", &fire1);
	CrateTexture("images/smoke1.png", &smoke1);
	CrateTexture("images/sparks_maybe.png", &sparks1);

	room5.setUpAllModels();
	mainMenu.setUpAllModels();
	pauseMenu.setUpAllModels();
	endScreen.setUpAllModels();
	mansion.setUpAllModels();
	mansionBox.setUpAllModels();
	tPlan.setUpAllModels();
	playerBox.setUpAllModels();
	mansionStairs.setUpAllModels();
	walkTerrain.setUpAllModels();
	doorsAndWalls.setUpAllModels();
	room1.setUpAllModels();
	doors.setUpAllModels();
	inventoryBacround.setUpAllModels();
	pianoBackground.setUpAllModels();
	inventorySlot.setUpAllModels();
	clockNumbers.setUpAllModels();
	pickableObjets.setUpAllModels();
	pickableObjets.setShouldRenderByIndex(pianoKey, false);
	pickableObjets.setShouldRenderByIndex(redKey, false);
	pickableObjets.setShouldRenderByIndex(EndKey, false);
	pickableObjets.setShouldRenderByIndex(pianoNote2, false);
	pickableObjets.setShouldRenderByIndex(pianoNote3, false);
	interactable.setUpAllModels();
	piano.setUpAllModels();
	room8.setUpAllModels();
	roof.setUpAllModels();
	room2.setUpAllModels();
	culling.setUpAllModels();
	overDoor.setUpAllModels();
	room4.setUpAllModels();
	clock.setUpAllModels();
	clockHandel.setUpAllModels();
	writingAndTabel.setUpAllModels();
	bookShelves.setUpAllModels();
	room3.setUpAllModels();
	room6.setUpAllModels();
	room7.setUpAllModels();
	doorLock.setUpAllModels();
	marker.setUpAllModels();
	room3lights.setUpAllModels();



	//-----View and Projection Matrix-----//
	viewMatrix = glm::lookAt(camPos, camPos + directions, upVec);
	projectionMatrix = glm::perspective(FOV, aspectRatio, nearPlane , farPlane);

	//-----Shadow Mapping-----//
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int gDepthMapFrameBuffer;
	glGenFramebuffers(1, &gDepthMapFrameBuffer);
	unsigned int gDepthMap;

	glGenTextures(1, &gDepthMap);
	glBindTexture(GL_TEXTURE_2D, gDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//-----This makes shadows not exist outside of the shadowmaps area-----//
	float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, gDepthMapFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-----Enable depth and Blending-----//
	//glEnable(GL_CULL_FACE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	while (!glfwWindowShouldClose(gWindow))
	{
		glfwPollEvents();

		SoundEngine->setListenerPosition(vec3df(camPos.x, camPos.y, camPos.z), vec3df(0, 0, 0));

		glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, 80.0f);
		///addTest flyttar ljuset men ändrar inte skuggorna men om du bara lägger addtest på den första ändras ljuset


		glm::mat4 lightView = glm::lookAt(lightPos[0], glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 lightSpace = lightProjection * lightView;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, gDepthMapFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//-----ViewPort-----//
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		float first = cos(verticalAngle) * sin(horizontalAngle);
		float second = sin(verticalAngle);
		float third = cos(verticalAngle) * cos(horizontalAngle);
		directions = glm::vec3(first, second, third);

		rightVec = glm::vec3(sin(horizontalAngle - 3.14 / 2.0f), 0, cos(horizontalAngle - 3.14 / 2.0f));
		upVec = glm::cross(rightVec, directions);
		
		viewMatrix = glm::lookAt(camPos, camPos + directions, upVec);
		projectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, nearPlane, farPlane);

		if (resetItems == true)
		{
			resetItems = false;
			for (int i = 0; i < pickableObjets.nrOfModels(); i++)
			{
				pickableObjets.setShouldRenderByIndex(i, true);
				inventorySlot.moveUVByIndex(i, 0, 0);
				player.setAItemInInventory(i, false);
			}
			pickableObjets.setShouldRenderByIndex(pianoKey, false);
			pickableObjets.setShouldRenderByIndex(redKey, false);
			pickableObjets.setShouldRenderByIndex(EndKey, false);
			pickableObjets.setShouldRenderByIndex(pianoNote2, false);
			pickableObjets.setShouldRenderByIndex(pianoNote3, false);
			pickableObjets.updateByIndex(0,glm::vec3(69.672f, 14.086f, 2.838f), glm::vec3(0, 0, 0), glm::vec3(0.5, 0.5, 0.5));
			pickableObjets.updateByIndex(1,glm::vec3(-54.305f, 19.217f, -5.796f), glm::vec3(-60, 0, 0));
			pickableObjets.updateByIndex(2,glm::vec3(77.314f, 18.619f, 26.133f), glm::vec3(0, 0, -90));
			pickableObjets.updateByIndex(3,glm::vec3(21.268f, 20.116f, -18.99f), glm::vec3(90, 0, 90));
			pickableObjets.updateByIndex(4,glm::vec3(-49.45f, 19.186f, -5.355f), glm::vec3(-6.469, 60.93, 5.56));
			pickableObjets.updateByIndex(5,glm::vec3(-47.461f, 4.055f, 5.26f), glm::vec3(0, 0, 0));
			pickableObjets.updateByIndex(6,glm::vec3(-28.754, 14.108f, 21.788f), glm::vec3(-89.166, 3.126, 29.852));
			pickableObjets.updateByIndex(7,glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 0, 0));
			pickableObjets.updateByIndex(8,glm::vec3(49.133f, 3.265f, 3.323f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
			pickableObjets.updateByIndex(9,glm::vec3(31.222f, 14.667f, -6.197f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
			pickableObjets.updateByIndex(10, glm::vec3(76.976f, 1.383f, 11.79f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));
			pickableObjets.updateByIndex(11,glm::vec3(41.922f, 14.302f, 28.354f), glm::vec3(0, 0, 0), glm::vec3(1.0, 1.0, 1.0));

			for (int i = 0; i < 13; i++)
			{
				player.setEvent(i, false);
			}
			door1Rotation = 0;
			door2Rotation = 0;
			door3Rotation = 0;
			lightPos[1] = glm::vec3(75.859, 2.0f, 11.713f);
			renderFire = true;
			pianoDone = false;
			pianoBool = true;
			firePlaceNotInteract = false;
			for (int i = 0; i < 7; i++)
			{
				combArray[i] = "";
			}
			counter123 = 0;
			clockMenyON = false;
			for (int i = 0; i < 4; i++)
			{
				clockCode[i] = -1;
			}
			int candleArray[5] = { 0, 0, 0, 0, 0 };

			lightCounter = 0;
			pressLight1 = true;
			pressLight2 = true;
			pressLight3 = true;
			pressLight4 = true;
			pressLight5 = true;
			renderGoldkey = false;
			endKeyBool = true;


		}

		if (gameState == 0)
		{
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback);
			camPos = glm::vec3(10.f, 5.f, 12.f);
			horizontalAngle = quitHorizontal;
			verticalAngle = quitVertical;
			lightPosR0[0] = camPos;
			mainMenu.render2DAllModels();
		}

		marker.moveUVByIndex(0, 0.0f, 0.5f);
		if (pianoDone == false)
		{
			if (player.rayIntersect(piano.getMinByIndex(0), piano.getMaxByIndex(0), rayWorld, camPos) && player.getTValue() < 5)
			{
				marker.moveUVByIndex(0, 0, 0);
				if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E))
				{
					gameState = 4;
					tempVerticalAngle = verticalAngle;
					tempHorizontalAngle = horizontalAngle;
				}
			}
		}
	
		if (gameState == 4)
		{

			if (pianoDone == true || GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_R))
			{
				gameState = 1;
				verticalAngle = tempVerticalAngle;
				horizontalAngle = tempHorizontalAngle;
			}
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_piano);
			pianoBackground.render2DAllModels();
		}

		if (gameState == 5)
		{

			//när ni kallar så turn on inventoryON för att röra musen
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_PauseMenu);


			pauseMenu.render2DAllModels();


		}

		if (gameState == 6)
		{
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_endScreen);
			endScreen.render2DAllModels();

		}

		//inventory (by presing I)
		if (gameState == 2)
		{
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_inventory);
			//to klick a button anywher
			if (mouseLeftClicked)
				for (int i = 0; i < pickableObjets.nrOfModels(); i++)
				{
					if ((inventorySlot.getMaxByIndex(i).x + 1) / 2 * WIDTH >= mousePos[0] && (inventorySlot.getMinByIndex(i).x + 1) / 2 * WIDTH <= mousePos[0] &&
						(inventorySlot.getMaxByIndex(i).y + 1) / 2 * HEIGHT >= HEIGHT - mousePos[1] && (inventorySlot.getMinByIndex(i).y + 1) / 2 * HEIGHT
						<= HEIGHT - mousePos[1] && player.getAItemInInventory(i))
					{
						gameState = 1;
						lightPos[1] = glm::vec3(-100, -5, 0);
						tempPosition = position;
						tempCamPos = camPos;
						position = glm::vec3(-110, -5, 0);
						camPos = glm::vec3(-100, -5, 0);
						tempRotation = rotation;
						tempVerticalAngle = verticalAngle;
						tempHorizontalAngle = horizontalAngle;
						horizontalAngle = 11;
						verticalAngle = 0.08;
						OutputDebugStringA("I pressed for intersect\n");
						rotateObjectBool = true;
						inspecingObjektID = i;
						pickableObjets.setShouldRenderByIndex(i, true);
						break;
					}
				}

			mouseLeftClicked = false;
			inventorySlot.render2DAllModels();
			inventoryBacround.render2DAllModels();

			if (GLFW_RELEASE == glfwGetKey(gWindow, GLFW_KEY_I))
			{
				inventoryDelay = true;
			}
			if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_I) && inventoryDelay)
			{
				gameState = 1;
				inventoryDelay = false;
				inventoryON = false;
				SoundEngine->play2D("Audio/closeInv.mp3", GL_FALSE);
			}
		}
		if (gameState == 3)
		{
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_inventory);
			if (mouseLeftClicked)
				for (int i = 0; i < pickableObjets.nrOfModels(); i++)
				{
					if ((inventorySlot.getMaxByIndex(i).x + 1) / 2 * WIDTH >= mousePos[0] && (inventorySlot.getMinByIndex(i).x + 1) / 2 * WIDTH <= mousePos[0] &&
						(inventorySlot.getMaxByIndex(i).y + 1) / 2 * HEIGHT >= HEIGHT - mousePos[1] && (inventorySlot.getMinByIndex(i).y + 1) / 2 * HEIGHT
						<= HEIGHT - mousePos[1] && player.getAItemInInventory(i))
					{
						selectedItem = i;
						inventoryON = false;
						gameState = 1;
						break;
					}
				}

			mouseLeftClicked = false;
			inventorySlot.render2DAllModels();
			inventoryBacround.render2DAllModels();

			if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_I))
			{
				gameState = 1;
				inventoryDelay = false;
				inventoryON = false;
			}
		}

		if (gameState == 1)
		{
			ez4enceCount++;
			glfwSetMouseButtonCallback(gWindow, mouse_button_callback_start);
			if (GLFW_RELEASE == glfwGetKey(gWindow, GLFW_KEY_I))
			{
				inventoryDelay = true;
			}
			if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_I) && inventoryDelay && !clockMenyON)
			{
				if (inventoryON)
				{
					if (renderFire == true)
					{
						lightPos[1] = glm::vec3(75.859, 2.0f, 11.713f);
					}
					position = glm::vec3(0.0f, 0.0f, 0.0f);
					camPos = tempCamPos;
					playerMoveBool = true;
					pause = false;
					rotateObjectBool = false;
					rotation = tempRotation;
					horizontalAngle = tempHorizontalAngle;
					verticalAngle = tempVerticalAngle;
					pickableObjets.setShouldRenderByIndex(inspecingObjektID, false);
				}
				gameState = 2;
				SoundEngine->play2D("Audio/openInv.mp3", GL_FALSE);
				inventoryDelay = false;
				inventoryON = true;
				//inventory look go back to inventory
			}
			 if (rotateObjectBool == false && clockMenyON == false)
			{
				if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_ESCAPE)) {
					gameState = 5;
					tempHorizontalAngle = horizontalAngle;
					tempVerticalAngle = verticalAngle;
				}
			}
			//lightPos[0] = glm::vec3(-50.0f, 5.0f, 12.0f);

			deltaTime = ImGui::GetIO().DeltaTime;
			//-----move along X-----//
			gIncrement += 1.0f * deltaTime;
			gOffsetX = sin(gIncrement);
			glUniform1f(10, gOffsetX);

			// prepare IMGUI output
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			//-----Creates a delta time close to  one-----//
			float dt = (1 / deltaTime) / 60;
			float increase = 0.1f;

			player.setPlayerPos(camPos);
			glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //hide cursor
			if (!clockMenyON)
				glfwSetCursorPos(gWindow, WIDTH / 2, HEIGHT / 2);


			if (playerMoveBool == true)
			{
				camPos = player.movePlayer(gWindow, directions, speed, rightVec, deltaTime);
			}

			playerBox.updateAllModels(camPos, glm::vec3(0, 0, 0));

			const int interactObjectTavlor = 1;
			glm::vec3 arrayMin[interactObjectTavlor] = { room3.getMinByIndex(0) };
			glm::vec3 arrayMax[interactObjectTavlor] = { room3.getMaxByIndex(0) };

			for (int i = 0; i < mansion.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(mansion.getMinByIndex(i), mansion.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			for (int i = 0; i < doors.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(doors.getMinByIndex(i), doors.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			for (int i = 0; i < mansionBox.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(mansionBox.getMinByIndex(i), mansionBox.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			player.playerIntersectWithObjects(piano.getMinByIndex(0), piano.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);

			for (int i = 0; i < interactable.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(interactable.getMinByIndex(i), interactable.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			//clock klick
			for (int i = 0; i < clock.nrOfModels(); i++)
			{
				if (player.rayIntersect(clock.getMinByIndex(i), clock.getMaxByIndex(i), rayWorld, camPos) && player.getTValue() < 10 && !player.getEvent(clockDone))
				{
					marker.moveUVByIndex(0, 0, 0);
					OutputDebugStringA("intersect\n");
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10 && clock.getShouldRenderByIndex(i))
					{
						clockMenyON = true;
						inventoryON = true;
						playerMoveBool = false;
					}
				}
			}
			// have klickt
			if (clockMenyON)
			{
				glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetMouseButtonCallback(gWindow, mouse_button_callback_inventory);
				clockNumbers.render2DAllModels();
				//to klick a button anywher
				if (mouseLeftClicked)
					for (int i = 0; i < clockNumbers.nrOfModels(); i++)
					{
						if ((clockNumbers.getMaxByIndex(i).x + 1) / 2 * WIDTH >= mousePos[0] && (clockNumbers.getMinByIndex(i).x + 1) / 2 * WIDTH <= mousePos[0] &&
							(clockNumbers.getMaxByIndex(i).y + 1) / 2 * HEIGHT >= HEIGHT - mousePos[1] && (clockNumbers.getMinByIndex(i).y + 1) / 2 * HEIGHT
							<= HEIGHT - mousePos[1])
						{

							//30 * 12 = 360
							if (clockCode[0] == 10)
							{
								if (clockCode[1] == 3)
								{
									if (clockCode[2] == 0)
									{
										if (clockCode[3] == -1)
											clockCode[3] = i;
										if (clockCode[3] == 4)
										{
											//du klarade det
											clockMenyON = false;
											inventoryON = false;
											player.setEvent(clockDone, true);
											pickableObjets.setShouldRenderByIndex(pianoNote2, true);
											playerMoveBool = true;
										}
										else
										{
											clockCode[0] = -1;
											clockCode[1] = -1;
											clockCode[2] = -1;
											clockCode[3] = -1;
										}
									}
									else
									{
										if (clockCode[2] == -1)
											clockCode[2] = i;
										else
										{
											clockCode[0] = -1;
											clockCode[1] = -1;
											clockCode[2] = -1;
										}
									}
								}
								else
								{
									if (clockCode[1] == -1)
										clockCode[1] = i;
									else
									{
										clockCode[0] = -1;
										clockCode[1] = -1;
									}
								}
							}
							else
								clockCode[0] = i;

							clockHandel.updateByIndex(0, glm::vec3(-27.594f, 20.448f, 21.875f), glm::vec3((i + 1)*30.0f, 0.779f, -0.055f));
						}
					}
				mouseLeftClicked = false;
				if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_R))
				{
					clockMenyON = false;
					inventoryON = false;
					playerMoveBool = true;
				}
			}

			int rayIntersectCount = 2;


			//get full note
			if (player.getAItemInInventory(pianoNote1) && player.getAItemInInventory(pianoNote2))
			{
				player.setAItemInInventory(pianoNote3, true);
				// textur är nu 5x5 
				inventorySlot.moveUVByIndex(pianoNote3, ((pianoNote3 + 1) / 5) * 0.2f, ((pianoNote3 + 1) % 5) * 0.2f);
				player.setAItemInInventory(pianoNote1, false);
				player.setAItemInInventory(pianoNote2, false);
				inventorySlot.moveUVByIndex(pianoNote1, 0, 0);
				inventorySlot.moveUVByIndex(pianoNote2, 0, 0);
			}

			//pick up to inventory
			for (int i = 0; i < pickableObjets.nrOfModels(); i++)
			{
				if (player.rayIntersect(pickableObjets.getMinByIndex(i), pickableObjets.getMaxByIndex(i), rayWorld, camPos) && player.getTValue() < 10)
				{
					if (pickableObjets.getShouldRenderByIndex(i))
						marker.moveUVByIndex(0, 0, 0);
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10 && pickableObjets.getShouldRenderByIndex(i))
					{
						SoundEngine->play2D("Audio/PickUpKey.wav", GL_FALSE);
						pickableObjets.setShouldRenderByIndex(i, false);
						player.setAItemInInventory(i, true);
						// textur är nu 5x5 
						inventorySlot.moveUVByIndex(i, ((i + 1) / 5) * 0.2f, ((i + 1) % 5) * 0.2f);
					}
				}
			}
			//door1
			//interact whit first door
			if (!player.getEvent(EventKeys::door1Open))
				if (player.rayIntersect(doors.getMinByIndex(0), doors.getMaxByIndex(0), rayWorld, camPos) && player.getTValue() < 10)
				{
					marker.moveUVByIndex(0, 0, 0);
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10)
					{
						player.setEvent(EventKeys::door1, true);
						gameState = 3;
						inventoryON = true;
					}
				}
			//door2
			if (!player.getEvent(EventKeys::door2Open))
				if (player.rayIntersect(doors.getMinByIndex(1), doors.getMaxByIndex(1), rayWorld, camPos) && player.getTValue() < 10)
				{
					marker.moveUVByIndex(0, 0, 0);;
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10)
					{
						player.setEvent(EventKeys::door2, true);
						gameState = 3;
						inventoryON = true;
					}
				}
			//door3
			if (!player.getEvent(EventKeys::door3Open))
				if (player.rayIntersect(doors.getMinByIndex(3), doors.getMaxByIndex(3), rayWorld, camPos) && player.getTValue() < 10)
				{
					marker.moveUVByIndex(0, 0, 0);
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10)
					{
						player.setEvent(EventKeys::door3, true);
						gameState = 3;
						inventoryON = true;
					}
				}

			//firePlace
			if (!player.getEvent(EventKeys::firePlaceDoor))
			{
				if (player.rayIntersect(interactable.getMinByIndex(0), interactable.getMaxByIndex(0), rayWorld, camPos) && player.getTValue() < 10)
				{

					marker.moveUVByIndex(0, 0, 0);
					if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10)
					{
						player.setEvent(EventKeys::firePlace, true);
						gameState = 3;
						inventoryON = true;
					}
				}
			}
			
			//go to game skiping inventory
			if (rotateObjectBool == true)
			{
				playerMoveBool = false;
				moveObjectIntersect(gWindow);
				pause = true;
				pickableObjets.updateItemByIndex(inspecingObjektID, glm::vec3(position), rotation, testRotation.x, rotation2, testRotation.y); //glm::vec3(rotation.x , rotation.y, rotation.z)
	
			}

			///Events---------------------------------------------------------------------------------------------
			//door1
			if (player.getEvent(EventKeys::door1) && selectedItem == itemNamesInventory::key1)
			{
				player.setAItemInInventory(itemNamesInventory::key1, false);
				inventorySlot.moveUVByIndex(itemNamesInventory::key1, 0, 0);
				door1Rotation = -0.2f;
				SoundEngine->play2D("Audio/openDoor.mp3", GL_FALSE);
				player.setEvent(EventKeys::door1, false);
				player.setEvent(EventKeys::door1Open, true);
				selectedItem = -1;
			}
			else if (player.getEvent(EventKeys::door1) && gameState == 1)
			{
				player.setEvent(EventKeys::door1, false);
				selectedItem = -1;
			}
			if (door1Rotation < 0 && door1Rotation > -170)
				door1Rotation -= 50 * deltaTime;

			//door2
			if (player.getEvent(EventKeys::door2) && selectedItem == itemNamesInventory::pianoKey)
			{
				player.setAItemInInventory(itemNamesInventory::pianoKey, false);
				inventorySlot.moveUVByIndex(itemNamesInventory::pianoKey, 0, 0);
				door2Rotation = -0.2f;
				SoundEngine->play2D("Audio/openDoor.mp3", GL_FALSE);
				player.setEvent(EventKeys::door2, false);
				player.setEvent(EventKeys::door2Open, true);
				selectedItem = -1;
			}
			else if (player.getEvent(EventKeys::door2) && gameState == 1)
			{
				player.setEvent(EventKeys::door2, false);
				selectedItem = -1;
			}
			if (door2Rotation < 0 && door2Rotation > -170)
				door2Rotation -= 50 * deltaTime;
			//door3
			if (player.getEvent(EventKeys::door3) && selectedItem == itemNamesInventory::redKey)
			{
				player.setAItemInInventory(itemNamesInventory::redKey, false);
				inventorySlot.moveUVByIndex(itemNamesInventory::redKey, 0, 0);
				//doors.setShouldRenderByIndex(0, false);
				door3Rotation = -0.2f;
				SoundEngine->play2D("Audio/openDoor.mp3", GL_FALSE);
				player.setEvent(EventKeys::door3, false);
				player.setEvent(EventKeys::door3Open, true);
				selectedItem = -1;
			}
			else if (player.getEvent(EventKeys::door3) && gameState == 1)
			{
				player.setEvent(EventKeys::door3, false);
				selectedItem = -1;
			}
			if (door3Rotation < 0 && door3Rotation > -170)
				door3Rotation -= 50 * deltaTime;



			//event for fireplace
			if (player.getEvent(EventKeys::firePlace) && selectedItem == itemNamesInventory::bucket)
			{
				player.setAItemInInventory(itemNamesInventory::bucket, false);
				inventorySlot.moveUVByIndex(itemNamesInventory::bucket, 0, 0);
				player.setEvent(EventKeys::firePlace, false);
				selectedItem = -1;
				renderFire = false;
				SoundEngine->play2D("Audio/FireOFF.wav", GL_FALSE);
				lightPos[1] = glm::vec3(1000.0f, 1000.0f, 1000.0f);
				pickableObjets.setShouldRenderByIndex(redKey, true);
				player.setEvent(EventKeys::firePlaceDoor, true);


			}
			else if (player.getEvent(EventKeys::firePlace) && gameState == 1)
			{
				player.setEvent(EventKeys::firePlace, false);
				selectedItem = -1;
			}


			if (pianoDone == true && pianoBool == true)
			{
				pickableObjets.setShouldRenderByIndex(pianoKey, true);
				
				pianoBool = false;
			}

			///-----------------------------------------------------------------------------

			if (rotateObjectBool == true)
			{
				playerMoveBool = false;
				moveObjectIntersect(gWindow);
				pause = true;
			}

			if (player.rayIntersect(interactable.getMinByIndex(1), interactable.getMaxByIndex(1), rayWorld, camPos) && player.getTValue() < 10)
			{

				marker.moveUVByIndex(0, 0, 0);
				if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && player.getTValue() < 10)
				{
					player.setEvent(EventKeys::finalDoor, true);
					gameState = 3;
					inventoryON = true;
				}
			}

			if (player.getEvent(EventKeys::finalDoor) && selectedItem == itemNamesInventory::EndKey)
			{
				player.setAItemInInventory(itemNamesInventory::EndKey, false);
				inventorySlot.moveUVByIndex(itemNamesInventory::EndKey, 0, 0);
				player.setEvent(EventKeys::finalDoor, false);
				gameState = 6;
				selectedItem = -1;
				SoundEngine->stopAllSounds();

				int randDistens = rand() % (int)2;
				if(randDistens == 1)
					SoundEngine->play2D("Audio/puzzle02song.mp3", GL_FALSE);
				else
					SoundEngine->play2D("Audio/puzzle01song.mp3", GL_FALSE);
			}
			else if (player.getEvent(EventKeys::finalDoor) && gameState == 1)
			{
				player.setEvent(EventKeys::finalDoor, false);
				selectedItem = -1;
			}
			if (player.rayIntersect(kungen.getMinByIndex(0), kungen.getMaxByIndex(0), rayWorld, camPos) && player.getTValue() < 10 && GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && ez4enceCount > 30)
			{
				SoundEngine->play2D("Audio/ez4ence.mp3", GL_FALSE);
				ez4enceCount = 0;
			}

			if (pianoDone == true && pianoBool == true)
			{
				pickableObjets.setShouldRenderByIndex(pianoKey, true);

				pianoBool = false;
			}

			for (int i = 0; i < room1.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room1.getMinByIndex(i), room1.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}
			for (int i = 0; i < room2.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room2.getMinByIndex(i), room2.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			player.playerIntersectWithObjects(room3.getMinByIndex(0), room3.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			player.playerIntersectWithObjects(room3.getMinByIndex(1), room3.getMaxByIndex(1), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			
			for (int i = 1; i < room4.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room4.getMinByIndex(i), room4.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}
			for (int i = 1; i < room5.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room5.getMinByIndex(i), room5.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}
			for (int i = 1; i < room6.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room6.getMinByIndex(i), room6.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}
			for (int i = 4; i < room8.nrOfModels(); i++)
			{
				player.playerIntersectWithObjects(room8.getMinByIndex(i), room8.getMaxByIndex(i), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			}

			player.playerIntersectWithObjects(bookShelves.getMinByIndex(0), bookShelves.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);
			player.playerIntersectWithObjects(bookShelves.getMinByIndex(1), bookShelves.getMaxByIndex(1), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);

			player.playerIntersectWithObjectsSphere(writingAndTabel.getMinByIndex(0), writingAndTabel.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), writingAndTabel.getSphereInnerRadiusByIndex(0));
			player.playerIntersectWithObjects(clock.getMinByIndex(0), clock.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0), lastPos);

			//culling with rooms
			if (player.AABBIntersect(culling.getMinByIndex(0), culling.getMaxByIndex(0), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room1.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room4.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);			
			}
			if (player.AABBIntersect(culling.getMinByIndex(1), culling.getMaxByIndex(1), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room2.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3lights.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);

			}
			if (player.AABBIntersect(culling.getMinByIndex(2), culling.getMaxByIndex(2), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room2.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3lights.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);

			}
			if (player.AABBIntersect(culling.getMinByIndex(3), culling.getMaxByIndex(3), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room1.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room4.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);


			}
			if (player.AABBIntersect(culling.getMinByIndex(4), culling.getMaxByIndex(4), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				clock.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				clockHandel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				writingAndTabel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				bookShelves.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room5.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room6.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);


			}
			if (player.AABBIntersect(culling.getMinByIndex(5), culling.getMaxByIndex(5), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				clock.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				clockHandel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				writingAndTabel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				bookShelves.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room5.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room6.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);


			}
			if (player.AABBIntersect(culling.getMinByIndex(6), culling.getMaxByIndex(6), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room7.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				doorLock.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			}

			if (player.AABBIntersect(culling.getMinByIndex(7), culling.getMaxByIndex(7), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room8.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				doorLock.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				piano.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			}
			if (player.AABBIntersect(culling.getMinByIndex(8), culling.getMaxByIndex(8), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				
				clock.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				clockHandel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				bookShelves.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				writingAndTabel.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room5.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room6.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room8.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);

			}
			if (player.AABBIntersect(culling.getMinByIndex(9), culling.getMaxByIndex(9), playerBox.getMinByIndex(0), playerBox.getMaxByIndex(0)))
			{
				room2.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room1.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room4.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room8.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
				room3lights.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);

			}
			lastPos = player.getPlayerPos();
			camPos = player.getPlayerPos();

			glm::vec3 ray = glm::vec3(0.0f, -1.0f, 0.0f);

			camPos.y = (player.getPlayerPos().y - walkTerrain.updateForTerrain(ray, player.getPlayerPos()));

			//------updateObjekts before render ---------------//
			doors.updateByIndex(0, glm::vec3(71.418f, 14.073f, 6.115f), glm::vec3(0, door1Rotation, 0));
			doors.updateByIndex(1, glm::vec3(52.233f, 14.073f, 6.115f), glm::vec3(0, door2Rotation, 0));
			doors.updateByIndex(2, glm::vec3(71.418f, 14.073f, 16.217f), glm::vec3(0, doorAllRotation, 0));
			doors.updateByIndex(3, glm::vec3(52.233f, 14.073f, 16.217f), glm::vec3(0, door3Rotation, 0));
			doors.updateByIndex(4, glm::vec3(-45.794f, 14.073f, 6.115f), glm::vec3(0, doorAllRotation, 0));
			doors.updateByIndex(5, glm::vec3(-45.794f, 14.073f, 16.217f), glm::vec3(0, doorAllRotation, 0));


			
			//clockAndTabel.updateByIndex(2, glm::vec3(-2.349f, 3.824f, 4.993f), glm::vec3(-306.736f, 0.0f, 0.0f));
			
			tPlan.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			interactable.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			pickableObjets.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			mansion.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			mansionStairs.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			doorsAndWalls.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			doors.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			roof.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);
			overDoor.renderAllModels(viewMatrix, projectionMatrix, lightSpace, camPos, lightPos, gDepthMap);

			lightPos[0] = player.getPlayerPos();
			//fireParticles
			if (renderFire == true)
			{
				fireParticle1.update(glm::vec3(77.859, 1.0, 11.713), 50 * deltaTime, 8, 5, 100, 50, glm::vec3(0.025, 0.06, 0.025), glm::vec3(-0.025, 0.005, -0.025), glm::vec3(0, 0, 0),
					glm::vec4(1, 1, 0, 1), glm::vec4(1, 1, 0, 1), 1.0f, 0.3f, 0.0f, 0.0f);
				fireParticle1.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);

				smokeParticle1.update(glm::vec3(77.859, 1.0, 11.713), 50 * deltaTime, 8, 5, 100, 50, glm::vec3(0.025, 0.1, 0.025), glm::vec3(-0.025, 0.005, -0.025), glm::vec3(0, 0, 0),
					glm::vec4(1, 1, 1, 0.5), glm::vec4(0.5, 0.5, 0.5, 0.5), 1.0f, 0.3f, 10.0f, -10.0f);
				smokeParticle1.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &smoke1);

				sparkParticle1.update(glm::vec3(77.859, 1.0, 11.713), 50 * deltaTime, 10, 2, 100, 50, glm::vec3(0.05, 0.05, 0.05), glm::vec3(-0.05, 0.01, -0.05), glm::vec3(0, 0, 0),
					glm::vec4(1, 1, 0, 1), glm::vec4(1, 1, 0, 1), 1.0f, 0.5f, 90.0f, -90.0f);
				sparkParticle1.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &sparks1);
			}

			int tempCandle = 0;
			if (GLFW_RELEASE == glfwGetKey(gWindow, GLFW_KEY_E))
			{
				candleDelay = true;
			}
			//room3 light puzzle
			if (player.rayIntersect(room3lights.getMinByIndex(0), room3lights.getMaxByIndex(0), rayWorld, camPos)
				&& player.getTValue() < 10 && pressLight1 == true)
			{
				marker.moveUVByIndex(0,0,0);
				if(GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && candleDelay)
				{
					pressLight1 = false;
					OutputDebugStringA("1");
					tempCandle = 1;
					candleArray[lightCounter] = tempCandle;
					candleDelay = false;
					lightCounter++;
					SoundEngine->play2D("Audio/lightCandle.ogg", GL_FALSE);
				}
			}
			
			if (player.rayIntersect(room3lights.getMinByIndex(1), room3lights.getMaxByIndex(1), rayWorld, camPos)
				&& player.getTValue() < 10 && pressLight2 == true)
			{
				marker.moveUVByIndex(0, 0, 0);
				if(GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && candleDelay)
				{
					pressLight2 = false;
					OutputDebugStringA("2");
					tempCandle = 2;
					candleArray[lightCounter] = tempCandle;
					candleDelay = false;
					lightCounter++;
					SoundEngine->play2D("Audio/lightCandle.ogg", GL_FALSE);

				}
			}
			
			if (player.rayIntersect(room3lights.getMinByIndex(2), room3lights.getMaxByIndex(2), rayWorld, camPos)
				&& player.getTValue() < 10 && pressLight3 == true)
			{
				marker.moveUVByIndex(0, 0, 0);
				if(GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && candleDelay)
				{
					pressLight3 = false;
					OutputDebugStringA("3");
					tempCandle = 3;
					candleArray[lightCounter] = tempCandle;
					candleDelay = false;
					lightCounter++;
					SoundEngine->play2D("Audio/lightCandle.ogg", GL_FALSE);

				}
			}
			
			if (player.rayIntersect(room3lights.getMinByIndex(3), room3lights.getMaxByIndex(3), rayWorld, camPos)
				&& player.getTValue() < 10 && pressLight4 == true)
			{
				marker.moveUVByIndex(0, 0, 0);
				if(GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && candleDelay)
				{
					pressLight4 = false;
					OutputDebugStringA("4");
					tempCandle = 4;
					candleArray[lightCounter] = tempCandle;
					candleDelay = false;
					lightCounter++;
					SoundEngine->play2D("Audio/lightCandle.ogg", GL_FALSE);

				}
			}
		
			if (player.rayIntersect(room3lights.getMinByIndex(4), room3lights.getMaxByIndex(4), rayWorld, camPos)
				&& player.getTValue() < 10 && pressLight5 == true)
			{
				marker.moveUVByIndex(0, 0, 0);
				if(GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_E) && candleDelay)
				{
					pressLight5 = false;
					OutputDebugStringA("5");
					tempCandle = 5;
					candleArray[lightCounter] = tempCandle;
					candleDelay = false;
					lightCounter++;
					SoundEngine->play2D("Audio/lightCandle.ogg", GL_FALSE);

				}
			}
			
			if (pressLight1 == false)
			{
				candle1.update(glm::vec3(39.631f, 15.032f, 31.566f), 50 * deltaTime, 8, 5, 20, 50, glm::vec3(0.0025, 0.016, 0.0025), glm::vec3(-0.0025, 0.0015, -0.0025), glm::vec3(0, 0, 0),
					glm::vec4(0, 1, 0, 0.5), glm::vec4(0, 0.5, 0.0, 0.5), 0.35f, 0.01f, 0.0f, 0.0f);
				candle1.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);
			}
			if (pressLight2 == false)
			{
				candle2.update(glm::vec3(45.795f, 15.032f, 27.064f), 50 * deltaTime, 8, 5, 20, 50, glm::vec3(0.0025, 0.016, 0.0025), glm::vec3(-0.0025, 0.0015, -0.0025), glm::vec3(0, 0, 0),
					glm::vec4(0, 1, 0, 0.5), glm::vec4(0, 0.5, 0.0, 0.5), 0.35f, 0.01f, 0.0f, 0.0f);
				candle2.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);
			}
			if (pressLight3 == false)
			{
				candle3.update(glm::vec3(38.102f, 15.032f, 27.032f), 50 * deltaTime, 8, 5, 20, 50, glm::vec3(0.0025, 0.016, 0.0025), glm::vec3(-0.0025, 0.0015, -0.0025), glm::vec3(0, 0, 0),
					glm::vec4(0, 1, 0, 0.5), glm::vec4(0, 0.5, 0.0, 0.5), 0.35f, 0.01f, 0.0f, 0.0f);
				candle3.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);
			}
			if (pressLight4 == false)
			{
				candle4.update(glm::vec3(44.35f, 15.032f, 31.577f), 50 * deltaTime, 8, 5, 20, 50, glm::vec3(0.0025, 0.016, 0.0025), glm::vec3(-0.0025, 0.0015, -0.0025), glm::vec3(0, 0, 0),
					glm::vec4(0, 1, 0, 0.5), glm::vec4(0, 0.5, 0.0, 0.5), 0.35f, 0.01f, 0.0f, 0.0f);
				candle4.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);
			}
			if (pressLight5 == false)
			{
				candle5.update(glm::vec3(41.941f, 15.032f, 24.272f), 50 * deltaTime, 8, 5, 20, 50, glm::vec3(0.0025, 0.016, 0.0025), glm::vec3(-0.0025, 0.0015, -0.0025), glm::vec3(0, 0, 0),
					glm::vec4(0, 1, 0, 0.5), glm::vec4(0, 0.5, 0.0, 0.5), 0.35f, 0.01f, 0.0f, 0.0f);
				candle5.renderParticals(&gShaderProgramPartical, &gVertexAttributePartical, viewMatrix, projectionMatrix, &fire1);
			}
		
		

			if (candleArray[0] == 1 && candleArray[1] == 2 && candleArray[2] == 3 && candleArray[3] == 4 && candleArray[4] == 5 && lightCounter == 5)
			{
				OutputDebugStringA("nice key\n");
				renderGoldkey = true;

			}
			else if (lightCounter == 5)
			{
				lightCounter = 0;
				pressLight1 = true;
				pressLight2 = true;
				pressLight3 = true;
				pressLight4 = true;
				pressLight5 = true;
				
			}
			
			if (renderGoldkey == true && endKeyBool == true)
			{
				pickableObjets.setShouldRenderByIndex(EndKey, true);

				endKeyBool = false;
			}


			marker.render2DAllModels();
			
			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		glfwSwapBuffers(gWindow);
	}

	// SHUTDOWN
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glDeleteVertexArrays(1, &gVertexAttributeFS);
	glDeleteVertexArrays(1, &gVertexAttributePartical);
	glDeleteBuffers(1, &gVertexBufferFS);
	glfwTerminate();

	return 0;

}

void initWindow(unsigned int w, unsigned int h)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	gWindow = glfwCreateWindow(w, h, "Mansion Escape", NULL, NULL);
	if (!gWindow) {
		fprintf(stderr, "error creating window\n");
		exit(-1);
	}
	glfwMakeContextCurrent(gWindow);
	glewExperimental = GL_TRUE;

	glfwSwapInterval(1);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
	}
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	fprintf(stderr, "Renderer: %s\n", renderer);
	fprintf(stderr, "OpenGL version %s\n", version);
	
	/// start up time
	/// timerStart = glfwGetTime();
	glClearColor(0.9f, 0.2f, 0.2f, 0.0f);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);

	return;
}