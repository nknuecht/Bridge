/* Nicholas Nuechterlein
 * Contract Bridge Project, 2015
 * Hand.cpp
 */


#include "Card.h"
#include "Deck.h"
#include "Hand.h"
#include "Bid.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
using namespace std;



void Add_card(Hand &hand, Card &card)
{
	hand.cards_held[card.suit].push_back(card);
}

static bool hand_order(Card a, Card b)
{
	if(a.suit == b.suit)
	{
		return a.rank > b.rank;
	}
	//these else if's can be optimized . . . 
	else if(a.suit == DIAMONDS || b.suit == SPADES)
		return false;
	else if(a.suit == SPADES || b.suit == DIAMONDS)
		return true;
	else if(a.suit == HEARTS && b.suit == CLUBS)
		return true;
	else if(a.suit == CLUBS && b.suit == HEARTS)
		return false;
	else
	{
		cerr<<"ERROR: invalid card comparison / we're missing suit combos" << endl;
		assert(0);
	}
}

void Sort_hand(Hand &hand)
{
	for(int s = 0; s < 4; s++)
	{
		sort(hand.cards_held[s].begin(), hand.cards_held[s].end(), hand_order);
	}
} 

void Sort_hands(vector<Hand> &hands)
{
	for(int hand = 0; hand < 4; hand++)
	{
		Sort_hand(hands[hand]);
	}
}

//REQUIRES Deck has 52 valid cards
//MODIFIES hands
//EFFECTS  Deals cards from deck into the four hands held in hands
void Fill_hands(Deck &deck, vector<Hand> &hands, int dealer)
{
	for(int i = 0; i < 52; i++)
	{
		for(int cur_hand = 0; cur_hand < 4; cur_hand++)
		{
			Add_card(hands[(cur_hand+dealer)%4], deck.card_deck[i]);
			if(cur_hand < 3)
				i++;
		}
	}
	Sort_hands(hands); 
}




//can clean this function up.
void Hand_print(Hand &hand)
{
	vector<Suit> suit = {SPADES, HEARTS, CLUBS, DIAMONDS};
	vector<string> suit_letter = {"S: ", "H: ", "C: ", "D: "};
	for(int s = 0; s < 4; ++s)
	{
		cout << suit_letter[s];
		if(hand.cards_held[suit[s]].size() > 0)
		{
			for(int i = 0; i < hand.cards_held[suit[s]].size() - 1; ++i)
			{
				cout << SHORT_RANK_NAMES[hand.cards_held[suit[s]][i].rank] << " ";
			}
			//re-write this line for readabilty & speed.
			cout << SHORT_RANK_NAMES[hand.cards_held[suit[s]][hand.cards_held[suit[s]].size()-1].rank] << endl;
		}
		else
			cout << endl;
	}
}


void Print_hand(Hand &hand)
{
	cout << endl; //this seems silly to have here
	cout << hand.name << "'s hand: " << endl;
	Hand_print(hand);
	cout<<endl;

	cout<< "This hand has " << hand.hc_points + hand.dis_points << " points." << endl;
	cout<< "It has " << hand.cards_held[SPADES].size() << " spades. " << endl;
	cout<< "It has " << hand.cards_held[HEARTS].size() << " hearts. " << endl;
	cout<< "It has " << hand.cards_held[CLUBS].size() << " clubs. " << endl;
	cout<< "It has " << hand.cards_held[DIAMONDS].size() << " diamonds. " << endl;
}

void Print_hands(vector<Hand> &hands)
{
	for(int j = 0; j < 4; j++)
	{
		Print_hand(hands[j]);
	}
}

static void Add_card_hcp_value(Hand &hand, int suit, int card_index) //helper function
{
	if(hand.cards_held[suit][card_index].rank == ACE)
	{
		hand.hc_points += 4;
	}	
	else if(hand.cards_held[suit][card_index].rank == KING)
	{
		hand.hc_points += 3;
	}
	else if(hand.cards_held[suit][card_index].rank == QUEEN)
	{
		hand.hc_points += 2;
	}
	else if(hand.cards_held[suit][card_index].rank == JACK)
	{
		hand.hc_points += 1;
	}
	else
	{
		assert(0 && "reached else in Add_Card_hcp_value");
	}	
}

void Get_hand_long_points(Hand &hand)
{
	hand.hc_points = 0;
	hand.dis_points = 0;		
	for(int s = 0; s < hand.cards_held.size(); s++) //loops through suit vectors
	{
		//high card points (can we just interate through the 2D vector rather than doing this nested loop)
		for(int j = 0; j < hand.cards_held[s].size(); ++j)
		{
			Add_card_hcp_value(hand, s, j);
		}
		//long suit points.
		if(hand.cards_held[s].size() > 4)
		{
			//adds 1 point for a 5-card suit, 2 points for a 6-card suit, 3 points for a 7-card suit . . .
			hand.dis_points += hand.cards_held[s].size() - 4;
		}
	}
}

