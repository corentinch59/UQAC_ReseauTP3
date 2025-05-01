#include <DuoBoloNetwork/BinarySerialize.h>

void BinarySerializeType(std::vector<uint8_t>& byteArray, std::string& data)
{
	BinarySerializeType<std::uint16_t>(byteArray, data.size());

	const std::size_t oldSize = byteArray.size();
	byteArray.resize(oldSize + data.size());
	std::memcpy(&byteArray[oldSize], data.data(), data.size());
}

std::string BinaryDeserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset)
{
	std::size_t strlen = BinaryDeserialize<std::uint16_t>(byteArray, offset);

	std::string value(strlen, '\0');
	std::memcpy(&value[0], &byteArray[offset], strlen);
	offset += strlen;

	return value;
}
