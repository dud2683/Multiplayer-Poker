#pragma once
#include <iostream>
#include<string>
#include <vector>

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
	Action,
	StartGame,
	SentAction,
	BoardCards,
	RevealCards
};


