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

	PxTransform MyScene::getActorPosition()
	{
		PxTransform actor = GetSelectedActor()->getGlobalPose();
		printf("actor_pos=%fx, %fy, %fz\n", actor.p.x, actor.p.y, actor.p.z);
		return actor;
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

	void MyScene::CustomUpdate()
	{
		// Set damping to relevant ball
		if (strcmp(GetSelectedActor()->getName(), "golfBall") == 0) {
			angularVel = GetSelectedActor()->getAngularVelocity();
			printAngularVelocity();
		}
	}

	void MyScene::CustomInit()
	{
		SetVisualisation();

		//set materials
		GetMaterial()->setStaticFriction(.2f);

		///Initialise and set the customised event callback
		my_callback = new MySimulationEventCallback();
		px_scene->setSimulationEventCallback(my_callback);

		//create and add plane to scene
		plane = new Plane();
		plane->SetColour(PxVec3(209.f / 255.f, 198.f / 255.f, 177.f / 255.f));
		plane->Name("OutOfBounds");
		Add(plane);

		//create the level
		SetLevel();

		// Set golf club
		club = new GolfClub(level_colours[4], PxVec3(2.5f, 7.2f, 2.f));
		club->AddToScene(this);

		//add default ball to scene on first run
		if (firstRun)
		{
			//set golf ball
			CreateBall(startPosition, ballMaterial, colour_palette[2], "golfBall", 0.5);
			firstRun = false;
		}
	}

	void MyScene::CreateBall(PxVec3 position, PxMaterial* material, PxVec3 colour, string name, PxReal damping)
	{
		Sphere* ball = new Sphere(PxTransform(position), .5f);
		ball->Material(material);
		ball->SetColour(colour);
		ball->Name(name);
		ball->SetDamping(damping);
		Add(ball);
	}

	void MyScene::CreateRollingPin(PxVec3 position, PxVec2 size, PxMaterial* material, PxVec3 colour, string name, PxReal damping)
	{
		rollingPin = new Capsule(PxTransform(position), size);
		rollingPin->Material(material);
		rollingPin->SetColour(colour);
		rollingPin->Name(name);
		rollingPin->SetDamping(damping);
		Add(rollingPin);
	}

	void MyScene::SetLevel()
	{
		PxVec3 trackSize = PxVec3(10.f, 3.f, 80.f);
		PxVec3 trackPos = PxVec3(0.f, 3.f, -70.f);
		PxVec3* holeLoc = &holeLocation;

		vector<PxVec3> trackColours = { level_colours[5], level_colours[4] };
		vector<PxVec3> flagColours = { level_colours[4], level_colours[2] };

		// Tee box
		tee = new StaticBox(PxTransform(PxVec3(0.f, 2.f, 0.f)), PxVec3(1.f, 0.25f, 1.f));
		tee->SetColour(level_colours[4]);
		Add(tee);

		// First straight
		track1 = new StraightTrack(trackColours, trackSize, trackPos);
		track1->AddToScene(this);

		frontWall = new StaticBox(PxTransform(PxVec3(0.f, 0.f, 10.f)), PxVec3(10.f, 3.2f, .8f));
		frontWall->SetColour(level_colours[4]);
		Add(frontWall);

		// Second straight
		track2 = new StraightTrack(trackColours, trackSize, PxVec3(trackPos.x, trackPos.y, -230.f));
		track2->AddToScene(this);

		trackEnd = new StaticBox(PxTransform(PxVec3(0.f, 0.f, -310.f)), PxVec3(10.f, 3.2f, .8f));
		trackEnd->SetColour(level_colours[4]);
		Add(trackEnd);

		// Hole and flag
		flag = new Flag(flagColours, PxVec3(holeLoc->x, holeLoc->y + 13.f, holeLoc->z));
		flag->AddToScene(this);

		goalHole = new StaticBox(PxTransform(*holeLoc), PxVec3(3.5f, .15f, 3.5f), 1.0);
		goalHole->SetColour(level_colours[4]);
		goalHole->SetTrigger(true);
		goalHole->Name("Goal");
		Add(goalHole);
	}

	void MyScene::UpdateBall(unsigned char key) {
		switch (key)
		{
		case 49:
			Reset();
			//set golf ball
			CreateBall(checkpointPosition, ballMaterial, colour_palette[2], "golfBall", 0.5);
			break;
		case 50:
			Reset();
			//set rolling pin
			CreateRollingPin(checkpointPosition, PxVec2(.3f, 2.f), ballMaterial, colour_palette[1], "golfBall", 0.5);
			break;
		}
	}

	float MyScene::Distance(PxVec3 v1, PxVec3 v2) {
		return sqrt(pow(v1.x * v2.x, 2) + pow(v1.y * v2.y, 2) + pow(v1.z * v2.z, 2));
	}

	void MyScene::LimitClubHeight()
	{

	}
}