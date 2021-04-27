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

	struct FilterGroup
	{
		enum Enum
		{
			eCLUB = (1 << 0),
			eBALL = (1 << 1),
			eRPIN = (1 << 2)
		};
	};

	void MyScene::SetVisualisation()
	{
		px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

		px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_HORIZONTAL, 1.0f);
		px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_VERTICAL, 1.0f);
		px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_BENDING, 1.0f);
		px_scene->setVisualizationParameter(PxVisualizationParameter::eCLOTH_SHEARING, 1.0f);
	}

	void MyScene::SetAngularVelocity()
	{
		angularVel = PxVec3(0.f);
	}

	PxVec3 MyScene::GetAngularVelocity()
	{
		return angularVel;
	}

	PxVec3 MyScene::GetStartPos()
	{
		return startPosition;
	}

	PxVec3 MyScene::GetBallPos()
	{
		return ballPosition;
	}

	int MyScene::GetStrokesTaken()
	{
		return strokesTaken;
	}

	float MyScene::GetHoleDistance()
	{
		return distanceToHole;
	}

	float MyScene::GetBallSpeed()
	{
		return ballSpeed;
	}

	bool MyScene::GetReady()
	{
		return ready;
	}

	void MyScene::SetFirstRun(bool value)
	{
		firstRun = value;
	}

	bool MyScene::GetLevelComplete()
	{
		return levelComplete;
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
			ForceStop(golfBall->Get());
		}
		else if (std::find(activeActorNames.begin(), activeActorNames.end(), "rollingPin") != activeActorNames.end())
		{
			ballPosition = rollingPin->GetActorPosition();
			angularVel = rollingPin->GetAngularVelocity();
			ballName = "rollingPin";
			ForceStop(rollingPin->Get());
		}
		activeActorNames.clear(); // reset names in vector

		// Check if ball is ready to move
		ready = ReadyCheck(angularVel, threshold);

		// Calculate ball distance to hole and ball speed
		distanceToHole = Distance(ballPosition, holePosition);
		ballSpeed = AverageVelocity(angularVel, threshold);

		// Increment hit count when: 
		// - Ball is moving 
		// - Count hasn't been increased
		// - Ball isn't being changed
		if (BallMovingCheck(angularVel) && notIncreased && !changingBall)
		{
			// Increment stroke and update bools
			strokesTaken++;
			notIncreased = false;
			clubPosUpdated = false;
			forceStop = true;
			RemoveAggregate(*activeClub);
		}
		else if (!BallMovingCheck(angularVel) && !changingBall)
		{
			notIncreased = true;
		}

		// Check for hole trigger
		levelComplete = my_callback->getInHole();

		// Update checkpoint position
		if (ready && !BallMovingCheck(angularVel) && !clubPosUpdated)
		{
			checkpointPosition = ballPosition;
			AddAggregate(*activeClub);
			club->MoveActor(PxVec3(clubPosition.x, clubPosition.y, checkpointPosition.z + 1.5f));
			clubPosUpdated = true;
		}
	}

	void MyScene::CustomInit()
	{
		strokesTaken = 0;
		levelComplete = false;

		SetVisualisation();

		//set materials
		GetMaterial()->setStaticFriction(.2f);

		///Initialise and set the customised event callback
		my_callback = new MySimulationEventCallback();
		px_scene->setSimulationEventCallback(my_callback);
		filter_shader = my_callback->CustomFilterShader;

		//create and add plane to scene
		plane = new Plane();
		plane->SetColour(PxVec3(209.f / 255.f, 198.f / 255.f, 177.f / 255.f));
		Add(plane);

		//create the level and ball types
		SetLevel();
		checkpointPosition = startPosition;

		//add default ball to scene on first run
		if (firstRun)
		{
			//set golf ball
			SetAggregateRollingPin();
			SetAggregateGolfBall();
			RemoveAggregate(*activeRollingPin);
			firstRun = false;
		}

		// Setup filtering
		club->SetupFiltering(FilterGroup::eCLUB, FilterGroup::eBALL | FilterGroup::eRPIN);
		golfBall->SetupFiltering(FilterGroup::eBALL, FilterGroup::eCLUB);
		rollingPin->SetupFiltering(FilterGroup::eRPIN, FilterGroup::eCLUB);
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

	void MyScene::CreateGolfClub(PxVec3 position)
	{
		vector<PxBoxGeometry> clubParts = { PxBoxGeometry(PxVec3(.5f, 4.5f, .2f)), PxBoxGeometry(PxVec3(.8f, 1.f, .5f)), PxBoxGeometry(2.5f, .5f, .5f) };
		vector<PxVec3> clubLocalPoses = { PxVec3(-4.7f, 0.f, 0.f), PxVec3(-4.7f, 3.f, 0.f), PxVec3(-2.8f, -4.25f, 0.f) };
		vector<PxReal> clubShapeDensities = { 1.f, 1.f, .8f };
		club = new GolfClub(clubParts, clubLocalPoses, clubShapeDensities, level_colours[4], PxTransform(position));
	}

	void MyScene::SetLevel()
	{
		// Set initial variables for level
		PxVec3 trackSize = PxVec3(10.f, 30.f, 80.f);
		PxVec3 trackPos = PxVec3(0.f, 3.f, -70.f);
		PxReal trackHeight = trackSize.y + trackPos.y;
		holePosition = PxVec3(trackPos.x, trackHeight + .1f, -abs(trackPos.z * 2));
		startPosition = PxVec3(trackPos.x, trackHeight + .7f, 0.f);
		PxVec3* holeLoc = &holePosition;

		vector<PxVec3> trackColours = { level_colours[5], level_colours[4] };
		vector<PxVec3> flagColours = { level_colours[4], level_colours[2] };
		vector<PxVec3> spinnerColours = { level_colours[4], level_colours[3] };

		PxVec3 flagPose = PxVec3(holeLoc->x, holeLoc->y + 13.f, holeLoc->z);

		PxReal spinnerSpeed = -1.f;
		PxVec3 spinnerPose1 = PxVec3(trackPos.x, trackHeight, -20.f);
		PxVec3 spinnerPose2 = PxVec3(spinnerPose1.x, spinnerPose1.y, spinnerPose1.z - 60.f);
		PxReal spinnerXSize = trackSize.x - .6f;
		PxVec3 spbSize = PxVec3(spinnerXSize, 3.f, .5f);
		PxVec3 sphLocalPose = PxVec3(spinnerXSize, 3.f, 0.f);

		// Set golf club
		// Club shape order - shaft, handle, bottom
		clubPosition = PxVec3(startPosition.x + 2.5f, trackHeight + 5.1f, startPosition.z + 2.f);
		SetAggregateGolfClub();

		// Tee box
		tee = new StaticBox(PxTransform(PxVec3(trackPos.x, trackHeight + .1f, 0.f)), PxVec3(1.f, .05f, 1.f));
		tee->SetColour(level_colours[4]);
		Add(tee);

		// Counter top
		track1Floor = new TrackFloor(trackColours[0], trackSize, PxTransform(trackPos));
		track1Walls = new TrackWalls(trackColours[1], trackSize, PxTransform(trackPos));
		Add(track1Floor);
		Add(track1Walls);

		// Hole and flag
		pole = new StaticBox(PxTransform(flagPose), PxVec3(.5f, 10.f, .5f));
		pole->SetColour(flagColours[0]);
		flag = new Flag(flagColours[1], PxTransform(flagPose.operator+(PxVec3(0.f, .6f, 0.f)), PxQuat(PxHalfPi, PxVec3(0, 0, 1))), PxVec2(8.f, 8.f), 40, 40);
		Add(pole);
		Add(flag);

		goalHole = new StaticBox(PxTransform(*holeLoc), PxVec3(2.5f, .15f, 2.5f));
		goalHole->SetColour(level_colours[4]);
		goalHole->SetTrigger(true);
		goalHole->Name("Goal");
		Add(goalHole);

		// Spinners
		spb1 = new SpinnerBlade(PxTransform(spinnerPose1), spbSize, spinnerColours[1]);
		sph1 = new SpinnerHandles(PxTransform(spinnerPose1), sphLocalPose, PxVec3(.5f, sphLocalPose.y, .5f), spinnerColours[0]);
		spb2 = new SpinnerBlade(PxTransform(spinnerPose2), spbSize, spinnerColours[1]);
		sph2 = new SpinnerHandles(PxTransform(spinnerPose2), sphLocalPose, PxVec3(.5f, sphLocalPose.y, .5f), spinnerColours[0]);

		spj1 = new RevoluteJoint(sph1, PxTransform(PxVec3(0.f, trackPos.y, 0.f)), spb1, PxTransform(PxVec3(0.f, 0.f, 0.f)));
		spj1->DriveVelocity(spinnerSpeed);
		spj2 = new RevoluteJoint(sph2, PxTransform(PxVec3(0.f, trackPos.y, 0.f)), spb2, PxTransform(PxVec3(0.f, 0.f, 0.f)));
		spj2->DriveVelocity(spinnerSpeed);

		Add(sph1); Add(spb1);	Add(sph2); Add(spb2);
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

	void MyScene::SetAggregateGolfClub()
	{
		activeClub = CreateAggregate(2);
		CreateGolfClub(clubPosition);
		activeClub->addActor(*club->Get());
		AddAggregate(*activeClub);
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

	void MyScene::ForceStop(PxActor* actor)
	{
		if (ready && forceStop)
		{
			((PxRigidDynamic*)actor)->putToSleep();
			forceStop = false;
		}
	}

	float MyScene::Distance(PxVec3 v1, PxVec3 v2) {
		return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}

	float MyScene::AverageVelocity(PxVec3 velocity, float threshold)
	{
		if (velocity.x == 0.f || velocity.y == 0.f || velocity.z == 0.f)
			return 0.f;
		else
			return abs(abs(velocity.x + velocity.y + velocity.z) - threshold);
	}
}