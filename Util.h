#ifndef UTIL_H
#define UTIL_H

/* Util.h
 *
 * 
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

void Fill_suit_pq(std::priority_queue<Suit_Length, std::vector<Suit_Length>, CompSuits> &suit_pq, Hand &hand, Bid partner_bid);
void hand_data_Suit_intit(Hand_Public &hand_data, std::pair <int, int> points, int suit, int suit_length, Bid bid_name);
void hand_data_NT_intit(Hand_Public &hand_data, std::pair <int, int> points, Bid bid_name);
void hand_data_PASS_intit(Hand_Public &hand_data);
void Fill_opening_data(std::unordered_map<std::string, Hand_Public> &opening_data);

void ReadFile(std::ifstream &file, 
			  std::string &line, 
			  int &total_num_hands, 
			  int &number_hands, 
			  Bid &north_opening, 
			  Bid &east_opening, 
			  Bid &south_opening, 
			  Bid &west_opening, 
			  Bid &correct_north_opening, 
			  Bid &correct_east_opening, 
			  Bid &correct_south_opening, 
			  Bid &correct_west_opening, 
			  std::unordered_map<std::string, Hand_Public> &opening_data, 
			  std::vector<Hand_Public> &pub_vec, 
			  int &bidder, std::vector<Hand> 
			  &hands);



#endif
