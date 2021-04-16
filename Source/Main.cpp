#include <iostream>
#include "..\Headers\VisualDebugger.h"

using namespace std;

/*
	The program created follows a simple minigolf setting set in a kitchen environment. The player can rotate the golf club that collides into the golf ball, causing it to move. The application uses compound shapes to create aspects of the level stored within the respective 'level' files. The golf ball and the track both have unique materials to ensure appropriate frictions and restitution. The hole has a flag above it that indicates its location. Underneath it, a 'hole box' uses a trigger to activate the game completion screen and pauses it, requiring the level to be restarted to play the game again. The console provides feedback for the 'scoring mechanism', where it displays the number of strokes taken and outputs the ball's distance from the hole when moving.  

	Furthermore, a checkpoint system is implemented, allowing different ball types to be loaded once the ball has stopped moving. All controls are displayed on the help screen once the game is loaded.
*/
int main()
{
	try 
	{ 
		VisualDebugger::Init("A1 - Mini Golf", 800, 800); 
	}
	catch (Exception exc) 
	{ 
		cerr << exc.what() << endl;
		return 0; 
	}

	VisualDebugger::Start();

	return 0;
}