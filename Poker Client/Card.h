#pragma once
#include <string>
#include <iostream>
#include <Windows.h>


enum class Suit {
	Clubs,
	Hearts,
	Spades,
	Diamonds
};

class Card
{
public:
	Card();
	Card(int v, Suit s);

	int GetValue();
	Suit GetSuit();
	
	bool operator< (const Card& lhs) const;
	bool operator> (const Card& lhs) const;
	bool operator<=(const Card& lhs) const;
	bool operator>=(const Card& lhs) const;
	bool operator==(const Card& lhs) const;

	void Print() const;
	std::string GetValName() const;
	static std::string GetValName(int val);
	std::string GetSuitName() const;

private:

	
	int value;
	Suit suit;
};

