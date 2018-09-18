#include "Game/App.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/DebugRenderSystem.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"
#include "Engine/Networking/UDPTests.hpp"

using namespace std;

App::~App()
{
	CommandShutdown();
	LogSystemShutdown();
	Net::Shutdown();
	delete g_theGame;
	delete g_theAudio;
	delete g_theInput;
	delete g_theRenderer;
	delete g_Window;
	delete g_devConsole;
}

App::App(HINSTANCE applicationInstanceHandle)
{
	tinyxml2::XMLDocument gameConfigDoc;
	gameConfigDoc.LoadFile("Data/Data/GameConfig.xml");
	tinyxml2::XMLElement* configElement = gameConfigDoc.FirstChildElement("GameConfig");

	g_gameConfigBlackboard.PopulateFromXmlElementAttributes(*configElement);

	float aspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	m_nearBottomLeft = g_gameConfigBlackboard.GetValue("bottomLeft", Vector3::ZERO);
	float screenWidth = g_gameConfigBlackboard.GetValue("width", 1000.f);
	float screenHeight = screenWidth / aspect;
	std::string name = g_gameConfigBlackboard.GetValue("appName", "Win32 OpenGL Test App");
	g_Window = new Window(name.c_str(), aspect, applicationInstanceHandle);

	m_appTime = 0.f;
	m_farTopRight = m_nearBottomLeft + Vector3(screenWidth,screenHeight, 100.f);
	m_backgroundColor = RGBA(0,0,0,255);

	g_theRenderer = new Renderer();

	g_theRenderer->Initialize();
	g_theRenderer->LoadShadersFromFile("shaders.xml");
	g_theRenderer->LoadMaterialsFromFile("materials.xml");
	g_theRenderer->SetAmbientLight(RGBA(120,120,128,128));
	
	g_theInput = new InputSystem();
	g_theAudio = new AudioSystem();
	g_theAudio->LoadAudioGroupsFromFile("Audio.xml");
	
	g_theGame = new Game();
	//g_theInput->LockMouse();
	g_theInput->ShowCursor(false);

	g_Window->SetInputSystem(g_theInput);
	AABB2 UIBounds = g_theGame->m_uiCamera->GetBounds();
	Net::Startup();
	g_devConsole = new DevConsole(UIBounds);
	g_devConsole->PostStartup();
	g_devConsole->SetRenderer(g_theRenderer);

	g_profilerVisualizer = new ProfilerVisualizer(g_theRenderer, g_theInput, UIBounds);

	LogSystemStartup();

	

	RegisterCommands();
	CommandStartup();
	Startup();

}


void App::RunFrame()
{
	Profiler::GetInstance()->MarkFrame();
	PROFILE_PUSH("APP::BEGINFRAME");
	GetMasterClock()->BeginFrame();
	g_theGame->SetGameCamera();
	g_theRenderer->BeginFrame(m_nearBottomLeft, m_farTopRight, m_backgroundColor);
	g_theInput->BeginFrame();
	g_theAudio->BeginFrame();
	PROFILE_POP();
	Update();
	Render();
	PROFILE_PUSH("APP::ENDFRAME");
		PROFILE_PUSH("Audio::EndFrame");
	g_theAudio->EndFrame();
		PROFILE_POP();
		PROFILE_PUSH("Input::EndFrame");
	g_theInput->EndFrame();
		PROFILE_POP();
		PROFILE_PUSH("Renderer::EndFrame");
	g_theRenderer->EndFrame(g_Window->m_displayContext);
		PROFILE_POP();
	PROFILE_POP();
	
}

void App::Update()
{
	//Profiler::GetInstance()->Push("App::Update");
	PROFILE_PUSH_FUNCTION_SCOPE();
	m_deltaTime = static_cast<float>(GetCurrentTimeSeconds() - m_appTime);

	float ds = m_deltaTime;
	if (g_theInput->IsKeyDown('T')){
		ds/=10.f;
	}
	ds  = ClampFloat(ds, .005f, .4f);

	if (g_theGame->m_isPaused){
		ds = 0.f;
	}

	g_theGame->Update();
	//if (DevConsoleIsOpen()){
	g_devConsole->Update(ds);
	//}
	g_profilerVisualizer->Update();

	m_appTime = GetCurrentTimeSeconds();
	HandleInput();
	g_theRenderer->UpdateClock(ds, m_deltaTime);
	//Profiler::GetInstance()->Pop();
	//PROFILE_POP();
}

