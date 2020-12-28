#include <iostream>
#include <olc_net.h>
#include "Inputs.h"




class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()	
	{
		std::cout << "Pinging server";

		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		// Caution with this...
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();		

		msg << timeNow;
		Send(msg);
	}

	void MessageAll()
	{
		std::cout << "Messaging all\n";
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;		
		Send(msg);
	}
	
	void DisconnectFromServer() {
		std::cout << "Disconnecting...\n";
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Disconnect;
		Send(msg);
		Disconnect();
	}
	void StartGame() {
		std::cout << "Starting Game\n";
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::StartGame;
		Send(msg);
	}
		
	
	void SendString(std::string string, olc::net::message<CustomMsgTypes> msg) {
		int numberOfChars = string.length();
		std::vector<char> word;
		for (int i = numberOfChars;i >= 0;i--) {
			msg << string[i];
		}
		msg << numberOfChars;
		Send(msg);
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
};

int main()
{
	Inputs inputObj;
	CustomClient c;
	
	c.Connect("127.0.0.1", 60000);

	bool key[4] = { false, false, false, false };
	bool old_key[4] = { false, false, false, false };

	bool bQuit = false;
	while (!bQuit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
			key[3] = GetAsyncKeyState('4') & 0x8000;
		}

		if (key[0] && !old_key[0]) c.PingServer();
		if (key[1] && !old_key[1]) c.MessageAll();
		if (key[2] && !old_key[2]) c.DisconnectFromServer();
		if (key[3] && !old_key[3]) c.StartGame();

		for (int i = 0; i < 4; i++) old_key[i] = key[i];

		if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{


				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted Connection\n";
				}
				break;


				case CustomMsgTypes::ServerPing:
				{
					// Server has responded to a ping request
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;
					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;

				case CustomMsgTypes::ServerMessage:
				{
					// Server has responded to a ping request	
					
					std::string name = c.ReceiveString(msg);

					std::cout << "Hello from [" << name << "]\n";
				}
				break;

				case CustomMsgTypes::GetName:
				{
					std::cout << "What is your name?\n";
					std::string name;
					std::getline(std::cin, name);
					olc::net::message<CustomMsgTypes> NameMsg;
					NameMsg.header.id = CustomMsgTypes::GetName;
					c.SendString(name, NameMsg);
				
				}
				break;
				case CustomMsgTypes::Action:
				{
					
					uint32_t id;
					msg >> id;
					std::cout << "What would you like to do?\n";
					Inputs::Option op=inputObj.GetUserInput();
					olc::net::message<CustomMsgTypes> action;
					action.header.id = CustomMsgTypes::Action;
					action << op;
					action << id;
					c.Send(action);

				}
				break;
				case CustomMsgTypes::SentAction:
				{
					Inputs::Option Input;
					msg >> Input;
					std::string name = c.ReceiveString(msg);
					
					

					Input.Print(name);
				
				
				}
				break;
				}
			}
		}
		else
		{
			std::cout << "Server Down\n";
			bQuit = true;
		}

	}

	return 0;
}