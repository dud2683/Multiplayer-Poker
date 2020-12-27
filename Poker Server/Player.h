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
	int id=0;
	int bankroll=10000;
	Hand hand;
	bool inTheHand = false;
	bool inPot[10] ;
	int currentWager = 0;
	Inputs input;
	bool isAllIn = false;
public:
	Player();
	std::string name;
	void ReceiveCard(Card& card);
	const Hand& GetHand() const;
	const bool GetInTheHand();
	void RevealHand();

	void ClearHand();
	bool GetIsAllIn();
	void SetIsAllIn(bool b);
	Inputs::Option GetDecision();
	void AddToBankroll(int x);
	void RemoveFromBankroll(int x);
	const int GetBankroll() const;
	const int GetCurrentWager() const;
	void SetCurrentWager(int x);
	void SetInTheHand(bool set);

	

};

