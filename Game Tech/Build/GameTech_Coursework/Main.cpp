#include <enet/enet.h>

#include <nclgl\Window.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\PerfTimer.h>

#include "Scene_Main.h"

const Vector4 status_colour = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
const Vector4 status_colour_header = Vector4(0.2f, 0.2f, 0.2f, 1.0f);

bool show_perf_metrics = false;

PerfTimer timer_total, timer_physics, timer_update, timer_render;
uint shadowCycleKey = 4;
uint sceneIdx = 0;
uint sceneMax = 0;

static int sc = 0;
int settingState = 0;

// Program Deconstructor
//  - Releases all global components and memory
//  - Optionally prints out an error message and
//    stalls the runtime if requested.
void Quit(bool error = false, const string &reason = "") {
	//Release Singletons
	SceneManager::Release();
	PhysicsEngine::Release();
	enet_deinitialize();  
	Window::Destroy();

	//Show console reason before exit
	if (error) {
		std::cout << reason << std::endl;
		system("PAUSE");
		exit(-1);
	}
}


// Program Initialise
//  - Generates all program wide components and enqueues all scenes
//    for the SceneManager to display
void Initialize()
{
	//Initialise the Window
	if (!Window::Initialise("Game Technologies", 1280, 720, false))
		Quit(true, "Window failed to initialise!");

	if (enet_initialize() != 0)
	{
		Quit(true, "ENET failed to initialize!");
	}

	//Initialise the PhysicsEngine
	PhysicsEngine::Instance();

	//Initialize Renderer
	SceneManager::Instance()->InitializeOGLContext(Window::GetWindow());
	if (!SceneManager::Instance()->HasInitialised())
		Quit(true, "Renderer failed to initialise!");

	//Enqueue All Scenes
	SceneManager::Instance()->EnqueueScene(new Scene_Main("That's No Moon"));
}

// Print Debug Info
//  - Prints a list of status entries to the top left
//    hand corner of the screen each frame.
void PrintStatusEntries()
{
	//Print Current Scene Name
	NCLDebug::AddStatusEntry(Vector4(1.0, 0.0, 0.0, 1.0), "[%d/%d]: %s",
		SceneManager::Instance()->GetCurrentSceneIndex() + 1,
		SceneManager::Instance()->SceneCount(),
		SceneManager::Instance()->GetCurrentScene()->GetSceneName().c_str()
		);

	NCLDebug::AddStatusEntry(status_colour, "          ");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "J --- fire balls");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "K --- fire missles");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "Q --- Activate Shield");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "E --- Use the Force");
	NCLDebug::AddStatusEntry(status_colour, "          ");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "I --- Submit score");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 1.0, 1.0), "R --- Reload scene");

	NCLDebug::AddStatusEntry(status_colour, "          ");

	//Print Performance Timers
	NCLDebug::AddStatusEntry(Vector4(1.0, 1.0, 1.0, 1.0), "FPS: %5.2f  (Press U for %s info)", 1000.f / timer_total.GetAvg(), show_perf_metrics ? "less" : "more");
	if (show_perf_metrics)
	{
		NCLDebug::AddStatusEntry(status_colour, "          ");
		timer_total.PrintOutputToStatusEntry(status_colour, "          Total Time     :");
		timer_update.PrintOutputToStatusEntry(status_colour, "          Scene Update   :");
		timer_physics.PrintOutputToStatusEntry(status_colour, "          Physics Update :");
		timer_render.PrintOutputToStatusEntry(status_colour, "          Render Scene   :");

		NCLDebug::AddStatusEntry(status_colour, "          ");
		NCLDebug::AddStatusEntry(status_colour_header, "NCLTech Settings");
		NCLDebug::AddStatusEntry(status_colour, "          Physics Engine: %s (Press P to toggle)", PhysicsEngine::Instance()->IsPaused() ? "Paused  " : "Enabled ");
		NCLDebug::AddStatusEntry(status_colour, "          Monitor V-Sync: %s (Press V to toggle)", SceneManager::Instance()->GetVsyncEnabled() ? "Enabled " : "Disabled");
	}

	Scene* scene = SceneManager::Instance()->GetCurrentScene();
	Scene_Main* scenemain = dynamic_cast<Scene_Main*>(scene);

	ostringstream Score;
	Score << "SCORE: " << scenemain->GetScorePoint();

	NCLDebug::AddStatusEntry(status_colour, "          ");
	NCLDebug::AddStatusEntry(Vector4(0.0, 1.0, 0.0, 1.0), Score.str().c_str());
	sc++;
}


