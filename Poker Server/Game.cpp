#include "Game.h"
#include <assert.h>



Game::Game()
{
	
}

void Game::AddPlayer(Player pla)
{
	players.push_back(pla);
	numberOfPlayers++;
}

void Game::Swap(Player& p1, Player& p2)
{
	auto temp = p1;
	p1 = p2;
	p2 = temp;
}

void Game::RemovePlayer(uint32_t id)
{
	for (int i = 0;i < numberOfPlayers;i++) {
		if (players[i].GetId() == id) {
			players.erase(players.begin() + i);
		}
	}
	numberOfPlayers--;
}

Player* Game::FindPlayerWithID(uint32_t id)
{
	for (int i = 0;i < numberOfPlayers;i++) {
		if (players[i].GetId() == id) {
			Player* pp = &players[i];
			return pp;
		}	 
	}
	return nullptr;
}

bool Game::SortPlayersByHand()
{
	copy.clear();
	for (int i = 0;i < numberOfPlayers;i++) {
		copy.push_back(players[i]);
	}
	bool thereWasASwap = true;
	bool thereWasATie = false;
	while (thereWasASwap) {
		thereWasASwap = false;
		for (int i = 0;i < numberOfPlayers - 1;i++) {
			int r = rules::CompareHands(copy[i], copy[i + int(1)], cCards).leftWon;
			if (r < 0) {
				thereWasASwap = true;
				Swap(copy[i], copy[i + int(1)]);
			}
			else if (r == 0) {
				if (copy[i].maximumReturnPerPlayer < copy[i + int(1)].maximumReturnPerPlayer) {
					thereWasASwap = true;
					thereWasATie = true;
					Swap(copy[i], copy[i + int(1)]);
				}
			}
		}
	}
	return thereWasATie;
}

void Game::Rotate()
{
	Player temp = players[0];
	for (int i = 0;i < players.size()-1;i++) {
		int next = i + 1;
		players[i] = players[next];	
	}
	players.back() = temp;
}

void Game::PayoutWinnersWithAllIn()
{
	winners.clear();
	bool tie = SortPlayersByHand();
	int numberOfTimes = 0;
	for (int i = 0;i < numberOfPlayers;i++) {
		int totalForThisPlayer = 0;
		int winnerMaxReturnpp;
		for (int j = numberOfTimes;j < numberOfPlayers;j++) {
			
			winnerMaxReturnpp = copy[i].maximumReturnPerPlayer;
			int totalWagerFromOther = copy[j].GetTotalWager();
			if (totalWagerFromOther >= winnerMaxReturnpp)
				totalForThisPlayer += winnerMaxReturnpp;
			else
				totalForThisPlayer += totalWagerFromOther;
			
		}
		if (totalPot > 0) {
			numberOfTimes++;
			int remainingPot = totalPot	- totalForThisPlayer;
			
			Player* reference = FindPlayerWithID(copy[i].GetId());
			winners.insert(winners.begin(), reference);
			if(remainingPot<=0){
				totalForThisPlayer = totalPot;
			}
			if(totalForThisPlayer>0)
				SplitPotWinners.push_back(pA(*reference, totalForThisPlayer));
			reference->AddToBankroll(totalForThisPlayer);
			totalPot -= totalForThisPlayer;
			RemoveFromPossibleWinOfAllPlayers(winnerMaxReturnpp);
			if (totalPot <= 0) {
				i = numberOfPlayers;
			}
		}
	}
}

int Game::Betting(bool isFirstBetting)
{
	int i=personToBet;
	
	if (isFirstBetting) {
		i = 2;
		currentBet = bigBlind;
	}
	

	Player* lastPlayerToBet = nullptr;
	
	
	if (&players[i] == lastPlayerToBet && numberOfPlayersInTheHand > 1)
		bettingFlag = true;

	if (!bettingFlag) {
		if (lastPlayerToBet == nullptr) {
			lastPlayerToBet = &players[i];
		}
		if (players[i].GetInTheHand() && !players[i].GetIsAllIn()) {
			Inputs::Option chosen = players[i].GetDecision();
			while (!ValidOption(chosen, players[i])) {
				std::cout << "You cannot do that at this time\n";
				chosen = players[i].GetDecision();
			}
			if (ExecuteOption(chosen, players[i])) {
				lastPlayerToBet = &players[i];
			}

		}
		if (i < numberOfPlayers - 1) {
			i++;
		}
		else {
			i = 0;
		}
		return true;
	}
	
}

