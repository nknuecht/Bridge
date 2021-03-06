#ifndef DECK_H
#define DECK_H
/* Deck.h
 *
 * Represents a deck of playing cards
 *
 * by Nicholas Nuechterlein
 * nknuecht@umich.edu
 * 2015-03-02
 */

#include <vector>
#include <string>
#include <cassert>
#include <map>
#include <unordered_map>
#include <utility>
#include <queue>
#include <iostream>


struct Deck {
	std::vector<Card> card_deck;
	//should i keep track of a pointer to the "next" card to be dealt?  Card* next;
};


//MODIFIES deck, stdout
//EFFECTS  Initializes Deck
void Deck_init(Deck &deck);

//MODIFIES deck
//EFFECTS  Shuffles the Deck.
void Deck_shuffle(Deck &deck);

//MODIFIES stdout
//EFFECTS  Prints each Card in the Deck to stdout followed by a newline, using
//  the same format as Card_print().
void Print_deck(const Deck &deck);

#endif
