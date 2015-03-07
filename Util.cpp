/* Nicholas Nuechterlein
 * Util.cpp
 */

#include "Card.h"
#include "Deck.h"
#include "Hand.h"
#include "Bid.h"
#include "Util.h"


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


void Fill_suit_pq(priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq, Hand &hand, Bid last_bid){


		Suit_Length clubs(0, hand.cards_held[CLUBS].size(), last_bid);
		Suit_Length diamonds(1, hand.cards_held[DIAMONDS].size(), last_bid);
		Suit_Length hearts(2, hand.cards_held[HEARTS].size(), last_bid);
		Suit_Length spades(3, hand.cards_held[SPADES].size(), last_bid);

		suit_pq.push(clubs);
		suit_pq.push(diamonds);
		suit_pq.push(hearts);
		suit_pq.push(spades);
		
		
		/* not sure why the following doesn't work . . .
		for(int i = 0; i < 4; i++)
		{
			Suit_Length temp = (i, bidder_hand.suit_length[static_cast<Suit>(i)], last_bid);
			suit_pq.push(temp);
		}
		*/

}


	
void hand_data_Suit_intit(Hand_Public &hand_data, pair <int, int> points, int suit, int suit_length, Bid bid_name){
		//Hand_Public hand_data();
		hand_data.point_range = points;
		hand_data.suit_length[suit] = suit_length;
		hand_data.bid_name = bid_name;
		hand_data.passed = false;
		//no informatoin about forcing.

		}
		
		
	void hand_data_NT_intit(Hand_Public &hand_data, pair <int, int> points, Bid bid_name){
		//Hand_Public hand_data();
		hand_data.hcp_range = points;
		hand_data.suit_length = {2,2,2,2};
		hand_data.balanced = true; 
		hand_data.bid_name = bid_name;
		hand_data.passed = false;
		//does it promise stoppers?
		//no informatoin about forcing.
		//return hand_data;
	}
	
void hand_data_PASS_intit(Hand_Public &hand_data){
		hand_data.point_range = make_pair(0,12);
		hand_data.passed = true;
	}
	
	
void Fill_opening_data(unordered_map<string, Hand_Public> &opening_data){
			
	
			//don't know which ones are forcing besides 2C
			hand_data_Suit_intit(opening_data["1C"], make_pair(13, 21), 0, 3, Bid(0, 1));
			hand_data_Suit_intit(opening_data["1D"], make_pair(13, 21), 1, 3, Bid(1, 1));
			hand_data_Suit_intit(opening_data["1H"], make_pair(13, 21), 2, 5, Bid(2, 1));
			hand_data_Suit_intit(opening_data["1S"], make_pair(13, 21), 3, 5, Bid(3, 1));
			hand_data_NT_intit(opening_data["1NT"], make_pair(15, 17), Bid(4, 1));
			
			hand_data_Suit_intit(opening_data["2C"], make_pair(22, 37), 0, 0, Bid(0, 2));
			opening_data["2C"].forcing = true;
			hand_data_Suit_intit(opening_data["2D"], make_pair(5, 12), 1, 6, Bid(1, 2));
			hand_data_Suit_intit(opening_data["2H"], make_pair(5, 12), 2, 6, Bid(2, 2));
			hand_data_Suit_intit(opening_data["2S"], make_pair(5, 12), 3, 6, Bid(3, 2));
			hand_data_NT_intit(opening_data["2NT"], make_pair(20, 21), Bid(4, 2));
			
			hand_data_Suit_intit(opening_data["3C"], make_pair(5, 12), 0, 7, Bid(0, 3));
			hand_data_Suit_intit(opening_data["3D"], make_pair(5, 12), 1, 7, Bid(1, 3));
			hand_data_Suit_intit(opening_data["3H"], make_pair(5, 12), 2, 7, Bid(2, 3));
			hand_data_Suit_intit(opening_data["3S"], make_pair(5, 12), 3, 7, Bid(3, 3));
			hand_data_NT_intit(opening_data["3NT"], make_pair(25, 27), Bid(4, 3));
			
			hand_data_Suit_intit(opening_data["4C"], make_pair(5, 12), 0, 8, Bid(0, 4));
			hand_data_Suit_intit(opening_data["4D"], make_pair(5, 12), 1, 8, Bid(1, 4));
			hand_data_Suit_intit(opening_data["4H"], make_pair(5, 12), 2, 8, Bid(2, 4));
			hand_data_Suit_intit(opening_data["4S"], make_pair(5, 12), 3, 8, Bid(3, 4));
			
			hand_data_PASS_intit(opening_data["PASS"]);
		
}


