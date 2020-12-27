#pragma once
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "Rules.h"
class Game
{

public:
	void Go();
	Game();
private:
	int numberOfPlayers = 3;
	std::vector<Player> players;
	std::vector<Player*> winners;
	int smallBlind = 10;
	int bigBlind = 2 * smallBlind;

	int numberOfPlayersInTheHand = numberOfPlayers;
	void Rotate();
	void Betting(bool isFirst=false);
	void InitalDeal();
	void Flop();
	void Turn();
	void River();
	void Reveal();
	void Reset();
	bool ValidOption(Inputs::Option input, const Player& pla);
	
	void SmallBlind(Player& pla);
	void BigBlind(Player& pla);
	bool ExecuteOption(Inputs::Option option, Player& pla);
	void ServerReveal();
	void RaiseBy(int amount, Player& pla);
	void RaiseTo(int amount, Player& pla);
	void Bet(int amount, Player& pla);
	void Call(Player& pla);
	void All_In(Player& pla);
	void Fold(Player& pla);
	void Check(Player& pla);
	void PayOutWinners();
	void PrintBankrolls();
	void PrintPot();
	
	int potThisRound = 0;
	int pots[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int currentBet = 0;
	int lastRaise = 0;
	std::vector<Card> cCards;
	Deck deck;
};

