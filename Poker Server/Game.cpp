#include "Game.h"
#include <assert.h>

void Game::Go()
{
	
	
	InitalDeal();
	ServerReveal();
	PrintBankrolls();
	Betting(true);
	
	if (numberOfPlayersInTheHand > 1) {
		Flop();
		Betting();
		if (numberOfPlayersInTheHand > 1) {
			Turn();
			Betting();
			if (numberOfPlayersInTheHand > 1) {
				River();
				Betting();


			}
		
		}
	}
	Reveal();
	PayOutWinners();
	Reset();
	Rotate();
}

Game::Game()
{
	
	
	for (int i = 0;i < numberOfPlayers;i++) {
		players.push_back(Player());
		int j = i + 1;
		players[i].name = "Player ";
		players[i].name += std::to_string(j);
		players[i].SetInTheHand(true);
	}
	
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

void Game::Betting(bool isFirstBetting)
{
	int i=0;
	
	if (isFirstBetting) {
		i = 2;
		currentBet = bigBlind;
	}
	

	Player* lastPlayerToBet = nullptr;
	while (&players[i] != lastPlayerToBet&&numberOfPlayersInTheHand>1) {
		if (lastPlayerToBet == nullptr) {
			lastPlayerToBet = &players[i];
		}
		if (players[i].GetInTheHand() && !players[i].GetIsAllIn()) {
			Inputs::Option chosen = players[i].GetDecision();
			while (!ValidOption(chosen, players[i])) {
				std::cout << "You cannot do that at this time\n";
				chosen= players[i].GetDecision();
			}
			if (ExecuteOption(chosen, players[i])) {
				lastPlayerToBet = &players[i];
			}
			
		}
		if (i < numberOfPlayers-1) {
			i++;
		}
		else {
			i = 0;
		}
	}
	pots[0] += potThisRound;
	potThisRound = 0;
	currentBet = 0;
	for (int i = 0;i < numberOfPlayers;i++) {
		players[i].SetCurrentWager(0);
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
		players[i].SetInTheHand(true);
	}
	cCards.clear();
	numberOfPlayersInTheHand = numberOfPlayers;
	for (auto i = 0;i < 10;i++) {
		pots[i] = 0;
	}
	currentBet = 0;
	potThisRound = 0;
}

bool Game::ValidOption(Inputs::Option input, const Player& pla)
{

	switch (input.d) {
		bool rightTime;
		bool enoughMoney;
		bool raiseEnough;
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
		enoughMoney = (pla.GetBankroll() >= currentBet - pla.GetCurrentWager());
		return rightTime && enoughMoney;
		break;
	case Inputs::Decisions::Raise_By:
		 rightTime = currentBet != 0;
		 enoughMoney = (pla.GetBankroll() >= input.value);
		 raiseEnough = (input.value >= lastRaise);
		 return rightTime && enoughMoney && raiseEnough;
		 break;
	case Inputs::Decisions::Raise_To:
		return(ValidOption(Inputs::Option(Inputs::Decisions::Raise_By, input.value-currentBet), pla));
	default:
		return false;
		break;
	}
}

void Game::SmallBlind(Player& pla)
{
	std::cout <<"The blinds are "<<smallBlind<<", "<<bigBlind<<".\n" <<pla.name << " is the small blind,";
	pla.RemoveFromBankroll(smallBlind);
	pots[0] += smallBlind;
	pla.SetCurrentWager(smallBlind);
	
}

void Game::BigBlind(Player& pla)
{
	std::cout << " and " << pla.name << " is the big blind.\n";
	pla.RemoveFromBankroll(bigBlind);
	pots[0] += bigBlind;
	pla.SetCurrentWager(bigBlind);
}

bool Game::ExecuteOption(Inputs::Option option, Player& pla)
{
	switch (option.d) {
	case Inputs::Decisions::All_In:
		All_In(pla);
		return true;
		break;
	case Inputs::Decisions::Bet:
		Bet(option.value, pla);
		return true;
		break;
	case Inputs::Decisions::Call:
		Call(pla);
		return false;
		break;
	case Inputs::Decisions::Check:
		Check(pla);
		return false;
		break;
	case Inputs::Decisions::Fold:
		Fold(pla);
		numberOfPlayersInTheHand--;
		return false;
		break;
	case Inputs::Decisions::Raise_By:
		RaiseBy(option.value, pla);
		return true;
		break;
	case Inputs::Decisions::Raise_To:
		RaiseTo(option.value, pla);
		return true;
		break;
	default:
		return false;
		break;
	}
}

void Game::ServerReveal()
{
	for (int i = 0;i < numberOfPlayers;i++) {
		
		std::cout << "\t";
		players[i].RevealHand();
		
	}
}

void Game::RaiseBy(int amount, Player& pla)
{
	Call(pla);
	currentBet += amount;
	pla.RemoveFromBankroll(amount);
	pla.SetCurrentWager(currentBet);
}

void Game::RaiseTo(int amount, Player& pla)
{
	RaiseBy(amount - currentBet,pla);
}

void Game::Bet(int amount, Player& pla)
{
	currentBet = amount;
	potThisRound += amount;
	pla.RemoveFromBankroll(amount);
	pla.SetCurrentWager(amount);
}

void Game::Call(Player& pla)
{
	int delta = currentBet - pla.GetCurrentWager();
	pla.RemoveFromBankroll(delta);
	potThisRound += delta;
	pla.SetCurrentWager(currentBet);
}

void Game::All_In(Player& pla)
{
}

void Game::Fold(Player& pla)
{
	pla.SetInTheHand(false);
}

void Game::Check(Player& pla)
{
}

void Game::PayOutWinners()
{
	int numberOfWinners= int(winners.size());
	int amount = pots[0] / numberOfWinners;

	for (int i = 0;i < numberOfWinners;i++) {
		winners[i]->AddToBankroll(amount);
		std::cout << winners[i]->name << " received " << amount<<".\n";
	}
	pots[0] = 0;
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
	int totalPot = 0;
	for (int i = 0;i < 10;i++) {
		totalPot+=pots[i];
	}
	std::cout << "There is " << totalPot << " in the pot.\n";
}
