#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"


DevConsole::DevConsole(AABB2 screenBounds)
{
	m_screenBounds = screenBounds;
	//m_outputLines = std::vector<OutputLine>();
	m_currentInput = std::string();

	//is it safe to access g_gameConfig from windowAspect??
	//float aspect = g_gameConfigBlackboard.GetValue("windowAspect", 1.f);
	//float screenWidth = g_gameConfigBlackboard.GetValue("width", 1000.f);
	float screenWidth = screenBounds.GetWidth();
	float screenHeight = screenBounds.GetHeight();
	Vector2 screenMins = screenBounds.mins;

	m_consoleArea = AABB2(screenMins.x, screenMins.y, screenMins.x + (screenWidth * m_consoleWidthRatio), screenMins.y + (screenHeight* m_consoleHeightRatio));
	//m_consoleArea = AABB2(0.f,0.f, screenWidth * m_consoleWidthRatio, screenHeight * m_consoleHeightRatio);
	m_consoleBox = AABB2(m_consoleArea);


	//calculate line height based on console height and lines visible
	m_lineHeight = m_consoleBox.GetHeight() / (m_maxLinesVisible + .5f);		//add .5f to prevent some overflow at the top because of the padding added to boxes for display
	m_drawTextHeight = .75f * m_lineHeight;
	m_consoleBox.AddPaddingToSides(m_lineHeight * -.15f, 0.f);		//the actual text area
	m_consoleBox.Translate(0.f, m_lineHeight * .15f);		//shifts it up a bitt
	m_consoleArea.AddPaddingToSides(0.f, m_lineHeight * .25f);

	//split the console draw area into an input area and ouput area - input area is 1 line high
	//input line box should never translate
	m_consoleBox.SplitAABB2Horizontal(1.f / (m_maxLinesVisible *.9f), m_outputBox, m_inputLineBox);
	m_outputBox.AddPaddingToSides(-.25f * m_lineHeight, -.05f * m_lineHeight);
	m_inputLineBox.AddPaddingToSides(-.25f * m_lineHeight,-.1f * m_lineHeight);

	m_currentInput = "";
	//m_commandHistory = Strings();

	RegisterCommands();
	ReadConsoleHistoryFromFile();



}

DevConsole::~DevConsole()
{
	WriteHistoryToFile();
}

void DevConsole::SetRenderer(Renderer * renderer)
{
	m_renderer = renderer;
}

void DevConsole::PostStartup()
{
	//Startup RCS
	RemoteCommandService::GetInstance()->m_consoleReference = this;
	//ThreadCreateAndDetach( (thread_cb) RemoteCommandServiceUpdate);
}

void DevConsole::Update(float deltaSeconds)
{
	if (m_isOpen){
		m_age += deltaSeconds;
		if (m_age - m_lastFlash > (m_cursorFlashTime * 2.f)){
			m_lastFlash = m_age;
		}
		FindAutoCompleteStrings();
	}

	RemoteCommandService::GetInstance()->Update();
	
}

void DevConsole::Render()
{
	ASSERT_OR_DIE(m_renderer != nullptr, "Set renderer on dev console dingus");
	m_renderer->DrawAABB2(m_consoleArea, m_backgroundColor);		//draw the background quad
	m_renderer->DrawAABB2Outline(m_consoleBox, m_defaultTextColor.GetColorWithAlpha(128));
	RenderInput();
	RenderOutput();
	RenderAutoComplete();
	RemoteCommandService::GetInstance()->Render(m_renderer, m_screenBounds);
}

void DevConsole::RenderInput()
{
	AABB2 border = AABB2(m_inputLineBox);
	border.AddPaddingToSides(m_drawTextHeight * -.05f, m_drawTextHeight * -.05f);
	m_renderer->DrawAABB2(border, m_backgroundColor.GetColorWithAlpha(200));			//makes the input area less opaque so it's super easy to read
	m_renderer->DrawAABB2Outline(border, m_defaultTextColor.GetColorWithAlpha(128));		//border around input area
	//m_renderer->DrawAABB2(m_inputLineBox, RGBA::RED);
	m_renderer->DrawTextInBox2D("> " + m_currentInput, m_inputLineBox, Vector2(0.f,0.5f), m_drawTextHeight, TEXT_DRAW_OVERRUN, m_defaultTextColor);
	float cursorXOffset = m_renderer->GetTextWidth("> " + m_currentInput.substr(0, m_cursorPosition), m_drawTextHeight);

	//render the cursor
	if (m_age - m_lastFlash < m_cursorFlashTime){
		Vector2 cursorStart = Vector2(m_inputLineBox.mins.x + cursorXOffset, m_inputLineBox.mins.y);
		Vector2 cursorEnd = Vector2(m_inputLineBox.mins.x + cursorXOffset, m_inputLineBox.maxs.y);
		m_renderer->DrawLine2D(cursorStart, cursorEnd, m_defaultTextColor.GetColorWithAlpha(200), m_defaultTextColor.GetColorWithAlpha(200), 2.f );
	}
}

