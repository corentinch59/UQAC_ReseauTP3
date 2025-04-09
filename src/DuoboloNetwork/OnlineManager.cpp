#include <DuoBoloNetwork/OnlineManager.h>

#include <enet6/enet.h>
#include <spdlog/spdlog.h>

OnlineManager::OnlineManager() :
mHost(nullptr)
{
	spdlog::info("Initializing enet6...");
	if (enet_initialize() == 0)
		spdlog::info("Initialization successful");
}

OnlineManager::~OnlineManager()
{
	if (mHost)
		enet_host_destroy(mHost);
	enet_deinitialize();
}


