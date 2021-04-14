#include "..\Headers\PhysicsEngine.h"
#include <iostream>

namespace PhysicsEngine
{
	using namespace physx;
	using namespace std;

	//default error and allocator callbacks
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	//PhysX objects
	PxFoundation* foundation = 0;
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
	debugger::comm::PvdConnection* pvd = 0;
#else
	PxPvd*  pvd = 0;
#endif
	PxPhysics* physics = 0;
	PxCooking* cooking = 0;

	///PhysX functions
	void PxInit()
	{
		//foundation
		if (!foundation) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#else
			foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
#endif
		}

		if (!foundation)
			throw new Exception("PhysicsEngine::PxInit, Could not create the PhysX SDK foundation.");

		//visual debugger
		if (!pvd) {
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			pvd = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), "localhost", 5425, 100,
				PxVisualDebuggerExt::getAllConnectionFlags());
#else
			pvd = PxCreatePvd(*foundation);
			PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
			pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
		}

		//physics
		if (!physics)
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
#else
			physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
#endif

		if (!physics)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the PhysX SDK.");

		if (!cooking)
			cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));

		if(!cooking)
			throw new Exception("PhysicsEngine::PxInit, Could not initialise the cooking component.");

		//create a deafult material
		CreateMaterial();
	}

	void PxRelease()
	{
		if (cooking)
			cooking->release();
		if (physics)
			physics->release();
		if (pvd)
			pvd->release();
		if (foundation)
			foundation->release();
	}

	PxPhysics* GetPhysics() 
	{ 
		return physics; 
	}

	PxCooking* GetCooking()
	{
		return cooking;
	}

	PxMaterial* GetMaterial(PxU32 index)
	{
		std::vector<PxMaterial*> materials(physics->getNbMaterials());
		if (index < physics->getMaterials((PxMaterial**)&materials.front(), (PxU32)materials.size()))
			return materials[index];
		else
			return 0;
	}

	PxMaterial* CreateMaterial(PxReal sf, PxReal df, PxReal cr) 
	{
		return physics->createMaterial(sf, df, cr);
	}

	///Actor methods

	PxActor* Actor::Get()
	{
		return actor;
	}

	void Actor::SetColour(PxVec3 new_colour, PxU32 shape_index)
	{
		//change colour of all shapes
		if (shape_index == -1)
		{
			for (unsigned int i = 0; i < colours.size(); i++)
				colours[i] = new_colour;
		}
		//or only the selected one
		else if (shape_index < colours.size())
		{
			colours[shape_index] = new_colour;
		}
	}

	const PxVec3* Actor::GetColour(PxU32 shape_indx)
	{
		if (shape_indx < colours.size())
			return &colours[shape_indx];
		else 
			return 0;			
	}

	void Actor::Material(PxMaterial* new_material, PxU32 shape_index)
	{
		std::vector<PxShape*> shape_list = GetShapes(shape_index);
		for (PxU32 i = 0; i < shape_list.size(); i++)
		{
			std::vector<PxMaterial*> materials(shape_list[i]->getNbMaterials());
			for (unsigned int j = 0; j < materials.size(); j++)
				materials[j] = new_material;
			shape_list[i]->setMaterials(materials.data(), (PxU16)materials.size());
		}
	}

	PxShape* Actor::GetShape(PxU32 index)
	{
		std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
		if (index < ((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size()))
			return shapes[index];
		else
			return 0;
	}

	std::vector<PxShape*> Actor::GetShapes(PxU32 index)
	{
		std::vector<PxShape*> shapes(((PxRigidActor*)actor)->getNbShapes());
		((PxRigidActor*)actor)->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());
		if (index == -1)
			return shapes;
		else if (index < shapes.size())
		{
			return std::vector<PxShape*>(1, shapes[index]);
		}
		else
			return std::vector<PxShape*>();
	}

	void Actor::SetTrigger(bool value, PxU32 shape_index)
	{
		std::vector<PxShape*> shape_list = GetShapes(shape_index);
		for (PxU32 i = 0; i < shape_list.size(); i++)
		{
			shape_list[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !value);
			shape_list[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, value);
		}
	}

	void Actor::SetupFiltering(PxU32 filterGroup, PxU32 filterMask, PxU32 shape_index)
	{
		std::vector<PxShape*> shape_list = GetShapes(shape_index);
		for (PxU32 i = 0; i < shape_list.size(); i++)
			shape_list[i]->setSimulationFilterData(PxFilterData(filterGroup, filterMask, 0, 0));

		// PxFilterData(word0, word1, 0, 0)
		// word0 = own ID
		// word1 = ID mask to filter pairs that trigger a contact callback
	}

	void Actor::Name(const string& new_name)
	{
		name = new_name;
		actor->setName(name.c_str());
	}

	string Actor::Name()
	{
		return name;
	}

	DynamicActor::DynamicActor(const PxTransform& pose) : Actor()
	{
		actor = (PxActor*)GetPhysics()->createRigidDynamic(pose);
		Name(name);
	}

	DynamicActor::~DynamicActor()
	{
		for (unsigned int i = 0; i < colours.size(); i++)
			delete (UserData*)GetShape(i)->userData;
	}

	void DynamicActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidDynamic*)actor)->createShape(geometry,*GetMaterial());
		PxRigidBodyExt::updateMassAndInertia(*(PxRigidDynamic*)actor, density);
		colours.push_back(default_colour);
		//pass the colour pointers to the renderer
		shape->userData = new UserData();
		for (unsigned int i = 0; i < colours.size(); i++)
			((UserData*)GetShape(i)->userData)->colour = &colours[i];
	}

	void DynamicActor::SetKinematic(bool value, PxU32 index)
	{
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
		((PxRigidDynamic*)actor)->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, value);
