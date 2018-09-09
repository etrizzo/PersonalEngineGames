#include "Engine/Input/Command.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Renderer.hpp"
#include "Engine/Networking/Net.hpp"
#include "Engine/Networking/RemoteCommandService.hpp"
#include <corecrt_io.h>

unsigned int Command::g_commandCount = 0;
CommandDefinition* Command::g_commands[128];


void CommandStartup()
{
}

void CommandShutdown()
{
}

void CommandRegister( const char* name, command_cb cb, const char* helpText, const char* syntax)
{

	ASSERT_OR_DIE(Command::g_commandCount < sizeof(Command::g_commands), "Too many commands registered, increase sizeof Command::g_commands");
	CommandDefinition* oldDef = GetCommandDefinition(name);
	if (oldDef != nullptr){
		ConsolePrintf("Command: \"%s\" already registered\n  Registered help text: \"%s\"", name, oldDef->m_helpText.c_str());
	} else {
		CommandDefinition* def = new CommandDefinition((std::string) name, cb, (std::string) helpText, (std::string) syntax);
		Command::g_commands[Command::g_commandCount] = def;
		Command::g_commandCount++;
	}
}

bool CommandRun(char const * command)
{
	if (std::string(command) == ""){
		return false;
	}
	Command cmd = Command(command);
	std::string name = cmd.GetName();
	CommandDefinition* def = GetCommandDefinition(name);
	if (def != nullptr){
		def->m_callback(cmd);
		return true;
	} else {
		return false;
	}
}

void CommandQuit(Command & cmd)
{
	UNUSED(cmd);
	g_Window->Quit();
}

void CommandSum(Command & cmd)
{
	int x = cmd.GetNextInt();
	int y = cmd.GetNextInt();
	int sum = x + y;
	ConsolePrintf("  %i + %i = %i\n", x,y,sum);
}

void CommandHelp(Command & cmd)
{
	RGBA color = RGBA(200,255,220,255);
	std::string keyword = cmd.GetNextString();
	Strip(keyword, '-');	//because i always add the flag signifier
	for (unsigned int i = 0; i < Command::g_commandCount; i++){
		CommandDefinition* def = Command::g_commands[i];
		if (Contains(def->m_name, keyword)){
			if (def->m_syntax != ""){
				ConsolePrintf(color,"%s: (%s) %s", def->m_name.c_str(), def->m_syntax.c_str(), def->m_helpText.c_str());
			} else {
				ConsolePrintf(color,"%s: %s", def->m_name.c_str(), def->m_helpText.c_str());
			}
		}
	}
}

void CommandClear(Command & cmd)
{
	UNUSED(cmd);
	g_devConsole->ClearConsole();
	ConsolePrintf(RGBA::GREEN, "\n> clear");
}

// syntax:
// "echo_with_color (255,255,0) "Hello World"
void CommandEchoWithColor(Command & cmd)
{
	RGBA color = cmd.GetNextColor();
	std::string toPrint = cmd.GetNextString();
	ConsolePrintf(color, toPrint.c_str());
}

void CommandStrip(Command & cmd)
{
	std::string stringToStrip = cmd.GetNextString();
	char stripChar = cmd.GetNextString()[0];
	Strip(stringToStrip, stripChar);
	ConsolePrintf(stringToStrip.c_str());
}

void CommandSaveLog(Command & cmd)
{

	FILE *file = nullptr;
	std::string fileName = cmd.GetNextString();
	Strings extension = Strings(); 
	Split(fileName, '.', extension);
	if (extension.size() > 2){
		ConsolePrintf(RGBA::RED, "Please don't include '.' in the file name, you animal\n format: save_log <filename>.txt");
		return;
	}
	if (fileName == ""){
		fileName = DEFAULT_LOG_FILE;
	}
	fileName = LOG_DIRECTORY + fileName;
	char errorString[512];
	errno = 0;
	_chmod(fileName.c_str(), _S_IREAD | _S_IWRITE);
	fopen_s( &file, fileName.c_str(), "w+" );		
	if (file == nullptr){	 
		ConsolePrintf("Failed to open file. error %d\n", errno);
		perror(errorString);
		ConsolePrintf("%s\n", errorString);
	} else {
		std::string output = g_devConsole->GetOutput();
		Strings outputSplit = Strings();
		Split(output, '\n', outputSplit);
		for (unsigned int i = 0; i < outputSplit.size(); i++){
			fputs(outputSplit[i].c_str(), file);
			fputs("\n", file);
		}
		fclose(file);
		ConsolePrintf(RGBA::GREEN, "Log saved to: %s", fileName.c_str());
	}
}



CommandDefinition* GetCommandDefinition(std::string name)
{
	for (unsigned int i = 0; i < Command::g_commandCount; i++){
		if (name == Command::g_commands[i]->m_name)
		{
			return Command::g_commands[i];
		}
	}
	return nullptr;
}

Command::Command(char const * str)
{
	Strings quoteSplitStrings = Strings();
	Split((std::string) str, '"', quoteSplitStrings );		//get the quoted parts of the string - this works if there's no quotes within quotes but should really check for escaped quotes later

	m_splitCommand = Strings();
	for (unsigned int i = 0; i < quoteSplitStrings.size(); i++){
		Strip(quoteSplitStrings[i], ' ');
		Strings splitSegment = Strings();
		if (i % 2 == 0){
			Split(quoteSplitStrings[i], ' ', splitSegment);
		} else {
			splitSegment.push_back(quoteSplitStrings[i]);
		}
		for (unsigned int j = 0; j < splitSegment.size(); j++){
			m_splitCommand.push_back(splitSegment[j]);
		}
	}

	m_rawCommand = (std::string) str;
	m_splitIndex = 1;

}

