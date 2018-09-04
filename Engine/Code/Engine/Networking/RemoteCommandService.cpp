#include "RemoteCommandService.hpp"

RemoteCommandService* RemoteCommandService::g_remoteCommandService = nullptr;


RemoteCommandService * RemoteCommandService::GetInstance()
{
	if (g_remoteCommandService == nullptr){
		g_remoteCommandService = new RemoteCommandService();
	}
	return g_remoteCommandService;
}
