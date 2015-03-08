#ifndef CARD_H
#define CARD_H
/* Card.h
 *
 * Represents a single playing card
 *
 * 
 */
 
#include <vector>
#include <string>
#include <cassert>
#include <map>
#include <unordered_map>
#include <utility>
#include <queue>
#include <iostream>

enum Suit {CLUBS, DIAMONDS, HEARTS, SPADES, NOTRUMPS};
enum Rank {TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE};

const std::vector<std::string> SUIT_NAMES = {"Clubs", "Diamonds", "Hearts", "Spades", "Notrumps"};
const std::vector<std::string> SHORT_SUIT_NAMES = {"C", "D", "H", "S", "NT"};

const std::vector<std::string> RANK_NAMES = {"Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"}; 
const std::vector<std::string> SHORT_RANK_NAMES = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

//MODIFIES stdout
//EFFECTS  prints Suit to standard output, for example "Spades" or "Hearts"
void Suit_print(Suit suit);


// A card has a rank and a suit, for example "Two of Spades"
struct Card{
	Rank rank;
	Suit suit;
	Card() : rank(ACE), suit(NOTRUMPS) {}
};

//MODIFIES card
//EFFECTS Initializes Card to specified card
void Card_init(Card &card, Rank rank, Suit suit);

//REQUIRES rank is one of "Two", "Three", "Four", "Five", "Six", "Seven",
//  "Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"
//  suit is one of "Spades", "Hearts", "Clubs", "Diamonds"  
//MODIFIES card
//EFFECTS Initializes Card  to specified card
void Card_init(Card &card, std::string rank, std::string suit);

//MODIFIES stdout
//EFFECTS  Prints Card to standard output, for example "Two of Spades"
void Card_print(Card &card);


//EFFECTS Returns true if the card_ptr is a trump card.  All cards of the trump
// suit are trump cards.
bool Card_is_trump(const Card &card, Suit trump);

//EFFECTS Returns zero if suits are equal.  A value greater than zero indicates
//  that the Suit of a has a higher value than s.  A value less than zero 
//  indicates that the Suit s has a higher value than a.
int card_compare_suit(const Card &a, Suit s, Suit trump);

//EFFECTS Returns zero if cards are equal.  A value greater than zero indicates
//  that a has a higher value than b.  A value less than zero indicates that b
//  has a higher value than a.  Does not consider trump.
int Card_compare(const Card &a, const Card &b);

//EFFECTS Returns zero if cards are equal.  A value greater than zero indicates
//  that a has a higher value than b.  A value less than zero indicates that b
//  has a higher value than a.  Uses trump to determine order.
int Card_compare (const Card &a, const Card &b, Suit trump);

//EFFECTS Returns zero if cards are equal.  A value greater than zero indicates
//  that a has a higher value than b.  A value less than zero indicates that b
//  has a higher value than a.  Uses both trump and the suit led to determine
//  order.
int Card_compare(const Card &a, const Card &b, Suit trump, Suit led);



#endif
