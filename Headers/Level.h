#include "BasicActors.h"

namespace PhysicsEngine
{
	class StraightTrack
	{
		StaticActor* track;
		PxMaterial* floorMat;
		PxMaterial* wallMat;

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
}