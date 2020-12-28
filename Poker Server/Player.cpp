#include "Player.h"

Player::Player()
{
	for (int i = 0;i < 10;i++) {
		inPot[i] = false;
		inTheHand = true;
	}
}

void Player::ReceiveCard(Card& card)
{
	assert(hand.cards.size()<2);
	hand.cards.push_back(card);


}

const Hand& Player::GetHand() const
{
	return hand;
}

const bool Player::GetInTheHand()
{
	return inTheHand;
}

void Player::RevealHand()
{
	std::cout << name << " has ";
	hand.Print();
}

uint32_t Player::GetId()
{
	return id;
}

void Player::ClearHand()
{
	hand.Clear();
}






Inputs::Option Player::GetDecision()
{
	std::cout << name << ", what would you like to do?\n";
	return input.GetUserInput();
}

const int Player::GetCurrentWager() const
{
	return currentWager;
}

void Player::SetInTheHand(bool set)
{
	inTheHand = set;
	
}

void Player::SetID(uint32_t ID)
{
	id = ID;
}

void Player::SetCurrentWager(int x)
{
	currentWager = x;
}

const int Player::GetBankroll() const
{
	return bankroll;
}

void Player::AddToBankroll(int x)
{
	bankroll += x;
}

bool Player::GetIsAllIn()
{
	return isAllIn;
}
void Player::RemoveFromBankroll(int x)
{
	bankroll -= x;
}

void Player::SetIsAllIn(bool b)
{
	isAllIn = b;
}
