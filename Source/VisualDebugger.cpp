#include "..\Headers\VisualDebugger.h"
#include "..\Headers\Camera.h"
#include "..\Extras\Renderer.h"
#include "..\Extras\HUD.h"
#include <vector>

namespace VisualDebugger
{
	using namespace physx;

	enum RenderMode
	{
		DEBUG,
		NORMAL,
		BOTH
	};

	enum HUDState
	{
		EMPTY = 0,
		HELP = 1,
		PAUSE = 2,
		CUSTOMIZE = 3,
		COMPLETE = 4
	};

	struct directions {
		float x;
		float y;
		float z;
	} cameraDirection;

	//function declarations
	void KeyHold();
	void KeySpecial(int key, int x, int y);
	void KeyRelease(unsigned char key, int x, int y);
	void KeyPress(unsigned char key, int x, int y);

	void motionCallback(int x, int y);
	void onMouseClick(int button, int state, int x, int y);
	void exitCallback(void);

	void RenderScene();
	void ToggleRenderMode();
	void HUDInit();

	///simulation objects
	Camera* camera;
	PhysicsEngine::MyScene* scene;
	PxReal delta_time = 1.f/60.f;
	
	PxReal gForceStr = 20;
	PxReal swingStr = 300;

	RenderMode render_mode = NORMAL;
	const int MAX_KEYS = 256;
	bool key_state[MAX_KEYS];
	bool hud_show = true;
	HUD hud;

	bool leftMouseClicked = false;
	bool rightMouseClicked = false;
	bool aiming = false;
	bool ready = true;

	PxTransform actorPos;
	PxVec3 cameraDir;

	///mouse handling
	int mMouseX = 0;
	int mMouseY = 0;

	//Init the debugger
	void Init(const char *window_name, int width, int height)
	{
		///Init PhysX
		PhysicsEngine::PxInit();
		scene = new PhysicsEngine::MyScene();
		scene->Init();

		///Init renderer
		Renderer::BackgroundColor(PxVec3(150.f/255.f,150.f/255.f,150.f/255.f));
		Renderer::SetRenderDetail(40);
		Renderer::InitWindow(window_name, width, height);
		Renderer::Init();

		cameraDirection.x = 0.f;
		cameraDirection.y = -.5f;
		cameraDirection.z = -1.f;
		cameraDir = PxVec3(cameraDirection.x, cameraDirection.y, cameraDirection.z);

		camera = new Camera(PxVec3(0.f, 15.f, 30.f), cameraDir, 25.f);

		//initialise HUD
		HUDInit();

		///Assign callbacks
		//render
		glutDisplayFunc(RenderScene);

		//keyboard
		glutKeyboardFunc(KeyPress);
		glutSpecialFunc(KeySpecial);
		glutKeyboardUpFunc(KeyRelease);

		//mouse
		glutMotionFunc(motionCallback); //detect mouse movement
		glutMouseFunc(onMouseClick); //detect mouse click

		//exit
		atexit(exitCallback);

		//init motion callback
		motionCallback(0, 0);
	}

