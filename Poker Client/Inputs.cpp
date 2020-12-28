#include "Inputs.h"





Inputs::Option Inputs::GetUserInput()
{
	Inputs::Decisions command;
	int value = 0;
	std::getline(std::cin, rawInput);
	splitInput.push_back("");
	splitInput.push_back("");


	for (int i = 0;i < rawInput.size();i++) {
		rawInput[i] = std::tolower(rawInput[i]);
	}
	int space = CheckForSpace(rawInput);
	if (space > 0) {
		Split(rawInput);
		command = ParseForDecision(splitInput[0]);
		value = ParseForValue(splitInput[1]);
		splitInput.clear();
	}
	else {
		command = ParseForDecision(rawInput);
	}
	Option option = Option(command, value);

	if (option.d == Decisions::Invalid) {
		return GetUserInput();
	}
	else
		return option;
}

int Inputs::CheckForSpace(std::string test)
{
	int size = int(test.size());
	for (int i = 0;i < size;i++) {
		if (test[i] == ' ')
			return i;
	}
	return -1;
}

void Inputs::Split(std::string raw)
{
	
	std::string word = "";
	
	int index = 0;
	for (auto x : raw)
	{
		if (x == ' ')
		{
			splitInput[index] = word;
			word = "";
			index++;
			
		}
		else {
			word = word + x;
		}
	}
	splitInput[index] = word;
}

Inputs::Decisions Inputs::ParseForDecision(std::string input)
{
	if (input == "check"||input=="") {
		return Decisions::Check;
	}
	else if (input == "fold") {
		return Decisions::Fold;
	}
	else if (input == "bet") {
		return Decisions::Bet;
	}
	else if (input == "raiseto") {
		return Decisions::Raise_To;
	}
	else if (input == "raiseby") {
		return Decisions::Raise_By;
	}
	else if (input == "allin") {
		return Decisions::All_In;
	}
	else if (input == "call") {
		return Decisions::Call;
	}
	else {
		std::cout << "That was not a valid input. Make sure to specifie RaiseTo or RaiseBy, no spaces.\n";
		return Decisions::Invalid;
	}
}

int Inputs::ParseForValue(std::string input)
{
	int x=std::stoi(input);
	return x;
}

void Inputs::Option::Print(std::string plaName)
{
	std::string out = plaName;
	switch (this->d)
	{
	case Decisions::Check:
		out += " checks.\n";
		break;
	case Decisions::Call:
		out += " calls.\n";
		break;
	case Decisions::Bet:
		out += " bets ";
		out += std::to_string(this->value);
		out += ".\n";
		break;
	case Decisions::Raise_To:
		out += " raises to ";
		out += std::to_string(this->value);
		out += ".\n";
		break;
	
	case Decisions::Fold:
		out += " folds.\n";
		break;
	case Decisions::All_In:
		out += " goes all in for ";
		out += std::to_string(this->value);
		out += ".\n";
		break;
	
	
	default:
		break;
		
	}
	std::cout << out;

}
