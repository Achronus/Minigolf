#include "..\Headers\BasicActors.h"

namespace PhysicsEngine
{
	//pyramid vertices
	static PxVec3 pyramid_verts[] = { PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1) };
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = { 1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1 };

	Pyramid::Pyramid(PxTransform pose, PxReal density) : ConvexMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), pose, density) {}

	PyramidStatic::PyramidStatic(PxTransform pose) : TriangleMesh(vector<PxVec3>(begin(pyramid_verts), end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs), end(pyramid_trigs)), pose) {};

	Plane::Plane(PxVec3 normal, PxReal distance) : StaticActor(PxTransformFromPlaneEquation(PxPlane(normal, distance)))
	{
		CreateShape(PxPlaneGeometry());
	}

	Sphere::Sphere(const PxTransform& pose, PxReal radius, PxReal density) : DynamicActor(pose)
	{
		CreateShape(PxSphereGeometry(radius), density);
	}

	Box::Box(const PxTransform& pose, PxVec3 dimensions, PxReal density) : DynamicActor(pose)
	{
		CreateShape(PxBoxGeometry(dimensions), density);
	}

	StaticBox::StaticBox(const PxTransform& pose, PxVec3 dimensions, PxReal density) : StaticActor(pose)
	{
		CreateShape(PxBoxGeometry(dimensions), density);
	}

	Capsule::Capsule(const PxTransform& pose, PxVec2 dimensions, PxReal density) : DynamicActor(pose)
	{
		CreateShape(PxCapsuleGeometry(dimensions.x, dimensions.y), density);
	}

	ConvexMesh::ConvexMesh(const std::vector<PxVec3>& verts, const PxTransform& pose, PxReal density)	: DynamicActor(pose)
	{
		PxConvexMeshDesc mesh_desc;
		mesh_desc.points.count = (PxU32)verts.size();
		mesh_desc.points.stride = sizeof(PxVec3);
		mesh_desc.points.data = &verts.front();
		mesh_desc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
		mesh_desc.vertexLimit = 256;

		CreateShape(PxConvexMeshGeometry(CookMesh(mesh_desc)), density);
	}

	PxConvexMesh* ConvexMesh::CookMesh(const PxConvexMeshDesc& mesh_desc)
	{
		PxDefaultMemoryOutputStream stream;

		if (!GetCooking()->cookConvexMesh(mesh_desc, stream))
			throw new Exception("ConvexMesh::CookMesh, cooking failed.");

		PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

		return GetPhysics()->createConvexMesh(input);
	}

	TriangleMesh::TriangleMesh(const std::vector<PxVec3>& verts, const std::vector<PxU32>& trigs, const PxTransform& pose) : StaticActor(pose)
	{
		PxTriangleMeshDesc mesh_desc;
		mesh_desc.points.count = (PxU32)verts.size();
		mesh_desc.points.stride = sizeof(PxVec3);
		mesh_desc.points.data = &verts.front();
		mesh_desc.triangles.count = (PxU32)trigs.size() / 3;
		mesh_desc.triangles.stride = 3 * sizeof(PxU32);
		mesh_desc.triangles.data = &trigs.front();

		CreateShape(PxTriangleMeshGeometry(CookMesh(mesh_desc)));
	}

	PxTriangleMesh* TriangleMesh::CookMesh(const PxTriangleMeshDesc& mesh_desc)
	{
		PxDefaultMemoryOutputStream stream;

		if (!GetCooking()->cookTriangleMesh(mesh_desc, stream))
			throw new Exception("TriangleMesh::CookMesh, cooking failed.");

		PxDefaultMemoryInputData input(stream.getData(), stream.getSize());

		return GetPhysics()->createTriangleMesh(input);
	}

	DistanceJoint::DistanceJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
	{
		PxRigidActor* px_actor0 = 0;
		if (actor0)
			px_actor0 = (PxRigidActor*)actor0->Get();

		joint = (PxJoint*)PxDistanceJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
		joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
		((PxDistanceJoint*)joint)->setDistanceJointFlag(PxDistanceJointFlag::eSPRING_ENABLED, true);
		Damping(1.f);
		Stiffness(1.f);
	}

	void DistanceJoint::Stiffness(PxReal value)
	{
		((PxDistanceJoint*)joint)->setStiffness(value);
	}

	PxReal DistanceJoint::Stiffness()
	{
		return ((PxDistanceJoint*)joint)->getStiffness();
	}

	void DistanceJoint::Damping(PxReal value)
	{
		((PxDistanceJoint*)joint)->setDamping(value);
	}

	PxReal DistanceJoint::Damping()
	{
		return ((PxDistanceJoint*)joint)->getDamping();
	}

	RevoluteJoint::RevoluteJoint(Actor* actor0, const PxTransform& localFrame0, Actor* actor1, const PxTransform& localFrame1)
	{
		PxRigidActor* px_actor0 = 0;
		if (actor0)
			px_actor0 = (PxRigidActor*)actor0->Get();

		joint = PxRevoluteJointCreate(*GetPhysics(), px_actor0, localFrame0, (PxRigidActor*)actor1->Get(), localFrame1);
		joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);
	}

	void RevoluteJoint::DriveVelocity(PxReal value)
	{
		//wake up the attached actors
		PxRigidDynamic* actor_0, * actor_1;
		((PxRevoluteJoint*)joint)->getActors((PxRigidActor*&)actor_0, (PxRigidActor*&)actor_1);
		if (actor_0)
		{
			if (actor_0->isSleeping())
				actor_0->wakeUp();
		}
		if (actor_1)
		{
			if (actor_1->isSleeping())
				actor_1->wakeUp();
		}
		((PxRevoluteJoint*)joint)->setDriveVelocity(value);
		((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
	}

	PxReal RevoluteJoint::DriveVelocity()
	{
		return ((PxRevoluteJoint*)joint)->getDriveVelocity();
	}

	void RevoluteJoint::SetLimits(PxReal lower, PxReal upper)
	{
		((PxRevoluteJoint*)joint)->setLimit(PxJointAngularLimitPair(lower, upper));
		((PxRevoluteJoint*)joint)->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	}

	Cloth::Cloth(PxTransform pose, const PxVec2& size, PxU32 width, PxU32 height, bool fix_top)
	{
		//prepare vertices
		PxReal w_step = size.x / width;
		PxReal h_step = size.y / height;

		PxClothParticle* vertices = new PxClothParticle[(width + 1) * (height + 1) * 4];
		PxU32* quads = new PxU32[width * height * 4];

		for (PxU32 j = 0; j < (height + 1); j++)
		{
			for (PxU32 i = 0; i < (width + 1); i++)
			{
				PxU32 offset = i + j * (width + 1);
				vertices[offset].pos = PxVec3(w_step * i, 0.f, h_step * j);
				if (fix_top && (j == 0)) //fix the top row of vertices
					vertices[offset].invWeight = 0.f;
				else
					vertices[offset].invWeight = 1.f;
			}

			for (PxU32 j = 0; j < height; j++)
			{
				for (PxU32 i = 0; i < width; i++)
				{
					PxU32 offset = (i + j * width) * 4;
					quads[offset + 0] = (i + 0) + (j + 0) * (width + 1);
					quads[offset + 1] = (i + 1) + (j + 0) * (width + 1);
					quads[offset + 2] = (i + 1) + (j + 1) * (width + 1);
					quads[offset + 3] = (i + 0) + (j + 1) * (width + 1);
				}
			}
		}

		//init cloth mesh description
		mesh_desc.points.data = vertices;
		mesh_desc.points.count = (width + 1) * (height + 1);
		mesh_desc.points.stride = sizeof(PxClothParticle);

		mesh_desc.invMasses.data = &vertices->invWeight;
		mesh_desc.invMasses.count = (width + 1) * (height + 1);
		mesh_desc.invMasses.stride = sizeof(PxClothParticle);

		mesh_desc.quads.data = quads;
		mesh_desc.quads.count = width * height;
		mesh_desc.quads.stride = sizeof(PxU32) * 4;

		//create cloth fabric (cooking)
		PxClothFabric* fabric = PxClothFabricCreate(*GetPhysics(), mesh_desc, PxVec3(0, -1, 0));

		//create cloth
		actor = (PxActor*)GetPhysics()->createCloth(pose, *fabric, vertices, PxClothFlags());
		//collisions with the scene objects
		((PxCloth*)actor)->setClothFlag(PxClothFlag::eSCENE_COLLISION, true);

		colours.push_back(default_colour);
		actor->userData = new UserData(&colours.back(), &mesh_desc);
	}

	Cloth::~Cloth()
	{
		delete (UserData*)actor->userData;
	}
}