void DevConsole::RenderOutput()
{
	// reset the output line box position every frame
	// (could also copy from inputBox and translate up by line height maybe)
	m_outputLineBox = AABB2(m_outputBox.mins, Vector2(m_outputBox.maxs.x, m_outputBox.mins.y + m_lineHeight));
	
	//draw each line (for now)
	if ((int) m_outputLines.size() > 1){
		int linesDrawn = 0;
		for(unsigned int i = (unsigned int) m_outputLines.size(); i > 0; i--){
			linesDrawn++;
			if (linesDrawn >= floorf(m_maxLinesVisible)){
				break;		//break if off the console screen
			} 
			OutputLine outputLine = m_outputLines.at(i-1);
			m_renderer->DrawTextInBox2D(outputLine.text, m_outputLineBox, Vector2(0.f,0.5f), m_drawTextHeight, TEXT_DRAW_OVERRUN, outputLine.color);
			m_outputLineBox.Translate(0.f, m_lineHeight);
		}
	}

	
}

void DevConsole::Open()
{
	g_Window->RegisterHandler(DevConsoleHandler);
	m_isOpen = true;
	m_currentInput = "";
	m_cursorPosition = 0;
	m_autoCompleteIndex = 0;
	SetCursor(g_Window->m_cursor);
}

