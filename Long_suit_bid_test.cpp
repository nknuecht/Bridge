//Long_suit_bid_test.cpp

#include "Card.h"
#include "Deck.h"
#include "Hand.h"
#include "Bid.h"
#include "Util.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <queue> //pq
#include <unordered_map>
#include <string>
using namespace std;

int main() {	

	std::unordered_map<std::string, Hand_Public> opening_data;
	Fill_opening_data(opening_data);

	////********* reading custom hands in from file 2 *************** 

	ifstream file;
	//list of files: suit response test cases: hand0.txt, SORex.txt
	//file.open("hand0.txt");
	file.open("SORex.txt");
	string line;
	getline(file, line); //this should be the number of total hands in the file (an int for our big loop)
	int number_hands = stoi(line);
	int total_num_hands = number_hands;
	cout << endl << endl << endl << endl;
	cout<< "Number of total hands in file: " << number_hands << endl << endl;


	while(number_hands > 0 && !file.eof()) // running through test cases.
	{
		Hand west;	Hand north;	Hand east;	Hand south;
		vector<Hand> hands; //vector of hands
		hands.push_back(north); 	hands.push_back(east);	hands.push_back(south);	hands.push_back(west);

		vector<Hand_Public> pub_vec(4, Hand_Public()); //contains public bidding information.
		vector<Bid> bids;  
		int bidder = 0; //hardcode north as dealer / bidder.
		
		Bid south_opening;  Bid west_opening;  Bid north_opening;  Bid east_opening;
		Bid correct_south_opening;  Bid correct_west_opening;  Bid correct_north_opening;  Bid correct_east_opening;
		vector< pair <Bid, Bid> > bid_checks({make_pair(north_opening, correct_north_opening), 
												 make_pair(east_opening, correct_east_opening),
												 make_pair(south_opening, correct_south_opening),
												 make_pair(west_opening, correct_north_opening)});
		bool passed = false;
		
		//hopefully can cut this down . . . 
		//ReadFile(file, line, total_num_hands, number_hands, north_opening, east_opening, south_opening, west_opening, correct_north_opening, correct_east_opening, correct_south_opening, correct_west_opening, opening_data, pub_vec, bidder, hands);
		
		ReadFile(file, line, total_num_hands, number_hands, bid_checks, opening_data, pub_vec, bidder, hands);
		
		//because these are copies inside of bid_checks I have to copy them back.
		north_opening = bid_checks[0].first;
		east_opening = bid_checks[1].first;
		correct_south_opening = bid_checks[2].second;
		west_opening = bid_checks[3].first;
		
		Sort_hand(hands[2]); //hard_code just sort south's hand.
		Get_hand_long_points(hands[2]); //hard_code just get long points for south's hand.
		Print_hand(hands[2]); //hard_code just print south's hand.

		cout<< endl << endl << " -------------------------- BIDDING HAND " << total_num_hands - number_hands - 1 << " -------------------------- " << endl;

		//////////////  Bidding ///////////////////////////////

		bidder = 0; //reset from file-readin.	

		cout<<"North's Opening Bid: ";
		Print_bid(north_opening);
		Interpret_bid(pub_vec, bidder);
		bids.push_back(north_opening);
		cout<<endl;
		bidder++; //now bidder should be 1, east
		bids.push_back(east_opening);

		// Bidding loop ///
		bidder++; // now bidder should be 2.
		/////  3rd seat /////////////
		//partner bid => Response or OvercallResponse.
	
		//if partner opened.
		if(Player_bid(pub_vec, bidder, PARTNER))
		{
				//RESPONSE OR PASS!
				if(!Player_bid(pub_vec, bidder, PREV))
				{
						Bid partner_bid = bids[bids.size()-PARTNER];
						Bid response;
					
						//Response to one suit opening. if partner_bid.suit = 0,1,2,3 && partner_bid.level == 1.
					
						int points = Get_points(hands[bidder], pub_vec, partner_bid.suit, bidder);
						//PASS! if fewer than 6 points (think about exceptions when there might be a very bad fit).
						if(points < 6)
						{
							Pass(response, pub_vec[bidder], partner_bid.level, make_pair(0,5));
						}
						//RESPONSE!
						else
						{
							std::priority_queue<Suit_Length, vector<Suit_Length>, CompSuits> suit_pq;
							Fill_suit_pq(suit_pq, hands[bidder], partner_bid);

							if(partner_bid.suit == HEARTS || partner_bid.suit == SPADES)
							{
								if(Check_support(hands[bidder], partner_bid.suit))
								{
									Support_major(hands[bidder], response, pub_vec[bidder], partner_bid, suit_pq);
								}
								else // partner opened major we don't have 3-card support for.
								{
									Unsuported_major_response(hands[bidder], pub_vec, response, bidder, partner_bid, suit_pq);
								}
							}
							else //either opener bid a minor, or opener bid a major and partner couldn't support it.
							{
								Minor_suit_response(hands[bidder], pub_vec, response, bidder, partner_bid, suit_pq);								
							}	
						} // END OF RESPONSE!
					
						cout<<"South's Response: ";
						Print_bid(response);
						Interpret_bid(pub_vec, bidder);
						bids.push_back(response);
						cout<<endl;
					
						//takes into account that passing can have different levels.
						if((response.level != correct_south_opening.level && (response.suit != -1 && correct_south_opening.suit == -1)) 
						       || response.suit != correct_south_opening.suit)
						{
							cerr << "ERROR: South's response is not correct!" << endl;
							assert(0);
						}
						else
						{
							cout << " * CONGRATULATIONS!  SOUTH MADE THE CORRECT RESPONSE! * " << endl;
						}
					
					
				} // end of RESPONSE OR PASS!

				// OVERCALL RESPONSE! (if 1st and 2nd hand both bid)
				else
				{
					cout << "south needs an overcall response, have not covered yet" << endl; // this is a problem!!
				}
		} // END OF PARTNER DID NOT PASS.
	
		else //IN THE CASE PARTNER PASSED!
		{
				//DIRECT OVERCALL (if previous opponent bid)
				if(pub_vec[1].passed != true) //again, should "if(pub_vec[(bidder-1)%4].passed != true)"
				{
						cout << "south needs direct overcall, have not covered yet" << endl;
				}
				//OPENING BID (from 3rd seat)
				else
				{
						Opening_Bid(hands[bidder], south_opening, pub_vec[bidder]); //south bids
						cout<<"South's Opening Bid: ";
						Print_bid(south_opening);
						Interpret_bid(pub_vec, bidder);
						bids.push_back(south_opening);
				}
		}
		
		///////////////////////////// END 3RD SEAT ////////////////////////////////////

		bidder++; // should now be 3.
		bids.push_back(west_opening);

		//////////////////////////////////////////////////////////////////
		cout<<"=========================================================================" << endl;
		cout<<"==========================  END OF HAND " << total_num_hands - number_hands - 1 << " ===============================" << endl;
		cout<<"=========================================================================" << endl << endl << endl;
	}
return 0;
}
