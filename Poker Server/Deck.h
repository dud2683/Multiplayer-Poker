#pragma once
#include "Card.h"
#include <random>
#include "Player.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
#include <cstdlib>
class Deck
{
public:
	Deck();
	void Deal(Player& receiver);
	void Deal(std::vector<Card>& cCards, int numberOfTimes);
	void Shuffle(int numberOfTimes = 8);

private:
	static const int numberOfDecks = 1;
	static const int numberOfCardsPerDeck = 52;
	static const int numberOfCards = numberOfCardsPerDeck * numberOfDecks;
	
	int pointInDeck = 0;
	Card cards[numberOfCards];
	int scratchPad[52];
	void Swap(int a, int b);
};

