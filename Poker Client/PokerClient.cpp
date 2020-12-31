#include <iostream>
#include <string>
#include <limits>
#include <olc_net.h>
#include "Inputs.h"
#include "Hand.h"




class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	bool gameIsStarted = false;
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
		if (!gameIsStarted) {
			std::cout << "Starting Game\n";
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::StartGame;
			Send(msg);
		}
		else {
			std::cout << "The game is already started.\n";
		}
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
void PrintMyCards(Card c1, Card c2) {
	std::cout << "Your cards are ";
	c1.Print();
	std::cout << " and ";
	c2.Print();
	std::cout << ".\n";
}
int main()
{

	struct player {
		player(Card c1, Card c2, std::string name) {
			this->c1 = c1;
			this->c2 = c2;
			this->name = name;
		}
		
		Card c1;
		Card c2;
		std::string name;
	};

	Inputs inputObj;
	CustomClient c;
	std::vector<Card> cCards;
	std::vector<Card> myCards;
	std::vector <player> winners;
	
	std::string ip;
	std::cout << "Enter ip adress:";
	std::getline(std::cin, ip);
	std::cout << "\n";
	c.Connect(ip, 45000);

	bool key[5] = { false, false, false, false, false };
	bool old_key[5] = { false, false, false, false, false };

	std::string Dealer;
	std::string SB;
	std::string BB;
	std::string optional;
	bool titles[3] = { false,false,false };

	bool bQuit = false;
	
	
	while (!bQuit)
	{
		
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
			key[3] = GetAsyncKeyState('4') & 0x8000;
			key[4] = GetAsyncKeyState('5') & 0x8000;
		}

		if (key[0] && !old_key[0]) c.PingServer();
		if (key[1] && !old_key[1]) c.MessageAll();
		if (key[2] && !old_key[2]) c.DisconnectFromServer();
		if (key[3] && !old_key[3]) c.StartGame();
		if (key[4] && !old_key[4]) PrintMyCards(myCards[0], myCards[1]);

		for (int i = 0; i < 5; i++) old_key[i] = key[i];

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
				case CustomMsgTypes::InvalidAction:
					std::cout << "You cannot do that at this time.\n"; 
					[[fallthrough]];
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
				case CustomMsgTypes::YourCards:
				{
					

					Card c1;
					Card c2;
					
					msg >> c1;
					msg >> c2;

					myCards.push_back(c1);
					myCards.push_back(c2);

					std::cout << "You have ";
					
					c1.Print();
					std::cout << " and ";
					c2.Print();
					std::cout << ".\n";


				}
				break;
				case CustomMsgTypes::Flop:
				{
					
					Card c3;
					Card c2;
					Card c1;
					
					msg >> c3;
					msg >> c2;
					msg >> c1;

					cCards.push_back(c1);
					cCards.push_back(c2);
					cCards.push_back(c3);
					std::cout << "The Flop comes\n\t";

					c1.Print();
					std::cout << ",\n\t";
					c2.Print();
					std::cout << " and\n\t";
					c3.Print();
					std::cout << ".\n\n";
					PrintMyCards(myCards[0], myCards[1]);
					

				}
				break;
				case CustomMsgTypes::Turn:
				{
					Card c1;
					msg >> c1;
					cCards.push_back(c1);

					std::cout << "The Turn comes\n\t";

					c1.Print();
					std::cout << ".\nMakeing the Board";
					for (int i = 0;i < cCards.size();i++) {
						if (i == cCards.size() - 1) {
							std::cout << " and";
						}
						else {
							std::cout << ",";
						}
						std::cout << "\n\t";
						cCards[i].Print();
					}
					std::cout << "\n\n";
					PrintMyCards(myCards[0], myCards[1]);
				}
				break;
				
				case CustomMsgTypes::River:
				{
					Card c1;
					msg >> c1;
					cCards.push_back(c1);

					std::cout << "The River comes\n\t";

					c1.Print();
					std::cout << ".\nMakeing the Board";
					for (int i = 0;i < cCards.size();i++) {
						if (i == cCards.size() - 1) {
							std::cout << " and";
						}
						else {
							std::cout << ",";
						}
						std::cout << "\n\t";
						cCards[i].Print();
					}
					std::cout << "\n\n";
					PrintMyCards(myCards[0], myCards[1]);

				}
				break;
				case CustomMsgTypes::Bankrolls:
				{
					int playerBankroll;
					msg >> playerBankroll;
					std::string playerName = c.ReceiveString(msg);
					std::cout <<"\t"<< playerName << " has " << playerBankroll << ".\n";
					
				}
				break;
				case CustomMsgTypes::RevealCards:
				{
					
					
					Card c1;
					Card c2;
					
					msg >> c2;
					msg >> c1;
					std::string plaName = c.ReceiveString(msg);


					Hand pHand;
					
					
					for (int i = 0;i < cCards.size();i++) {
						pHand.cards.push_back(cCards[i]);
					}
					pHand.cards.push_back(c1);
					pHand.cards.push_back(c2);
					HandType handT = pHand.FindBestHand();
					
					


					std::cout << plaName << " shows ";
					c1.Print();
					std::cout << " and ";
					c2.Print();
					
					std::cout << ". Making a ";
					handT.Name();

					std::cout <<handT.name<< "\n";
					
				}
				break;
				case CustomMsgTypes::NewRound:
				{
					c.gameIsStarted = true;
					cCards.clear();
					myCards.clear();
					int numberOfNames;
					msg >> numberOfNames;
					
					if (numberOfNames == 0) {
						Dealer = c.ReceiveString(msg);

						if (Dealer == "Deli")
							optional = "and would you believe it, ";
						else
							optional = "and ";
						titles[0] = true;
					}
					else if (numberOfNames == 1) {
						SB = c.ReceiveString(msg);
						titles[1] = true;
					}
					else if (numberOfNames == 2) {
						BB = c.ReceiveString(msg);
						titles[2] = true;
					}

					if (titles[0] && titles[1] && titles[2]) {
						std::cout << SB << " is the small blind, " << BB << " is the big blind " << optional << Dealer << " is on the button.\n";
						for (int i = 0;i < 3;i++) {
							titles[i] = false;
						}
					}
				}
				break;
				case CustomMsgTypes::Folds:
				{
					std::string name = c.ReceiveString(msg);

					std::cout << name << " mucks.\n";
				}
				break;
				case CustomMsgTypes::PotSize:
				{
					int size;
					msg >> size;
					std::cout << "\nThere is " << size << " in the pot.\n\n";

				}
				break;
				
				case CustomMsgTypes::Winners:
				{
					int numberOfWinners;
					int numberOfKickers;
					int amountWon;
					
					Card c1;
					Card c2;

					msg >> amountWon;
					msg >> numberOfWinners;
					msg >> numberOfKickers;
					msg >> c1;
					msg >> c2;
					std::string name = c.ReceiveString(msg);

					winners.push_back(player(c1, c2, name));

					if (numberOfWinners == winners.size()) {
							
						for (int i = 0;i < winners.size();i++) {

							Hand pHand;
							for (int i = 0;i < cCards.size();i++) {
								pHand.cards.push_back(cCards[i]);
							}
							pHand.cards.push_back(winners[i].c1);
							pHand.cards.push_back(winners[i].c2);
							HandType handT = pHand.FindBestHand();

							std::cout << winners[i].name << " won "<<amountWon<<" with a ";
							handT.Name();
							handT.Print(numberOfKickers);
							if ((i <= (winners.size() - 2))&&winners.size()>1) {
								std::cout << " and\n";
							}
							else if (i == winners.size() - 1) {
								std::cout << ".\n";
							}
						}
						
						winners.clear();
					}
				}
				break;
				
				case CustomMsgTypes::SinglePlayerLeft:
				{
					std::string name;
					name = c.ReceiveString(msg);
					std::cout << name << " wins.\n";
				
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