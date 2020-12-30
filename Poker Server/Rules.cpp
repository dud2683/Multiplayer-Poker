#include "Rules.h"

using namespace rules;




Result rules::CompareHands(const Player& p1, const Player& p2, const std::vector<Card>& cCards)
{
	HandType p1Hand = Merge(p1, cCards).FindBestHand();
	HandType p2Hand = Merge(p2, cCards).FindBestHand();
	if (p1Hand.rank > p2Hand.rank) {
		return Result(0, 1, p1Hand);
	}
	else if (p1Hand.rank < p2Hand.rank) {
		return Result(0, -1, p2Hand);
	}

	else {
		int results[5];
		for (int i = 0;i < 5; i++) {
			results[i] = CompareKicker(i, p1Hand, p2Hand);
		}
		switch (p1Hand.rank) {
		
		case 0:
			for (int i = 0; i < 5;i++) {
				if (results[i] == 1)
					return Result(i, 1, p1Hand);
				else if (results [i]==-1)
					return Result(i, -1, p2Hand);
			}
			break;
		case 1:
			for (int i = 1;i < 5;i++) {
				if (results[i] == 1)
					return Result(i-1, 1, p1Hand);
				else if (results[i] == -1)
					return Result(i-1, -1, p2Hand);
			}
			break;
		case 2:
			for (int i = 0;i < 5; i+=2) {
				if (results[i] == 1) {
					if (i == 0 || i == 2)
						return Result(0, 1, p1Hand);
					else
						return Result(1, 1, p1Hand);
				}
				else if (results[i] == -1) {
					if (i == 0 || i == 2)
						return Result(0, -1, p2Hand);
					else
						return Result(1, -1, p2Hand);
				}
			}
			break;
		case 3:
			for (int i = 2;i < 5;i++) {
				if (results[i] == 1)
					return Result(i - 2, 1, p1Hand);
				else if (results[i] == -1)
					return Result(i - 2, -1, p2Hand);
			}
			break;
		case 4:
			if(results[1]==1)
				return Result(0, 1, p1Hand);
			else if (results[1] == -1)
				return Result(0, -1, p2Hand);
			break;
		case 5:
			for (int i = 0; i < 5;i++) {
				if (results[i] == 1)
					return Result(i, 1, p1Hand);
				else if (results[i] == -1)
					return Result(i, -1, p2Hand);
			}
			break;
		case 6:
			for (int i = 2;i < 5;i++) {
				if (results[i] == 1)
					return Result(i-2, 1, p1Hand);
				else if (results[i] == -1)
					return Result(i-2, -1, p2Hand);
			}
			break;
		case 7:
			for (int i = 3;i < 5;i++) {
				if (results[i] == 1)
					return Result(i - 3, 1, p1Hand);
				else if (results[i] == -1)
					return Result(i - 3, -1, p2Hand);
			}
			break;
		case 8:
			if (results[1] == 1)
				return Result(0, 1, p1Hand);
			else if (results[1] == -1)
				return Result(0, -1, p2Hand);
			break;
		default:
			break;

			

		}
		return Result(0, 0, p1Hand);
	}
}

Hand rules::Merge(const Player& p, const std::vector<Card>& cCards)
{
	Hand returnHand;
	std::vector<Card> usableCards;
	for (int i = 0;i < 2;i++) {
		usableCards.push_back(p.GetHand(i));
	}
	for (int i = 0;i < cCards.size();i++) {
		usableCards.push_back(cCards[i]);
	}
	returnHand.cards = usableCards;
	return returnHand;
}

int rules::CompareKicker(int kickerToCompare, HandType& left, HandType& right)
{
	switch (kickerToCompare) {
	case 0:
		if (left.highestCard > right.highestCard) {
			return 1;
		}
		else if (right.highestCard > left.highestCard) {
			return -1;
		}
		break;
	case 1:
		if (left.kicker1 > right.kicker1) {
			return 1;
		}
		else if (right.kicker1 > left.kicker1) {
			return -1;
		}
		break;
	case 2:
		if (left.kicker2 > right.kicker2) {
			return 1;
		}
		else if (right.kicker2 > left.kicker2) {
			return -1;
		}
		break;
	case 3:
		if (left.kicker3 > right.kicker3) {
			return 1;
		}
		else if (right.kicker3 > left.kicker3) {
			return -1;
		}
		break;
	case 4:
		if (left.kicker4 > right.kicker4) {
			return 1;
		}
		else if (right.kicker4 > left.kicker4) {
			return -1;
		}
		break;
	default:
		break;

	}
	return 0;
}

void rules::Result::Print(const Player& pla) 
{
	std::cout << pla.name << " wins with a "<<winningHand.name;
	winningHand.Print(numberOfKickersUsed);
	
}
