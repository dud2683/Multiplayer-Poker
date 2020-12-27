#pragma once
#include "Player.h"
#include "Card.h"
#include <string>


namespace rules {
	
	struct Result {
		Result() = default;
		
		int numberOfKickersUsed = 0;
		int leftWon=0;
		HandType winningHand;

		Result(int n, int lWon, HandType& h/*1=left,0=tie,-1=right*/) {

			numberOfKickersUsed = n;
			leftWon = lWon;
			winningHand = h;
		}
		void Print(const Player& pla) ;

	};
	//add any helper functions you want, you dont need to put all the logic into the one methode
	
	Result CompareHands (const Player& p1, const Player& p2, const std::vector<Card>& cCards);

	Hand Merge(const Player& p, const std::vector<Card>& cCards);
	
	int CompareKicker(int kickerToCompare, HandType& left, HandType& right);
}


