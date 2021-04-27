#include <iostream>
#include "..\Headers\VisualDebugger.h"

using namespace std;

/*
	The program created follows a simple minigolf setting set in a kitchen environment. The player can swing the golf club back and forth that collides into the golf ball, causing it to move. The application uses compound shapes to create aspects of the level stored within the respective 'level' files. Rotating obstacles are put in place to make the level slightly more challenging. Additionally, the golf ball and the track both have unique materials to ensure appropriate frictions and restitution. The hole has a flag above it that indicates its location. Underneath it, a 'hole box' uses a trigger to activate the game completion screen and pauses it, requiring the level to be restarted to play the game again. The HUD displays the number of strokes taken, the distance of the ball from the hole, and the balls speed (angular velocity). The golf club and the golf ball types are created using aggregates, providing the ability to swap the balls on a whim and hide the club after successfully hitting the ball.

	Furthermore, a checkpoint system is implemented, allowing different ball types to be loaded once the ball has stopped moving. The checkpoint system also assists in moving the golf club behind the ball at its stopped location. All controls are displayed on the help screen once the game is loaded. The user also can follow the ball while it is moving, using the respective toggle key.
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