#include "BasicActors.h"

namespace PhysicsEngine
{
	class TrackFloor : public StaticActor
	{
	public:
		TrackFloor(PxVec3 colour, PxVec3 size, PxTransform& pose, PxReal density = 1.f);
	};

	class TrackWalls : public StaticActor
	{
	public:
		TrackWalls(PxVec3 colour, PxVec3 size, PxTransform& pose, PxReal density = 1.f);
	};

	class Flag : public Cloth
	{
	public:
		Flag(PxVec3 colour, PxTransform& pose, PxVec2 size, PxU32 width, PxU32 height);
	};

	class GolfClub : public DynamicActor
	{
	public:
		GolfClub(vector<PxBoxGeometry> shapes, vector<PxVec3> positions, vector<PxReal> shapeDensities, PxVec3 colour, PxTransform& mainPose);
	};

	class SpinnerHandles : public DynamicActor
	{
	public:
		SpinnerHandles(PxTransform& pose, PxVec3 size, PxVec3 localPose, PxVec3 colour);
	};

	class SpinnerBlade : public DynamicActor
	{
	public:
		SpinnerBlade(PxTransform& pose, PxVec3 size, PxVec3 colour);
	};
}