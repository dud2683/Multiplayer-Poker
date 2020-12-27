#include "Deck.h"

Deck::Deck()
{
	int index = 0;
	for (int i = 0;i < numberOfDecks;i++) {
		for (int j = 1;j <= 13;j++) {
			for (int k = 0;k < 4;k++) {
				cards[index] = Card(j, Suit(k));
				index++;
			}
		}
	}
	for (int i = 0;i < 52;i++) {
		scratchPad[i] = i;
	}
	
}

void Deck::Deal(Player& receiver)
{
	receiver.ReceiveCard(cards[pointInDeck]);
	cards[pointInDeck] = Card();
	pointInDeck++;

}

void Deck::Deal(std::vector<Card>& cCards, int numberOfTimes)
{
	for (int i = 0;i < numberOfTimes;i++) {
		cCards.push_back(cards[pointInDeck]);
		cards[pointInDeck] = Card();
		pointInDeck++;
	}
}

void Deck::Shuffle(int numberOfTimes)
{
	std::cout << "Shuffling...\n";
	
	std::random_device rd;
	std::mt19937 rng(rd());
	for (int i = 0;i < numberOfTimes;i++) {

		
		for (int i = 51;i >= 0;i--) {
			std::uniform_int_distribution<int> cardDist(0, i);
			int a = cardDist(rng);
			Swap(a, i);
		}
		Card copie[52];
		for (int i = 0;i < 52;i++) {
			copie[i] = cards[i];
		}
		for (int i = 0;i < 52;i++) {
			cards[i] = copie[scratchPad[i]];
		}
	}
		pointInDeck = 0;
		//Sleep(1000);
		
		std::cout << "Shuffling done\n\n";
}

void Deck::Swap(int a, int b)
{
	int temp = scratchPad[a];
	scratchPad[a] = scratchPad[b];
	scratchPad[b] = temp;
}
