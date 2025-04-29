#pragma once

#include <entt/entt.hpp>
#include <cstdint>

struct NetworkComponent
{
	int id;

	void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);

	static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset);
};

