/* Nicholas Nuechterlein
 * Bid.cpp
 */
#include "Card.h"
#include "Deck.h"
#include "Hand.h"
#include "Bid.h"




#include <cassert>
#include <cstring>
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


//REQUIRES 
//MODIFIES none.
//EFFECTS  prints bid level and strain i.e. 1C
void Print_bid(Bid &bid)
{
	if(bid.suit == -1)
		cout << "PASS" << endl;
	else if(bid.suit < 0 || bid.suit > 4)
	{
		cerr << "ERROR: PASS, Double, or Rbl, or 'bad bid' entered . . . " << endl;
		assert(0);
	}
	else
		cout << bid.level << SHORT_SUIT_NAMES[bid.suit] << endl;
}


//REQUIRES Player has bid (or else we get what?)
//MODIFIES none.
//EFFECTS  Displays information promised by bidder's latest bid.
void Interpret_bid(vector<Hand_Public> &pub_vec, int bidder)
{
	//should have an input (either in pub_vec, or in hand or hands) that has the players's name . . . but i'll fix that later.
	if(bidder == 0) //really need to change these to names rather than numbers so I'm not just always hardcoding them . . . 
		cout << "North's ";
	else if(bidder == 1)
		cout << "East's ";
	else if(bidder == 2)
		cout << "South's ";
	else if(bidder == 3)
		cout << "West's ";
	else
		assert(0);
	
	cout << "bids promises: " << endl;
	if(pub_vec[bidder].point_range.first == -1 && pub_vec[bidder].point_range.second == -1 || pub_vec[bidder].balanced == true)
	{
		cout << " " << pub_vec[bidder].hcp_range.first << "-" << pub_vec[bidder].hcp_range.second << " high card points," << endl;
		if(pub_vec[bidder].balanced == true)
			cout << " a balanced hand," << endl;
	}
	else
	{
		if(pub_vec[bidder].point_range.second != -1)
		{
			cout << " " << pub_vec[bidder].point_range.first << "-" << pub_vec[bidder].point_range.second << " total points," << endl;
		}
		else
		{
			cout << " " << pub_vec[bidder].point_range.first << "+ total points," << endl;
		}
		
	}
		
	cout << " at least " << pub_vec[bidder].suit_length[0] << " clubs," << endl;
	cout << " at least " << pub_vec[bidder].suit_length[1] << " diamonds," << endl;
	cout << " at least " << pub_vec[bidder].suit_length[2] << " hearts," << endl;
	cout << " at least " << pub_vec[bidder].suit_length[3] << " spades," << endl;
	
	if(pub_vec[bidder].game_forcing == true)
		cout << " and the last bid made was game-forcing." << endl;
	else if(pub_vec[bidder].forcing == true)
		cout << " and the last bid made was forcing." << endl;
	else if(pub_vec[bidder].invitational == true)
		cout << " and the last bid made was invitational." << endl;
}

//REQUIRES
//MODIFIES none.
//EFFECTS  Given the last bid and a suit, returns the lowest possible next of this suit.
Bid Next_bid(int suit, Bid &last_bid)
{
	Bid next_bid;
	next_bid.suit = suit;
	if(suit <= last_bid.suit)
	{
		next_bid.level = last_bid.level + 1;
	}
	else
		next_bid.level = last_bid.level;
	
	return next_bid;
}


//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Sets response to PASS and encodes PASS promises into pub_vec
void Pass(Bid &response, Hand_Public &hand_data, int level, pair<int, int> points)
{
	response.suit = -1;
	response.level = level; // this might be useful
	hand_data.point_range = points;
	hand_data.hcp_range = points; //might not need this
	hand_data.passed = true;
}


