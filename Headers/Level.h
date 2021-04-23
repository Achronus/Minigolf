#include "BasicActors.h"

namespace PhysicsEngine
{
	class StraightTrack
	{
		StaticActor* track;

	public:
		StaticActor* trackWalls;

		StraightTrack(vector<PxVec3> colours, PxVec3 size, PxVec3 pose, PxReal height = 1.f, PxReal density = 1.f);

		void AddToScene(Scene* scene);
	};

	class Flag
	{
	public:
		Cloth* flag;
		StaticBox* pole;

		Flag(vector<PxVec3> colours, PxVec3 pose);

		void AddToScene(Scene* scene);
	};

	class GolfClub
	{
	public:
		DynamicActor* club;

		GolfClub(vector<PxBoxGeometry> shapes, vector<PxVec3> positions, vector<PxReal> shapeDensities, PxVec3 colour, PxVec3 mainPose);

		void AddToScene(Scene* scene);

		void MoveActor(PxVec3 position);

		PxVec3 GetActorPosition();
	};

	class Spinner
	{
		DynamicActor* handles;
		DynamicActor* blade;
		RevoluteJoint* joint;

	public:
		Spinner(PxVec3 pose, PxReal handleXPose, PxVec2 handleSize, PxVec2 bladeSize, PxReal speed, PxReal thickness, vector<PxVec3> colours);

		void AddToScene(Scene* scene);
	};
}