void App::Render()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	g_theGame->Render();

	PROFILE_PUSH("RenderProfiler");
	if (g_profilerVisualizer->IsOpen()){
		g_profilerVisualizer->Render();
	}
	PROFILE_POP();

	PROFILE_PUSH("RenderProfiler");
	if (DevConsoleIsOpen()){
		g_theGame->SetUICamera();
		g_devConsole->Render();
		g_theGame->SetGameCamera();
	}
	PROFILE_POP();


}

void App::RegisterCommands()
{
	//should this be registered in Command::CommandRegister or nah? uses g_theRenderer which is kinda sorta game specific, idk
	CommandRegister("recompile_shaders", CommandRecompileShaders, "Recompiles all shaders registered to the renderer");	

	CommandRegister("debug_startup", CommandDebugRenderStartup, "Starts debug render mode from the gameplay camera");
	CommandRegister("debug_shutdown", CommandDebugRenderShutdown, "Shuts down debug render system and returns camera to gameplay camera");
	CommandRegister("debug_clear", CommandDebugRenderClear, "Clears all current debug render tasks");
	CommandRegister("debug_toggle", CommandDebugRenderToggle, "Toggles rendering of the current debug render tasks. (tasks still age)");
	CommandRegister("debug_set_task", CommandDebugRenderSetTask, "Sets default task (x) for debug render system with optional lifetime and start/end color", "debug_set_task <render_task> <lifetime> <startColor> <endColor>");
	CommandRegister("debug_set_mode", CommandDebugRenderSetDepth, "Sets debug render mode", "debug_set_mode <use_depth|ignore_depth|hidden|xray>" );
	CommandRegister("debug_detach_camera", CommandDebugDetachCamera, "Detaches camera from game camera and enables 5 DOF camera movement");
	CommandRegister("debug_reattach_camera", CommandDebugReattachCamera, "Reattaches camera to in-game camera");
	CommandRegister("debug_show_tasks", CommandDebugPrintTasks, "Prints all debug render types");
	CommandRegister("debug_task", CommandDebugDrawTask, "Draws debug render task", "debug_task <render_task_type>");


	CommandRegister("new_light", CommandMakeNewLight, "Adds new light of specified in front of the camera, with color rgba", "new_light <point|dir|spot> <r,g,b,a>");
	CommandRegister("delete_light", CommandRemoveLight, "Removes light at index i", "delete_light <i>");
	CommandRegister("delete_lights_all", CommandRemoveAllLights, "Removes all lights in the scene");
	CommandRegister("set_light_color", CommandSetLightColor, "Sets light color to rgba", "set_light_color <r,g,b,a>");
	CommandRegister("set_light_pos", CommandSetLightPosition, "Sets light position to xyz", "set_light_pos <x,y,z>");
	CommandRegister("set_light_attenuation", CommandSetLightAttenuation, "Sets attenuation for light i to a0,a1,a2", "set_light_attenuation <i> <a0,a1,a2>");
	CommandRegister("set_ambient_light", CommandSetAmbientLight, "Sets ambient light on renderer.", "set_ambient_light <r,g,b,a>");

	CommandRegister("set_god_mode", CommandSetGodMode, "Sets god mode", "set_god_mode <bool>");
	CommandRegister("toggle_god_mode", CommandToggleGodMode, "Toggles god mode");
	CommandRegister("tgm", CommandToggleGodMode, "Toggles god mode");

	CommandRegister("profiler", CommandToggleProfiler, "Toggles profiler view");
	CommandRegister("profiler_report", CommandPrintProfilerReport, "Prints a frame of the profiler to the console", "profiler_report <tree|flat>");
	CommandRegister("profiler_pause", CommandProfilePause, "Pauses profiling");
	CommandRegister("profiler_resume", CommandProfileResume, "Resumes profiling");

	CommandRegister("threaded_test",CommandConsoleThreadedTest, "Runs threading test");
	CommandRegister("nonthreaded_test", CommandConsoleNonThreadedTest, "Runs non-threaded test");
	CommandRegister("log_thread_test", CommandLogThreadTest, "Runs logging thread test", "log_thread_test <thread_count>");
	CommandRegister("log_flush_test", CommandLogFlushTest, "Tests log flushing");
	CommandRegister("log_warning", CommandLogTestWarning, "Prints a test warning log");
	CommandRegister("log_error", CommandLogTestError, "Prints a test error log");
	CommandRegister("log_print", CommandLogTest, "Prints a test log message");
	CommandRegister("log_hide_tag", CommandLogHideFilter, "Hides tag from log", "log_hide_tag <tagname>");
	CommandRegister("log_show_tag", CommandLogShowFilter, "Shows tag in log", "log_show_tag <tagname>");
	CommandRegister("log_toggle_filters", CommandLogToggleWhitelist, "toggles hidden tags to be shown and vice versa");
	CommandRegister("log_hide_all", CommandLogHideAll, "Hides all log tags");
	CommandRegister("log_show_all", CommandLogShowAll, "Shows all log tags");

	CommandRegister("get_address", CommandGetAddress, "Gets machine address");

	CommandRegister("udp_test_start", CommandUDPTestStart, "Starts udp test system lol");
	CommandRegister("udp_test_stop", CommandUDPTestStop, "Stops udp test systemlol");
	CommandRegister("udp_test_send", CommandUDPTestSend, "Sends message through udp test system", "udp_test_send <ip:port> \"msg\"");
}

