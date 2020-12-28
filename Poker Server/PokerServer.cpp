#pragma once
#include <iostream>
#include <olc_net.h>
#include "Game.h"




class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	
	Inputs::Option InvalidOption = Inputs::Option(Inputs::Decisions::Check, -1);
private:
	struct ListableClient {
		ListableClient(std::shared_ptr<olc::net::connection<CustomMsgTypes>>& pointerToClient, uint32_t trackableClientId) {
			clientPtr = pointerToClient;
			clientId = trackableClientId;
		}

		std::shared_ptr<olc::net::connection<CustomMsgTypes>> clientPtr;
		uint32_t clientId;
	};
	struct OwnedInputs {
		OwnedInputs(uint32_t id, Inputs::Option a) {
			clientId = id;
			action = a;
		}
		
		uint32_t clientId;
		Inputs::Option action;
	};
public:
	Inputs::Option FindOptionWithId(uint32_t id) {
		for (int i = 0;i < myInputs.size();i++) {
			if (myInputs[i].clientId == id) {
				auto temp = myInputs[i];
				myInputs.erase(myInputs.begin() + i);
				return temp.action;
			}
		}
		return InvalidOption;
	}
	
	std::vector<ListableClient> clients;
public:
	std::shared_ptr<olc::net::connection<CustomMsgTypes>> FindClientWithId(uint32_t clientId) {
		for (int i = 0;i < clients.size();i++) {
			if (clients[i].clientId == clientId) {
				return clients[i].clientPtr;
			}
		}
	}
	void UpdateIDs() {
		for (int i = 0;i < clients.size();i++) {
			auto id = clients[i].clientPtr->GetID();
			clients[i].clientId = id;
		}
	}

public:
	Game gm;
	bool gameIsStarted = false;
	std::vector<OwnedInputs> myInputs;
	CustomServer(uint16_t nPort) : olc::net::server_interface<CustomMsgTypes>(nPort)
	{
		
	}
	std::string ReceiveString(olc::net::message<CustomMsgTypes> msg) {
		
		int numberOfChars;
		msg >> numberOfChars;

		std::vector<char> input;
		for (int i = 0;i < numberOfChars;i++) {
			char message;
			msg >> message;
			input.push_back(message);
		}

		std::string s(input.begin(), input.end());
		
		return s;
		
	}
	
	void CheckForServerInputs() {
		bool key[3] = { false, false, false };
		bool old_key[3] = { false, false, false };

		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) { gameIsStarted = true; };
		if (key[1] && !old_key[1]);
		if (key[2] && !old_key[2]);
	}

protected:
	void MakeStringSendable(std::string string, olc::net::message<CustomMsgTypes>& msg) {
		int numberOfChars = string.length();
		std::vector<char> word;
		for (int i = numberOfChars;i >= 0;i--) {
			msg << string[i];
		}
		msg << numberOfChars;
	}
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::string accept;
		std::getline(std::cin, accept);
		if (accept == "yes") {
			olc::net::message<CustomMsgTypes> msg;
			olc::net::message<CustomMsgTypes> msg2;
			msg.header.id = CustomMsgTypes::ServerAccept;
			msg.header.id = CustomMsgTypes::GetName;
			client->Send(msg);
			client->Send(msg2);
			clients.push_back(ListableClient(client, 0));
			return true;
		}
		else
			return false;
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetName() << "]\n";
	}

	// Called when a message arrives
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client, olc::net::message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->GetName() << "]: Server Ping\n";

			// Simply bounce message back to client
			client->Send(msg);
		}
		break;

		case CustomMsgTypes::MessageAll:
		{
			std::cout << "[" << client->GetName() << "]: Message All\n";

			// Construct a new message and send it to all clients
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::ServerMessage;
			MakeStringSendable(client->GetName(), msg);
			MessageAllClients(msg, client);

		}
		break;
		case CustomMsgTypes::GetName:
		{	std::string name = ReceiveString(msg);
		client->SetName(name);
		std::cout << name << " has joinned the server.\n";
		Player currentPlayer;
		currentPlayer.name = name;
		currentPlayer.SetID(client->GetID());
		gm.AddPlayer(currentPlayer);
		}
		break;
		case CustomMsgTypes::Disconnect:
		{
			std::cout << client->GetName() << " has disconnected\n";
			client->Disconnect();
		}
		break;
		case CustomMsgTypes::StartGame:
		{
			std::cout << "Starting Game.\n";
			gameIsStarted = true;
			
		}
		break;
		case CustomMsgTypes::Action:
		{
			uint32_t id;
			Inputs::Option Input;
			msg >> id;
			msg >> Input;
			OwnedInputs myInput = OwnedInputs(id, Input);
			myInputs.push_back(myInput);

			olc::net::message<CustomMsgTypes> newmsg;
			newmsg.header.id = CustomMsgTypes::SentAction;
			
			MakeStringSendable( client->GetName(),newmsg);
			newmsg << Input;
			
			MessageAllClients(newmsg, client);
			

		}
		break;
		
		}
	}
};

void Deal(CustomServer& server) {
	server.gm.InitalDeal();
}
void BettingRound(CustomServer& server, bool first) {
	if (first) {
		server.gm.SetFirstBettingRound();
	}
	bool resolution = false;
	bool firstPass = true;
	while (!resolution) {
		auto& player = server.gm.GetPlayerToBet();
		if (server.gm.lastPlayerToRaise != nullptr) {
			resolution = server.gm.lastPlayerToRaise->GetId() == player.GetId();
		}
		if (!resolution) {
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Action;
			msg << player.GetId();
			auto client = server.FindClientWithId(player.GetId());
			client->Send(msg);

			bool messageFound = false;
			Inputs::Option foundAction;
			bool actionIsValid = false;
			while (!actionIsValid) {
				while (!messageFound) {
					server.Update();
					auto action = server.FindOptionWithId(player.GetId());
					if (action != server.InvalidOption) {
						messageFound = true;
						foundAction = action;
					}
				}

				actionIsValid = server.gm.ValidOption(foundAction, player);
				if (!actionIsValid) {
					olc::net::message<CustomMsgTypes> msg;
					msg.header.id = CustomMsgTypes::Action;
					msg << player.GetId();
					auto client = server.FindClientWithId(player.GetId());
					client->Send(msg);
					messageFound = false;
				}

			}

			if (firstPass) {
				server.gm.lastPlayerToRaise = &player;
				firstPass = false;
			}
			if (server.gm.ExecuteOption(foundAction, player)) {
				server.gm.lastPlayerToRaise = &player;
			}
		}
	}
	server.gm.UpdatePot();
	server.gm.ResetBettingRound();
}

int main()
{
	CustomServer server(60000); 
	server.Start();
	

	while (1)
	{
		server.UpdateIDs();
		server.gm.Reset();
		bool firstRound = true;
		if (server.gameIsStarted) {
			Deal(server);
			server.gm.ServerReveal();
			server.gm.PrintBankrolls();
			BettingRound(server, firstRound);
			firstRound = false;
			
			if (server.gm.numberOfPlayersInTheHand > 1) {
				server.gm.Flop();
				BettingRound(server,firstRound);
				if (server.gm.numberOfPlayersInTheHand > 1) {
					server.gm.Turn();
					BettingRound(server, firstRound);
					if (server.gm.numberOfPlayersInTheHand > 1) {
						server.gm.River();
						BettingRound(server, firstRound);


					}

				}
			}
			server.gm.Reveal();
			server.gm.PayOutWinners();
			
			server.gm.Rotate();
		}

		server.Update(-1, true);
	}
	


	return 0;
}