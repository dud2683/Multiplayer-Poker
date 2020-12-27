#pragma once
#include <string>
#include <iostream>
#include "Card.h"
#include <vector>


struct HandType {
	HandType() = default;

	HandType(std::vector<Card> crds, int r);

	std::string name;//flush King high, two pair Kings and Queens, Full house 3s over Jacks
	std::string k1="";//
	std::string k2="";
	std::string k3="";
	std::string k4="";
	int rank=0;
	Card highestCard;
	Card kicker1;
	Card kicker2;
	Card kicker3;
	Card kicker4;

	void Name();
	void Print(int numberOfKickers);
	

};


class Hand
{

public:

	std::vector<Card> cards;
	std::string name;
	int value;
	HandType FindBestHand();
	void Print() const;
	void Clear();
private:
	void Sort(int start = 0);
	void SortSuit(Suit s);

	int FindStraightFlush();
	int FindFourOfAKind();
	int FindFullHouse();
	int FindFlush();
	int FindStraight();
	int FindThreeOfAKind();
	int FindTwoPair();
	int FindPair(int start=0);
	void StraightSort(int x);
	void SortTwoPair();
	void Swap(Card& a, Card& b);

};

