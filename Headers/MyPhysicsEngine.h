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
		StaticBox *tee, *goalHole, *pole;
		TrackFloor* track1Floor;
		TrackWalls* track1Walls;
		Flag* flag;
		SpinnerHandles *sph1, *sph2;
		SpinnerBlade *spb1, *spb2;
		RevoluteJoint *spj1, *spj2;

		MySimulationEventCallback* my_callback;
		PxMaterial* ballMaterial = CreateMaterial(.2f, .4f, .1f); // sf, df, r

		string ballName;
		vector<PxActor*> activeActors;
		vector<string> activeActorNames;

		Sphere* golfBall;
		Capsule* rollingPin;
		GolfClub* club;
		PxAggregate *activeGolfBall, *activeRollingPin, *activeClub;
		bool ballExists = false;

		bool notIncreased = true; // Check for stroke counter increase
		PxVec3 clubPosition;
		bool clubPosUpdated = false;
		float threshold = 0.15f; // ready check
		
		PxVec3 holePosition;
		PxVec3 checkpointPosition;
		PxVec3 startPosition;
		PxVec3 ballPosition;

		int strokesTaken = 0;
		double distanceToHole = 0.f;
		double ballSpeed = 0.f;
		bool forceStop = true;
		bool ready = true;
		bool firstRun = true;
		bool levelComplete = false;

	public:
		//specify your custom filter shader here
		//PxDefaultSimulationFilterShader by default
		MyScene() : Scene() {};

		///A custom scene class
		void SetVisualisation();

		//Set the angular velocity
		void SetAngularVelocity();

		//Return angular velocity
		PxVec3 GetAngularVelocity();

		//Return start position
		PxVec3 GetStartPos();

		//Return ball position
		PxVec3 GetBallPos();

		//Return strokes taken
		int GetStrokesTaken();

		//Return distance to hole
		float GetHoleDistance();

		//Return ball speed
		float GetBallSpeed();

		//Return ready flag
		bool GetReady();

		//Set first run flag
		void SetFirstRun(bool value);

		//Return level complete flag
		bool GetLevelComplete();

		//Custom update function
		virtual void CustomUpdate();

		//Custom scene initialisation
		virtual void CustomInit();

		//Create ball types
		void CreateBall(PxVec3 position, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		void CreateRollingPin(PxVec3 position, PxVec2 size, PxMaterial* material, PxVec3 colour, string name, PxReal damping);

		//Create the golf club
		void CreateGolfClub(PxVec3 position);

		//create level
		void SetLevel();

		//create aggregate golf ball
		void SetAggregateGolfBall();

		//create aggregate rolling pin
		void SetAggregateRollingPin();

		//create aggregate golf club
		void SetAggregateGolfClub();

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
