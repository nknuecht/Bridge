/* Nicholas Nuechterlein
 * 
 * Deck.cpp
 * 
 */

#include "Card.h"
#include "Deck.h"


#include <cassert>
#include <cstring> //why?
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <utility>
using namespace std;



void Deck_init(Deck &deck)
{
	for(int i = 0; i <4; i++)
	{
		for(int j = 0; j < 13; j++)
		{
			Card temp_card;
			Card_init(temp_card, static_cast<Rank>(j), static_cast<Suit>(i));
			deck.card_deck.push_back(temp_card);
		}
	}
}

void Deck_shuffle(Deck &deck)
{
	std::srand(std::time(0));
	std::random_shuffle(deck.card_deck.begin(), deck.card_deck.end());
}

void Print_deck(Deck &deck)
{
	for(int i = 0; i < deck.card_deck.size(); i++)
	{
		Card_print(deck.card_deck[i]);
	}
}

