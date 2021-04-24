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
		StaticBox *tee, *goalHole;
		StraightTrack* track1;
		Flag* flag;
		Spinner *spinner1, *spinner2;

		MySimulationEventCallback* my_callback;
		PxMaterial* ballMaterial = CreateMaterial(.2f, .4f, .1f); // sf, df, r

		string ballName;
		vector<PxActor*> activeActors;
		vector<string> activeActorNames;

		Sphere* golfBall;
		Capsule* rollingPin;
		PxAggregate* activeGolfBall;
		PxAggregate* activeRollingPin;
		bool ballExists = false;

		bool notIncreased = true; // Check for stroke counter increase
		PxVec3 clubPosition;
		bool clubPosUpdated = false;
		float threshold = 0.15f; // ready check

	public:
		GolfClub* club;
		
		PxVec3 holePosition = PxVec3(0.f, 60.1f, -140.f);
		PxVec3 startPosition = PxVec3(0.f, 60.7f, 0.f);
		PxVec3 ballPosition;
		PxVec3 checkpointPosition;

		int strokesTaken = 0;
		double distanceToHole = 0.f;
		double ballSpeed = 0.f;
		bool ready = true;
		bool firstRun = true;
		bool levelComplete = false;
		bool forceStop = true;

		//specify your custom filter shader here
		//PxDefaultSimulationFilterShader by default
		MyScene() : Scene() {};

		///A custom scene class
		void SetVisualisation();

		//Return actor position and print it to the console
		PxVec3 getActorPosition();

		//Set the angular velocity
		void SetAngularVelocity();

		//Return angular velocity
		PxVec3 GetAngularVelocity();

		//Print linear velocity to console
		void printLinearVelocity();

		//Print angular velocity to console
		void printAngularVelocity();

		//Print ball position to console
		void printBallPosition(PxVec3 position);

		//Custom update function
		virtual void CustomUpdate();

		//Custom scene initialisation
		virtual void CustomInit();

		//Create ball types
		void CreateBall(PxVec3 position, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		void CreateRollingPin(PxVec3 position, PxVec2 size, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		//create level
		void SetLevel();

		//create aggregate golf ball
		void SetAggregateGolfBall();

		//create aggregate rolling pin
		void SetAggregateRollingPin();

		//update ball
		virtual void UpdateBall(unsigned char key);

		//forcefully stop ball from moving
		void ForceStop(PxActor* actor);

		//distance from one point to another
		float Distance(PxVec3 v1, PxVec3 v2);

		//calculate balls average velocity
		float AverageVelocity(PxVec3 velocity, float threshold);
	};
}
