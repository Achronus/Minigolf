#include "BasicActors.h"

namespace PhysicsEngine
{
	///A customised collision class, implementing various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		bool trigger;
		bool inHole = false;

		MySimulationEventCallback() : trigger(false) {}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count);

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

		//Method called inside onTrigger that is used to handle custom triggers
		void customTriggers(PxTriggerPair pair);

		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
		virtual void onWake(PxActor** actors, PxU32 count) {}
		virtual void onSleep(PxActor** actors, PxU32 count) {}
#if PX_PHYSICS_VERSION >= 0x304000
		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
#endif

		//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
		static PxFilterFlags CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
			PxFilterObjectAttributes attributes1, PxFilterData filterData1,
			PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
	};
}