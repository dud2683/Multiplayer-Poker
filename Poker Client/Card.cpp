#include "Card.h"



Card::Card()
{
    value = 0;
    suit = Suit::Clubs;
}

Card::Card(int v, Suit s)
{
    value = v;
    suit = s;
}


int Card::GetValue()
{
    return value;
}

Suit Card::GetSuit()
{
    return suit;
}

bool Card::operator<(const Card& lhs) const
{
    if (this->value == 1 && lhs.value != 1) {
        return false;
    }
    else if (this->value != 1 && lhs.value == 1) {
        return true;
    }
    else {
        return this->value < lhs.value;
    }
}

bool Card::operator>(const Card& lhs) const
{
    return lhs < *this;
  
}

bool Card::operator<=(const Card& lhs) const
{
    return !(*this > lhs);
    
}

bool Card::operator>=(const Card& lhs) const
{
    return !(*this < lhs); 
}

bool Card::operator==(const Card& lhs) const
{
    return (this->value==lhs.value);
}

void Card::Print() const

{
#ifdef _WIN32
    int k;
    switch (suit) {
    case Suit::Clubs:
        k = 10;
            break;
    case Suit::Diamonds:
        k = 1;
        break;
    case Suit::Hearts:
        k = 4;
        break;
    case Suit::Spades:
        k = 8;
        break;
    default:
        k = 7;
        break;
    }


    HANDLE  hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, k);
#endif 



    std::string optional = "";
    if (GetValName() == "Ace" || GetValName() == "8") {
        optional = "n";
    }


    std::cout << "a" << optional << " " << GetValName() << " of " << GetSuitName();
#ifdef _WIN32
    k = 7;
    SetConsoleTextAttribute(hConsole, k);
#endif 
}
std::string Card::GetValName() const
{
    return GetValName(value);
}

std::string Card::GetValName(int val)
{
    std::string valueName;
    switch (val) {
    case 14:
    case 1:
        valueName = "Ace";
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        valueName = std::to_string(val);
        break;
    case 11:
        valueName = "Jack";
        break;
    case 12:
        valueName = "Queen";
        break;
    case 13:
        valueName = "King";
        break;
   
    default:
        valueName = "Invalid";
        break;
    }
    return valueName;
   
}

std::string Card::GetSuitName() const
{
    std::string suitName;
    switch (suit) {

    case Suit::Clubs:
        suitName = "Clubs";
        break;

    case Suit::Spades:
        suitName = "Spades";
        break;
    case Suit::Diamonds:
        suitName = "Diamonds";
        break;

    case Suit::Hearts:
        suitName = "Hearts";
        break;

    default:
        suitName = "Invalid";
        break;
    }
    return suitName;
}
