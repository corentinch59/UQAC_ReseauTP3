#include <DuoBoloNetwork/LinkingContext.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>

void LinkingContext::ProcessPacket(const std::vector<uint8_t>& byteArray)
{
	std::size_t offset = 0;
	Opcode code = static_cast<Opcode>(BinaryDeserialize<uint8_t>(byteArray, offset));

	switch(code)
	{
	case Opcode::WorldInit:
		{

			const int entities = BinaryDeserialize<std::uint16_t>(byteArray, offset);
			for(std::size_t i = 0; i < entities; ++i )
			{
				
			}
			break;
		}
	case Opcode::GameData:
		{
			break;
		}
	case Opcode::Entity:
		{
			break;
		}
	}
}