// Process Input
//  - Handles all program wide keyboard inputs for
//    things such toggling the physics engine and 
//    cycling through scenes.
void HandleKeyboardInputs()
{
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
		PhysicsEngine::Instance()->SetPaused(!PhysicsEngine::Instance()->IsPaused());

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_V))
		SceneManager::Instance()->SetVsyncEnabled(!SceneManager::Instance()->GetVsyncEnabled());

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
	{
		Scene* scene = SceneManager::Instance()->GetCurrentScene();
		Scene_Main* scenemain = dynamic_cast<Scene_Main*>(scene);

		//scenemain->SendDataToServer();
		SceneManager::Instance()->JumpToScene(sceneIdx);
		scenemain->CalculateScore(0);
	}

	//Pass datapack to the server
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_I))
	{
		Scene* scene = SceneManager::Instance()->GetCurrentScene();
		Scene_Main* scenemain = dynamic_cast<Scene_Main*>(scene);
		scenemain->SendDataToServer();
		scenemain->CalculateScore(0);
	}

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_U))
		show_perf_metrics = !show_perf_metrics;

	//Press Q to activate the shield
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Q))
	{
		switch (settingState % 2)
		{
		case 0:
		{
			PhysicsEngine::Instance()->SetShieldState(true);
			settingState++;
			break;
		}
		case 1:
		{
			PhysicsEngine::Instance()->SetShieldState(false);
			settingState++;
			break;
		}
		}
	}

	//Press E to Use the force
	if (Window::GetKeyboard()->KeyHeld(KEYBOARD_E))
	{
		PhysicsEngine::Instance()->SetForce(true);
	}
	else
	{
		PhysicsEngine::Instance()->SetForce(false);
	}
}

// Program Entry Point
int main()
{
	//Initialize our Window, Physics, Scenes etc
	Initialize();

	Window::GetWindow().GetTimer()->GetTimedMS();

	//Create main game-loop
	while (Window::GetWindow().UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		//Start Timing
		float dt = Window::GetWindow().GetTimer()->GetTimedMS() * 0.001f;	//How many milliseconds since last update?
		timer_total.BeginTimingSection();

		//Print Status Entries
		PrintStatusEntries();

		//Handle Keyboard Inputs
		HandleKeyboardInputs();

		//Update Performance Timers (Show results every second)
		timer_total.UpdateRealElapsedTime(dt);
		timer_physics.UpdateRealElapsedTime(dt);
		timer_update.UpdateRealElapsedTime(dt);
		timer_render.UpdateRealElapsedTime(dt);

		//Update Scene
		timer_update.BeginTimingSection();
		SceneManager::Instance()->UpdateScene(dt);
		timer_update.EndTimingSection();

		//Update Physics
		timer_physics.BeginTimingSection();
		PhysicsEngine::Instance()->Update(dt);
		timer_physics.EndTimingSection();

		//Render Scene
		timer_render.BeginTimingSection();
		SceneManager::Instance()->RenderScene();
		{
			//Forces synchronisation if vsync is disabled
			// - This is solely to allow accurate estimation of render time
			// - We should NEVER normally lock our render or game loop!		
			glClientWaitSync(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, NULL), GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
		}
		timer_render.EndTimingSection();

		//Finish Timing
		timer_total.EndTimingSection();

		//Let other programs on the computer have some CPU time
		Sleep(0);
	}

	//Cleanup
	Quit();
	return 0;
}
