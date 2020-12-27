#pragma once
#include<string>
#include<iostream>
#include <vector>
class Inputs
{
public:
	enum class Decisions{
		Check,
		Call,
		Bet,
		Raise_To,
		Raise_By,
		Fold,
		All_In,
		Invalid
	};
	struct Option {
		Option(Decisions d_, int v) {
			d = d_;
			value = v;
		};
		Decisions d=Decisions::Fold;
		int value=0;
	};
	 Option UserInput();
private:
	std::string rawInput;
	std::vector<std::string> splitInput;
	int CheckForSpace(std::string test);
	void Split(std::string raw);
	Decisions ParseForDecision(std::string input);
	int ParseForValue(std::string input);
};

