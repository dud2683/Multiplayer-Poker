#include "Hand.h"

HandType Hand::FindBestHand()
{
	Sort();
	
	int returnVal;
	if (FindStraightFlush() > 0) {
		returnVal = 8;
	}
	else if (FindFourOfAKind() > 0) {
		returnVal = 7;
	}
	else if (FindFullHouse() > 0) {
		returnVal = 6;
	}
	else if (FindFlush() > 0) {
		returnVal = 5;
	}
	else if (FindStraight() > 0) {
		int v = FindStraight();
		StraightSort(v);
		returnVal = 4;
	}
	else if (FindThreeOfAKind() > 0) {
		returnVal = 3;
	}
	else if (FindTwoPair() > 0) {
		SortTwoPair();
		returnVal = 2;
	}
	else if (FindPair() > 0) {
		returnVal = 1;
	}
	else {
		returnVal = 0;	
	}
	cards.pop_back();
	cards.pop_back();
	
	return HandType(cards,returnVal);
}

void Hand::Print() const
{
	for (int i = 0;i < cards.size();i++) {
		if (cards.size() - 1 == i) {
			cards[i].Print();
			std::cout << "\n";
		}
		else {
			cards[i].Print();
			std::cout << " and ";
		}
	}
	std::cout << "\n";
}

void Hand::Clear()
{
	int size = int(cards.size());
	for (int i = 0;i < size;i++) {
		cards.pop_back();
	}
}

void Hand::Sort(int start)
{
	bool thereWasASwaps = true;
	while (thereWasASwaps) {
		thereWasASwaps = false;
		for (int i = start;i < cards.size() - 1;i++) {
			int next = i + 1;
			if (cards[i] < cards[next]) {
				Swap(cards[i], cards[next]);
				thereWasASwaps = true;
			}
		}
	}
}

void Hand::SortSuit(Suit s)
{
	
	for (int i = 0;i < cards.size();i++) {
		if (cards[i].GetSuit() != s) {
			cards[i] = Card();
		}
	}
	Sort();
}

int Hand::FindStraightFlush()
{
	std::vector<Card> temp = cards;
	if (FindFlush() > 0) {
		cards.pop_back();
		cards.pop_back();
		int v = FindStraight();
		if ( v > 0) {
			cards.push_back(Card());
			cards.push_back(Card());
			return v;
		}
		else {
			cards = temp;
			return 0;
		}
	}
	else {
		cards = temp;
		return 0;
	}
	
}

int Hand::FindFourOfAKind()
{
	int returnValue = 0;
	Card* CardPointer1 = nullptr;
	Card* CardPointer2 = nullptr;
	Card* CardPointer3 = nullptr;
	Card* CardPointer4 = nullptr;
	for (int i = 0;i < cards.size() - 3; i++) {
		int next = i + 1;
		int nextNext = next + 1;
		int nextNextNext = nextNext + 1;
		int cardVal = cards[i].GetValue();
		if (cardVal == cards[nextNextNext].GetValue()) {
			CardPointer1 = &cards[i];
			CardPointer2 = &cards[next];
			CardPointer3 = &cards[nextNext];
			CardPointer4 = &cards[nextNextNext];
			if (cardVal == 1)
				returnValue = 14;

			else
				returnValue = cardVal;
		}
	}
	if (CardPointer1 != nullptr && CardPointer2 != nullptr && CardPointer3 != nullptr) {
		Swap(*CardPointer1, cards.front());
		Swap(*CardPointer2, *(&cards.front() + 1));
		Swap(*CardPointer3, *(&cards.front() + 2));
		Swap(*CardPointer4, *(&cards.front() + 3));
		Sort(4);
		
		
	}
	return returnValue;
}

int Hand::FindFullHouse()
{
	std::vector<Card> temp = cards;
	int returnVal = 0;
	int tempVal = 0;
	int v = FindThreeOfAKind();
	
	if (v > 0) {
		
		tempVal = v;
		int v = FindPair(3);
		
		if (v > 0) {
			returnVal = tempVal;
			return returnVal;
		}
	}

	cards = temp;
	return returnVal;
}