void Game::InitalDeal()
{
	deck = Deck();
	deck.Shuffle(1);
	for (int i = 0;i < players.size();i++) {
		deck.Deal(players[i]);
	}
	for (int i = 0;i < players.size();i++) {
		deck.Deal(players[i]);
	}
	SmallBlind(players[0]);
	BigBlind(players[1]);

	
}

void Game::Flop()
{
	deck.Deal(cCards, 3);
	std::cout << "The Flop comes ";
	cCards[0].Print();
	std::cout << ", ";
	cCards[1].Print();
	std::cout << " and ";
	cCards[2].Print();
	std::cout << ".";
	std::cout << "\nMaking the board,\n\t";
	cCards[0].Print();
	std::cout << ",\n\t";
	cCards[1].Print();
	std::cout << " and\n\t";
	cCards[2].Print();
	std::cout << ".\n";
	PrintPot();
	std::cout << "\n";
}

void Game::Turn()
{
	deck.Deal(cCards, 1);
	
	std::cout << "The Turn comes ";
	cCards[3].Print();
	std::cout << "\nMaking the board,\n\t";
	cCards[0].Print();
	std::cout << ",\n\t";
	cCards[1].Print();
	std::cout << ",\n\t";
	cCards[2].Print();
	std::cout << " and\n\t";
	cCards[3].Print();
	std::cout << ".\n";
	PrintPot();
	std::cout << "\n";

}

void Game::River()
{
	deck.Deal(cCards, 1);
	std::cout << "The River comes ";
	cCards[4].Print();
	std::cout << "\nMaking the board,\n\t";
	cCards[0].Print();
	std::cout << ",\n\t";
	cCards[1].Print();
	std::cout << ",\n\t";
	cCards[2].Print();
	std::cout << ",\n\t";
	cCards[3].Print();
	std::cout << " and\n\t";
	cCards[4].Print();
	std::cout << ".\n";
	
	PrintPot();
	std::cout << "\n";

}

void Game::Reveal()
{
	std::cout << "\n";
	Player* lastPlayer = nullptr;
	rules::Result r;
	int highestKicker = 0;
	int highestRank = 0;
	bool thereIsATie = false;
	bool thereWasAFold = false;
	
	HandType winningHand;
	for (int i = 0;i < players.size();i++) {
		if (players[i].GetInTheHand()) {
			if (lastPlayer == nullptr) {
				lastPlayer = &players[i];
				if (numberOfPlayersInTheHand > 1) {
					players[i].RevealHand();
				}
				else {
					std::cout << players[i].name << " won.\n";
				}
				winners.push_back(&players[i]);
			}
			else {
				r = rules::CompareHands(players[i], *lastPlayer, cCards);
				if (r.leftWon > 0) {
					if (thereWasAFold) {
						thereWasAFold = false;
						std::cout << "\n";
					}
					players[i].RevealHand();
					lastPlayer = &players[i];
					int rank = r.winningHand.rank;
					winningHand = r.winningHand;
					
					thereIsATie = false;
					while (winners.size() > 0) {
						winners.pop_back();
					}
					winners.push_back(&players[i]);
					if (rank > highestRank) {
						highestRank = rank;
					}
					else if (rank = highestRank) {
						if (r.numberOfKickersUsed > highestKicker) {
							highestKicker = r.numberOfKickersUsed;
						}
					}
				}
				else if (r.leftWon == 0) {
					
					thereIsATie = true;
					winners.push_back(&players[i]);
					if (thereWasAFold) {
						thereWasAFold = false;
						std::cout << "\n";
					}
					players[i].RevealHand();
				}
				else if (r.leftWon == -1) {
					thereWasAFold = true;
					std::cout << players[i].name<<" folds\n";
				}
			}
		}
	}
	if (thereWasAFold) {
		thereWasAFold = false;
		std::cout << "\n";
	}
	r.numberOfKickersUsed = highestKicker;
	if (!thereIsATie) {
		if (numberOfPlayersInTheHand > 1 && lastPlayer != nullptr) {
			r.Print(*lastPlayer);
			std::cout << "\n";
		}
	}
	else {
		if (winners.size() >= 3) {
			for (int i = 0;i < winners.size() - 2;i++) {
				std::cout << winners[i]->name << ", ";
			}
		}
		int index = int (winners.size()) - 2;
		int index2 = int (winners.size()) - 1;
		(winners.size() < 2);
		
		std::cout << winners[index]->name << " and " << winners[index2]->name << " won with a " << r.winningHand.name;
		std::cout << "\n";


	}
	
}

