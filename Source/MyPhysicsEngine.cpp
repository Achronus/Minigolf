#include "..\Headers\MyPhysicsEngine.h"

namespace PhysicsEngine
{
	using namespace std;

	//list of ball colours
	static const PxVec3 colour_palette[] = {
		PxVec3(255.f / 255.f, 255.f / 255.f, 0.f / 255.f), // yellow
		PxVec3(172.f / 255.f, 129.f / 255.f, 94.f / 255.f), // brown
		PxVec3(255.f / 255.f, 255.f / 255.f, 255.f / 255.f) // white
	};

	//list of level colours
	static PxVec3 level_colours[] = {
		PxVec3(76.f / 255.f, 62.f / 255.f, 49.f / 255.f), // dark-brown
		PxVec3(171 / 255.f, 136.f / 255.f, 95.f / 255.f), // light-brown
		PxVec3(17.f / 255.f, 27.f / 255.f, 122.f / 255.f), // dark-blue
		PxVec3(192.f / 255.f, 192.f / 255.f, 192.f / 255.f), // silver
		PxVec3(32.f / 255.f, 32.f / 255.f, 32.f / 255.f), // dark grey
		PxVec3(102.f / 255.f, 111.f / 255.f, 108.f / 255.f), // dark green
		PxVec3(239.f / 255.f, 241.f / 255.f, 240.f / 255.f) // light-silver
	};

	void MyScene::SetVisualisation()
	{
		px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	}

	PxVec3 MyScene::getActorPosition()
	{
		PxVec3 actor = ((PxRigidDynamic*)golfBall)->getGlobalPose().p;
		printf("actor_pos=%fx, %fy, %fz\n", actor.x, actor.y, actor.z);
		return actor;
	}

	void MyScene::SetAngularVelocity()
	{
		angularVel = PxVec3(0.f);
	}

	PxVec3 MyScene::GetAngularVelocity()
	{
		return angularVel;
	}

	void MyScene::printLinearVelocity()
	{
		printf("linear_velocity=%fx, %fy, %fz\n", linearVel.x, linearVel.y, linearVel.z);
	}

	void MyScene::printAngularVelocity()
	{
		printf("angular_velocity=%fx, %fy, %fz\n", angularVel.x, angularVel.y, angularVel.z);
	}

	void MyScene::printBallPosition(PxVec3 position)
	{
		printf("BallPosition: x=%f, y=%f, z=%f\n", position.x, position.y, position.z);
	}

	void MyScene::CustomUpdate()
	{
		// Check what ball is active
		activeActors = GetAllActors();
		for (int i = 0; i < activeActors.size(); i++)
		{
			string name = activeActors[i]->getName();
			if (!name.empty())
			{
				activeActorNames.push_back(name);
			}
		}

		// Set ball position to active ball type
		if (std::find(activeActorNames.begin(), activeActorNames.end(), "golfBall") != activeActorNames.end())
		{
			ballPosition = golfBall->GetActorPosition();
			angularVel = golfBall->GetAngularVelocity();
			ballName = "golfBall";
			//printAngularVelocity();
			//printBallPosition(ballPosition);
		}
		else if (std::find(activeActorNames.begin(), activeActorNames.end(), "rollingPin") != activeActorNames.end())
		{
			ballPosition = rollingPin->GetActorPosition();
			angularVel = rollingPin->GetAngularVelocity();
			ballName = "rollingPin";
			//printAngularVelocity();
			//printBallPosition(ballPosition);
		}
		activeActorNames.clear(); // reset names in vector

		// Check if ball is ready to move
		ready = ReadyCheck(angularVel);

		// Check ball distance to hole
		distanceToHole = Distance(ballPosition, holePosition);
		printf("strokesTaken=%d, distanceToHole=%f\n", strokesTaken, distanceToHole);

		// Increment hit count
		if (BallMovingCheck(angularVel) && notIncreased && !changingBall)
		{
			// Increment stroke and update bools
			strokesTaken++;
			notIncreased = false;
			clubPosUpdated = false;
		}
		else if (!BallMovingCheck(angularVel) && !changingBall)
		{
			notIncreased = true;
		}

		// Check for hole trigger
		levelComplete = my_callback->inHole;

		// Update checkpoint position
		if (ready && !BallMovingCheck(angularVel) && !clubPosUpdated)
		{
			checkpointPosition = ballPosition;
			club->MoveActor(PxVec3(clubPosition.x, clubPosition.y, checkpointPosition.z + 1.5));
			clubPosUpdated = true;
		}
	}