Command::~Command()
{
}

//gets the cleaned (lowercase, stripped of whitespace on ends) name of the command
std::string Command::GetName()
{
	std::string cleanedName = m_splitCommand[0];
	Strip(cleanedName, ' ');
	//reference: http://en.cppreference.com/w/cpp/algorithm/transform
	std::transform(cleanedName.begin(), cleanedName.end(), cleanedName.begin(), 
		[](unsigned char c) -> unsigned char { return (unsigned char) std::tolower(c); });
	return m_splitCommand[0];
}

std::string Command::GetNextString()
{
	std::string nextString = "";
	while (m_splitIndex < m_splitCommand.size() && nextString == ""){
		nextString = m_splitCommand[m_splitIndex];
		m_splitIndex++;
	}
	return nextString;
}

float Command::GetNextFloat()
{
	std::string str = GetNextString();
	if (str == ""){
		return -1.f;
	}
	return ParseStringFloat(str);	
}

int Command::GetNextInt()
{
	std::string str = GetNextString();
	return ParseStringInt(str);			//do some error checking research on atoi i HATE it
}

RGBA Command::GetNextColor()
{
	std::string str = GetNextString();
	if (str == ""){
		return RGBA::BLACK;
	}
	RGBA color = RGBA();
	color.SetFromText(str.c_str());
	return color;
}

Vector3 Command::GetNextVec3()
{
	std::string str = GetNextString();
	if (str == ""){
		return Vector3::ZERO;
	}
	Vector3 pos = Vector3();
	pos.SetFromText(str.c_str());
	return pos;
}

bool Command::GetNextBool()
{
	std::string str = GetNextString();
	if (str == "" || str == "true"){
		return true;
	}
	return false;
}

int Command::GetNumArguments()
{
	return (int) m_splitCommand.size() - 1;
}

CommandDefinition::CommandDefinition(std::string name, command_cb callback, std::string helpText, std::string syntax)
{
	m_name = name;
	m_callback = callback;
	m_syntax = syntax;
	m_helpText = helpText;
}



void CommandPrintLocalAddress(Command & cmd)
{
	UNUSED(cmd);
	NetAddress addr = NetAddress::GetLocal();
	LogTaggedPrintf( "net", "My Address: %s", addr.ToString().c_str() );
}

void CommandSendMessage(Command& cmd)
{
	//sockaddr_storage addr;
	//int addr_len;
	//bool gotit = GetAddressForHost((sockaddr*) &addr, &addr_len, "10.8.139.114", "12345");
	//sockaddr_in* in = (sockaddr_in*) &addr;
	std::string addrstr = cmd.GetNextString();	//"10.8.151.155:12345"
	std::string msg = cmd.GetNextString();

	ConnectExample(addrstr, msg);
}

void CommandHostServer(Command & cmd)
{
	UNUSED(cmd);
	ThreadCreateAndDetach((thread_cb) HostExample, "12345");
	//ThreadCreateAndDetach((thread_cb) HostExampleWinSock, "12345");
	//HostExampleWinSock("12345");
}

void CommandSendRemoteMessage(Command & cmd)
{
	std::string intString = cmd.GetNextString();
	std::string msg;
	int rcsIndex = 0;
	if (atoi(intString.c_str()) != 0){
		rcsIndex = atoi(intString.c_str());
		msg = cmd.GetNextString();
	} else {
		msg = intString;
	}
	RemoteCommandService::GetInstance()->SendAMessageToAHotSingleClientInYourArea(rcsIndex, msg);
}

void CommandSendRemoteMessageBroadcast(Command & cmd)
{
	std::string msg = cmd.GetNextString();
	RemoteCommandService::GetInstance()->SendMessageBroadcast(msg);
}

void CommandSendRemoteMessageAll(Command & cmd)
{
	std::string msg = cmd.GetNextString();
	RemoteCommandService::GetInstance()->SendMessageAll(msg);
}

void CommandRemoteJoin(Command & cmd)
{
	std::string address = cmd.GetNextString();
	RemoteCommandService::GetInstance()->JoinRCS(address);
}

void CommandRemoteHost(Command & cmd)
{
	std::string port = cmd.GetNextString();
	RemoteCommandService::GetInstance()->HostRCS(port);
}

void CommandRemoteSetEcho(Command & cmd)
{
	bool echo = cmd.GetNextBool();
	RemoteCommandService::GetInstance()->SetShouldEcho(echo);
}

void CommandSpawnProcess(Command & cmd)
{
	int numToSpawn = cmd.GetNextInt();
	if (numToSpawn == 0){
		numToSpawn == 1;
	}

	for (int i = 0; i < numToSpawn; i++){
		//get the module name
		wchar_t buffer[MAX_PATH]; //or wchar_t * buffer;
		::GetModuleFileName(NULL, buffer, MAX_PATH) ;




		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );


		// Start the child process. 
		if( !CreateProcess( buffer,   // No module name (use command line)
			::GetCommandLine(),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
			) 
		{
			printf( "CreateProcess failed (%d).\n", GetLastError() );
			return;
		}
	}

}