void Game::Reset()
{
	for (int i = 0;i < players.size();i++) {
		players[i].ClearHand();
		players[i].SetInTheHand(players[i].GetBankroll() > 0);
		players[i].SetTotalWager(0);
		players[i].maximumReturnPerPlayer = players[i].GetBankroll();

	}
	SplitPotWinners.clear();
	cCards.clear();
	int total = 0;
	for (int i = 0;i < numberOfPlayers;i++) {
		if (players[i].GetInTheHand())
			total++;
		if (total == 1)
			SmallBlindPlayer = &players[i];
		else if (total == 2)
			BigBlindPlayer = &players[i];
	}
	//dealer assign
	int temp = 0;
	for (int i = 0;i < numberOfPlayers;i++) {
		if (players[i].GetInTheHand())
			temp++;
		if (temp == total)
			Button = &players[i];
		
	}
	numberOfPlayersInTheHand = total;
	
	currentBet = 0;
	totalPot = 0;
	personToBet = 2;
	thereWasAnAllIn = false;
	
	
}

bool Game::ValidOption(Inputs::Option input, const Player& pla)
{

	switch (input.d) {
		bool rightTime;
		bool enoughMoney;
		bool raiseEnough;
	case Inputs::Decisions::All_In:
		return true;
		break;

	case Inputs::Decisions::Fold:
		return true;
		break;
	case Inputs::Decisions::Check:
		return (currentBet==pla.GetCurrentWager());
		break;
	case Inputs::Decisions::Bet:
		rightTime = (currentBet == 0);
		enoughMoney =  pla.GetBankroll()>= input.value;
		return rightTime && enoughMoney;
		break;
	case Inputs::Decisions::Call:
		rightTime = pla.GetCurrentWager()<currentBet;
		enoughMoney = (pla.GetBankroll() > currentBet - pla.GetCurrentWager());
		return rightTime && enoughMoney;
		break;
	
	case Inputs::Decisions::Raise_To:
		rightTime = currentBet != 0;
		enoughMoney = pla.GetBankroll() > input.value-pla.GetCurrentWager();
		raiseEnough = input.value - currentBet >= lastRaise;
		return rightTime && enoughMoney && raiseEnough;
		break;
	default:
		return false;
		break;
	}
}

void Game::SmallBlind(Player& pla)
{
	std::cout <<"The blinds are "<<smallBlind<<", "<<bigBlind<<".\n" <<pla.name << " is the small blind,";
	pla.RemoveFromBankroll(smallBlind);
	totalPot += smallBlind;
	pla.SetCurrentWager(smallBlind);
	
}

void Game::BigBlind(Player& pla)
{
	std::cout << " and " << pla.name << " is the big blind.\n";
	pla.RemoveFromBankroll(bigBlind);
	totalPot += bigBlind;
	pla.SetCurrentWager(bigBlind);
}

bool Game::ExecuteOption(Inputs::Option& option, Player& pla)
{
	
	bool returnBool;
	auto copy = option;
	switch (option.d) {
	case Inputs::Decisions::All_In:
		All_In(pla);
		returnBool = true;
		break;
	case Inputs::Decisions::Bet:
		Bet(option.value, pla);
		returnBool = true;
		break;
	case Inputs::Decisions::Call:
		Call(pla);
		returnBool = false;
		break;
	case Inputs::Decisions::Check:
		Check(pla);
		returnBool = false;
		break;
	case Inputs::Decisions::Fold:
		Fold(pla);
		numberOfPlayersInTheHand--;
		returnBool = false;
		break;
	
	case Inputs::Decisions::Raise_To:
		RaiseTo(option.value, pla);
		returnBool = true;
		break;
	default:
		returnBool = false;
		break;
	}
	IncreasePlayerToBet();
	
	return returnBool;
}

