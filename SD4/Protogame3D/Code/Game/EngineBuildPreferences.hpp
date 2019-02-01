//-----------------------------------------------------------------------------------------------
// EngineBuildPreferences.hpp
//
// Defines build preferences that the Engine should use when building for this particular game.
//
// Note that this file is an exception to the rule "engine code shall not know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//


const static Vector3 GRAVITY = Vector3(0.f, -9.8f, 0.f);

#define AUDIO_ENABLED	// (If uncommented) Disables AudioSystem code and fmod linkage.
#define PROFILING_ENABLED		// (If uncommented) Enables profiling in game code
//#define RCS_ENABLED			// (If uncommented) Enables remote command service in dev console
//#define LOGGING_ENABLED			// (If uncommented) Enables log system
//#define NET_ENABLED				// (If uncommented) Enables networking system