void App::HandleInput()
{
	PROFILE_PUSH_FUNCTION_SCOPE();
	if (g_theInput->WasKeyJustPressed(192)){		//the ` key
		if (!DevConsoleIsOpen()){
			g_devConsole->Open();
			g_theInput->SetMouseLocked(false);
			g_theInput->ShowCursor(true);
		} else {
			g_devConsole->Close();
		}
	}

	if (g_theInput->WasKeyJustPressed(VK_F2)){
		g_profilerVisualizer->ToggleOpen();
	}


	if (!DevConsoleIsOpen()){
		//universal keys
		if (g_theInput->WasKeyJustPressed(VK_ESCAPE)){
			CommandRun("quit");
		}

		//have game handle input
		if (!g_profilerVisualizer->IsControllingInput()){
			g_theGame->HandleInput();
		}
		if (g_profilerVisualizer->IsOpen()){
			g_profilerVisualizer->HandleInput(g_theInput);
		}

	/*	if (g_theInput->WasKeyJustPressed(VK_F1)){
			g_theInput->ToggleMouseLock();
			g_theInput->ToggleCursor();
		}*/

	}

	//if (g_theInput->WasKeyJustPressed('M')){
	//	AddProfilerFrameAsTreeToConsole();
	//}
	//if (g_theInput->WasKeyJustPressed('N')){
	//	AddProfilerFrameAsFlatToConsole();
	//}
}

void App::PostStartup()
{
	g_theGame->PostStartup();
	
}




bool App::IsQuitting()
{
	return g_Window->m_isQuitting;
}

void App::Startup()
{
	BytePacker packer = BytePacker(BIG_ENDIAN);
	packer.WriteString("yeezy");
	char out[50];
	packer.ReadString(out, 50);

	BytePacker packer2 = BytePacker(BIG_ENDIAN);
	packer2.WriteBytes(packer.GetWrittenByteCount(), packer.GetBuffer(), false);
	char out2[50];
	packer2.ReadString(out2, 50);
	std::string strang = out2;

}

void CommandDebugRenderStartup(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugStartup();
}

void CommandDebugRenderShutdown(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugShutdown();
}

void CommandDebugRenderClear(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugClear();
}

void CommandDebugRenderToggle(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->DebugToggleRendering();
}

void CommandDebugRenderSetDepth(Command & cmd)
{
	std::string mode = cmd.GetNextString();
	g_theGame->m_debugRenderSystem->SetDrawMode(StringToDebugRenderMode(mode));
}

