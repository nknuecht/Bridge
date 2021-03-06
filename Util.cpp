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



//REQUIRES suit_pq is empty.
//MODIFIES suit_pq
//EFFECTS  converts hands' four suits into Suit_Length structs and adds them to the empty suit_pq
void Fill_suit_pq(priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> &suit_pq, Hand &hand, Bid last_bid)
{
	for(int s = 0; s < 4; ++s)
	{
		Suit_Length temp(s, hand.cards_held[static_cast<Suit>(s)].size(), last_bid);
		suit_pq.push(temp);
	}
}


//REQUIRES requires suit \in {-1, 0, 1, 2, 3, 4}
//MODIFIES hand_data
//EFFECTS  adds information to hand_data's point_range, suit_length, bid_name, and passed stautus promised by the opening bid made.
void Set_opening_data(Hand_Public &hand_data, pair<int, int> points, int suit, int suit_length, Bid bid_name)
{
	if(suit > -1 && suit < 4)//SUIT BID
	{
		hand_data.point_range = points;
		hand_data.suit_length[suit] = suit_length;
		hand_data.bid_name = bid_name; // would like to get rid of this to solve file dependency problems
		hand_data.passed = false;
	}
	else if(suit == 4)//NT Bid
	{
		hand_data.hcp_range = points;
		hand_data.suit_length = {2,2,2,2};
		hand_data.balanced = true; 
		hand_data.bid_name = bid_name;
		hand_data.passed = false;
	}
	else if(suit == -1)//PASS
	{
		hand_data.point_range = make_pair(0,12);
		hand_data.passed = true;
		hand_data.bid_name = bid_name; //or does it already default to PASS?
	}
	else //Invalid suit
	{
		assert(0);
	}
}


//REQUIRES 
//MODIFIES opening_data
//EFFECTS  fill a table in opening_data which contains the information promised by all opening bids
void Fill_opening_data(unordered_map<string, Hand_Public> &opening_data)
{		
	//don't know which opening bids are forcing besides 2C
	Set_opening_data(opening_data["1C"], make_pair(13, 21), 0, 3, Bid(0, 1));
	Set_opening_data(opening_data["1D"], make_pair(13, 21), 1, 3, Bid(1, 1));
	Set_opening_data(opening_data["1H"], make_pair(13, 21), 2, 5, Bid(2, 1));
	Set_opening_data(opening_data["1S"], make_pair(13, 21), 3, 5, Bid(3, 1));
	Set_opening_data(opening_data["1NT"], make_pair(15, 17), 4, -1, Bid(4, 1));

	Set_opening_data(opening_data["2C"], make_pair(22, 37), 0, 0, Bid(0, 2));
	opening_data["2C"].forcing = true; //2C is a forcing opening bid
	Set_opening_data(opening_data["2D"], make_pair(5, 12), 1, 6, Bid(1, 2));
	Set_opening_data(opening_data["2H"], make_pair(5, 12), 2, 6, Bid(2, 2));
	Set_opening_data(opening_data["2S"], make_pair(5, 12), 3, 6, Bid(3, 2));
	Set_opening_data(opening_data["2NT"], make_pair(20, 21), 4, -1, Bid(4, 2));

	Set_opening_data(opening_data["3C"], make_pair(5, 12), 0, 7, Bid(0, 3));
	Set_opening_data(opening_data["3D"], make_pair(5, 12), 1, 7, Bid(1, 3));
	Set_opening_data(opening_data["3H"], make_pair(5, 12), 2, 7, Bid(2, 3));
	Set_opening_data(opening_data["3S"], make_pair(5, 12), 3, 7, Bid(3, 3));
	Set_opening_data(opening_data["3NT"], make_pair(25, 27), 4, -1, Bid(4, 3));

	Set_opening_data(opening_data["4C"], make_pair(5, 12), 0, 8, Bid(0, 4));
	Set_opening_data(opening_data["4D"], make_pair(5, 12), 1, 8, Bid(1, 4));
	Set_opening_data(opening_data["4H"], make_pair(5, 12), 2, 8, Bid(2, 4));
	Set_opening_data(opening_data["4S"], make_pair(5, 12), 3, 8, Bid(3, 4));

	Set_opening_data(opening_data["PASS"], make_pair(0,12), -1, -1, Bid(-1, 0));	
}

//REQUIRES hand is empty
//MODIFIES hand
//EFFECTS  adds cards from file to given hand
void ReadLine(string line, Hand &hand)
{	
	if(line.length() > 2) //otherwise we have a void
	{
		Suit suit;
		if(line.substr(0, 2).compare("S:") == 0)
			suit = static_cast<Suit>(3);
		else if(line.substr(0, 2).compare("H:") == 0)
			suit = static_cast<Suit>(2);
		else if(line.substr(0, 2).compare("C:") == 0)
			suit = static_cast<Suit>(0);
		else if(line.substr(0, 2).compare("D:") == 0)
			suit = static_cast<Suit>(1);
		int pos = 2;
		int value;
	
		//(2) read in the entire line of cards (which should be all the cards in the given suit)
		while(pos < line.length() - 1)
		{
			if(line.substr(pos,1).compare(" ") == 0)
				pos++;
			if(pos < line.length()-1 && line.substr(pos+1,1).compare(" ") == 0)
			{
				value = stoi(line.substr(pos, 1));
				pos++;
			}
			else
			{
				value = stoi(line.substr(pos, 2));
				pos+=2;
			}
			Card temp_card;
			Card_init(temp_card, static_cast<Rank>(value-2), suit);
			Add_card(hand, temp_card);
		}

	}
}

