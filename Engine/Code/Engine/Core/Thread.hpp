#include <map>
#include <thread>
#include <vector>

/************************************************************************/
/* TYPES                                                                */
/************************************************************************/

// an example of a type-safe pointer type.  We never
// need to define thread_handle_type_t, as it is never actually used,
// just the pointer is; 
struct thread_handle_type_t;
typedef thread_handle_type_t* thread_handle_t; 

// If the above feels gross, this is another option, but no type safety
 //typedef void* thread_handle_t;
//typedef std::thread::id thread_handle_t;

// callback type - this is a very general purpose 
// C function.  Even more advanced types will usually have a function
// like this as their initial filter; 
typedef void (*thread_cb)( void *user_data ); 

typedef std::thread Thread;

/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/

static std::vector<Thread*> s_threads;

// Creating
Thread* ThreadCreate( thread_cb cb, void *user_data = nullptr ); 

// Releasing - how we free an above resource
// Join will wait until the thread is complete before return control to the calling thread
void ThreadJoin( Thread* th ); 
// Detach will return control immediately, and the thread will be freed automatically when complete;
void ThreadDetach( Thread* th ); 

//Thread* GetThreadByID( Thread th );

// OPTIONAL - Helpers
// If you want to start work, but don't want to track it; 
void ThreadCreateAndDetach( thread_cb cb, void *user_data = nullptr ); 

// Control
void ThreadSleep( unsigned int ms ); 
void ThreadYield(); 

// Debug - Optional
void ThreadSetName( char const *name ); 

// Just so you're aware - but do not implement yet
// These control how the OS treats these threads.
//
// CoreAffinity -> Which Processor/Core do we prefer to run this thread on;
// void ThreadSetCoreAffinity( uint core_bitfield ); 
// Priority -> How important is it that this thread run (lower means it may be 
// get starved out.  Higher means it could starve others).
// void ThreadSetPriority( uint priority ); 