	void HUDInit()
	{
		//initialise HUD
		//add an empty screen
		hud.AddLine(EMPTY, "");
		//add a help screen
		hud.AddLine(HELP, " Simulation");
		hud.AddLine(HELP, "    F1 - reset level");
		hud.AddLine(HELP, "    F2 - pause");
		hud.AddLine(HELP, "    F3 - change golf ball");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Display");
		hud.AddLine(HELP, "    F5 - help on/off");
		hud.AddLine(HELP, "    F6 - shadows on/off");
		hud.AddLine(HELP, "    F7 - render mode");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Camera");
		hud.AddLine(HELP, "    W,S,A,D,Q,Z - forward,backward,left,right,up,down");
		hud.AddLine(HELP, "    mouse + click - change orientation");
		hud.AddLine(HELP, "    spacebar + hold - follow golfball");
		hud.AddLine(HELP, "    F8 - reset view");
		hud.AddLine(HELP, "");
		hud.AddLine(HELP, " Force (applied to golf club)");
		hud.AddLine(HELP, "    I,K - swing backward, swing forward");
		//add a pause screen
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "");
		hud.AddLine(PAUSE, "   Simulation paused. Press F2 to continue.");
		//add a customize screen
		hud.AddLine(CUSTOMIZE, " Styles");
		hud.AddLine(CUSTOMIZE, "    1 - default");
		hud.AddLine(CUSTOMIZE, "    2 - rolling pin");
		hud.AddLine(CUSTOMIZE, "");
		hud.AddLine(CUSTOMIZE, " Changing ball. Press F3 to return to commands list.");
		//set font size for all screens
		hud.FontSize(0.018f);
		//set font color for all screens
		hud.Color(PxVec3(0.f,0.f,0.f));
	}

	//Start the main loop
	void Start()
	{ 
		glutMainLoop(); 
	}

	//Render the scene and perform a single simulation step
	void RenderScene()
	{
		//handle pressed keys
		KeyHold();

		//start rendering
		Renderer::Start(camera->getEye(), camera->getDir());

		if ((render_mode == DEBUG) || (render_mode == BOTH))
		{
			Renderer::Render(scene->Get()->getRenderBuffer());
		}

		if ((render_mode == NORMAL) || (render_mode == BOTH))
		{
			std::vector<PxActor*> actors = scene->GetAllActors();
			if (actors.size())
				Renderer::Render(&actors[0], (PxU32)actors.size());
		}

		//adjust the HUD state
		if (hud_show)
		{
			if (scene->Pause())
				hud.ActiveScreen(PAUSE);
			else if (scene->Customize())
				hud.ActiveScreen(CUSTOMIZE);
			else
				hud.ActiveScreen(HELP);
		}
		else
			hud.ActiveScreen(EMPTY);

		//render HUD
		hud.Render();

		//finish rendering
		Renderer::Finish();

		//perform a single simulation step
		scene->Update(delta_time);

		//set actor position and ball velocity
		actorPos = scene->GetSelectedActor()->getGlobalPose();
		//actorPos = scene->getActorPosition();

		// Check if ball is ready to move
		ready = scene->ReadyCheck(scene->angularVel);
	}

	//handle camera control keys
	void CameraInput(int key)
	{
		switch (toupper(key))
		{
		case 'W':
			camera->MoveForward(delta_time);
			break;
		case 'S':
			camera->MoveBackward(delta_time);
			break;
		case 'A':
			camera->MoveLeft(delta_time);
			break;
		case 'D':
			camera->MoveRight(delta_time);
			break;
		case 'Q':
			camera->MoveUp(delta_time);
			break;
		case 'Z':
			camera->MoveDown(delta_time);
			break;
		case ' ': // spacebar
			camera->FollowBall(PxVec3(actorPos.p.x, actorPos.p.y + 7.3f, actorPos.p.z + 20.0f));
		default:
			break;
		}
	}

	//handle force control keys
	void ForceInput(int key)
	{
		if (!scene->GetSelectedActor())
			return;

		switch (toupper(key))
		{
			// Force controls on the selected actor
		case 'I': //forward
			//scene->GetSelectedActor()->addForce(PxVec3(1,0,0)*gForceStr/2);
			scene->GetSelectedActor()->addTorque(PxVec3(-1, 0, 0) * (swingStr * gForceStr));
			break;
		case 'K': //backward
			//scene->GetSelectedActor()->addForce(PxVec3(1,0,0)*gForceStr/2);
			scene->GetSelectedActor()->addTorque(PxVec3(1, 0, 0) * (swingStr * gForceStr));
			break;
		default:
			break;
		}
	}

	///handle special keys
	void KeySpecial(int key, int x, int y)
	{
		//simulation control
		switch (key)
		{
			//display control
			case GLUT_KEY_F5:
				//hud on/off
				hud_show = !hud_show;
				break;
			case GLUT_KEY_F6:
				//shadows on/off
				Renderer::ShowShadows(!Renderer::ShowShadows());
				break;
			case GLUT_KEY_F7:
				//toggle render mode
				ToggleRenderMode();
				break;
			case GLUT_KEY_F8:
				//reset camera view
				camera->Reset();
				break;

			//simulation control
			case GLUT_KEY_F1:
				//reset scene
				scene->firstRun = true;
				scene->Reset();
				break;
			case GLUT_KEY_F2:
				//toggle pause scene
				scene->Pause(!scene->Pause());
				break;	
			case GLUT_KEY_F3:
				//toggle customize scene
				scene->Customize(!scene->Customize());
				break;
			default:
				break;
		}
	}

	//handle single key presses
	void KeyPress(unsigned char key, int x, int y)
	{
		//do it only once
		if (key_state[key] == true)
			return;

		key_state[key] = true;

		//change ball type
		if (scene->Customize())
			scene->UpdateBall(key);

		//exit (ESC)
		if (key == 27)
			exit(0);
	}

	//handle key release
	void KeyRelease(unsigned char key, int x, int y)
	{
		key_state[key] = false;
	}

	//handle holded keys
	void KeyHold()
	{
		for (int i = 0; i < MAX_KEYS; i++)
		{
			if (key_state[i]) // if key down
			{
				CameraInput(i);
				ForceInput(i);
			}
		}
	}

	void motionCallback(int x, int y)
	{
		int dx = mMouseX - x;
		int dy = mMouseY - y;

		if (leftMouseClicked && !aiming && ready)
		{
			aiming = true;
		}
		else if (rightMouseClicked)
		{
			camera->Motion(dx, dy, delta_time);
		}

		mMouseX = x;
		mMouseY = y;
	}

	void onMouseClick(int button, int state, int x, int y)
	{
		if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
		{
			leftMouseClicked = true;
		}
		else
		{
			leftMouseClicked = false;
		}

		if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
		{
			rightMouseClicked = true;
		}
		else
		{
			rightMouseClicked = false;
		}
	}

	void ToggleRenderMode()
	{
		if (render_mode == NORMAL)
			render_mode = DEBUG;
		else if (render_mode == DEBUG)
			render_mode = BOTH;
		else if (render_mode == BOTH)
			render_mode = NORMAL;
	}

	///exit callback
	void exitCallback(void)
	{
		delete camera;
		delete scene;
		PhysicsEngine::PxRelease();
	}
}

