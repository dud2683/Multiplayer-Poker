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
	void AddPlayer(Player pla);
private:
	
	int currentID = 0;
	
	int smallBlind = 10;
	int bigBlind = 2 * smallBlind;

	
public:
	Player* BigBlindPlayer = nullptr;
	Player* SmallBlindPlayer = nullptr;
	Player* Button = nullptr;
	int numberOfPlayers = 0;
	int numberOfPlayersInTheHand = numberOfPlayers;
	std::vector<Player> players;
	std::vector<Player*> winners;
	void RemovePlayer(uint32_t id);
	void Rotate();
	int Betting(bool isFirst=false);
	void InitalDeal();
	void Flop();
	void Turn();
	void River();
	void Reveal();
	void Reset();
	bool ValidOption(Inputs::Option input, const Player& pla);
	void ServerReveal();
	void PayOutWinners();
	void PrintBankrolls();
	void PrintPot();
	Player& GetPlayerToBet();
public:
	void SmallBlind(Player& pla);
	void BigBlind(Player& pla);
	bool ExecuteOption(Inputs::Option& option, Player& pla);
	int GetPot();
	void IncreasePlayerToBet();
	void RaiseBy(int amount, Player& pla);
	void RaiseTo(int amount, Player& pla);
	void Bet(int amount, Player& pla);
	void Call(Player& pla);
	void All_In(Player& pla);
	void Fold(Player& pla);
	void Check(Player& pla);
	void ResetBettingRound();
	void UpdatePot();
	void SetFirstBettingRound();
	Player* lastPlayerToRaise;
	std::vector<Card> cCards;
private:
	bool bettingFlag = false;
	int personToBet = 2;
	int potThisRound = 0;
	int pots[10] = { 0,0,0,0,0,0,0,0,0,0 };
	int currentBet = 0;
	int lastRaise = 0;
	
	
	Deck deck;
};

