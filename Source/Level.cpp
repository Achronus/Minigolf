#include "..\Headers\Level.h"

namespace PhysicsEngine
{
	StraightTrack::StraightTrack(vector<PxVec3> colours, PxVec3 size, PxVec3 pose, PxReal height, PxReal density)
	{
		// Set materials
		PxMaterial* floorMat = CreateMaterial(.7f, 1.9f, .01f);
		PxMaterial* wallMat = CreateMaterial(.7f, 1.9f, .01f);
		wallMat->setRestitution(0.9f);

		// Set straight track (floor)
		track = new StaticActor(PxTransform(pose.x, height, pose.z));
		track->CreateShape(PxBoxGeometry(size.x, height, size.z), density);
		track->Material(floorMat);
		track->SetColour(colours[0]);

		// Set track walls
		trackWalls = new StaticActor(PxTransform(pose.x, height, pose.z));
		PxQuat rotation = PxQuat(PxHalfPi, PxVec3(0, 1, 0));
		int walls = 4;

		// Create walls (front, left, back, right)
		for (int i = 0; i < walls/2; i++) {
			trackWalls->CreateShape(PxBoxGeometry(size.x / 10, height + 1, size.z / 8), density); // front, back
			trackWalls->CreateShape(PxBoxGeometry(size.x / 10, height + 1, size.z), density); // left, right
		}

		// Set wall positions
		trackWalls->GetShape(0)->setLocalPose(PxTransform(0.f, 0.f, size.z, rotation)); // front
		trackWalls->GetShape(1)->setLocalPose(PxTransform(pose.x - size.x / 1, 0.f, 0.f)); // left
		trackWalls->GetShape(2)->setLocalPose(PxTransform(0.f, 0.f, -size.z, rotation)); // back
		trackWalls->GetShape(3)->setLocalPose(PxTransform(pose.x + size.x / 1, 0.f, 0.f)); // right
		
		// Set remaining properties
		trackWalls->Material(wallMat);
		trackWalls->SetColour(colours[1]);
	}

	void StraightTrack::AddToScene(Scene* scene)
	{
		scene->Add(track);
		scene->Add(trackWalls);
	}

	Flag::Flag(vector<PxVec3> colours, PxVec3 pose)
	{
		// Set flag pole
		pole = new StaticBox(PxTransform(PxVec3(pose.x, pose.y, pose.z)), PxVec3(.5f, 10.f, .5f));
		pole->SetColour(colours[0]);

		// Set flag cloth
		flag = new Cloth(PxTransform(PxVec3(pose.x + .5f, pose.y + .5f, pose.z + .5f), PxQuat(PxHalfPi, PxVec3(0, 0, 1))), PxVec2(8.f, 8.f), 40, 40);
		((PxCloth*)flag->Get())->setExternalAcceleration(PxVec3(25.f, 0.f, 0.f));
		flag->SetColour(colours[1]);
		flag->Name("Flag");
	}

	void Flag::AddToScene(Scene* scene)
	{
		scene->Add(pole);
		scene->Add(flag);
	}

	GolfClub::GolfClub(vector<PxBoxGeometry> shapes, vector<PxVec3> localPositions, vector<PxReal> shapeDensities, PxVec3 colour, PxVec3 mainPose)
	{
		club = new DynamicActor(PxTransform(mainPose));

		// Create shapes
		for (int i = 0; i < shapes.size(); i++)
			club->CreateShape(shapes[i], shapeDensities[i]);
		
		// Change local positions of created shapes
		for (int i = 0; i < localPositions.size(); i++)
			club->GetShape(i)->setLocalPose(PxTransform(localPositions[i]));

		// Set remaining aspects
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

	Spinner::Spinner(PxVec3 pose, PxReal handleXPose, PxVec2 handleSize, PxVec2 bladeSize, PxReal speed, PxReal thickness, vector<PxVec3> colours)
	{
		PxMaterial* Mat = CreateMaterial(1.f, 1.f, .3f);

		handles = new DynamicActor(PxTransform(pose.x, pose.y, pose.z));
		blade = new DynamicActor(PxTransform(pose.x, pose.y, pose.z));

		handles->CreateShape(PxBoxGeometry(handleSize.x, handleSize.y, thickness), 1.f);
		handles->CreateShape(PxBoxGeometry(handleSize.x, handleSize.y, thickness), 1.f);
		handles->GetShape(0)->setLocalPose(PxTransform(handleXPose, pose.y, 0.f)); // right
		handles->GetShape(1)->setLocalPose(PxTransform(-handleXPose, pose.y, 0.f)); // left
		handles->SetKinematic(true);
		handles->SetColour(colours[0]);

		blade->CreateShape(PxBoxGeometry(handleXPose - .5f, 2.5f, thickness/4), 1.f);
		blade->SetColour(colours[1]);
		blade->Material(Mat);

		joint = new RevoluteJoint(handles, PxTransform(PxVec3(0.f, pose.y, 0.f)), blade, PxTransform(PxVec3(0.f, 0.f, 0.f)));
		joint->DriveVelocity(speed);
	}

	void Spinner::AddToScene(Scene* scene)
	{
		scene->Add(handles);
		scene->Add(blade);
	}
}