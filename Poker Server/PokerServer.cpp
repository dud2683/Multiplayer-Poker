#pragma once
#include <iostream>
#include <olc_net.h>
#include "Game.h"




class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	
	Inputs::Option InvalidOption = Inputs::Option(Inputs::Decisions::Check, -1);
private:
	std::vector<Player> waitlist;
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
	void UpdateIDs(CustomServer& server) {
		while (waitlist.size() > 0) {
			auto temp = waitlist.back();
			waitlist.pop_back();
			gm.AddPlayer(temp);
		}
		waitlist.clear();
		for (int i = 0;i < clients.size();i++) {
			auto id = clients[i].clientPtr->GetID();
			clients[i].clientId = id;
		}
		server.gm.Reset();
		if (server.gm.numberOfPlayersInTheHand < 3&&server.gameIsStarted) {
			
			server.Update(-1, true);
			server.gameIsStarted = false;
			UpdateIDs(server);
		}
		
	}
	void AddToWaitlist(Player pla) {
		waitlist.push_back(pla);
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
	
	

public:
	void MakeStringSendable(std::string string, olc::net::message<CustomMsgTypes>& msg) {
		int numberOfChars = string.length();
		std::vector<char> word;
		for (int i = numberOfChars;i >= 0;i--) {
			msg << string[i];
		}
		msg << numberOfChars;
	}
protected:
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::string accept; //= "yes";
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
		else {
			
			return false;
		}
		
	}

	// Called when a client appears to have disconnected
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes>> client)
	{
		std::cout << "Removing client [" << client->GetName() << "]\n";
		gm.RemovePlayer(client->GetID());
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
		waitlist.push_back(currentPlayer);
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

		
			
			

		}
		break;
		
		}
	}
};
void SendAllBankrolls(CustomServer& server) {

	
	

	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		if (server.gm.players[i].GetInTheHand()) {
			olc::net::message<CustomMsgTypes> msg;
			msg.header.id = CustomMsgTypes::Bankrolls;
			
			server.MakeStringSendable(server.gm.players[i].name, msg);
			msg << server.gm.players[i].GetBankroll();
			for (int i = 0;i < server.gm.numberOfPlayers;i++) {
				auto client = server.FindClientWithId(server.gm.players[i].GetId());
				client->Send(msg);
			}

		}
	}
	
	
}
void SendPlayerCards(CustomServer& server){
	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto id = server.gm.players[i].GetId();
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::YourCards;
		msg << server.gm.players[i].GetHand(1);
		msg << server.gm.players[i].GetHand(0);
		
		
		auto client = server.FindClientWithId(id);
		client->Send(msg);

	}
}
void Deal(CustomServer& server) {
	server.gm.InitalDeal();
	SendPlayerCards(server);
	SendAllBankrolls(server);
}
void BettingRound(CustomServer& server, bool first) {
	server.gm.lastPlayerToRaise = nullptr;
	olc::net::message<CustomMsgTypes> bmsg;
	bmsg.header.id = CustomMsgTypes::PotSize;
	bmsg<<server.gm.GetPot();
	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(bmsg);
	}
	
	
	
	if (first) {
		server.gm.SetFirstBettingRound();
	}
	bool resolution = false;
	bool firstPass = true;
	while (!resolution) {
		int loops = 0;
		bool everyoneIsAllIn = false;
		while (!server.gm.GetPlayerToBet().GetInTheHand()&&!everyoneIsAllIn) {
			server.gm.IncreasePlayerToBet();
			loops++;
			if (!(loops < server.gm.numberOfPlayers)) {
				everyoneIsAllIn = true;
				resolution = true;
			}
		}
		auto& player = server.gm.GetPlayerToBet();
		if (server.gm.numberOfPlayersInTheHand == 1)
			resolution = true;
		
		else if(!resolution) {
			
			if (server.gm.lastPlayerToRaise != nullptr&&server.gm.numberOfPlayersInTheHand>1) {
				resolution = server.gm.lastPlayerToRaise->GetId() == player.GetId();
			}
			if (server.gm.lastPlayerToRaise == nullptr)
				server.gm.lastPlayerToRaise = &server.gm.GetPlayerToBet();
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
						if (action.d == Inputs::Decisions::All_In) {
							action.value = player.GetBankroll();
						}
					}
				}

				actionIsValid = server.gm.ValidOption(foundAction, player);
				if (!actionIsValid) {
					olc::net::message<CustomMsgTypes> msg;
					msg.header.id = CustomMsgTypes::InvalidAction;
					msg << player.GetId();
					auto client = server.FindClientWithId(player.GetId());
					client->Send(msg);
					messageFound = false;
				}
				
				else {
					olc::net::message<CustomMsgTypes> newmsg;
					newmsg.header.id = CustomMsgTypes::SentAction;
					server.MakeStringSendable(client->GetName(), newmsg);
					if (foundAction.d == Inputs::Decisions::All_In) {
						foundAction.value = player.GetBankroll();
					}
					newmsg << foundAction;
					server.MessageAllClients(newmsg, client);
					foundAction.Print(client->GetName());
					
						
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

void Flop(CustomServer& server) {
	server.gm.Flop();
	olc::net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::Flop;
	msg << server.gm.cCards[0];
	msg << server.gm.cCards[1];
	msg << server.gm.cCards[2];
	
	
	
	
	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client =server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg);
	}
	SendAllBankrolls(server);
}
void Turn(CustomServer& server) {
	server.gm.Turn();
	olc::net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::Turn;
	msg << server.gm.cCards[3];

	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg);
	}
	SendAllBankrolls(server);
}
void River(CustomServer& server) {
	server.gm.River();
	olc::net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::River;
	msg << server.gm.cCards[4];

	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg);
	}
	SendAllBankrolls(server);
}
int Reveal(CustomServer& server) {


	std::cout << "\n";
	Player* lastPlayer = nullptr;
	rules::Result r;
	int highestKicker = 0;
	int highestRank = 0;
	bool thereIsATie = false;
	bool thereWasAFold = false;

	HandType winningHand;

	if (server.gm.numberOfPlayersInTheHand == 1) {
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::SinglePlayerLeft;
		int i;
		for (int index = 0;index < server.gm.numberOfPlayers;index++) {
			if (server.gm.players[index].GetInTheHand()|| server.gm.players[index].GetIsAllIn())
				i = index;
		}
		std::string name = server.gm.players[i].name;
		server.MakeStringSendable(name, msg);
	
		for (int i = 0;i < server.gm.numberOfPlayers;i++) {
			auto client = server.FindClientWithId(server.gm.players[i].GetId());
			client->Send(msg);
		}
	}
	else {
		//findLastPlayerToBet
		int posOfLastRaise = 0;
		for (int i = 0;i < server.gm.players.size();i++) {
			if (server.gm.lastPlayerToRaise == &server.gm.players[i])
				posOfLastRaise = i;
		}
		int i = posOfLastRaise;
		do {
			 
			auto client = server.FindClientWithId(server.gm.players[i].GetId());
			if (server.gm.players[i].GetInTheHand()) {
				if (lastPlayer == nullptr) {
					lastPlayer = &server.gm.players[i];
					if (server.gm.numberOfPlayersInTheHand > 1) {
						{
							olc::net::message<CustomMsgTypes> msg;
							msg.header.id = CustomMsgTypes::RevealCards;
							Card c1 = server.gm.players[i].GetHand(0);
							Card c2 = server.gm.players[i].GetHand(1);
							std::string name = server.gm.players[i].name;

							server.MakeStringSendable(name, msg);
							msg << c1;
							msg << c2;

							for (int i = 0;i < server.gm.numberOfPlayers;i++) {
								auto client = server.FindClientWithId(server.gm.players[i].GetId());
								client->Send(msg);
							}
						}

					}

					server.gm.winners.push_back(&server.gm.players[i]);
				}
				else {
					r = rules::CompareHands(server.gm.players[i], *lastPlayer, server.gm.cCards);
					if (r.leftWon > 0) {
						if (thereWasAFold) {
							thereWasAFold = false;
							std::cout << "\n";
						}
						{
							olc::net::message<CustomMsgTypes> msg;
							msg.header.id = CustomMsgTypes::RevealCards;
							Card c1 = server.gm.players[i].GetHand(0);
							Card c2 = server.gm.players[i].GetHand(1);
							std::string name = server.gm.players[i].name;

							server.MakeStringSendable(name, msg);
							msg << c1;
							msg << c2;

							for (int i = 0;i < server.gm.numberOfPlayers;i++) {
								auto client = server.FindClientWithId(server.gm.players[i].GetId());
								client->Send(msg);
							}
						}
						lastPlayer = &server.gm.players[i];
						int rank = r.winningHand.rank;
						winningHand = r.winningHand;

						thereIsATie = false;
						while (server.gm.winners.size() > 0) {
							server.gm.winners.pop_back();
						}
						server.gm.winners.push_back(&server.gm.players[i]);
						if (rank > highestRank) {
							highestRank = rank;
						}
						else if (rank = highestRank) {
							if (r.numberOfKickersUsed > highestKicker) {
								highestKicker = r.numberOfKickersUsed;
							}
						}
					}
					else if (r.leftWon == 0) {

						thereIsATie = true;
						server.gm.winners.push_back(&server.gm.players[i]);
						if (thereWasAFold) {
							thereWasAFold = false;
							std::cout << "\n";
						}
						{
							olc::net::message<CustomMsgTypes> msg;
							msg.header.id = CustomMsgTypes::RevealCards;
							Card c1 = server.gm.players[i].GetHand(0);
							Card c2 = server.gm.players[i].GetHand(1);
							std::string name = server.gm.players[i].name;

							server.MakeStringSendable(name, msg);
							msg << c1;
							msg << c2;

							for (int i = 0;i < server.gm.numberOfPlayers;i++) {
								auto client = server.FindClientWithId(server.gm.players[i].GetId());
								client->Send(msg);
							}
						}
					}
					else if (r.leftWon == -1) {
						thereWasAFold = true;
						std::cout << server.gm.players[i].name << " folds\n";
						olc::net::message<CustomMsgTypes> msg;
						msg.header.id = CustomMsgTypes::Folds;

						std::string name = server.gm.players[i].name;

						server.MakeStringSendable(name, msg);


						for (int i = 0;i < server.gm.numberOfPlayers;i++) {
							auto client = server.FindClientWithId(server.gm.players[i].GetId());
							client->Send(msg);
						}
					}
				}
			}
			i++;
			if(i==server.gm.players.size())
				i=0;
		} while (posOfLastRaise!=i);
		
	}
	return highestKicker;
}
void ResetRound(CustomServer& server) {
	server.gm.Reset();
	olc::net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::NewRound;
	olc::net::message<CustomMsgTypes> msg1;
	msg1.header.id = CustomMsgTypes::NewRound;
	olc::net::message<CustomMsgTypes> msg2;
	msg2.header.id = CustomMsgTypes::NewRound;
	int m = 0;
	int n = 1;
	int o = 2;
	std::string nameOfBB = server.gm.BigBlindPlayer->name;
	std::string nameOfSB = server.gm.SmallBlindPlayer->name;
	std::string nameOfButton = server.gm.Button->name;
	server.MakeStringSendable(nameOfBB, msg2);
	server.MakeStringSendable(nameOfSB, msg1);
	server.MakeStringSendable(nameOfButton, msg);
	msg << m;
	msg1 << n;
	msg2 << o;

	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg);
	}
	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg1);
	}
	for (int i = 0;i < server.gm.numberOfPlayers;i++) {
		auto client = server.FindClientWithId(server.gm.players[i].GetId());
		client->Send(msg2);
	}
	
}
void BankModifs(CustomServer& server) {
	std::cout << "Modify bankrolls?\n";
	std::string ans1;
	std::string ans2;
	std::string vstring;
	int amount;
	std::getline(std::cin, ans1);
	if (ans1 == "y") {
		for (int i = 0;i < server.gm.numberOfPlayers;i++) {
			std::cout << server.gm.players[i].name << " "<< server.gm.players[i].GetBankroll()<<": ";
			std::getline(std::cin, ans2);
			if (ans2 == "a") {
				std::cout << "\nHow much would you like to add?\n";
				std::cin >> amount;
				server.gm.players[i].AddToBankroll(amount);
			}
			else if (ans2 == "r") {
				std::cout << "\nHow much would you like to remove?\n";
				std::cin >> amount;
				server.gm.players[i].RemoveFromBankroll(amount);
			}
			std::getline(std::cin, vstring);
		}
		
	}


}
void PayOutWinners(CustomServer& server, int numberOfKickers) {
	int amountWon = server.gm.PayOutWinners();
	int numberOfWinners = server.gm.winners.size();
	for (int i = 0;i < numberOfWinners; i++) {
		
		
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::Winners;
		
		
		Card c1;
		Card c2;
		
		if (!server.gm.thereWasAnAllIn) {
			
			c1 = server.gm.winners[i]->GetHand(0);
			c2 = server.gm.winners[i]->GetHand(1);
			server.MakeStringSendable(server.gm.winners[i]->name, msg);
		}
		else {
			c1 = server.gm.SplitPotWinners[i].p.GetHand(0);
			c2 = server.gm.SplitPotWinners[i].p.GetHand(1);
			server.MakeStringSendable(server.gm.SplitPotWinners[i].p.name, msg);
			amountWon = server.gm.SplitPotWinners[i].amount;
		}
		
		msg << c2;
		msg << c1;
		msg << numberOfKickers;
		msg << numberOfWinners;
		msg << amountWon;

		for (int i = 0;i < server.gm.numberOfPlayers;i++) {
			auto client = server.FindClientWithId(server.gm.players[i].GetId());
			client->Send(msg);
		}
	}
	
	
}