	void MyScene::CustomInit()
	{
		levelComplete = false;
		strokesTaken = 0;
		checkpointPosition = startPosition;

		SetVisualisation();

		//set materials
		GetMaterial()->setStaticFriction(.2f);

		///Initialise and set the customised event callback
		my_callback = new MySimulationEventCallback();
		px_scene->setSimulationEventCallback(my_callback);

		//create and add plane to scene
		plane = new Plane();
		plane->SetColour(PxVec3(209.f / 255.f, 198.f / 255.f, 177.f / 255.f));
		Add(plane);

		//create the level and ball types
		SetLevel();

		// Set golf club
		clubPosition = PxVec3(2.5f, 7.f, 2.f);
		club = new GolfClub(level_colours[4], clubPosition);
		club->AddToScene(this);

		//add default ball to scene on first run
		if (firstRun)
		{
			//set golf ball
			SetAggregateRollingPin();
			SetAggregateGolfBall();
			RemoveAggregate(*activeRollingPin);
			AddAggregate(*activeGolfBall);
			firstRun = false;
		}
	}

	void MyScene::CreateBall(PxVec3 position, PxMaterial* material, PxVec3 colour, string name, PxReal damping)
	{
	  golfBall = new Sphere(PxTransform(position), .5f);
		golfBall->Material(material);
		golfBall->SetColour(colour);
		golfBall->Name(name);
		golfBall->SetDamping(damping);
	}

	void MyScene::CreateRollingPin(PxVec3 position, PxVec2 size, PxMaterial* material, PxVec3 colour, string name, PxReal damping)
	{
		rollingPin = new Capsule(PxTransform(position), size);
		rollingPin->Material(material);
		rollingPin->SetColour(colour);
		rollingPin->Name(name);
		rollingPin->SetDamping(damping);
	}

	void MyScene::SetLevel()
	{
		PxVec3 trackSize = PxVec3(10.f, 3.f, 80.f);
		PxVec3 trackPos = PxVec3(0.f, 3.f, -70.f);
		PxVec3* holeLoc = &holePosition;

		vector<PxVec3> trackColours = { level_colours[5], level_colours[4] };
		vector<PxVec3> flagColours = { level_colours[4], level_colours[2] };

		// Tee box
		tee = new StaticBox(PxTransform(PxVec3(0.f, 2.f, 0.f)), PxVec3(1.f, .05f, 1.f));
		tee->SetColour(level_colours[4]);
		Add(tee);

		// First straight
		track1 = new StraightTrack(trackColours, trackSize, trackPos);
		track1->AddToScene(this);

		frontWall = new StaticBox(PxTransform(PxVec3(0.f, 0.f, 10.f)), PxVec3(10.f, 3.2f, .8f));
		frontWall->SetColour(level_colours[4]);
		Add(frontWall);

		trackEnd = new StaticBox(PxTransform(PxVec3(0.f, 0.f, -150.f)), PxVec3(10.f, 3.2f, .8f));
		trackEnd->SetColour(level_colours[4]);
		Add(trackEnd);

		// Hole and flag
		flag = new Flag(flagColours, PxVec3(holeLoc->x, holeLoc->y + 13.f, holeLoc->z));
		flag->AddToScene(this);

		goalHole = new StaticBox(PxTransform(*holeLoc), PxVec3(2.5f, .15f, 2.5f));
		goalHole->SetColour(level_colours[4]);
		goalHole->SetTrigger(true);
		goalHole->Name("Goal");
		Add(goalHole);
	}

	void MyScene::SetAggregateGolfBall()
	{
		activeGolfBall = CreateAggregate(1);
		CreateBall(checkpointPosition, ballMaterial, colour_palette[2], "golfBall", 0.5);
		activeGolfBall->addActor(*golfBall->Get());
		AddAggregate(*activeGolfBall);
		ballExists = true;
	}

	void MyScene::SetAggregateRollingPin()
	{
		activeRollingPin = CreateAggregate(1);
		CreateRollingPin(checkpointPosition, PxVec2(.5f, 2.f), ballMaterial, colour_palette[1], "rollingPin", 0.5);
		activeRollingPin->addActor(*rollingPin->Get());
		AddAggregate(*activeRollingPin);
		ballExists = true;
	}

	void MyScene::UpdateBall(unsigned char key) {
		switch (key)
		{
		case 49:
			//set golf ball
			if (ballExists) 
			{
				RemoveAggregate(*activeGolfBall);
				RemoveAggregate(*activeRollingPin);
				ballExists = false;
			}
			SetAggregateGolfBall();
			break;
		case 50:
			//set rolling pin
			if (ballExists)
			{
				RemoveAggregate(*activeRollingPin);
				RemoveAggregate(*activeGolfBall);
				ballExists = false;
			}
			SetAggregateRollingPin();
			break;
		}
	}

	float MyScene::Distance(PxVec3 v1, PxVec3 v2) {
		return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}
}