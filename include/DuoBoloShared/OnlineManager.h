#pragma once

#include <DuoBoloShared/INetworkEventListener.h>

#include <enet6/enet.h>

class OnlineManager
{
	public:
		OnlineManager();
		virtual ~OnlineManager();

		OnlineManager(const OnlineManager&) = delete;
		OnlineManager(OnlineManager&&) = delete;

		OnlineManager& operator=(const OnlineManager&) = delete;
		OnlineManager& operator=(OnlineManager&&) = delete;

		void SetListener(INetworkEventListener* listener) { mListener = listener; }
		void PollEvents();

		ENetHost* GetHost() const { return mHost; };

	protected:
		ENetHost* mHost;
		INetworkEventListener* mListener;
};