int Hand::FindFlush()
{
	int returnValue = 0;
	int temp = 0;
	Suit flush;
	for (int j = 0;j < 4;j++) {
		int numberOfThatSuit = 0;
		for (int i = 0;i < cards.size();i++) {
			if (cards[i].GetSuit() == Suit(j)) {
				if (numberOfThatSuit == 0) {
					temp = cards[i].GetValue();
				}
				numberOfThatSuit++;
				if (numberOfThatSuit == 5) {
					returnValue = temp;
					flush = Suit(j);
					SortSuit(flush);
				}
			}
		}
	}
	return returnValue;
}

int Hand::FindStraight()
{
	bool ace = false;
	int returnValue = 0;
	int numberOfConsecutives = 1;
	for (int i = 0;i < cards.size() - 1; i++) {
		int next = i + 1;
		int cardVal = cards[i].GetValue();
		if (cardVal == 1) {
			ace = true;
			cardVal = 14;
		}
		int cardValNext = cards[next].GetValue();
		int cardValConsecutive = cardVal - 1;
		if (cardValNext == cardVal) {}
		else if (cardValNext==cardValConsecutive) {
			numberOfConsecutives++;
			if (numberOfConsecutives == 5) {
				returnValue = cardVal+3;
			}
			else if (numberOfConsecutives == 4 && cardValConsecutive == 2 && ace) {
				returnValue = 5;
			}
		}
		else {
			numberOfConsecutives = 1;
		}

	}
	return returnValue;
}

int Hand::FindThreeOfAKind()
{
	int returnValue = 0;
	Card* CardPointer1 = nullptr;
	Card* CardPointer2 = nullptr;
	Card* CardPointer3 = nullptr;
	for (int i = 0;i < cards.size() - 2; i++) {
		int next = i + 1;
		int nextNext = next + 1;
		int cardVal = cards[i].GetValue();
		if (cardVal == cards[nextNext].GetValue()) {
			CardPointer1 = &cards[i];
			CardPointer2 = &cards[next];
			CardPointer3 = &cards[nextNext];
			if (cardVal == 1)
				returnValue = 14;

			else
				returnValue = cardVal;
		}
	}
	if (CardPointer1 != nullptr && CardPointer2 != nullptr && CardPointer3 != nullptr) {
		Swap(*CardPointer1, cards.front());
		Swap(*CardPointer2, *(&cards.front() + 1));
		Swap(*CardPointer3, *(&cards.front() + 2));
		Sort(3);
		
	}
	return returnValue;
}

int Hand::FindTwoPair()
{
	int temp = 0;
	bool foundAPair = false;
	int returnValue = 0;
	for (int i = 0;i < cards.size() - 1; i++) {
		int next = i + 1;
		int cardVal = cards[i].GetValue();
		if (cardVal == cards[next].GetValue()) {

			if (!foundAPair) {
				foundAPair = true;
				temp = cardVal;
			}
			else if (cardVal!=temp){
				returnValue = temp;

			}

		}
	}
	return returnValue;
		
}

int Hand::FindPair(int start)
{
	int returnValue = 0;
	Card* CardPointer1 = nullptr;
	Card* CardPointer2 = nullptr;
	for (int i = start;i < cards.size() - 1; i++) {
		int next = i + 1;
		int cardVal = cards[i].GetValue();
		if (cardVal == cards[next].GetValue()) {
			CardPointer1 = &cards[i];
			CardPointer2 = &cards[next];
			if (cardVal == 1)
				returnValue = 14;
				
			else
				returnValue = cardVal;
		}
	}
	if (CardPointer1 != nullptr&&CardPointer2!= nullptr) {
		Swap(*CardPointer1, *(&cards.front() + start));
		Swap(*CardPointer2, *(&cards.front()+1 + start));
		Sort(2+start);
		

	}
	return returnValue;
}

