#include "..\Headers\Level.h"

namespace PhysicsEngine
{
	StraightTrack::StraightTrack(vector<PxVec3> colours, PxVec3 size, PxVec3 position, PxReal height, PxReal density)
	{
		// Set materials
		floorMat = CreateMaterial(.7f, 1.9f, .01f);
		wallMat = CreateMaterial(.7f, 1.9f, .01f);
		wallMat->setRestitution(0.9f);

		trackPos = position;

		// Set straight track (floor)
		track = new StaticActor(PxTransform(trackPos.x, height, trackPos.z));
		track->CreateShape(PxBoxGeometry(size.x, height, size.z), density);
		track->Material(floorMat);
		track->SetColour(colours[0]);

		// Set track walls
		trackWalls = new StaticActor(PxTransform(trackPos.x, height, trackPos.z));
		trackWalls->CreateShape(PxBoxGeometry(size.x / 10, height, size.z), density);
		trackWalls->CreateShape(PxBoxGeometry(size.x / 10, height, size.z), density);
		trackWalls->GetShape(0)->setLocalPose(PxTransform(trackPos.x - size.x / 1, height, 0.f));
		trackWalls->GetShape(1)->setLocalPose(PxTransform(trackPos.x + size.x / 1, height, 0.f));
		trackWalls->Material(wallMat);
		trackWalls->SetColour(colours[1]);
	}

	void StraightTrack::AddToScene(Scene* scene)
	{
		scene->Add(track);
		scene->Add(trackWalls);
	}

	Flag::Flag(vector<PxVec3> colours, PxVec3 position)
	{
		flagPos = position;

		// Set flag pole
		pole = new StaticBox(PxTransform(PxVec3(flagPos.x, flagPos.y, flagPos.z)), PxVec3(.5f, 10.f, .5f));
		pole->SetColour(colours[0]);

		// Set flag cloth
		flag = new Cloth(PxTransform(PxVec3(flagPos.x + .5f, flagPos.y + .5f, flagPos.z + .5f), PxQuat(PxPi / 2, PxVec3(0.f, 0.f, 1.f))), PxVec2(8.f, 8.f), 40, 40);
		((PxCloth*)flag->Get())->setExternalAcceleration(PxVec3(25.f, 0.f, 0.f));
		flag->SetColour(colours[1]);
		flag->Name("Flag");
	}

	void Flag::AddToScene(Scene* scene)
	{
		scene->Add(pole);
		scene->Add(flag);
	}

	GolfClub::GolfClub(PxVec3 colour, PxVec3 position)
	{
		club = new DynamicActor(PxTransform(position));
		club->CreateShape(PxBoxGeometry(PxVec3(.5f, 4.5f, .2f)), 1.f); // top
		club->CreateShape(PxBoxGeometry(PxVec3(.8f, 1.f, .5f)), 1.f); // grip
		club->CreateShape(PxBoxGeometry(2.5f, .5f, .5f), .8f); // bottom
		club->GetShape(0)->setLocalPose(PxTransform(PxVec3(-4.7f, 0.f, 0.f)));
		club->GetShape(1)->setLocalPose(PxTransform(PxVec3(-4.7f, 3.f, 0.f)));
		club->GetShape(2)->setLocalPose(PxTransform(PxVec3(-2.8f, -4.25f, 0.f)));
		club->SetColour(colour);
		club->Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		club->SetMass(0.f, PxVec3(900.f, 0.f, 0.f));
		club->Name("GolfClub");
	}
	void GolfClub::AddToScene(Scene* scene)
	{
		scene->Add(club);
	}

	void GolfClub::MoveActor(PxVec3 position)
	{
		((PxRigidDynamic*)club->Get())->setGlobalPose(PxTransform(position));
		((PxRigidDynamic*)club->Get())->putToSleep();
		((PxRigidDynamic*)club->Get())->wakeUp();
	}

	PxVec3 GolfClub::GetActorPosition()
	{
		return ((PxRigidDynamic*)club->Get())->getCMassLocalPose().p;
	}
}