int Game::GetPot()
{
	return totalPot;
}

void Game::IncreasePlayerToBet()
{
	personToBet++;
	if (personToBet == numberOfPlayers)
		personToBet = 0;
}



void Game::ServerReveal()
{
	for (int i = 0;i < numberOfPlayers;i++) {
		
		std::cout << "\t";
		players[i].RevealHand();
		
	}
}



void Game::RaiseTo(int amount, Player& pla)
{
	int initialWager = pla.GetCurrentWager();
	int amountAdded = amount - initialWager;
	pla.SetCurrentWager(amount);
	pla.RemoveFromBankroll(amountAdded);
	int deltaRaised = amount - currentBet;
	lastRaise = deltaRaised;
	currentBet = amount;
	potThisBettingRound += amountAdded;
	totalPot += amountAdded;

}

void Game::Bet(int amount, Player& pla)
{
	currentBet = amount;
	potThisBettingRound += amount;
	totalPot += amount;
	lastRaise = amount;
	pla.RemoveFromBankroll(amount);
	pla.SetCurrentWager(amount);
}

void Game::Call(Player& pla)
{
	int delta = currentBet - pla.GetCurrentWager();
	pla.RemoveFromBankroll(delta);
	potThisBettingRound += delta;
	totalPot += delta;
	pla.SetCurrentWager(currentBet);
}

void Game::All_In(Player& pla)
{
	int total = pla.GetCurrentWager()+pla.GetBankroll();
	int delta = pla.GetBankroll();
	pla.SetIsAllIn(true);
	pla.SetInTheHand(false);
	if (total > 2 * currentBet)
		lastPlayerToRaise = nullptr;
	
	if (currentBet < total) {
		currentBet = total;
	}
	pla.SetCurrentWager(total);
	pla.RemoveFromBankroll(delta);
	potThisBettingRound += delta;
	totalPot += delta;
	thereWasAnAllIn = true;
	
	
}

void Game::Fold(Player& pla)
{
	pla.SetInTheHand(false);
	if (lastPlayerToRaise == &pla)
		lastPlayerToRaise = nullptr;
}

void Game::Check(Player& pla)
{
}

void Game::ResetBettingRound()
{
	for (int i = 0;i < numberOfPlayers;i++) {
		players[i].SetTotalWager(players[i].GetCurrentWager()+players[i].GetTotalWager());
		players[i].SetCurrentWager(0);
	}
	potThisBettingRound =0;
	lastRaise = bigBlind;
	personToBet = 0;
	currentBet = 0;
	
}

void Game::UpdatePot()
{
	
	
	lastRaise = bigBlind;
	
	personToBet = 0;
}

void Game::SetFirstBettingRound()
{
	
	lastPlayerToRaise = BigBlindPlayer;
	personToBet = 2;
	currentBet = bigBlind;
	potThisBettingRound = totalPot;
	
}

void Game::RemoveFromPossibleWinOfAllPlayers(int amount)
{
	for (int i = 0;i < numberOfPlayers;i++) {
		copy[i].maximumReturnPerPlayer -= amount;
		if (copy[i].maximumReturnPerPlayer < 0)
			copy[i].maximumReturnPerPlayer = 0;
		
		copy[i].SetCurrentWager(copy[i].GetCurrentWager()- amount);
		if (copy[i].GetCurrentWager() < 0)
			copy[i].SetCurrentWager(0);
	}
}

int Game::PayOutWinners()
{
	if (thereWasAnAllIn) {
		PayoutWinnersWithAllIn();
		return 0;
	}
	else if (winners.size() > 0)
	{
		int numberOfWinners = int(winners.size());
		int amount = totalPot / numberOfWinners;

		for (int i = 0;i < numberOfWinners;i++) {
			winners[i]->AddToBankroll(amount);
			std::cout << winners[i]->name << " received " << amount << ".\n";
		}
		return amount;
	}
	
}

void Game::PrintBankrolls()
{
	for (int i = 0;i < numberOfPlayers;i++) {

		std::cout << "\t";
		std::cout << players[i].name << " has " << players[i].GetBankroll() << "\n" ;

	}
}

void Game::PrintPot()
{
	
	std::cout << "There is " << totalPot << " in the pot.\n";
}

Player& Game::GetPlayerToBet()
{
	return players[personToBet];
}