int main()
{
	CustomServer server(45000); 
	server.Start();
	bool thereWasAGame = false;

	while (1)
	{
		if (thereWasAGame) {
			
			BankModifs(server);
			server.UpdateIDs(server);

		}
		server.UpdateIDs(server);
		bool firstRound = true;
		
		if (server.gameIsStarted) {

			server.UpdateIDs(server);
		}
		if (server.gameIsStarted) {

			BankModifs(server);
			ResetRound(server);
			Deal(server);
			
			server.gm.ServerReveal();
			server.gm.PrintBankrolls();
			BettingRound(server, firstRound);
			firstRound = false;
			
			if (server.gm.numberOfPlayersInTheHand > 1) {
				thereWasAGame = true;
				Flop(server);
				BettingRound(server,firstRound);
				if (server.gm.numberOfPlayersInTheHand > 1) {
					Turn(server);
					BettingRound(server, firstRound);
					if (server.gm.numberOfPlayersInTheHand > 1) {
						River(server);
						BettingRound(server, firstRound);


					}

				}
			}
			int temp = Reveal(server);
			
			PayOutWinners(server, temp);
			
			
			
			server.gm.Rotate();
		}
		else {
			server.Update(-1,true);
		}
	
			server.Update(-1);
			
	}
	


	return 0;
}