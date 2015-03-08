#ifndef HAND_H
#define HAND_H
/* Hand.h
 *
 * Representation of a Bridge player
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


struct Hand {
	std::string name;
	std::vector< std::vector<Card> > cards_held;
	int hc_points;
	int dis_points;
	
	//maybe the points should be initialized to negatives?
	Hand() : name(""), cards_held(4, std::vector<Card>()), hc_points(0), dis_points(0) {}
	Hand(std::string name_in) : name(name_in), cards_held(4, std::vector<Card>()), hc_points(0), dis_points(0) {}
};

struct Bid {
	//maybe should have a name / player attached to it.
	int suit; //maybe this should be a suit . . . 
	int level;
	Bid() : suit(-1), level(0) {}
	Bid(int suit_in, int level_in) : suit(suit_in), level(level_in) {}
};

//not sure what file to put this struct in . . . 
struct Hand_Public {
	std::pair <int, int> point_range;
	std::pair <int, int> hcp_range;
	bool balanced;
	std::vector< int > suit_length;
	bool forcing;
	bool game_forcing;
	bool passed;
	bool invitational;
	bool stoppers;
	Bid bid_name;

	Hand_Public() : point_range(-1,-1), hcp_range(-1, -1),  balanced(0), suit_length{0, 0, 0, 0}, forcing(0), game_forcing(0), passed(0), invitational(0), stoppers(0), bid_name() {}
};




void Add_card(Hand &hand, Card &card); //would be a helper function, but i call it from Util.cpp in ReadLine (maybe I should move it?)

//this kind of fills the hand_init kind of function, although it uses the deck, doesn't fill an invidiual hand, and doesn't allow an empty hand.
//void Hand_order(Card a, Card b); //helper function
void Sort_hand(Hand &hand); //helper function

void Sort_hands(Hand &hand);  //helper function
 
void Fill_hands(Deck &deck, std::vector<Hand> &hands, int dealer);

//difference between these print functions?
void Hand_print(Hand &hand); //function which just prints the cards, and not the points / suit length.
void Print_hand(Hand &hand);
void Print_hands(std::vector<Hand> &hands);

void Get_hand_long_points(Hand &hand);

void Get_hands_long_points(std::vector<Hand> &hands);

void Get_short_points(Hand &hand, Suit strain);

int Get_points(Hand &hand, std::vector<Hand_Public> &pub_vec, int partner_strain, int bidder);

bool Suit_stopped(Hand &hand, Suit suit);

bool Hand_stopped(Hand &hand);

bool Other_suits_stopped(Hand &hand, Suit suit);

bool Is_balanced(Hand &hand);




#endif
