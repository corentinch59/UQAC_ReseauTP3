#pragma once

#include <vector>
#include <cstring>

template <typename T>
void BinarySerializer::BinarySerializeType(std::vector<uint8_t>& byteArray, T data)
{
	const std::size_t oldSize = byteArray.size();
	byteArray.resize(oldSize + sizeof(data));
	std::memcpy(&byteArray[oldSize], &data, sizeof(data));
}

inline void BinarySerializer::BinarySerializeType(std::vector<uint8_t>& byteArray, std::string& data)
{
	BinarySerializeType<uint16_t>(byteArray, data.size());

	const std::size_t oldSize = byteArray.size();
	byteArray.resize(oldSize + data.size());
	std::memcpy(&byteArray[oldSize], data.data(), data.size());
}