//REQUIRES 
//MODIFIES opening, hand_data, bidder
//EFFECTS  assigns bid_name from file to opening, fills in promised data from this opening bid into hand_data via opening_data table, and increments bidder. 
void ReadBid(ifstream &file, string &line, Bid &opening_bid, Hand_Public &hand_data, unordered_map<string, Hand_Public> &opening_data, int &bidder)
{
	getline(file, line);//string name of opening
	opening_bid = opening_data[line].bid_name; // this is the name of opening_bid's bid (so we can check it for consistency later) (wonder if i can just leave this as a string . . . )
	hand_data = opening_data[line]; //this is the meaning of opening_bid's bid (this might be a really slow deep copy . . . maybe there's a faster way)
	bidder++;
}


//REQUIRES 
//MODIFIES correct_opening_bid
//EFFECTS  reads in the correct opening bid from file and verifies that it matches the open bid given earlier in the file. this is a check for human error.
static void CheckBids_helper(ifstream &file, string &line, Bid &correct_opening_bid, Bid &opening_bid, unordered_map<string, Hand_Public> &opening_data, string player_name)
{
	getline(file, line); //should be the correct player_name's bid
	correct_opening_bid = opening_data[line].bid_name;
	if(opening_bid.level != correct_opening_bid.level || opening_bid.suit != correct_opening_bid.suit)
	{
		cerr << "ERROR: " << player_name << "'s bids do not match!" << endl;
		assert(0);
	}
}

//REQUIRES 
//MODIFIES bid_checks
//EFFECTS  obtains south's (the test bid) correct bid for later verification. calls CheckBid_helper to varify that all other bids are consistent (human error check)
void CheckBids(ifstream &file, string &line, vector< pair<Bid, Bid> > &bid_checks, unordered_map<string, Hand_Public> &opening_data)
{
	getline(file, line); //Reads in the "-" line
	if(line.compare("-") != 0)
	{
		cerr << "ERROR: this should be the \"-\" line, but it's not!" << endl;
		assert(0);
	}

	//Note: later should enum Players {NORTH, EAST, SOUTH, WEST};
	//to be clear: bid_checks[x].first is X's opening bid, where X \in {NORTH, EAST SOUTH, WEST}
	//             bid_checks[x].second is X's CORRECT opening bid, where X \in {NORTH, EAST SOUTH, WEST}

	//check North's bid
	CheckBids_helper(file, line, bid_checks[0].second, bid_checks[0].first, opening_data, "North");

	//check East's bid
	CheckBids_helper(file, line, bid_checks[1].second, bid_checks[1].first, opening_data, "East");

	//obtain south's correct bid: will check it after we make a bid based on south's hand.
	getline(file, line); //SHOULD BE THE CORRECT SOUTH BID
	bid_checks[2].second = opening_data[line].bid_name;

	//check West's bid
	CheckBids_helper(file, line, bid_checks[3].second, bid_checks[3].first, opening_data, "West");

	getline(file, line); //should be the "---" line
	if(line.compare("---") != 0)
	{
		cerr << "ERROR: this should be the \"---\" line, but it's not!" << endl;
		assert(0);
	}
}

//REQUIRES south's hand is empty
//MODIFIES bid_checks, opening_data, public_vec, bidder, number_hands, hands
//EFFECTS  obtains south's (the test bid) correct bid for later verification.  fills south's hand. calls CheckBid_helper to varify that all other bids are consistent (human error check)
void ReadFile(ifstream &file, 
			  string &line, //not sure i need to be passing a string . . . can just make a new one.
			  int &total_num_hands, 
			  int &number_hands, 
			  vector< pair<Bid, Bid> > &bid_checks,
			  unordered_map<string, Hand_Public> &opening_data, 
			  vector<Hand_Public> &pub_vec, 
			  int &bidder, 
			  vector<Hand> &hands)
{
	getline(file, line); //this reads in the description of the hand
	cout<< "HAND " << total_num_hands - number_hands <<  " DESCRIPTION:\n\n " << line << "\n\n";
	getline(file, line); //this reads in a blank line.


	//Read in north_opening
	ReadBid(file, line, bid_checks[0].first, pub_vec[bidder], opening_data, bidder);

	//Read in east_opening
	ReadBid(file, line, bid_checks[1].first, pub_vec[bidder], opening_data, bidder);

	//Read in south's hand
	for(int ctr = 0; ctr < 4; ctr++)
	{
		getline(file, line);
		ReadLine(line, hands[bidder]);
	}
	bidder++;

	//Read in east_opening
	ReadBid(file, line, bid_checks[3].first, pub_vec[bidder], opening_data, bidder);


	//verify that the bids were read in correctly, and that the file was formatted correctly
	CheckBids(file, line, bid_checks, opening_data);

	number_hands--;
}
