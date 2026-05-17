#include "App.h"
#include "config.h"

int main()
{
	Config config{};
	App app{ config };
	app.run();
	return 0;
}
