#pragma once
#include "Engine/Core/EngineCommon.hpp"

const std::string LOG_DIRECTORY = "Logs/";
const std::string DEFAULT_LOG_FILE = "console_log.txt";
const std::string CONSOLE_HISTORY_FILE = "console_history.txt";
const unsigned int MAX_HISTORY_SIZE = 64;


class CommandDefinition;

struct OutputLine{
	OutputLine(std:: string outputText, RGBA outputColor = RGBA::WHITE) { text = outputText; color = outputColor;};
	OutputLine(){};
	~OutputLine(){};

	std::string text;
	RGBA color = RGBA();
};

class DevConsole 
{
public: 
	DevConsole(AABB2 bounds = AABB2::ZERO_TO_ONE);
	~DevConsole(); 

	void SetRenderer(Renderer* renderer);

	// Handles all input
	void Update(float deltaSeconds);

	// Renders the display
	void Render(); 
	void RenderInput();
	void RenderOutput();

	void Open(); 
	void Close(); 
	bool IsOpen(); 
	void ClearConsole();

	void ProcessInput(unsigned char key);
	void ProcessKeydownEvents(unsigned char key);
	

	void AddLineToOutput(std::string line, RGBA color);
	void AddLineToOutput(std::string line);

	std::string GetOutput();


	// [E02.00]
	// Returns a list containing the last entered
	// commands ordered by how recently they were used.
	// If a command is entered twice, it should not appear
	// twice, but instead just just move to the front. 
	// History length is up to you, but 32 or 64 is good.
	Strings GetCommandHistory(); 
	//adds a string to the front of command history, and removes any duplicate commands from the history
	void AddCommandToHistory(std::string commandText);

private: 
	void RegisterCommands();

	//input processing
	void ExecuteCurrentInput();
	void AddCharacterToInput(unsigned char key);
	void DeleteKeyFromInput(int offset = 0);

	void MoveInputCusor(int direction = 1);
	void MoveThroughCommandHistory(int direction);
	void MoveThroughAutoCompletes(int direction);

	void FindAutoCompleteStrings();
	void RenderAutoComplete();
	void WriteHistoryToFile();
	void ReadConsoleHistoryFromFile();


	ThreadSafeVector<OutputLine>	m_outputLines;
	//std::vector<OutputLine>		m_outputLines;		//how should output be represented??
	std::string m_currentInput;		//updates currentinput

	std::string m_currentAutoComplete;		//syntax of the current auto complete command
	std::vector<CommandDefinition*> m_autoCompletes;

	bool m_isOpen = false;
	int m_cursorPosition = 0;

	Strings m_commandHistory;		//newest commands first
	int m_historyPosition = 0;

	//config junk for the rendering
	RGBA m_backgroundColor = RGBA(16, 0, 64, 200);		//bg color of the console
	RGBA m_defaultTextColor = RGBA::WHITE;			//default text color
	float m_maxLinesVisible = 30.f;
	float m_consoleWidthRatio = 1.f;
	float m_consoleHeightRatio = .5f;
	float m_lineHeight;
	float m_drawTextHeight;		//text height slightly smaller than line height for some sweet sweet padding
	AABB2 m_consoleArea		= AABB2();
	AABB2 m_consoleBox		= AABB2();
	AABB2 m_inputLineBox	= AABB2();		//the single line of input
	AABB2 m_outputLineBox	= AABB2();		//the single line of output, 
	AABB2 m_outputBox		= AABB2();			//the entire area of output??

	int m_maxAutoCompletes  = 15;
	int m_autoCompleteIndex = 0;


	float m_cursorFlashTime = .4f;
	float m_age = 0.f;
	float m_lastFlash = 0.f;


	Renderer* m_renderer = nullptr;
	// excercise to the reader
	// add whatever members and methods you need to make it work

//public:
//	static DevConsole* GetInstance(); 
};

// Global functions

// I like this as a C function or a static 
// method so that it is easy for systems to check if the dev
// console is open.  
bool DevConsoleIsOpen(); 

// Should add a line of coloured text to the output 
void ConsolePrintf( RGBA const &color, char const *format, ... ); 

// Same as previous, be defaults to a color visible easily on your console
void ConsolePrintf( char const *format, ... ); 

void ConsolePrint(char const* content );

void DevConsoleHandler(unsigned int msg, size_t wparam, size_t lparam);