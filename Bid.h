#ifndef BID_H
#define BID_H
/* Bid.h
 *
 * Representation of a Bridge bid
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

enum Player{ME, NEXT, PARTNER, PREV}; //don't know where these should go (i.e. in what file)





struct Suit_Length{
	int suit;
	int length;
	Bid last_bid; //how do i get this into the constructor?
	Suit_Length() : suit(-1), length(0), last_bid() {}
	Suit_Length(Bid last_bid_in) : suit(-1), length(0), last_bid(last_bid_in) {}
	Suit_Length(int suit_in, int length_in, Bid last_bid_in) : suit(suit_in), length(length_in), last_bid(last_bid_in) {}
};

//Comparator Class
class CompSuits{
	public:

	bool operator()(Suit_Length& suit_length1, Suit_Length& suit_length2){
			//this might be backwards, but i'm trying to return false (meaning putting at the top) the longest suit. (i think this is right now)
			//probably should have a check that ensures they aren't the same suits.
			//something to deal with NT or other invalid suit ints or length ints or bids.


			if(suit_length1.length > suit_length2.length)
				return false;
			else if(suit_length1.length < suit_length2.length)
				return true;
			
			else // suits are of same length
			{
				if(suit_length1.length > 4)  //5 are 6 combined.
				{
					if(suit_length1.suit > suit_length2.suit)
						return false;
					else
						return true;
				}
				//want to return the suit that can be bid at the lowest level.
				else if(suit_length1.length < 5  && suit_length2.length < 5) //4 and lower can be combined.
				{

					Bid last_bid = suit_length1.last_bid;
					int bid_level1 = -1;  int bid_level2 = -1;
					
					if(suit_length1.suit <= suit_length1.last_bid.suit) //bid at higher level
						bid_level1 = last_bid.level*4 + suit_length1.suit;
					else //bid at current level
						bid_level1 = (last_bid.level-1)*4 + suit_length1.suit; //have to be careful about levels being 0.  passes, doubles, redobles, errors.
						
					if(suit_length2.suit <= suit_length1.last_bid.suit)
						bid_level2 = last_bid.level*4 + suit_length2.suit;
					else
						bid_level2 = (last_bid.level-1)*4 + suit_length2.suit; //have to be careful about levels being 0.  passes, doubles, redobles, errors.

					if(bid_level1 < bid_level2)
						return false;
					else if(bid_level1 > bid_level2)
						return true;
					else
					{
						std::cerr << "ERROR: The next bid for two different suits is the same!\n";
						std::cerr << "       bid_level1 = " << bid_level1 << "\n";
						std::cerr << "       bid_level2 = " << bid_level2 << "\n";
						std::cerr << "       The last bid was " << last_bid.level << " of suit " << last_bid.suit << "\n";
						std::cerr << "       suit_length1 is suit " << suit_length1.suit << " of length " << suit_length1.length << "\n";
						std::cerr << "       suit_length2 is suit " << suit_length2.suit << " of length " << suit_length2.length << "\n";
						assert(0);
					}

				}
				else
				{
					assert(0);
				}

			}

			
	}
};






void Print_bid(Bid &bid);

//REQUIRES Player has bid (or else we get what?)
//MODIFIES none.
//EFFECTS  Displays information promised by bidder's latest bid.
void Interpret_bid(std::vector<Hand_Public> &pub_vec, int bidder);

Bid Next_bid(int suit, Bid &last_bid);

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Sets response to PASS and encodes PASS promises into pub_vec
void Pass(Bid &response, Hand_Public &hand_data, int level, std::pair<int, int> points);

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 1 level, promises 6-9 points and 3+ support responding to a major, 4+ support responding to a minor.
void Single_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid);

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 3 levels, promises 6-9 points and 5+ support responding to a major, 6+ support responding to a minor.
void Double_jump_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid);

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Raises partner's suit by 2 levels, promises 10-11 points and 3+ support responding to a major, 4+ support responding to a minor.
void Limit_raise(Bid &response, Hand_Public &hand_data, Bid partner_bid);



//REQUIRES
//MODIFIES response, hand_data, suit_pq
//EFFECTS  Introduces longest new suit, promises 12+ points and 4+ cards in new suit.  Game forcing bid opposite opener.
void New_suit_intro(Bid &response, 
					Hand_Public &hand_data, 
					Bid partner_bid,
					std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);
//REQUIRES
//MODIFIES hand, response, hand_data, suit_pq
//EFFECTS  Reevaluates short points and, based on points, performs either a single raise (6-9), limit raise (10-11), double jump raise (6-9 and length), or 		  introduces a new suit (12+).					
void Support_major(Hand &hand, 
				   Bid &response, 
				   Hand_Public &hand_data, 
				   Bid &partner_bid,
				   std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);


//REQUIRES
//MODIFIES response, pub_vec[bidder]
//EFFECTS  makes response with priority (1) new 4-card major or 5-card minor (2) 2NT or 3NT (3) a suitable 1NT, (4) 1C -> 1D, (5) support of minor, (6) 1NT
void Minor_suit_response(Hand &hand, 
						 std::vector<Hand_Public> &pub_vec, 
						 Bid &response, 
						 int bidder, 
						 Bid &partner_bid, 
						 std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);


//REQUIRES
//MODIFIES response, pub_vec[bidder]
//EFFECTS  makes response with priority (1) new 4-card major or 5-card minor (2) 2NT or 3NT (3) a 1NT last resort.  records promised data in pub_vec[bidder].
void Unsuported_major_response(Hand &hand, 
							 std::vector<Hand_Public> &pub_vec, //still keeping this because it calls 2NT_3NT_raise
							 Bid &response, 
							 int bidder, 
							 Bid &partner_bid, 
							 std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);
//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  sets response to specified suit and level, sets hand_data to specified point range and length of strain bid.				   
void General_response(Bid &response, Hand_Public &hand_data, int suit, int level, std::pair<int, int> points, int suit_support);


//REQUIRES suit be valid and not NT
//MODIFIES 
//EFFECTS  checks to see whether a bidable 5-card major or 4-card major exists.
bool Check_4CM_5Cm(Hand &hand, int suit);


//REQUIRES
//MODIFIES 
//EFFECTS  checks to see (1) if partner bid 1C, (2) whether hand has 4 diamonds, (3) whether hand has a diamond stopper. 
bool Check_1D_over_1C(Hand &hand, int suit);

//REQUIRES suit is valid and not NT
//MODIFIES 
//EFFECTS  checks to see if hand 3+ card support in given major, or 4+ card support in a given minor.
bool Check_support(Hand &hand, int suit);



//REQUIRES suit be valid and not NT
//MODIFIES 
//EFFECTS  checks to see if hand has 6-9 points and suits besides partner's suit stopped.				   
bool Suitable_1NT_Res(Hand &hand, int suit);  

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  assigns response a NT response and recorded the bid's promises in hand_data
void NT_response(Bid &response, Hand_Public &hand_data, int level, std::pair<int, int> points, bool balanced, bool stoppers);


//REQUIRES
//MODIFIES 
//EFFECTS  returns true if conditions meet 2NT or 3NT raise: 13-18 hcp, no 5-card minor or 4-card major, no known 8-card fit, and balance.  returns false otherwise.
bool Check_2NT_3NT_raise(Hand &hand, std::vector<Hand_Public> &pub_vec, int bidder, Bid &partner_bid);

//REQUIRES: hand is has been checked for 2NT or 3NT before function is called.
//MODIFIES response, hand_data
//EFFECTS  Assigns response a bid of either 2NT or 3NT depending on points.  records information these bids promise in hand_data
void NT_Jump_Res(Hand &hand, 
				   Bid &response, 
				   Hand_Public &hand_data, 
				   Bid &partner_bid);

//REQUIRES
//MODIFIES response, hand_data
//EFFECTS  Reevaluates short points and, based on points, performs either a single raise (6-9), limit raise (10-11), double jump raise (6-9 and length), or 		  introduces a new suit (12+).				   
void Support_minor(Hand &hand, 
				   Bid &response, 
				   Hand_Public &hand_data, 
				   Bid &partner_bid, 
				   std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);

//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  assigns opening_bid 1NT and recorded 6-9 points, without promisig balance or stoppers				   
void NT_1Res(Bid &response, Hand_Public &hand_data, Bid &partner_bid);


//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  assigns opening_bid a NT opener and recorded the bid's promises in hand_data
void NT_Opener(Bid &opening_bid, Hand_Public &hand_data, int level, std::pair<int, int> points);


//this function is only used in opening_bid and can be replaced by passing the pq.  so we can remove this function.
//REQUIRES
//MODIFIES 
//EFFECTS  returns longest suit and its length, in suit order.
std::pair<int, int> Longest_Suit(std::vector<Hand> &hands, int bidder);



//feel like this function can be used in count hcp up above.
//REQUIRES suit \in {0, 1, 2, 3}
//MODIFIES 
//EFFECTS  Determines number of hcp points in specified suit
int Suit_strength(std::vector<Hand> &hands, int bidder, int suit);

//REQUIRES
//MODIFIES opening_bid, hand_data
//EFFECTS  Determines opening_bid and stores the information it promises in hand_data.					
void Opening_Bid(Hand &hand, Bid &opening_bid, Hand_Public &hand_data);

//REQUIRES
//MODIFIES
//EFFECTS  Returns true if specificed player bid at their last opportunity, returns false otherwise.
bool Player_bid(std::vector<Hand_Public> &pub_vec, int bidder, Player player);

//REQUIRES
//MODIFIES response, hand_data, suit_pq
//EFFECTS  Determines longest bidable suit and bids it, bypassing 4-card minors in favor of 4-card majors				   
void Bid_long_suit(Hand &hand, 
				   Bid &response, 
				   Hand_Public &hand_data, 
				   Bid &partner_bid, 
				   std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq);











//these two functions were never called.  I deleted them.
//void Four_card_major(std::vector<Hand> &hands,  Bid &response, std::vector<Hand_Public> &pub_vec, Bid &partner_bid, int bidder);
//void Five_card_minor_2level(Hand &hand,  Bid &response,  Hand_Public &hand_data,  Bid &partner_bid);




#endif