void DevConsole::Close()
{
	g_Window->UnregisterHandler(DevConsoleHandler);
	m_isOpen = false;
	//SetCursor(NULL);
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

void DevConsole::ClearConsole()
{
	//m_outputLines = ThreadSafeVector<OutputLine>();
	m_outputLines.clear();
}

void DevConsole::ProcessInput(unsigned char key)
{
	switch(key){
	case (VK_RETURN):
		ExecuteCurrentInput();
		break;
	case (VK_BACK):
		DeleteKeyFromInput();
		break;
	case (VK_TAB):
		if (m_autoCompletes.size() > 0){
			m_currentInput = m_autoCompletes[m_autoCompleteIndex]->m_name;
			m_cursorPosition = (int) m_currentInput.size();
		}
		break;
	case (VK_ESCAPE):		//clear current input
		m_currentInput = "";
		m_cursorPosition = 0;		
		break;
	default:
		AddCharacterToInput(key);
		break;
	}
}

void DevConsole::ProcessKeydownEvents(unsigned char key)
{
	switch(key){
		case (VK_RIGHT):
			MoveInputCusor(1);
			break;
		case (VK_LEFT):
			MoveInputCusor(-1);
			break;
		case (VK_UP):
			if (m_autoCompletes.size() > 0){
				MoveThroughAutoCompletes(1);
			} else {
				MoveThroughCommandHistory(1);
			}
			break;
		case (VK_DOWN):
			if (m_autoCompletes.size() > 0){
				MoveThroughAutoCompletes(-1);
			} else {
				MoveThroughCommandHistory(-1);
			}
			break;
		case (VK_ESCAPE):
			m_historyPosition = 0;
			m_currentInput = "";
			break;
		case (VK_DELETE):					//VK_DELETE reads as the same char as '.' from WM_CHAR soooo
			DeleteKeyFromInput(1);
			break;
	}
}

void DevConsole::MoveInputCusor(int direction)
{
	m_cursorPosition+=direction;
	if (m_cursorPosition < 0){
		m_cursorPosition = 0;
	}
	if (m_cursorPosition > (int) m_currentInput.size()){
		m_cursorPosition = (int) m_currentInput.size();
	}
}

void DevConsole::AddLineToOutput(std::string line, RGBA color)
{
	OutputLine newLine = OutputLine(line, color);
	m_outputLines.push_back(newLine);
	for(int i = 0; i < (int) m_hooks.size(); i++){
		m_hooks.at(i)(newLine);
	}
}

void DevConsole::AddLineToOutput(std::string line)
{
	AddLineToOutput(line, m_defaultTextColor);
	//m_outputLines.push_back(OutputLine(line, m_defaultTextColor));
}

void DevConsole::AddHook(consolehook_cb newHook)
{
	m_hooks.push_back(newHook);
}

void DevConsole::RemoveHook(consolehook_cb hook)
{
	for (int i = 0; i < (int) m_hooks.size(); i++){
		if (m_hooks.at(i) == hook){
			m_hooks.erase(i);
			break;
		}
	}
}

std::string DevConsole::GetOutput()
{
	std::string output = "";
	for (unsigned int i = 0; i < m_outputLines.size(); i++){
		output.append(m_outputLines.at(i).text + "\n");
	}
	return output;
}

Strings DevConsole::GetCommandHistory()
{

	return m_commandHistory.get_vector();
}

void DevConsole::AddCommandToHistory(std::string commandText)
{
	if (m_commandHistory.size() == 0){
		m_commandHistory.push_back(commandText);
	} else {
		for (unsigned int i = 0; i <  m_commandHistory.size() ; i++){
			if (m_commandHistory.at(i) == commandText){
				m_commandHistory.erase(i);
				break;
			}
		}

		if (m_commandHistory.size() == 0){
			m_commandHistory.push_back(commandText);
		} else {
			m_commandHistory.insert(1, commandText);
		}
	}
}

void DevConsole::MoveThroughCommandHistory(int direction)
{
	m_historyPosition+=direction;

	if (m_historyPosition > (int) m_commandHistory.size() - 1){
		m_historyPosition = (int) m_commandHistory.size() - 1;
	}
	if (m_historyPosition < 0){
		m_historyPosition = -1;
		m_currentInput = "";
		m_cursorPosition = 0;
		return;
	}
	m_currentInput = m_commandHistory.at(m_historyPosition);
	m_cursorPosition = (int) m_currentInput.size();
}

void DevConsole::MoveThroughAutoCompletes(int direction)
{
	m_autoCompleteIndex+=direction;
	if (m_autoCompleteIndex < 0){
		m_autoCompleteIndex = 0;
	}
	if (m_autoCompleteIndex > (int) m_autoCompletes.size() - 1){
		m_autoCompleteIndex = (int) m_autoCompletes.size() - 1;
	}
	if (m_autoCompletes[m_autoCompleteIndex]->m_syntax != ""){
		m_currentAutoComplete = m_autoCompletes[m_autoCompleteIndex]->m_syntax;
	} else {
		m_currentAutoComplete = m_autoCompletes[m_autoCompleteIndex]->m_name;
	}
	//m_cursorPosition = m_currentInput.size();
}

void DevConsole::FindAutoCompleteStrings()
{
	m_autoCompletes.clear();
	if (m_currentInput.size() >= 3){
		//find 5 strings that match it
 		for (unsigned int i = 0; i < Command::g_commandCount; i++){
			if ((int) m_autoCompletes.size() >= m_maxAutoCompletes){
				break;
			}
			CommandDefinition* def = Command::g_commands[i];
			if (StartsWith(def->m_name, m_currentInput)){
				m_autoCompletes.push_back(def);
			}
		}

		m_autoCompleteIndex = ClampInt(m_autoCompleteIndex, 0, (int) m_autoCompletes.size()-1);
		
	} 

	if (m_autoCompletes.size() == 1 && m_autoCompletes[0]->m_name == m_currentInput){
		m_autoCompletes.clear();
	}

	if (m_autoCompletes.size() > 0){
		if (m_autoCompletes[m_autoCompleteIndex]->m_syntax != ""){
			m_currentAutoComplete = m_autoCompletes[m_autoCompleteIndex]->m_syntax;
		} else {
			m_currentAutoComplete = m_autoCompletes[m_autoCompleteIndex]->m_name;
		}
	} else {
		m_currentAutoComplete = "";
	}
}

void DevConsole::RenderAutoComplete()
{
	float autoCompleteWidth = m_outputBox.GetWidth() * .3f;
	if (m_autoCompletes.size() > 0){
		AABB2 autoBG = AABB2(m_outputBox.mins, Vector2(m_outputBox.mins.x + autoCompleteWidth, m_outputBox.mins.y + (m_lineHeight * m_maxAutoCompletes)));
		m_renderer->DrawAABB2(autoBG, RGBA::BLACK.GetColorWithAlpha(164));
		m_renderer->DrawAABB2Outline(autoBG, RGBA::WHITE);
		AABB2 autoBox = AABB2(m_outputBox.mins, Vector2(m_outputBox.mins.x + autoCompleteWidth, m_outputBox.mins.y + m_lineHeight));
		autoBox.AddPaddingToSides(autoCompleteWidth * -.01f, autoCompleteWidth * -.01f);

		//draw each line (for now)
		
		int linesDrawn = 0;
		for(unsigned int i = 0; i < (unsigned int) m_autoCompletes.size(); i++){
			linesDrawn++;
			if (linesDrawn >= floorf(m_maxLinesVisible)){
				break;		//break if off the console screen
			}
			if ((int) i == m_autoCompleteIndex){
				m_renderer->DrawTextInBox2D(m_autoCompletes[i]->m_name, autoBox, Vector2(0.0f,0.5f), m_drawTextHeight, TEXT_DRAW_OVERRUN, RGBA::GREEN);
			} else {
				m_renderer->DrawTextInBox2D(m_autoCompletes[i]->m_name, autoBox, Vector2(0.0f,0.5f), m_drawTextHeight, TEXT_DRAW_OVERRUN, RGBA::WHITE.GetColorWithAlpha(240));
			}
			autoBox.Translate(0.f, m_lineHeight);
		}
	}

	m_renderer->DrawTextInBox2D("> " + m_currentAutoComplete, m_inputLineBox, Vector2(0.f,0.5f), m_drawTextHeight, TEXT_DRAW_OVERRUN, m_defaultTextColor.GetColorWithAlpha(128));
}

void DevConsole::WriteHistoryToFile()
{
	
	std::fstream* historyFile = new std::fstream();
	std::string fileLog = LOG_DIRECTORY + CONSOLE_HISTORY_FILE;
	historyFile->open(fileLog,  std::fstream::out | std::fstream::trunc );
	int numlines = Min((int) m_commandHistory.size(), (int) MAX_HISTORY_SIZE);
	
	for (int i = 0; i < numlines; i++){
		std::string line = m_commandHistory.at(i) + '\n';
		historyFile->write(line.c_str(), line.size());
	}

}

void DevConsole::ReadConsoleHistoryFromFile()
{
	//m_commandHistory = ThreadSafeVector<std::string>();
	std::fstream historyFile = std::fstream();
	std::string fileLog = LOG_DIRECTORY + CONSOLE_HISTORY_FILE;
	historyFile.open(fileLog,  std::fstream::in);
	std::string line;
	while (std::getline(historyFile, line)){
		m_commandHistory.push_back(line);
	}
}

void DevConsole::RegisterCommands()
{
	CommandRegister("quit", CommandQuit, "exits application");
	CommandRegister("sum", CommandSum, "sums 2 arguments as integers", "sum <int> <int>");
	CommandRegister("help", CommandHelp, "prints all known commands. Keyword only shows commands whose name contains keyword.", "help <keyword>");
	CommandRegister("clear", CommandClear, "clears output of dev console");
	CommandRegister("echo_with_color", CommandEchoWithColor, "prints <str> with color (<r>, <g>, <b>)", "echo_with_color (<r>, <g>, <b>, <optional a>) \"<string>\"");
	CommandRegister("save_log", CommandSaveLog, "Saves log to Logs/<filename.txt>", "save_log <filename.txt>");
	
	//networking
	CommandRegister("net_send_message", CommandSendMessage, "Sends a message to forseth", "net_send_message \"ip:port\" \"msg\"");
	CommandRegister("net_print_local_ip", CommandPrintLocalAddress, "prints local ip");
	CommandRegister("net_host_server", CommandHostServer, "starts hosting a server i guess");

	//RCS
	CommandRegister("rc", CommandSendRemoteMessage, "Sends command to specified connections through remote command service", "rc <clientindex> \"message\"");
	CommandRegister("rca", CommandSendRemoteMessageAll, "Sends command to all connections and runs locally", "rca \"message\"");
	CommandRegister("rcb", CommandSendRemoteMessageBroadcast, "Sends command to all connections and does not run locally", "rcb \"message\"");
	CommandRegister("rc_join", CommandRemoteJoin, "Tries to join RCS on specified address", "rc_join \"ip:port\"" );
	CommandRegister("rc_host", CommandRemoteHost, "Tries to host RCS on specified port (blank for default port)", "rc_host \"port\"" );
	CommandRegister("rc_echo", CommandRemoteSetEcho, "Sets echo functionality for RCS (default = true)", "rc_echo <enabled=true>");

	CommandRegister("spawn_process", CommandSpawnProcess, "Spawns n new clone(s) of this process", "spawn_process <numToSpawn=1>");
}


void DevConsole::ExecuteCurrentInput()
{
	AddLineToOutput("> " + m_currentInput);
	bool executed = CommandRun(m_currentInput.c_str());
	if (!executed){
		std::string printstatement = "   \"" + m_currentInput + "\"\n   is not a recognized command.\n   type \"help\" for information";
		ConsolePrintf(RGBA::RED, printstatement.c_str());

	} else {
		AddCommandToHistory(m_currentInput);
	}
	m_currentInput = "";
	m_cursorPosition = 0;
	m_historyPosition = 0;

}

void DevConsole::AddCharacterToInput(unsigned char key)
{
	std::string newInput = m_currentInput.substr(0, m_cursorPosition);
	newInput+= key;
	if (m_cursorPosition < (int) m_currentInput.size()){
		newInput += m_currentInput.substr(m_cursorPosition);
	}
	m_currentInput = newInput;
	MoveInputCusor(1);
}

void DevConsole::DeleteKeyFromInput(int offset)
{
	if (offset == 1 && m_cursorPosition >= (int) m_currentInput.size()){
		return;
	} else {
		if (m_cursorPosition == 0){
			return;
		}
		std::string newInput = m_currentInput.substr(0, m_cursorPosition-1 + offset);
		newInput += m_currentInput.substr(m_cursorPosition + offset);
		m_currentInput = newInput;
		if (offset == 0){
			MoveInputCusor(-1);
		}
	}
}


bool DevConsoleIsOpen()
{
	return g_devConsole->IsOpen();
}


void ConsolePrintf(RGBA const & color, char const * format, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	//message literal is now the right thing? so split into individual lines and print each line in reverse order with print in box
	Strings lines = Strings();
	Split((std::string) messageLiteral, '\n', lines);
	for (unsigned int i = 0; i < lines.size(); i++){
		g_devConsole->AddLineToOutput(lines[i], color);
	}
}


void ConsolePrintf(char const * format, ...)
{
	if (g_devConsole != nullptr){
		const int MESSAGE_MAX_LENGTH = 2048;
		char messageLiteral[ MESSAGE_MAX_LENGTH ];
		va_list variableArgumentList;
		va_start( variableArgumentList, format );
		vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, format, variableArgumentList );
		va_end( variableArgumentList );
		messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

														 //message literal is now the right thing? so split into individual lines and print each line in reverse order with print in box
		Strings lines = Strings();
		Split((std::string) messageLiteral, '\n', lines);
		for (unsigned int i = 0; i < lines.size(); i++){
			g_devConsole->AddLineToOutput(lines[i]);
		}
	}
}

void ConsolePrint(char const * content)
{
	Strings lines = Strings();
	Split((std::string) content, '\n', lines);
	for (unsigned int i = 0; i < lines.size(); i++){
		g_devConsole->AddLineToOutput(lines[i]);
	}
}

void DevConsoleHandler(unsigned int msg, size_t wparam, size_t lparam)
{
	UNUSED(lparam);
	unsigned char asKey = (unsigned char) wparam;
	switch (msg) {
	case(WM_CHAR):
		g_devConsole->ProcessInput(asKey);
		break;
	case(WM_KEYDOWN):			//arrow keys didn't seem to catch with WM_CHAR?
		g_devConsole->ProcessKeydownEvents(asKey);
		break;
	}
}

void RCSEchoHook(OutputLine hookLine)
{
	RemoteCommandService::GetInstance()->SendEchoMessage(hookLine.text);
}
