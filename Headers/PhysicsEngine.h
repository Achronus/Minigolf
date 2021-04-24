#pragma once

#include <vector>
#include "PxPhysicsAPI.h"
#include "Exception.h"
#include "..\Extras\UserData.h"
#include <string>

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;
	
	///Initialise PhysX framework
	void PxInit();

	///Release PhysX resources
	void PxRelease();

	///Get the PxPhysics object
	PxPhysics* GetPhysics();

	///Get the cooking object
	PxCooking* GetCooking();

	///Get the specified material
	PxMaterial* GetMaterial(PxU32 index=0);

	///Create a new material
	PxMaterial* CreateMaterial(PxReal sf=.0f, PxReal df=.0f, PxReal cr=.0f);

	static const PxVec3 default_colour(.8f,.8f,.8f);

	///Abstract Actor class
	///Inherit from this class to create your own actors
	class Actor
	{
	protected:
		PxActor* actor;
		std::vector<PxVec3> colours;
		std::string name;

	public:
		///Constructor
		Actor()
			: actor(0)
		{
		}

		PxActor* Get();

		void SetColour(PxVec3 new_colour, PxU32 shape_index=-1);

		const PxVec3* GetColour(PxU32 shape_indx=0);

		void Material(PxMaterial* new_material, PxU32 shape_index=-1);

		PxShape* GetShape(PxU32 index=0);

		std::vector<PxShape*> Actor::GetShapes(PxU32 index=-1);

		virtual void CreateShape(const PxGeometry& geometry, PxReal density) {}

		void SetTrigger(bool value, PxU32 index = -1);

		void SetupFiltering(PxU32 filterGroup, PxU32 filterMask, PxU32 shape_index = -1);

		void Actor::Name(const string& name);

		string Actor::Name();
	};

	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const PxTransform& pose);

		~DynamicActor();

		void CreateShape(const PxGeometry& geometry, PxReal density);

		void SetKinematic(bool value, PxU32 index=-1);

		void SetMass(PxReal mass, PxVec3 massSpace);

		void SetDamping(PxReal damping);

		PxVec3 GetActorPosition();

		PxVec3 GetAngularVelocity();
	};

	class StaticActor : public Actor
	{
	public:
		StaticActor(const PxTransform& pose);

		~StaticActor();

		void CreateShape(const PxGeometry& geometry, PxReal density=0.f);
	};

	///Generic scene class
	class Scene
	{
	protected:
		//a PhysX scene object
		PxScene* px_scene;
		//pause simulation
		bool pause;
		//customize ball
		bool customize;
		//selected dynamic actor on the scene
		PxRigidDynamic* selected_actor;
		//original and modified colour of the selected actor
		std::vector<PxVec3> sactor_colour_orig;
		//custom filter shader
		PxSimulationFilterShader filter_shader;

		void HighlightOn(PxRigidDynamic* actor);

		void HighlightOff(PxRigidDynamic* actor);

	public:
		bool changingBall = false;
		PxVec3 linearVel;
		PxVec3 angularVel;
		string activeBall;

		Scene(PxSimulationFilterShader custom_filter_shader = PxDefaultSimulationFilterShader) : filter_shader(custom_filter_shader) {}

		///Init the scene
		void Init();

		///User defined initialisation
		virtual void CustomInit() {}

		///Perform a single simulation step
		void Update(PxReal dt);

		///User defined update step
		virtual void CustomUpdate() {}

		///Add actors
		void Add(Actor* actor);

		//Add an aggregate actor
		void AddAggregate(PxAggregate& actor);

		//Remove an aggregate actor
		void RemoveAggregate(PxAggregate& actor);

		//Create an aggregate actor
		PxAggregate* CreateAggregate(PxReal nbActors);

		///Get the PxScene object
		PxScene* Get();

		///Reset the scene
		void Reset();

		///Set pause
		void Pause(bool value);

		///Get pause
		bool Pause();

		///Set customize
		void Customize(bool value);

		///Get customize
		bool Customize();

		//Get change ball
		bool ChangingBall();

		//Check if ball is ready to move
		bool ReadyCheck(PxVec3& velocity, float threshold);

		//Check if ball is moving
		bool BallMovingCheck(PxVec3& velocity);

		///Get the selected dynamic actor on the scene
		PxRigidDynamic* GetSelectedActor();

		///Switch to the next dynamic actor
		void SelectNextActor();

		///a list with all actors
		std::vector<PxActor*> GetAllActors();
	};

	///Generic Joint class
	class Joint
	{
	protected:
		PxJoint* joint;

	public:
		Joint() : joint(0) {}

		PxJoint* Get() { return joint; }
	};
}