void ReadFile(ifstream &file, 
			  string &line, 
			  int &total_num_hands, 
			  int &number_hands, 
			  Bid &north_opening, //could put these eight bids in a vector of four pairs.
			  Bid &east_opening, 
			  Bid &south_opening, 
			  Bid &west_opening, 
			  Bid &correct_north_opening, 
			  Bid &correct_east_opening, 
			  Bid &correct_south_opening,
			  Bid &correct_west_opening, 
			  unordered_map<string, Hand_Public> &opening_data, 
			  vector<Hand_Public> &pub_vec, 
			  int &bidder, 
			  vector<Hand> &hands){
	
			getline(file, line); //this should be "//description . . . "
			cout<< "HAND " << total_num_hands - number_hands <<  " DESCRIPTION: " << endl << endl;
			cout << line << endl << endl;
			getline(file, line); //this should be blank line.
		
			getline(file, line);//north bid
			north_opening = opening_data[line].bid_name; // this is the name of north's bid (so we can check it for consistency)
			pub_vec[bidder] = opening_data[line]; //this is the meaning of north's bid
			bidder++;
		
			getline(file, line);//east bid
			east_opening = opening_data[line].bid_name; // this is the name of east's bid (so we can check it for consistency)
			pub_vec[bidder] = opening_data[line]; //this is the meaning of east's bid
			bidder++;
		
			for(int ctr = 0; ctr < 4; ctr++) //south hand
			{
				getline(file, line);
				ReadLine(line, bidder, hands); //hard_code
			}
		 
			bidder++;
		
			getline(file, line);//west bid
			west_opening = opening_data[line].bid_name; // this is the name of west's bid (so we can check it for consistency)
			pub_vec[bidder] = opening_data[line]; //this is the meaning of west's bid
			bidder++;
		
			getline(file, line); //should be the "-" line
			if(line.compare("-") != 0)
			{
				cerr << "ERROR: this should be the \"-\" line, but it's not!" << endl;
				assert(0);
			}
			getline(file, line); //should be the correct north bid
			correct_north_opening = opening_data[line].bid_name;
			if(north_opening.level != correct_north_opening.level || north_opening.suit != correct_north_opening.suit)
			{
				cerr << "ERROR: North's bids do not match!" << endl;
				assert(0);
			}
		
			getline(file, line); //should be the correct east bid
			correct_east_opening = opening_data[line].bid_name;
			if(east_opening.level != correct_east_opening.level || east_opening.suit != correct_east_opening.suit)
			{
				cerr << "ERROR: East's bids do not match!" << endl;
				assert(0);
			}
		
			getline(file, line); //SHOULD BE THE CORRECT SOUTH BID
			correct_south_opening = opening_data[line].bid_name;
			getline(file, line); //should be the correct west bid
			correct_west_opening = opening_data[line].bid_name;
			if(west_opening.level != correct_west_opening.level || west_opening.suit != correct_west_opening.suit)
			{
				cerr << "ERROR: West's bids do not match!" << endl;
				assert(0);
			}
			getline(file, line); //should be the "---" line
			if(line.compare("---") != 0)
			{
				cerr << "ERROR: this should be the \"---\" line, but it's not!" << endl;
				assert(0);
			}
		
			number_hands--;
		}
