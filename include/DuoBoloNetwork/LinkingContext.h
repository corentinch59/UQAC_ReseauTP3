#pragma once

#include <DuoBoloShared/ComponentRegistry.h>

#include <vector>

class LinkingContext
{
	public:
		LinkingContext() = default;
		~LinkingContext() = default;

		LinkingContext(const LinkingContext&) = delete;
		LinkingContext(LinkingContext&&) = delete;

		LinkingContext& operator=(const LinkingContext&) = delete;
		LinkingContext& operator=(LinkingContext&&) = delete;

		void ProcessPacket(const std::vector<uint8_t>& byteArray);

	private:
};

