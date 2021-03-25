#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//list of ball colours
	static const PxVec3 color_palette[] = {
		PxVec3(255.f/255.f, 255.f/255.f, 0.f/255.f), // yellow
		PxVec3(172.f/255.f, 129.f/255.f, 94.f/255.f), // brown
		PxVec3(255.f/255.f, 255.f/255.f, 255.f/255.f) // white
	};

	//list of level colours
	static PxVec3 level_colors[] = {
		PxVec3(255.f/255.f, 255.f/255.f, 255.f/255.f), // white
		PxVec3(0.f/255.f, 0.f/255.f, 0.f/255.f), // black
		PxVec3(192.f/255.f, 192.f/255.f, 192.f/255.f), // silver
		PxVec3(82.f/255.f, 82.f/255.f, 82.f/255.f) // dark grey
	};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Sphere* golfBall;
		Capsule* rollingPin;
		Box* sponge;
		PxVec3 initialPosition = PxVec3(.0f, .7f, .0f);

	public:
		bool firstRun = true;
		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);
			GetMaterial()->setStaticFriction(10.f);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);

			//create the level
			SetLevel(level_colors);

			//add default ball to scene on first run
			if (firstRun)
			{
				//set golf ball
				golfBall = new Sphere(PxTransform(initialPosition), .5f);
				golfBall->Color(color_palette[2]);
				Add(golfBall);
				firstRun = false;
			}
		}

		//set floor tile
		void floorTile(PxVec3 color, PxVec3 position)
		{
			FloorBlock* tile = new FloorBlock(PxTransform(position), PxVec3(2.f, .1f, 2.f));
			tile->Color(color);
			Add(tile);
		}

		//create floor
		void Floor(PxVec3 mapColors[])
		{
			float x;
			float y = .1;
			float z = 16;
			PxVec3 white = mapColors[0];
			PxVec3 black = mapColors[1];
			PxVec3 temp;

			//starting floor
			for (int i = 0; i < 36; i++)
			{
				x = -40; //start value
				for (int j = 0; j < 10; j++)
				{
					//create multiple floor blocks
					floorTile(white, PxVec3(x, y, z)); //white tiles
					floorTile(black, PxVec3(x + 4, y, z)); //black tiles
					x += 8;
				}
				z -= 4;

				//alternate tile colors
				temp = black;
				black = white;
				white = temp;
			}
		}

		//create level
		void SetLevel(PxVec3 mapColors[])
		{
			Floor(mapColors);
		}

		//update ball
		virtual void UpdateBall(unsigned char key)
		{
			switch (key)
			{
				case 49:
						Reset();
						//set golf ball
						golfBall = new Sphere(PxTransform(initialPosition), .5f);
						golfBall->Color(color_palette[2]);
						Add(golfBall);
						SelectNextActor();
					break;
				case 50:
						Reset();
						//set sponge
						sponge = new Box(PxTransform(initialPosition), PxVec3(.8f, 1.f, .2f));
						sponge->Color(color_palette[0]);
						Add(sponge);
						SelectNextActor();
					break;
				case 51:
						Reset();
						//set rolling pin
						rollingPin = new Capsule(PxTransform(initialPosition), PxVec2(.3f, 2.f));
						rollingPin->Color(color_palette[1]);
						Add(rollingPin);
						SelectNextActor();
					break;
			}
		}
	};
}
