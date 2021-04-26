#include "..\Headers\Level.h"

namespace PhysicsEngine
{
	TrackFloor::TrackFloor(PxVec3 colour, PxVec3 size, PxTransform& pose, PxReal density) : StaticActor(pose)
	{
		// Set material
		PxMaterial* floorMat = CreateMaterial(.7f, 1.9f, .01f);

		// Set straight track (floor)
		CreateShape(PxBoxGeometry(size.x, size.y, size.z), density);
		Material(floorMat);
		SetColour(colour);
	}

	TrackWalls::TrackWalls(PxVec3 colour, PxVec3 size, PxTransform& pose, PxReal density) : StaticActor(pose)
	{
		// Set materials
		PxMaterial* wallMat = CreateMaterial(.7f, 1.9f, .01f);
		wallMat->setRestitution(0.9f);

		// Set track walls
		PxQuat rotation = PxQuat(PxHalfPi, PxVec3(0, 1, 0));
		int walls = 4;

		// Create walls (front, left, back, right)
		for (int i = 0; i < walls/2; i++) {
			CreateShape(PxBoxGeometry(size.x / 10, size.y + 1, size.z / 8), density); // front, back
			CreateShape(PxBoxGeometry(size.x / 10, size.y + 1, size.z), density); // left, right
		}

		// Set wall positions
		GetShape(0)->setLocalPose(PxTransform(0.f, 0.f, size.z, rotation)); // front
		GetShape(1)->setLocalPose(PxTransform(-size.x, 0.f, 0.f)); // left
		GetShape(2)->setLocalPose(PxTransform(0.f, 0.f, -size.z, rotation)); // back
		GetShape(3)->setLocalPose(PxTransform(size.x, 0.f, 0.f)); // right
		
		// Set remaining properties
		Material(wallMat);
		SetColour(colour);
	}

	Flag::Flag(PxVec3 colour, PxTransform& pose, PxVec2 size, PxU32 width, PxU32 height) : Cloth(pose, size, width, height)
	{
		((PxCloth*)Get())->setExternalAcceleration(PxVec3(25.f, 0.f, 0.f));
		SetColour(colour);
		Name("Flag");
	}

	GolfClub::GolfClub(vector<PxBoxGeometry> shapes, vector<PxVec3> localPositions, vector<PxReal> shapeDensities, PxVec3 colour, PxTransform& mainPose) : DynamicActor(mainPose)
	{
		// Create shapes
		for (int i = 0; i < shapes.size(); i++)
			CreateShape(shapes[i], shapeDensities[i]);
		
		// Change local positions of created shapes
		for (int i = 0; i < localPositions.size(); i++)
			GetShape(i)->setLocalPose(PxTransform(localPositions[i]));

		// Set remaining aspects
		SetColour(colour);
		Get()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		SetMass(0.f, PxVec3(900.f, 0.f, 0.f));
		Name("GolfClub");
	}

	SpinnerHandles::SpinnerHandles(PxTransform& pose, PxVec3 localPose, PxVec3 size, PxVec3 colour) : DynamicActor(pose)
	{
		// Shape order - left, right
		CreateShape(PxBoxGeometry(size.x, size.y, size.z), 1.f);
		CreateShape(PxBoxGeometry(size.x, size.y, size.z), 1.f);		
		GetShape(0)->setLocalPose(PxTransform(-localPose.x, localPose.y, localPose.z));
		GetShape(1)->setLocalPose(PxTransform(localPose.x, localPose.y, localPose.z)); 
		SetKinematic(true);
		SetColour(colour);
	}

	SpinnerBlade::SpinnerBlade(PxTransform& pose, PxVec3 size, PxVec3 colour) : DynamicActor(pose)
	{
		PxMaterial* Mat = CreateMaterial(1.f, 1.f, .3f);

		CreateShape(PxBoxGeometry(size.x - .5f, size.y, size.z / 4), 1.f);
		SetColour(colour);
		Material(Mat);
	}
}