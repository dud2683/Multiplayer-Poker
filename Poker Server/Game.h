#pragma once
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "Rules.h"
class Game
{

public:
	
	Game();
	void AddPlayer(Player pla);
private:
	
	int currentID = 0;
	
	int smallBlind = 10;
	int bigBlind = 2 * smallBlind;

	void Swap(Player& p1, Player& p2);
public:
	struct pA{
		pA(Player p, int amount) {
			this->amount = amount;
			this->p = p;
		}
		Player p;
		int amount;
	};
	Player* BigBlindPlayer = nullptr;
	Player* SmallBlindPlayer = nullptr;
	Player* Button = nullptr;
	std::vector<pA> SplitPotWinners;
	bool thereWasAnAllIn = false;
	int numberOfPlayers = 0;
	int numberOfPlayersInTheHand = numberOfPlayers;
	std::vector<Player> players;
	std::vector<Player*> winners;
	void RemovePlayer(uint32_t id);
	std::vector<Player> copy;
	Player* FindPlayerWithID(uint32_t id);
	bool SortPlayersByHand();
	void Rotate();
	void PayoutWinnersWithAllIn();
	int Betting(bool isFirst=false);
	void InitalDeal();
	void Flop();
	void Turn();
	void River();
	void Reveal();
	void Reset();
	void SortByMaxBet();
	bool ValidOption(Inputs::Option input, const Player& pla);
	void ServerReveal();
	int PayOutWinners();
	void PrintBankrolls();
	void PrintPot();
	Player& GetPlayerToBet();
public:
	void SmallBlind(Player& pla);
	void BigBlind(Player& pla);
	bool ExecuteOption(Inputs::Option& option, Player& pla);
	int GetPot();
	void IncreasePlayerToBet();
	
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
	void RemoveFromPossibleWinOfAllPlayers(int amount);
	bool bettingFlag = false;
	int personToBet = 2;
	int totalPot = 0;
	int potThisBettingRound = 0;
	int currentBet = 0;
	int lastRaise = bigBlind;
	
	
	Deck deck;
};