#else
		((PxRigidDynamic*)actor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, value);
#endif
	}

	void DynamicActor::SetMass(PxReal mass, PxVec3 massSpace)
	{
		((PxRigidDynamic*)actor)->setMass(mass);
		((PxRigidDynamic*)actor)->setMassSpaceInertiaTensor(massSpace);
	}

	void DynamicActor::SetDamping(PxReal damping)
	{
		((PxRigidDynamic*)actor)->setAngularDamping(damping);
	}

	StaticActor::StaticActor(const PxTransform& pose)
	{
		actor = (PxActor*)GetPhysics()->createRigidStatic(pose);
		Name("");
	}

	StaticActor::~StaticActor()
	{
		for (unsigned int i = 0; i < colours.size(); i++)
			delete (UserData*)GetShape(i)->userData;
	}

	void StaticActor::CreateShape(const PxGeometry& geometry, PxReal density)
	{
		PxShape* shape = ((PxRigidStatic*)actor)->createShape(geometry,*GetMaterial());
		colours.push_back(default_colour);
		//pass the colour pointers to the renderer
		shape->userData = new UserData();
		for (unsigned int i = 0; i < colours.size(); i++)
			((UserData*)GetShape(i)->userData)->colour = &colours[i];
	}

	///Scene methods
	void Scene::Init()
	{
		//scene
		PxSceneDesc sceneDesc(GetPhysics()->getTolerancesScale());

		if(!sceneDesc.cpuDispatcher)
		{
			PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			sceneDesc.cpuDispatcher = mCpuDispatcher;
		}

		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		px_scene = GetPhysics()->createScene(sceneDesc);

		if (!px_scene)
			throw new Exception("PhysicsEngine::Scene::Init, Could not initialise the scene.");

		//default gravity
		px_scene->setGravity(PxVec3(0.0f, -9.81f, 0.0f));

		CustomInit();

		pause = false;
		customize = false;

		selected_actor = 0;

		SelectNextActor();
	}

	void Scene::Update(PxReal dt)
	{
		if (pause)
			return;

		CustomUpdate();

		px_scene->simulate(dt);
		px_scene->fetchResults(true);
	}

	void Scene::Add(Actor* actor)
	{
		px_scene->addActor(*actor->Get());
	}

	PxScene* Scene::Get() 
	{ 
		return px_scene; 
	}

	void Scene::Reset()
	{
		px_scene->release();
		Init();
	}

	void Scene::Pause(bool value)
	{
		pause = value;
	}

	bool Scene::Pause()
	{
		return pause;
	}

	void Scene::Customize(bool value)
	{
		customize = value;
	}

	bool Scene::Customize()
	{
		return customize;
	}

	bool Scene::ReadyCheck(PxVec3& velocity)
	{
		if (velocity.magnitude() <= 0.1f) {
			return true;
		}
		else
			return false;
	}

	void Scene::AddForce(PxVec3 &force)
	{
		GetSelectedActor()->addForce(force);
	}

	void Scene::AddTorque(PxVec3& torque)
	{
		GetSelectedActor()->addTorque(torque);
	}

	PxRigidDynamic* Scene::GetSelectedActor()
	{
		return selected_actor;
	}

	void Scene::SelectNextActor()
	{
#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC));
		if (actors.size() && (px_scene->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC, (PxActor**)&actors.front(), (PxU32)actors.size())))
#else
		std::vector<PxRigidDynamic*> actors(px_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
		if (actors.size() && (px_scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC, (PxActor**)&actors.front(), (PxU32)actors.size())))
#endif
		{
			if (selected_actor)
			{
				for (unsigned int i = 0; i < actors.size(); i++)
					if (selected_actor == actors[i])
					{
						//select the next actor
						selected_actor = actors[(i + 1) % actors.size()];
						break;
					}
			}
			else
			{
				selected_actor = actors[0];
			}
		}
		else
			selected_actor = 0;
	}

	std::vector<PxActor*> Scene::GetAllActors()
	{
	#if PX_PHYSICS_VERSION < 0x304000 // SDK 3.3
			physx::PxActorTypeSelectionFlags selection_flag = PxActorTypeSelectionFlag::eRIGID_DYNAMIC | PxActorTypeSelectionFlag::eRIGID_STATIC |
				PxActorTypeSelectionFlag::eCLOTH;
	#else
			physx::PxActorTypeFlags selection_flag = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC |
				PxActorTypeFlag::eCLOTH;
	#endif
			std::vector<PxActor*> actors(px_scene->getNbActors(selection_flag));
			px_scene->getActors(selection_flag, (PxActor**)&actors.front(), (PxU32)actors.size());
			return actors;
	}

	void Scene::HighlightOn(PxRigidDynamic* actor)
	{
		//store the original colour and adjust brightness of the selected actor
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());

		sactor_colour_orig.clear();

		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			PxVec3* colour = ((UserData*)shapes[i]->userData)->colour;
			sactor_colour_orig.push_back(*colour);
			*colour += PxVec3(.2f,.2f,.2f);
		}
	}

	void Scene::HighlightOff(PxRigidDynamic* actor)
	{
		//restore the original colour
		std::vector<PxShape*> shapes(actor->getNbShapes());
		actor->getShapes((PxShape**)&shapes.front(), (PxU32)shapes.size());

		for (unsigned int i = 0; i < shapes.size(); i++)
			*((UserData*)shapes[i]->userData)->colour = sactor_colour_orig[i];
	}
}
