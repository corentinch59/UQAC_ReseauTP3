#pragma once

#include <vector>
#include <string>
#include <enet6/enet.h>

template<typename T> void BinarySerializeType(std::vector<uint8_t>& byteArray, T data);
void BinarySerializeType(std::vector<uint8_t>& byteArray, std::string& data);

template<typename T> T BinaryDeserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset);
std::string BinaryDeserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset);

template<typename T> ENetPacket* build_packet(const T& packet, enet_uint32 flags);

#include <DuoBoloNetwork/BinarySerialize.inl>