//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Sets response to a specificed raise of partner's suit.  also promises a specified min length for support in partner's suit.
static void Raise_helper(Bid &response, Hand_Public &hand_data, Bid partner_bid, int raise, pair<int, int> points, int maj_sup) 
{
	response.suit = partner_bid.suit;
	response.level = partner_bid.level + raise;

	hand_data.point_range = points;
	if(partner_bid.suit == 2 || partner_bid.suit == 3)
		hand_data.suit_length[partner_bid.suit] = maj_sup; //promises maj_sup support responding to a major.
	else if(partner_bid.suit == 0 || partner_bid.suit == 1)
		hand_data.suit_length[partner_bid.suit] = maj_sup+1; //promises maj_sup+1 support responding to a minor.
	hand_data.passed = false;
}

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 1 level, promises 6-9 points and 3+ support responding to a major, 4+ support responding to a minor.
void Single_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid)
{
	Raise_helper(response, hand_data, partner_bid, 1, make_pair(6,9), 3);
}

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 3 levels, promises 6-9 points and 5+ support responding to a major, 6+ support responding to a minor.
void Double_jump_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid)
{
	Raise_helper(response, hand_data, partner_bid, 3, make_pair(6,9), 5);
}

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 2 levels, promises 10-11 points and 3+ support responding to a major, 4+ support responding to a minor.
void Limit_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid)
{
	Raise_helper(response, hand_data, partner_bid, 2, make_pair(10,11), 3);
	hand_data.invitational = true;
}


