#pragma once
#include <iostream>
#include <olc_net.h>
#include "Game.h"




class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{



public:
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

		if (key[0] && !old_key[0]);
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
		olc::net::message<CustomMsgTypes> msg;
		olc::net::message<CustomMsgTypes> msg2;
		msg.header.id = CustomMsgTypes::ServerAccept;
		msg.header.id = CustomMsgTypes::GetName;
		client->Send(msg);
		client -> Send(msg2);
		return true;
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
			MakeStringSendable( client->GetName(),msg);
			MessageAllClients(msg, client);

		}
		break;
		case CustomMsgTypes::GetName:
		{	std::string name = ReceiveString(msg);
			client->SetName(name);
			std::cout << name << " has joinned the server.\n";
		}
		break;
		case CustomMsgTypes::Disconnect:
		{
			std::cout << client->GetName() << " has disconnected\n";
			client->Disconnect();
		}
		}
	}
};

int main()
{
	CustomServer server(60000); 
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	


	return 0;
}