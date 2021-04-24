#pragma once

#include "foundation/PxTransform.h"
#include "foundation/PxMat33.h"
#include <string>

namespace VisualDebugger
{
	using namespace physx;

	///Camera class
	class Camera
	{
	private:
		PxVec3 eye, eye_init;
		PxVec3 dir, dir_init;
		PxReal speed, speed_init;

	public:
		bool ballFollow = false;

		///constructor
		Camera(const PxVec3& _eye, const PxVec3& _dir, const PxReal _speed);

		///reset view
		void Reset();

		//follow ball
		void FollowBall(PxVec3& _e);

		//follow ball toggle
		void FollowBallToggle(bool value);

		//return ball follow value
		bool FollowBallToggle();

		///handle camera motion
		void Motion(int dx, int dy, PxReal delta_time);

		///handle analog move
		void AnalogMove(float x, float y);

		///get camera position
		PxVec3 getEye() const;

		///get camera direction
		PxVec3 getDir() const;

		///get camera transformation
		PxTransform	getTransform() const;

		///move camera forward
		void MoveForward(PxReal delta_time);

		///move camera backward
		void MoveBackward(PxReal delta_time);

		///move camera left
		void MoveLeft(PxReal delta_time);

		///move camera right
		void MoveRight(PxReal delta_time);

		///move camera up
		void MoveUp(PxReal delta_time);

		///move camera down
		void MoveDown(PxReal delta_time);
	};
}