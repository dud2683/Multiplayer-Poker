#pragma once
#include <string>
#include <vector>
#include "Card.h"
#include "Hand.h"
#include <assert.h>
#include "Inputs.h"
class Player
{

private:
	uint32_t id=0;
	int bankroll=10000;
	
	Hand hand;
	bool inTheHand = true;
	bool inPot[10] ;
	int currentWager = 0;
	int totalWager = 0;
	Inputs input;
	bool isAllIn = false;
public:
	int maximumReturnPerPlayer = bankroll;
	Player();
	std::string name;
	void ReceiveCard(Card& card);
	const Card& GetHand(int x) const;
	const bool GetInTheHand();
	void RevealHand();
	uint32_t GetId();
	void SetBankroll(int x);
	void RemoveFromTotalWager(int x);
	void ClearHand();
	bool GetIsAllIn();
	void SetIsAllIn(bool b);
	Inputs::Option GetDecision();
	void AddToBankroll(int x);
	void RemoveFromBankroll(int x);
	const int GetBankroll() const;
	const int GetCurrentWager() const;
	void SetCurrentWager(int x);
	const int GetTotalWager() const;
	void SetTotalWager(int x);
	void SetInTheHand(bool set);
	void SetID(uint32_t ID);

	

};