void Get_hands_long_points(vector<Hand> &hands)
{
	for(int j = 0; j < 4; j++)
	{
		Get_hand_long_points(hands[j]);
	}
}



//short suit points after finding a fit.  (counting voids, singletons, and doubletons)
void Get_short_points(Hand &hand, Suit strain)
{
	hand.dis_points = 0;
	for(int s = 0; s < 4; s++)
	{
		if(s != strain) // can't add short points in partner's suit . . . (but what if partner has multiple suits?)
		{
			//with only three card trump support think about 3,2,1 instead of 5,3,1
			//think about not count say both the hcp and short points for a singleton Q
			if(hand.cards_held[s].size() == 0) //void
			{
				hand.dis_points += 5;
			}
			else if(hand.cards_held[s].size() == 1) //singleton
			{
				hand.dis_points += 3;
			}
			else if(hand.cards_held[s].size() == 2) //doubleton
			{
				hand.dis_points += 1;
			}
		}
	}
}
	
int Get_points(Hand &hand, vector<Hand_Public> &pub_vec, int partner_strain, int bidder)
{
	int points = hand.hc_points + hand.dis_points;					
	if(pub_vec[(bidder+PARTNER)%4].suit_length[partner_strain] + hand.cards_held[partner_strain].size() > 7)
	{
		Get_short_points(hand, static_cast<Suit>(partner_strain));
		if(hand.hc_points + hand.dis_points < points) // if the new short points are less than the origional points
		{
			cerr << "ERROR: when short points were reevaluated, the total points count went down" << endl;
			cerr << "       in this case, we may want to take whatever point count is bigger . . . " << endl;
			assert(0);
		}
		points = hand.hc_points + hand.dis_points;
	}
	return points;
}
	
	
bool Suit_stopped(Hand &hand, Suit suit)
{
	if(hand.cards_held[suit].size() == 0) // voids are not stopped
		return false;
	else if(hand.cards_held[suit].size() == 1) // singltons must be an ace to be stopped (4hcp)
	{
		if(hand.cards_held[suit][0].rank == ACE)
			return true;
		else
			return false;
	}
	else if(hand.cards_held[suit].size() == 2) //doubletons require an Ace or King (3hcp isn't enough: QJ is not a stopper)
	{
		if(hand.cards_held[suit][0].rank == ACE || hand.cards_held[suit][0].rank == KING)
			return true;
		else
			return false;
	}
	else if(hand.cards_held[suit].size() == 3) // trippletons require A, K, or Q (2hpc)
	{
		if(hand.cards_held[suit][0].rank == ACE || hand.cards_held[suit][0].rank == KING || hand.cards_held[suit][0].rank == QUEEN)
			return true;
		else
			return false;
	}
	else if(hand.cards_held[suit].size() == 4) // four card suits require a Jack (1hcp)
	{
		if(hand.cards_held[suit][0].rank == ACE 
			|| hand.cards_held[suit][0].rank == KING 
			|| hand.cards_held[suit][0].rank == QUEEN 
			|| hand.cards_held[suit][0].rank == JACK)
		{
			return true;
		}
		else
		{
			return false;
		}
			
	}
	else//consider a 5+ card suit an effective stopper . . . but mabye shouldn't
		return true;

}

bool Hand_stopped(Hand &hand)
{
	for(int i = 0; i < 4; i++)
	{
		if(!Suit_stopped(hand, static_cast<Suit>(i)))
			return false;
	}
	return true;
}

bool Other_suits_stopped(Hand &hand, Suit suit)
{
	for(int i = 0; i < 4; i++)
	{
		if(i != suit) //don't check for stoppers in specificed suit (probably because partner bid it and has it stopped)
		{
			if(!Suit_stopped(hand, static_cast<Suit>(i)))
				return false;
		}
	}
	return true;
}


bool Is_balanced(Hand &hand)
{
	int doubleton = 0;
	int five_card = 0;
	for(int s = 0; s < 4; ++s)
	{
		if(hand.cards_held[s].size() < 2 || hand.cards_held[s].size() > 5)
			return false;
		if(hand.cards_held[s].size() == 2)
			doubleton++;
		if(hand.cards_held[s].size() == 5)
			five_card++;
	}
	if(doubleton > 1)
		return false;
	if(five_card > 1)
		return false;
	return true;
}

