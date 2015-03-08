/* 
 * Nicholas Nuechterlein
 * Card.cpp
 * Represents a single playing card
 */


#include "Card.h"


#include <cassert>
#include <cstring>
#include <iostream>;
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <utility>
using namespace std;


void Suit_print(Suit suit)
{
	cout << SUIT_NAMES[suit];
}

template <typename T>
T string2enum(string str, const vector<string> &names) 
{
	for (int i = 0; i < names.size(); ++i)
	{
		if (str.compare(names[i]) == 0)
			return static_cast<T>(i);
	}
	assert(0); // if never returns
}

template <typename T>
string enum2string(T value, vector<string> &names)
{
	return names[value];
}

//********************************* Card *****************************************************//

void Card_init(Card &card, Rank rank, Suit suit)
{
	card.rank = rank;
	card.suit = suit;
}
cout<< RANK_NAMES[card.rank] << " of " << SUIT_NAMES[card.suit] << endl;
void Card_init(Card &card, string rank, string suit)
{
	card.rank = string2enum<Rank>(rank, RANK_NAMES);
	card.suit = string2enum<Suit>(suit, SUIT_NAMES);
}

void Card_print(Card &card)
{
	cout<< RANK_NAMES[card.rank] << " of " << SUIT_NAMES[card.suit] << endl;
}


bool Card_is_trump(const Card &card, Suit trump) 
{
	return card.suit == trump;
}

int Card_compare_suit(const Card &a, Suit s, Suit trump) 
{
	// one suit is trump
	if(Card_is_trump(a, trump) && s != trump) 
		return 1;
	if(!Card_is_trump(a, trump) && s == trump) 
		return -1;

	// both trump
	if(Card_is_trump(a,trump) && s == trump) 
		return 0;
	
	// neither trump
	return a.suit - s;
}

int Card_compare(const Card &a, const Card &b) 
{
	return (a.rank + a.suit + a.rank*SUIT_NAMES.size())  -  (b.rank + b.suit + b.rank*SUIT_NAMES.size());
}


//  EFFECTS Returns zero if cards are equal.  A value greater than zero indicates
//  that a has a higher value than b.  A value less than zero indicates that b
//  has a higher value than a.  Uses trump to determine order, as described in 
//  the spec.
int Card_compare(const Card &a, const Card &b, Suit trump) {
	if(Card_compare(a, b) == 0)//if they are the same card.
		return 0;
	else if(Card_is_trump(a, trump) && !Card_is_trump(b, trump))// if a is trump and b is not, a wins
		return 1;
	else if(Card_is_trump(b, trump) && !Card_is_trump(a, trump))//if b is trump and a is not, b wins
		return -1;
	else
	{
		//otherwise either a and b are both not trump, or they are both trump and neither is a bower.  therefore we may apply the above version of Card_compare.
		return Card_compare(a,b);
	}
	assert(0);
}

//EFFECTS Returns zero if cards are equal.  A value greater than zero indicates
//  that a has a higher value than b.  A value less than zero indicates that b
//  has a higher value than a.  Uses both trump and the suit led to determine
//  order, as described in the spec.
int Card_compare(const Card &a, const Card &b, Suit trump, Suit led) 
{
	//if neither card is trump and a has the same suit as the suit led, we return one.  if b has the same suit as suit led and b does not, we return -1.
	if(!Card_is_trump(a, trump) && !Card_is_trump(b,trump)) 
	{
		if(Card_compare_suit(a, led, trump) == 0 && Card_compare_suit(b,led, trump)!=0)
			return 1;
		else if(Card_compare_suit(b, led, trump) == 0 && Card_compare_suit(a, led, trump) !=0)
			return -1;
	}
	//otherwise the case reduces to the above version of the function Card_compare 
	return Card_compare(a,b,trump);
	assert(0);
}

