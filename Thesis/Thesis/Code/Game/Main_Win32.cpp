#include "Game/App.hpp"


App* g_theApp;


//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" function
	}
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame()
{
	RunMessagePump();
	g_theApp->RunFrame();
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	srand(static_cast<unsigned int>(time(0)));		//seeds random generator: http://www.cplusplus.com/forum/beginner/5131/
	g_theApp = new App(applicationInstanceHandle);	
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	delete g_theApp;			
	g_theApp = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting()) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}


