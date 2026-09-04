#include "board.hpp"
#include "movegen.hpp"
#include "search.hpp"
#include "uci.hpp"
#include <iostream>
#include <stop_token>

int main()
{
	Uci::loop();
	return 0;
}
