#include <DuoBoloNetwork/LinkingContext.h>

LinkingContext::LinkingContext()
{
	RegisterLinks(); 
}

void LinkingContext::Register(Entry&& data)
{
	mEntries.push_back(std::move(data));
}

void LinkingContext::RegisterLinks()
{
}
