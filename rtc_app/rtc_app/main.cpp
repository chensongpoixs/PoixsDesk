#include <iostream>
#include "client.h"

int main(int argc, char* argv[])
{
	 
	 s_rtc_client.init(0);
	 s_rtc_client.Loop();
	return 0;
}