void CommandDebugRenderSetTask(Command & cmd)
{
	std::string task = cmd.GetNextString();
	float lifetime = cmd.GetNextFloat();
	RGBA startColor = cmd.GetNextColor();
	RGBA endColor = cmd.GetNextColor();
	RenderTaskType newTask = StringToDebugRenderTask(task);
	if (newTask != NUM_RENDER_TASKS){
		ConsolePrintf(RGBA::GREEN, "Set task to: %s (%s)", GetTaskName(newTask), GetTaskID(newTask));
		g_theGame->m_debugRenderSystem->SetCurrentTask(newTask);

		if (lifetime != -1.f){
			ConsolePrintf(RGBA::GREEN, "Set default lifetime to: %f", lifetime);
			g_theGame->m_debugRenderSystem->SetDefaultLifetime(lifetime);
		}

		if ( !(startColor == RGBA::BLACK)){
			
			if (endColor == RGBA::BLACK){
				ConsolePrintf(startColor, "Set task color");
				g_theGame->m_debugRenderSystem->SetDefaultColor(startColor, startColor);
			} else {
				ConsolePrintf(startColor, "Set task start color");
				ConsolePrintf(endColor, "Set task end color");
				g_theGame->m_debugRenderSystem->SetDefaultColor(startColor, endColor);
			}
		}
	} else {
		ConsolePrintf(RGBA::RED, "No task called: %s. \"Type debug_show_tasks\" to view all tasks",  task.c_str());
	}

		
}

void CommandDebugPrintTasks(Command & cmd)
{
	UNUSED(cmd);
	RGBA color2D = RGBA(255,200,200);
	RGBA color3D = RGBA(200,255,255);
	for (int i = 0; i < NUM_2D_TASKS; i++){
		RenderTaskType type = (RenderTaskType) i;
		ConsolePrintf(color2D, "%s (%s) : %s", GetTaskName(type), GetTaskID(type),GetTaskDescription(type));
	}
	for (int i = NUM_2D_TASKS + 1; i < NUM_RENDER_TASKS; i++){
		RenderTaskType type = (RenderTaskType) i;
		ConsolePrintf(color3D, "%s (%s) : %s", GetTaskName(type), GetTaskID(type), GetTaskDescription(type));
	}
}

void CommandDebugDrawTask(Command & cmd)
{
	std::string task = cmd.GetNextString();
	RenderTaskType newTask = StringToDebugRenderTask(task);
	bool draw2D = false;
	if (newTask != NUM_RENDER_TASKS){
		//set new task
		g_theGame->m_debugRenderSystem->SetCurrentTask(newTask);
		draw2D = newTask < NUM_2D_TASKS;
	}
	if (draw2D){
		g_theGame->m_debugRenderSystem->AddCurrent2DTask();
	} else {
		g_theGame->m_debugRenderSystem->AddCurrent3DTask();
	}
}

void CommandDebugDetachCamera(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_debugRenderSystem->DetachCamera();
}

void CommandDebugReattachCamera(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_debugRenderSystem->ReattachCamera();
}


void CommandRecompileShaders(Command & cmd)
{
	UNUSED(cmd);		//may want to add support for specifying the shader root later
	g_theRenderer->RecompileAllShaders();
	ConsolePrintf(RGBA::CYAN, "All shaders recompiled");
}



void CommandMakeNewLight(Command & cmd)
{
	Vector3 pos = cmd.GetNextVec3();
	std::string type = cmd.GetNextString();
	RGBA color = cmd.GetNextColor();
	if (color == RGBA::BLACK){		//default color for GetNextColor();
		color = RGBA::WHITE;
	}
	g_theGame->AddNewLight(type, color);
}

void CommandSetLightColor(Command & cmd)
{
	RGBA color = cmd.GetNextColor();
	if (color == RGBA::BLACK){		//default color for GetNextColor();
		color = RGBA::WHITE;
	}
	g_theGame->SetLightColor(color);
}

void CommandSetLightPosition(Command & cmd)
{
	Vector3 pos = cmd.GetNextVec3();
	g_theGame->SetLightPosition(pos);
}

