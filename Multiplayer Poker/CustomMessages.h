#pragma once
#include <iostream>
#include<string>
#include <vector>

enum class CustomMsgTypes : uint32_t
{
	//server stuff
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
	GetName,
	Disconnect,
	//game stuff
	YourCards,
	PotSize,
	Flop,
	Turn,
	River,
	Action,
	Folds,
	InvalidAction,
	StartGame,
	SentAction,
	RevealCards,
	Bankrolls,
	NewRound,
	Winners,
	SinglePlayerLeft
};


