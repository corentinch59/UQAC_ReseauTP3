#pragma once

#include <cstring>

template <typename T>
void BinarySerializeType(std::vector<uint8_t>& byteArray, T data)
{
	const std::size_t oldSize = byteArray.size();
	byteArray.resize(oldSize + sizeof(data));
	std::memcpy(&byteArray[oldSize], &data, sizeof(data));
}

template<typename T>
T BinaryDeserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset)
{
	T data;
	std::memcpy(&data, &byteArray[offset], sizeof(T));
	offset += sizeof(T);

	return data;
}

template<typename T>
ENetPacket* build_packet(const T& packet, enet_uint32 flags)
{
	std::vector<std::uint8_t> byteArray;

	BinarySerializeType<std::uint8_t>(byteArray, static_cast<uint8_t>(T::opcode));
	packet.Serialize(byteArray);

	return enet_packet_create(byteArray.data(), byteArray.size(), flags);
}