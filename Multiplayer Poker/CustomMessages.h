#pragma once
#include <iostream>


enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	GetName,
	Disconnect,
	YourCards,
	Flop,
	Turn,
	River,
	Action
};