//REQUIRES
//MODIFIES response, hand_data, suit_pq
//EFFECTS  Introduces longest new suit, promises 12+ points and 4+ cards in new suit.  Game forcing bid opposite opener.
void New_suit_intro(Bid &response, Hand_Public &hand_data, Bid partner_bid, priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{							
	Suit_Length long_suit = suit_pq.top(); //this should be guartented to have 4+ card in it.
	//next if can be deleted
	if(long_suit.length < 4)
	{
		//New_suit
		cerr << "ERROR: Longest suit is less than 4 cards long!  this might be because we're coming from the minor_support function" << endl;
		assert(0);
	}	
	if(long_suit.suit != partner_bid.suit) //if this is a new suit
	{
			response = Next_bid(long_suit.suit, partner_bid);
	}
	else //if somehow this is still partner's suit
	{
			suit_pq.pop();
			long_suit = suit_pq.top(); //long_suit now is second longest (but longest which is not partner's suit)
			response = Next_bid(long_suit.suit, partner_bid);
			//this if can be deleted.
			if(long_suit.length < 4)
			{
				cerr<< "ERROR: we need to bring up a new suit, but we don't have one that is 4+ cards" << endl;
				cerr<< "       should we bid NT, bid a 3-card suit, or jump in the 4+ support of partner's suit?" << endl;
				cerr<< "       probably we just bid the 3-card suit and then later show support" << endl;
				assert(0);
			}
	}
	hand_data.point_range = make_pair(12, -1); //don't know what the top range would be . . .
	hand_data.suit_length[response.suit] = 4; // this could be three, but i think it promises 4.
	hand_data.passed = false;
	hand_data.forcing = true;
	hand_data.game_forcing = true; //maybe forcing should be ennumerated.  NO, YES, GAME, SLAM
}




//REQUIRES
//MODIFIES hand, response, hand_data, suit_pq
//EFFECTS  Reevaluates short points and, based on points, performs either a single raise (6-9), limit raise (10-11), double jump raise (6-9 and length), or 		  introduces a new suit (12+).
void Support_major(Hand &hand, Bid &response, Hand_Public &hand_data, Bid &partner_bid, priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{			   
	//revaluate to short points.
	Get_short_points(hand, static_cast<Suit>(partner_bid.suit));
	int short_points = hand.hc_points + hand.dis_points;
	cout << "this hand now has " << short_points << " short points after reevaluating short points!" << endl;
	if(short_points > 5 && short_points < 10) // between 6-9
	{
			if(hand.cards_held[partner_bid.suit].size() < 5) // if 3 or 4 card support do a single raise
			{
				Single_raise(response, hand_data, partner_bid);
			}
			else // if  5 or more card support do a double jump raise to game.
			{
				Double_jump_raise(response, hand_data, partner_bid);
			}
	}
	else if(short_points > 9 && short_points < 12) // between 10-11 do a limit raise (This is invitational)
	{
			Limit_raise(response, hand_data, partner_bid);
	}
	else // 12+ means we introduce a new suit, which is game forcing
	{
			New_suit_intro(response, hand_data, partner_bid, suit_pq);
	}
}


//REQUIRES
//MODIFIES response, pub_vec[bidder]
//EFFECTS  makes response with priority (1) new 4-card major or 5-card minor (2) 2NT or 3NT (3) a suitable 1NT, (4) 1C -> 1D, (5) support of minor, (6) 1NT
void Minor_suit_response(Hand &hand, 
						 vector<Hand_Public> &pub_vec, 
						 Bid &response, 
						 int bidder, 
						 Bid &partner_bid, 
						 priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{
		if(Check_4CM_5Cm(hand, partner_bid.suit)) // have a 4-card major or a 5-card minor which we will bid.
		{
			Bid_long_suit(hand, response, pub_vec[bidder], partner_bid, suit_pq);
		}
		else if(Check_2NT_3NT_raise(hand, pub_vec, bidder, partner_bid)) //bid 2NT or 3NT
		{
			NT_Jump_Res(hand, response, pub_vec[bidder], partner_bid); 
		}
		else if(Suitable_1NT_Res(hand, partner_bid.suit)) // bit 1NT with 4-3, 4-4, or 3-4 in clubs and diamonds and no suitable 4-card major / 5-card minor
		{
			NT_1Res(response, pub_vec[bidder], partner_bid);
		}
		else if(Check_1D_over_1C(hand, partner_bid.suit)) // bit 1D over 1C
		{
			General_response(response, pub_vec[bidder], DIAMONDS, partner_bid.level, make_pair(6,-1), 4);
			pub_vec[bidder].forcing = true;
		}
		else if(Check_support(hand, partner_bid.suit)) // support partner's minor
		{
			Support_minor(hand, response, pub_vec[bidder], partner_bid, suit_pq);
		}
		else // Last resort 1NT Bid. only happens with 3-4-3-3 and no stopper in diamonds . . 
		{
			NT_1Res(response, pub_vec[bidder], partner_bid);
		}
}

//REQUIRES
//MODIFIES response, pub_vec[bidder]
//EFFECTS  makes response with priority (1) new 4-card major or 5-card minor (2) 2NT or 3NT (3) a 1NT last resort.  records promised data in pub_vec[bidder]
void Unsuported_major_response(Hand &hand, 
							   vector<Hand_Public> &pub_vec, 
							   Bid &response, 
							   int bidder, 
							   Bid &partner_bid, 
							   priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{
	if(Check_4CM_5Cm(hand, partner_bid.suit))
	{
		//bid 4-card major or 5-card minor
		Bid_long_suit(hand, response, pub_vec[bidder], partner_bid, suit_pq);
	}
	else if(Check_2NT_3NT_raise(hand, pub_vec, bidder, partner_bid))
	{
		//bid 2NT or 3NT
		NT_Jump_Res(hand, response, pub_vec[bidder], partner_bid); 
	}
	else
	{
		//Last resort 1NT
		NT_1Res(response, pub_vec[bidder], partner_bid);
	}
}


//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  sets response to specified suit and level, sets hand_data to specified point range and length of strain bid.
void General_response(Bid &response, Hand_Public &hand_data, int suit, int level, pair<int, int> points, int suit_support)
{
	response.suit = suit;
	response.level = level;

	hand_data.point_range = points;
	hand_data.suit_length[suit] = suit_support;
	hand_data.passed = false;
}


//REQUIRES suit be valid and not NT
//MODIFIES 
//EFFECTS  checks to see whether a bidable 5-card major or 4-card major exists.
bool Check_4CM_5Cm(Hand &hand, int suit)
{
	if(suit < 0 || suit > 3)
	{
		cerr << "ERROR: invalid suit" << endl;
		assert(0);
	}
	
	if((hand.cards_held[CLUBS].size() > 4 && suit <= CLUBS) || (hand.cards_held[CLUBS].size() > 4 && ((hand.hc_points + hand.dis_points > 9))))
	{
		return true;
	}
	if((hand.cards_held[DIAMONDS].size() > 4 && suit <= DIAMONDS) || (hand.cards_held[DIAMONDS].size() > 4 && ((hand.hc_points + hand.dis_points > 9))))
	{
		return true;
	}
	if((hand.cards_held[HEARTS].size() > 3 && suit <= HEARTS) || (hand.cards_held[HEARTS].size() > 3 && ((hand.hc_points + hand.dis_points > 9))))
	{
		return true;
	}
	if(hand.cards_held[SPADES].size() > 3) //wonder if this will mess anything  up.  if suit=spades, there should never be 4 spades.
	{
		return true;
	}
	return false;
}


//REQUIRES
//MODIFIES 
//EFFECTS  checks to see (1) if partner bid 1C, (2) whether hand has 4 diamonds, (3) whether hand has a diamond stopper. 
bool Check_1D_over_1C(Hand &hand, int suit)
{
	if(suit == CLUBS && hand.cards_held[DIAMONDS].size() == 4 && Suit_stopped(hand, DIAMONDS)) //suppose we would never see 5+ diamonds here . . .
		return true;
	return false;
}	



//REQUIRES suit valid and not NT
//MODIFIES 
//EFFECTS  checks to see if hand 3+ card support in given major, or 4+ card support in a given minor.
bool Check_support(Hand &hand, int suit)
{
	if(suit == HEARTS || suit == SPADES)
	{
		if(hand.cards_held[suit].size() > 2)
			return true;
		return false;
	}
	else if(suit == CLUBS || suit == DIAMONDS)
	{
		if(hand.cards_held[suit].size() > 3)
			return true;
		return false;
	}
	else
	{
		assert(0);
	}
}					

								


//REQUIRES suit be valid and not NT
//MODIFIES 
//EFFECTS  checks to see if hand has 6-9 points and suits besides partner's suit stopped.
bool Suitable_1NT_Res(Hand &hand, int suit)
{
	if(Other_suits_stopped(hand, static_cast<Suit>(suit)) && hand.hc_points < 10)
		return true;
	return false;
}

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  assigns response a NT response and recorded the bid's promises in hand_data
void NT_response(Bid &response, Hand_Public &hand_data, int level, pair<int, int> points, bool balanced, bool stoppers){
	response.suit = 4;
	response.level = level;
	
	hand_data.balanced = balanced;
	hand_data.hcp_range = points;
	hand_data.passed = false;
	hand_data.stoppers = stoppers;
}

//Note: the only way to rid this of "bidder" and "public_vec" seems to be if we do the 8-card fit check outside of this function (which i seem to claim is done by POE)
//REQUIRES
//MODIFIES 
//EFFECTS  returns true if conditions meet 2NT or 3NT raise: 13-18 hcp, no 5-card minor or 4-card major, no known 8-card fit, and balance.  returns false otherwise.
bool Check_2NT_3NT_raise(Hand &hand, vector<Hand_Public> &pub_vec, int bidder, Bid &partner_bid)
{
	//the only non-redudant check here is for points.  the rest are impossible based on the order of the if statements in main().
	if(hand.hc_points < 13 || hand.hc_points > 18)
		return false;
	if(hand.cards_held[CLUBS].size() > 4 || hand.cards_held[DIAMONDS].size() > 4 || hand.cards_held[HEARTS].size() > 3 || hand.cards_held[SPADES].size() > 3)
		return false;
	if(pub_vec[(bidder+PARTNER)%4].suit_length[partner_bid.suit] + hand.cards_held[partner_bid.suit].size() > 7)
		return false;
	if(!Is_balanced(hand)) 
		return false;
	return true;
}

//REQUIRES: hand is has been checked for 2NT or 3NT before function is called.
//MODIFIES response, hand_data
//EFFECTS  Assigns response a bid of either 2NT or 3NT depending on points.  records information these bids promise in hand_data
void NT_Jump_Res(Hand &hand, Bid &response, Hand_Public &hand_data, Bid &partner_bid)
{
	//currently am checking the balanced etc. critieria before calling this function.
	//2NT 13-15
	if(hand.hc_points > 12 && hand.hc_points < 16)
	{
		NT_response(response, hand_data, partner_bid.level+1, make_pair(13,15), true, false);
	}
	//3NT 16-18
	else if(hand.hc_points > 15 && hand.hc_points < 19)
	{
		NT_response(response, hand_data, partner_bid.level+2, make_pair(16,18), true, false);
	}
	else
	{
		assert(0);
	}
	hand_data.game_forcing = true;
	hand_data.forcing = true;		   	
}



//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Reevaluates short points and, based on points, performs either a single raise (6-9), limit raise (10-11), double jump raise (6-9 and length), or 		  introduces a new suit (12+).
void Support_minor(Hand &hand, Bid &response, Hand_Public &hand_data, Bid &partner_bid, priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{
	Get_short_points(hand, static_cast<Suit>(partner_bid.suit));  
	int short_points = hand.hc_points + hand.dis_points;
	cout << "this hand now has " << short_points << " short points after reevaluating short points!" << endl;
	if(short_points > 5 && short_points < 10) // between 6-9
	{
		
		if(hand.cards_held[partner_bid.suit].size() < 6) // if 4 or 5 card support do a single raise
		{
			Single_raise(response, hand_data, partner_bid);	
		}
		else // if  6 or more card support do a double jump raise to game.
		{
			Double_jump_raise(response, hand_data, partner_bid);
		}
	
	}
	else if(short_points > 9 && short_points < 12) // between 10-11 do a limit raise (This is invitational)
	{
		Limit_raise(response, hand_data, partner_bid);
	}
	else // more than 12 introduce a new suit. (this is where we're going to need the inforation from above.
	{
		New_suit_intro(response, hand_data, partner_bid, suit_pq);
		cerr<< "ALERT: what do you do when you have 4+ card support for partner's minor, but 12+ points?" << endl;
		cerr<< "       there isn't really a new suit to introduce, so how do you show your points?" << endl;
		assert(0);
	}		   
}



//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  assigns opening_bid 1NT and recorded 6-9 points, without promisig balance or stoppers
void NT_1Res(Bid &response, Hand_Public &hand_data, Bid &partner_bid)
{			  	
	NT_response(response, hand_data, partner_bid.level, make_pair(6,9), false, false);
}
					   

//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  assigns opening_bid a NT opener and recorded the bid's promises in hand_data
void NT_Opener(Bid &opening_bid, Hand_Public &hand_data, int level, pair<int, int> points)
{
	opening_bid.suit = 4;
	opening_bid.level = level;
	hand_data.hcp_range = points;
	hand_data.balanced = true;
	hand_data.suit_length = {2, 2, 2, 2};
}


//this function is only used in opening_bid and can be replaced by passing the pq.  so we can remove this function.
//REQUIRES
//MODIFIES 
//EFFECTS  returns longest suit and its length, in suit order.
pair<int, int> Longest_suit(Hand &hand)
{
	//question: when do i use this, why, and why not the pq?
	int suit_length = hand.cards_held[SPADES].size();
	int suit = 3;

	if(hand.cards_held[HEARTS].size() > suit_length)
	{
		suit = 2;
		suit_length = hand.cards_held[HEARTS].size();
	}
	if(hand.cards_held[DIAMONDS].size() > suit_length)
	{
		suit = 1;
		suit_length = hand.cards_held[DIAMONDS].size();
	}
	if(hand.cards_held[CLUBS].size() > suit_length)
	{
		suit = 0;
		suit_length = hand.cards_held[CLUBS].size();
	}
	return make_pair(suit, suit_length);
}


//feel like this function can be used in count hcp up above.
//REQUIRES suit \in {0, 1, 2, 3}
//MODIFIES 
//EFFECTS  Determines number of hcp points in specified suit
int Suit_strength(Hand &hand, int suit)
{
	//might want to check to make sure suit is a valid int.	
	int suit_count = 0;
	for(int i = 0; i < hand.cards_held[suit].size(); i++) //loops through desired suit of hand
	{	
			//checks for facecards within desired suit.
			if(hand.cards_held[suit][i].rank == ACE) 
				suit_count += 4;
			else if(hand.cards_held[suit][i].rank == KING)
				suit_count += 3;
			else if(hand.cards_held[suit][i].rank == QUEEN)
				suit_count += 2;
			else if(hand.cards_held[suit][i].rank == JACK)
				suit_count += 1;
	}
	return suit_count;
}


//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  Determines opening_bid and stores the information it promises in hand_data.
void Opening_Bid(Hand &hand, Bid &opening_bid, Hand_Public &hand_data)
{
	int hc_points = hand.hc_points;
	int clubs = hand.cards_held[CLUBS].size();
	int diamonds = hand.cards_held[DIAMONDS].size();
	int hearts = hand.cards_held[HEARTS].size();
	int spades = hand.cards_held[SPADES].size();

	//OPENING BID
	if(hand.hc_points + hand.dis_points > 12)
	{
		//check 1NT and 2NT
		if(hand.hc_points + hand.dis_points < 22 || (hand.hc_points < 22 && hand.dis_points == 1))
		{
			if(hand.dis_points < 2  && ((hand.hc_points > 14 && hand.hc_points < 18) || hand.hc_points == 20 || hand.hc_points == 21))
			{
				if(hand.hc_points > 14 && hand.hc_points < 18) // should add funtion to this that checks for stoppers
				{
					//1NT Opener
					NT_Opener(opening_bid, hand_data, 1, make_pair(15,17));	
				}
				else
				{
					//2NT Opener
					NT_Opener(opening_bid, hand_data, 2, make_pair(20,21));	
				}
			}
			//might open with spades with 6 spades and 7 of a minor.
			else if(spades > 4 && spades >= hearts && spades >= diamonds && spades >= clubs)
			{
				//1S Opener
				General_response(opening_bid, hand_data, 3, 1, make_pair(13,21), 5);
			}
			else if(hearts > 4 && hearts >= diamonds && hearts >= clubs)
			{
				//1H Opener
				General_response(opening_bid, hand_data, 2, 1, make_pair(13,21), 5);
			}
			//at this point we have not found a 5 card major (w/o six-card minor)
			else if(clubs == 3 && diamonds == 3) //not sure these three 1C, 1D, 1C all make sense . . . 
			{
				//1C Opener (specific)
				General_response(opening_bid, hand_data, 0, 1, make_pair(13,21), 3);
			}
			else if(diamonds >= clubs)
			{
				//1D Opener (general)
				General_response(opening_bid, hand_data, 1, 1, make_pair(13,21), 3);
			}
			else if(clubs >= diamonds)
			{
				//1C Opener (general)
				General_response(opening_bid, hand_data, 0, 1, make_pair(13,21), 3);
			}
			else
			{
				cerr << "ERROR: this is a case where we have an opening hand, but couldn't find something to open." << endl;
				assert(0);
			}
		}
		else // big hands
		{
			if(hand.dis_points < 2  && (hc_points > 24 && hc_points < 28))
			{
				//3NT Opener
				NT_Opener(opening_bid, hand_data, 3, make_pair(25,27));			
			}
			else
			{
				//2 Club Opener
				General_response(opening_bid, hand_data, 0, 2, make_pair(22,37), 0);
			}			
		}
	}

	else //fewer than 12 points.
	{
		//i should pass the pq here instead and just pop off the top suit.
		pair<int, int> longest_suit = Longest_suit(hand);
		int suit = longest_suit.first;
		int suit_length = longest_suit.second;
		
		if(suit_length > 5)
		{
			//suit_count is number of points in the longest suit.
			int suit_count = Suit_strength(hand, suit);

			if(suit_count > 4) 
			{	

				if(suit_length == 6)
				{
					if(suit == 0)
					{
						//2 Club Pass
						Pass(opening_bid, hand_data, 0, make_pair(0, 12));
					}	
					else if(suit > 0 && suit < 4) //don't make public the 5 points in trump
					{
						//2D, 2H, 2S Weak Openers
						General_response(opening_bid, hand_data, suit, 2, make_pair(5,12), suit_length);
					}	
					else
					{
						cerr << "ERROR: we have an out-of-bounds suit in the weak 2 openers." << endl;
						assert(0);
					}
				}

				else if(suit_length == 7)
				{
					//3C, 3D, 3H, 3S Prempts
					General_response(opening_bid, hand_data, suit, 3, make_pair(5,12), suit_length);
				}
				else if(suit_length > 7)
				{
					//4C, 4D, 4H, 4S Prempts
					//note: can't reveal real suit count--that would be table-talk.  instead limit it to 8.
					General_response(opening_bid, hand_data, suit, 4, make_pair(5,12), 8);
				}
			}
			else // PASS: we have a long enough suit for a pre-empt, but it's not a strong enough suit.
			{
				Pass(opening_bid, hand_data, 0, make_pair(0, 12));
			}
		}
		else // PASS: we do not have a long enough suit for a preempt, and we don't have enough points to open.
		{
			Pass(opening_bid, hand_data, 0, make_pair(0, 12));
		}
	}
}

//REQUIRES
//MODIFIES
//EFFECTS  Returns true if specificed player bid at their last opportunity, returns false otherwise.
bool Player_bid(std::vector<Hand_Public> &pub_vec, int bidder, Player player)
{
	if(pub_vec[(bidder+player) % 4].passed)
		return false;
	return true;
}



//REQUIRES
//MODIFIES response, hand_data, suit_pq
//EFFECTS  Determines longest bidable suit and bids it, bypassing 4-card minors in favor of 4-card majors
void Bid_long_suit(Hand &hand, Bid &response, Hand_Public &hand_data, Bid &partner_bid, priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq)
{
	//this while loop determines the longest bidable suit
	Suit_Length long_suit = suit_pq.top();
	while(partner_bid.suit > long_suit.suit && (hand.hc_points + hand.dis_points < 10))
	{
		suit_pq.pop();
		long_suit = suit_pq.top();
	}
	//if we must go to the 2-level to bid longest suit
	if(partner_bid.suit > long_suit.suit)
	{
		General_response(response, hand_data, long_suit.suit, partner_bid.level + 1, make_pair(10,-1), 5);
		hand_data.forcing = true;
	}
	//if we can bid longest suit at 1-level
	else if(partner_bid.suit < long_suit.suit)
	{
		General_response(response, hand_data, long_suit.suit, partner_bid.level, make_pair(6,-1), 4);
		hand_data.forcing = true;
	}
	else //same suit => that this is a minor, so we check for 4-card majors before we support the minor.
	{
		if(hand.cards_held[HEARTS].size() > 3)
		{
			//bid 1H
			General_response(response, hand_data, HEARTS, partner_bid.level, make_pair(6,-1), 4);
			hand_data.forcing = true;
		}
		else if(hand.cards_held[SPADES].size() > 3)
		{
			//bid 1S
			General_response(response, hand_data, SPADES, partner_bid.level, make_pair(6,-1), 4);
			hand_data.forcing = true;
		}
		else
		{
			Support_minor(hand, response, hand_data, partner_bid, suit_pq);
		}
	}
}
