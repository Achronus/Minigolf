#include "..\Headers\CustomCollisions.h"

namespace PhysicsEngine
{
	void MySimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		//you can read the trigger information here
		for (PxU32 i = 0; i < count; i++)
		{
			//filter out contact with the planes
			if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE)
			{
				//check if eNOTIFY_TOUCH_FOUND trigger
				if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					// Custom trigger functionality
					customTriggers(pairs[i]);
					trigger = true;
				}
				//check if eNOTIFY_TOUCH_LOST trigger
				if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					trigger = false;
				}
			}
		}
	}

	bool MySimulationEventCallback::getInHole()
	{
		return inHole;
	}

	void MySimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
	{
		cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

		//check all pairs
		for (PxU32 i = 0; i < nbPairs; i++)
		{
			//check eNOTIFY_TOUCH_FOUND
			if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
			}
			//check eNOTIFY_TOUCH_LOST
			if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
			}
		}
	}

	void MySimulationEventCallback::customTriggers(PxTriggerPair pair)
	{
		// Check that actor is valid
		if (strcmp(pair.otherActor->getName(), "golfBall") == 0 || strcmp(pair.otherActor->getName(), "rollingPin") == 0)
		{
			// Check if goal has been reached
			if (strcmp(pair.triggerActor->getName(), "Goal") == 0)
			{
				cerr << "\nGoal has been reached! Congratulations!" << endl;
				inHole = true;
			}
		}
	}

	PxFilterFlags MySimulationEventCallback::CustomFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{

		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		//enable continous collision detection
		pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= PxPairFlag::eSOLVE_CONTACT;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			cout << filterData0.word0 << endl;

			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
		//return PxFilterFlag::eDEFAULT;
	}
}