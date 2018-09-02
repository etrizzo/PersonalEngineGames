#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <queue>


class CommandDefinition;

// A command is a single submitted commmand
// NOT the definition (which I hide internally)
// Comments will be using a Command constructed as follows; 
// Command cmd = Command( "echo_with_color (255,255,0) \"Hello \\"World\\"\" ); 
class Command
{
public:
	Command( char const *str ); 
	~Command(); 

	std::string GetName(); // would return "echo_with_color"

	// Gets the next string in the argument list.
	// Breaks on whitespace.  Quoted strings should be treated as a single return 
	std::string GetNextString();   

	float GetNextFloat();
	int GetNextInt();
	RGBA GetNextColor();
	Vector3 GetNextVec3();

	int GetNumArguments();

	Strings m_splitCommand;
	std::string m_rawCommand;
	unsigned int m_splitIndex;

	static CommandDefinition* g_commands[128];		//don't need a lot of commands, so an array won't matter much.
	static unsigned int g_commandCount;					//keeps track of number of commands

};

// Command callbacks take a Command.
typedef void (*command_cb)( Command& cmd ); 

//later add help text/syntax
class CommandDefinition{
public:
	CommandDefinition(std::string name, command_cb callback, std::string helpText, std::string syntax);
	CommandDefinition(){};
	~CommandDefinition(){};

	std::string m_name;
	command_cb m_callback;
	std::string m_syntax;
	std::string m_helpText;
};




// Allows for setting up the system and clenaing up.
// Optional, but may help with some tasks. 
void CommandStartup();
void CommandShutdown(); 

// Registers a command with the system
// Example, say we had a global function named...
//    void Help( Command &cmd ) { /* ... */ }  
// 
// We then, during some startup, call
//    CommandRegister( "help", Help ); 
//	may want to add help text/ syntax to this later
void CommandRegister( const char*name, command_cb cb, const char* helpText = "", const char* syntax = ""); 

// Will construct a Command object locally, and if 
// a callback is associated with its name, will call it and 
// return true, otherwise returns false.
// Name is case-insensitive
bool CommandRun( char const *commandString ); 

void CommandQuit(Command& cmd);
void CommandSum(Command& cmd);
void CommandHelp(Command& cmd);
void CommandClear(Command& cmd);
void CommandEchoWithColor(Command& cmd);
void CommandStrip(Command& cmd);
void CommandSaveLog(Command& cmd);
void CommandRecompileShaders(Command& cmd);


void CommandPrintLocalAddress(Command& cmd);
void CommandSendMessage(Command& cmd);
void CommandHostServer(Command& cmd);

CommandDefinition* GetCommandDefinition(std::string name);

