#pragma once

#include <vector>
#include <string>

class BinarySerializer
{
	public:
		template<typename T> static void BinarySerializeType(std::vector<uint8_t>& byteArray, T data);

		static void BinarySerializeType(std::vector<uint8_t>& byteArray, std::string& data);
};

#include <DuoBoloShared/BinarySerializer.inl>