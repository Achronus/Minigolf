#pragma once

#include "Level.h"
#include "CustomCollisions.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		StaticBox* tee, *frontWall, *trackEnd, *goalHole;
		StraightTrack* track1;
		StraightTrack* track2;
		StraightTrack* track3;
		Flag* flag;

		Sphere* golfBall;
		Capsule* rollingPin;

		MySimulationEventCallback* my_callback;
		PxMaterial* ballMaterial = CreateMaterial(.2f, .2f, .1f);
		float distanceToHole = 0.f;

	public:
		GolfClub* club;
		
		PxVec3 holeLocation = PxVec3(0.f, 2.f, -300.f);
		PxVec3 startPosition = PxVec3(0.f, 2.1f, 0.f);
		PxVec3 checkpointPosition = startPosition;
		int strokesTaken = 0;
		bool firstRun = true;

		//specify your custom filter shader here
		//PxDefaultSimulationFilterShader by default
		MyScene() : Scene() {};

		///A custom scene class
		void SetVisualisation();

		//Return actor position and print it to the console
		PxTransform getActorPosition();

		//Return angular velocity
		PxVec3 GetAngularVelocity();

		//Print linear velocity to console
		void printLinearVelocity();

		//Print angular velocity to console
		void printAngularVelocity();

		//Custom update function
		virtual void CustomUpdate();

		//Custom scene initialisation
		virtual void CustomInit();

		//Create ball types
		void CreateBall(PxVec3 position, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		void CreateRollingPin(PxVec3 position, PxVec2 size, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		//create level
		void SetLevel();

		//update ball
		virtual void UpdateBall(unsigned char key);

		//distance from one location to another
		float Distance(PxVec3 v1, PxVec3 v2);

		//limit club movement
		void LimitClubHeight();
	};
}