void Hand::StraightSort(int x)
{
	int temp=0;
	for (int i = 0;i < 5;i++) {
		int v = cards[i].GetValue();
		
		if (x != 14 && i!=4) {
			if (v == 1) {
				cards.push_back(cards[i]);
				cards.erase(cards.begin() + i);
				i--;
				v = 0;
			}
		}

		if(v>x){
			cards.push_back(cards[i]);
			cards.erase(cards.begin() + i);
			i--;
		}
		else if (temp == v&&v!=0){
			cards.push_back(cards[i]);
			cards.erase(cards.begin() + i);
			i--;
		}
		temp = v;
	}
}

void Hand::SortTwoPair()
{
	
	for (int i = 0;i < 4;i++) {
		bool notIt = false;
		int v = cards[i].GetValue();
		for (int j = 0;j < 5;j++) {
			if (i == j) {}
			else if (cards[j].GetValue() == v) {
				notIt = true;
				j = 5;
			}
		}
		if (!notIt) {
			cards.push_back(cards[i]);
			cards.erase(cards.begin() + i);
			i--;
			
		}
	}
	Sort(4);
}

void Hand::Swap(Card& a, Card& b)
{
	Card temp = a;
	a = b;
	b = temp;
}



HandType::HandType(std::vector<Card> crds, int r)
{
	rank = r;
	highestCard = crds[0];
	kicker1 = crds[1];
	kicker2 = crds[2];
	kicker3 = crds[3];
	kicker4 = crds[4];
	Name();


}

void HandType::Name()
{
	switch (rank) {
	case 0:
		name = "High card " + highestCard.GetValName();
		k1 = kicker1.GetValName();
		k2 = kicker2.GetValName();
		k3 = kicker3.GetValName();
		k4 = kicker4.GetValName();
		break;
	case 1:
		name = "Pair of " + highestCard.GetValName() + "s";
		k1 = kicker2.GetValName();
		k2 = kicker3.GetValName();
		k3 = kicker4.GetValName();
		break;
	case 2:
		name = "Two pair, " + highestCard.GetValName() + "s and " + kicker2.GetValName()+"s";
		k1 = kicker4.GetValName();
		break;
	case 3:
		name = "Three of a kind, " + highestCard.GetValName() + "s";
		k1 = kicker3.GetValName();
		k2 = kicker4.GetValName();
		break;
	case 4:
		name = "Straight, " + highestCard.GetValName() + " high";
		break;
	case 5:
		name = "Flush, " + highestCard.GetValName() + " high";
		k1 = kicker1.GetValName();
		k2 = kicker2.GetValName();
		k3 = kicker3.GetValName();
		k4 = kicker4.GetValName();
		break;
	case 6:
		name = "Full house, "+ highestCard.GetValName() + "s over " + kicker3.GetValName() + "s";;
		break;
	case 7:
		name = "Four of a kind, " + highestCard.GetValName() + "s";
		k1 = kicker4.GetValName();
		break;
	case 8:
		name = "Straight flush, " + highestCard.GetValName() + " high";
		break;
	default:
		name = "undefined";
		break;
	}
}

void HandType::Print(int numberOfKickers)
{
	
	std::string optional = " ";
	switch(numberOfKickers) {
		
	case 0:
		break;
	
	case 1:
		if (k1 == "Ace" || k1 == "8") {
			optional = "n ";
		}
		std::cout << " and a" << optional << k1 << " kicker";
		break;
	
	case 2:
		if (k2 == "Ace" || k2 == "8") {
			optional = "n ";
		}
		std::cout << " and a" << optional << k2 << " second kicker";
		break;
	
	case 3:
		if (k3 == "Ace" || k3 == "8") {
			optional = "n ";
		}
		std::cout << " and a" << optional << k3 << " third kicker";
		break;
	
	case 4:
		if (k4 == "Ace" || k4 == "8") {
			optional = "n ";
		}
		std::cout << " and a" << optional << k4 << " fourth kicker";
		break;

	
	}
}
