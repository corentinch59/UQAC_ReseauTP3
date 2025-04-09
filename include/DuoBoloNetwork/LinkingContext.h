#pragma once

#include <DuoBoloNetwork/Opcodes.h>

#include <vector>
#include <functional>

class LinkingContext
{
	public:
		struct Entry
		{
			Opcodes id;
			std::function<void(std::vector<uint8_t>)> linkFunc;
		};

		LinkingContext();
		~LinkingContext() = default;

		LinkingContext(const LinkingContext&) = delete;
		LinkingContext(LinkingContext&&) = delete;

		LinkingContext& operator=(const LinkingContext&) = delete;
		LinkingContext& operator=(LinkingContext&&) = delete;

		void Register(Entry&& data);

	private:
		void RegisterLinks();
		std::vector<Entry> mEntries;
};