void CommandSetAmbientLight(Command & cmd)
{
	RGBA color = cmd.GetNextColor();
	if (color == RGBA::BLACK){
		ConsolePrintf(RGBA::RED, "No color specified. Specify color as r,g,b,a\n   ex: set_ambient_light 255,0,0,255");
	} else {
		g_theRenderer->SetAmbientLight(color);
	}
}

void CommandRemoveLight(Command & cmd)
{
	int idx = cmd.GetNextInt();	//returns 0 if none found so that works for now
	//g_theGame->RemoveLight(idx);
	if (idx < (int) g_theGame->GetNumActiveLights()){
		g_theGame->RemoveLight(idx);
	} else {
		ConsolePrintf(RGBA::RED, "Cannot remove light %i because there are only %i lights in the scene :(", idx, (int) (g_theGame->GetNumActiveLights()) );
	}
}

void CommandRemoveAllLights(Command & cmd)
{
	UNUSED(cmd);
	for (int i = 0; i < (int) g_theGame->GetScene()->m_lights.size(); i++){
		g_theGame->RemoveLight();
	}
	g_theGame->GetScene()->m_lights.clear();
}

void CommandSetLightAttenuation(Command & cmd)
{
	int idx = cmd.GetNextInt();
	Vector3 att = cmd.GetNextVec3();

	if (idx < MAX_LIGHTS && idx < (int) g_theGame->GetNumActiveLights()){
		g_theGame->SetLightAttenuation(idx, att);
		ConsolePrintf("Set light %i attenuation to: %f,%f,%f", idx, att.x, att.y, att.z);
	}
}

void CommandSetGodMode(Command & cmd)
{
	std::string gm = cmd.GetNextString();
	if (gm=="false"){
		g_theGame->m_godMode = false;
		ConsolePrintf("God mode is off");
	} else {
		g_theGame->m_godMode = true;
		ConsolePrintf("God mode is off");
	}
}

void CommandToggleGodMode(Command & cmd)
{
	UNUSED(cmd);
	g_theGame->m_godMode = !g_theGame->m_godMode;
	ConsolePrintf(("God mode is: " +  std::to_string(g_theGame->m_godMode)).c_str());
}

void CommandToggleProfiler(Command & cmd)
{
	UNUSED(cmd);
	g_profilerVisualizer->ToggleOpen();
}

void CommandPrintProfilerReport(Command & cmd)
{
	UNUSED(cmd);
	std::string type = cmd.GetNextString();
	if (type == "flat"){
		AddProfilerFrameAsFlatToConsole();
	} else {
		AddProfilerFrameAsTreeToConsole();
	}
}

void CommandProfilePause(Command & cmd)
{
	UNUSED(cmd);
	Profiler::GetInstance()->Pause();
}

void CommandProfileResume(Command & cmd)
{
	UNUSED(cmd);
	Profiler::GetInstance()->Resume();
}

void AddProfilerFrameAsTreeToConsole()
{
	profileMeasurement_t* tree = Profiler::GetInstance()->ProfileGetPreviousFrame();

	if (tree != nullptr){
		ProfilerReport* report = new ProfilerReport();
		report->GenerateReportTreeFromFrame(tree);
		if (tree != nullptr){
			PrintTree(report->m_root);
		}
	} else {
		ConsolePrintf(RGBA::RED, "No profiler frame found - profiling may be disabled in EngineBuildPreferences.hpp");
	}
}

void AddProfilerFrameAsFlatToConsole()
{
	profileMeasurement_t* tree = Profiler::GetInstance()->ProfileGetPreviousFrame();

	if (tree != nullptr){
		ProfilerReport* report = new ProfilerReport();
		report->GenerateReportFlatFromFrame(tree);
		if (tree != nullptr){
			PrintTree(report->m_root);
		}
	} else {
		ConsolePrintf(RGBA::RED, "No profiler frame found- profiling may be disabled in EngineBuildPreferences.hpp");
	}
}

