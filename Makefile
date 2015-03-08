all:
	g++ -std=c++11 Long_suit_bid_test.cpp Utility.cpp -o Long_suit_bid_test
whole:
	g++ -std=c++11 Long_suit_bid_test.cpp Card.cpp Deck.cpp Hand.cpp Bid.cpp Util.cpp -o Long_suit_bid_test
run:
	g++ -std=c++11 Long_suit_bid_test.cpp Card.cpp Deck.cpp Hand.cpp Bid.cpp Util.cpp -o Long_suit_bid_test
	./Long_suit_bid_test
