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
		PxVec3 trackPos;

		StraightTrack(vector<PxVec3> colours, PxVec3 size, PxVec3 position, PxReal height = 1.f, PxReal density = 1.f);

		void AddToScene(Scene* scene);
	};

	class Flag
	{
	public:
		Cloth* flag;
		StaticBox* pole;
		PxVec3 flagPos;

		Flag(vector<PxVec3> colours, PxVec3 position);

		void AddToScene(Scene* scene);
	};

	class GolfClub
	{
	public:
		DynamicActor* club;

		GolfClub(PxVec3 colour, PxVec3 position);

		void AddToScene(Scene* scene);

		void MoveActor(PxVec3 position);

		PxVec3 GetActorPosition();
	};
}