void PrintTree(ProfilerReportEntry * tree, int depth)
{
//	ConsolePrintf("%.64s : %.8fms", tree->m_id, tree->GetTotalElapsedTime());
	std::string text = FormatProfilerReport(tree, depth);
	ConsolePrint(text.c_str());
	for ( ProfilerReportEntry* child : tree->m_children){
		PrintTree(child, depth + 1);
	}
}


void ThreadTestWork(void *)
{
	// Open a file and output about 50MB of random numbers to it; 
	FILE* file = nullptr;
	fopen_s(&file, "garbage.dat", "w+");
	if (file == nullptr) {
		return; 
	} 

	for (unsigned int i = 0; i < 12000000; ++i) {
		int randint = GetRandomIntLessThan(100000); 
		fputs( std::to_string(randint).c_str(), file); 
	}

	fclose(file);
	ConsolePrintf( "Finished ThreadTestWork" ); 
}

void CommandConsoleNonThreadedTest(Command & cmd)
{
	UNUSED(cmd);
	ThreadTestWork(nullptr); 
}

void CommandConsoleThreadedTest(Command & cmd)
{
	UNUSED(cmd);
	ThreadCreateAndDetach( ThreadTestWork, nullptr ); 
}

void CommandLogThreadTest(Command & cmd)
{
	int threadCount = cmd.GetNextInt();
	std::string fileName = cmd.GetNextString();
	if (threadCount <= 0){
		threadCount = 1;
	}
	if (fileName == ""){
		fileName = "Data/big.txt";
	}
	LogTest((unsigned int) threadCount, fileName);
}

void CommandLogFlushTest(Command & cmd)
{
	UNUSED(cmd);
	LogTaggedPrintf("FLUSHTEST", "WE FLUSHING BOISS!!!!");
	LogSystemFlush();
}

void CommandLogTestWarning(Command & cmd)
{
	UNUSED(cmd);
	LogWarningf("Warning test!!!\n");
}

void CommandLogTestError(Command & cmd)
{
	UNUSED(cmd);
	LogErrorf("Error Test!!!!!!!!\n");
}

void CommandLogTest(Command & cmd)
{
	UNUSED(cmd);
	LogPrintf("Default log!!!!!!!!!!!!!!!\n");
}

void CommandLogShowFilter(Command & cmd)
{
	std::string filter = cmd.GetNextString();
	LogShowTag(filter);
}

void CommandLogHideFilter(Command & cmd)
{
	std::string filter = cmd.GetNextString();
	LogHideTag(filter);
}

void CommandLogToggleWhitelist(Command & cmd)
{
	UNUSED(cmd);
	LogToggleWhitelist();
}

void CommandLogHideAll(Command & cmd)
{
	UNUSED(cmd);
	LogHideAllTags();
}

void CommandLogShowAll(Command & cmd)
{
	UNUSED(cmd);
	LogShowAllTags();
}

void CommandGetAddress(Command & cmd)
{
	UNUSED(cmd);
	sockaddr_storage addr;
	int addr_len;
	GetAddressForHost((sockaddr*) &addr, &addr_len, "10.8.151.155", "12345");
	//sockaddr_in* in = (sockaddr_in*) &addr;
	//int x = 0;
	//LogIP((sockaddr_in*) addr);
	//GetAddressForHost(addr, addr_len, "https://www.google.com/");
	//GetAddressExample();
}

void CommandUDPTestStart(Command & cmd)
{

	g_theGame->m_udp->Start();
}

void CommandUDPTestStop(Command & cmd)
{
	g_theGame->m_udp->Stop();
}

void CommandUDPTestSend(Command & cmd)
{
	NetAddress addr;
	std::string str = cmd.GetNextString();
	std::string msg = cmd.GetNextString();
	addr = NetAddress(str);
	if (!addr.IsValid()){
		ConsolePrintf(RGBA::RED, "Requires address.");
		return;
	}
	if (msg == ""){
		ConsolePrintf(RGBA::RED, "Requires message.");
		return;
	}

	ConsolePrintf(RGBA::YELLOW, "Sending message \"%s\"...", msg.c_str());
	g_theGame->m_udp->SendTo(addr, msg.data(), (unsigned int) msg.size());
}

