#pragma once
#include <enet6/enet.h>
#include <vector>

class INetworkEventListener
{
	public:
		virtual ~INetworkEventListener() = default;

		virtual void OnConnected(ENetPeer* peer) = 0;
		virtual void OnDisconnected(ENetPeer* peer) = 0;
		virtual void OnTimedOut(ENetPeer* peer) = 0;
		virtual void OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data) = 0;
};

