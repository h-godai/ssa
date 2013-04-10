
#pragma once

#include <cmath>
#include <iostream>
#include "co_mouse.hpp"
#include "co_vec3.hpp"
#include "co_quat.hpp"

namespace ngs {

class Camera {
	float angle_;
	float aspect_;
	float nearZ_, farZ_;
	Vec3<float> at_;
	Vec3<float> pos_;
	Quat<float> rot_;
	float dist_;
	float width_, height_;
	int mode_;

	GLdouble proj_[16];
	GLint view_[4];

	Matrix rotMtx_;
	
	void settings()
	{
		height_ = nearZ_ * tan(angle_ / 2.0);
		width_ = height_ * aspect_;
	}

	Camera() {}
public:
	enum {
		PERSPECTIVE,
		ORTHOGONAL,
	};

	Camera(int mode) : angle_(30.0 * PI / 180.0), aspect_(1.0), nearZ_(1.0), farZ_(10.0), dist_(1.0),
										 width_(640.0 / 2.0), height_(320.0 / 2.0),
										 mode_(mode) {}
	~Camera() {}

	void setAngle(const float angle)
	{
		angle_ = angle;
		settings();
	}
	float getAngle() const
	{
		return angle_;
	}

	void setAspect(const float aspect)
	{
		aspect_ = aspect;
		settings();
	}
	float getAspect() const
	{
		return aspect_;
	}

	void setSize(const float width, const float height)
	{
		width_ = width;
		height_ = height;
	}
	Vec2<float> getSize() const
	{
		return Vec2<float>(width_ * 2.0, height_ * 2.0);
	}

	void setNearZ(const float z)
	{
		nearZ_ = z;
		settings();
	}
	float getNearZ() const
	{
		return nearZ_;
	}
	
	void scale(const float sx, const float sy)
	{
		float w = width_ * sx;
		float h = height_ * sy;

		aspect_ = w / h;
		angle_ = atan(h / nearZ_) * 2.0;
		settings();
	}

	void setFarZ(const float z)
	{
		farZ_ = z;
	}
	float getFarZ() const
	{
		return farZ_;
	}

	void setPos(const Vec3<float>& pos)
	{
		pos_ = pos;
	}
	Vec3<float> getPos() const
	{
		return pos_;
	}

	void setRot(const Quat<float>& rot)
	{
		rot_ = rot;
	}
	Quat<float> getRot() const
	{
		return rot_;
	}

	void setDist(const float dist)
	{
		dist_ = dist;
	}
	float getDist() const
	{
		return dist_;
	}

	void setMode(const int mode)
	{
		mode_ = mode;
	}
	int getMode() const
	{
		return mode_;
	}

	const Matrix& getMatrix() const
	{
		return rotMtx_;
	}
	
	void setup()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		switch(mode_)
		{
		case PERSPECTIVE:
			{
				rotMtx_.rotate(rot_);
				glFrustum(width_, -width_, -height_, height_, nearZ_, farZ_);
				glTranslated(0.0, 0.0, -dist_);
				glMultMatrixf(rotMtx_.value());

				rotMtx_.reverse();
			}
			break;

			case ORTHOGONAL:
			{
				glOrtho(-width_ / 2.0, width_ / 2.0, height_ / 2.0, -height_ / 2.0, -1.0, 1.0);
			}
			break;
		}

		glGetDoublev(GL_PROJECTION_MATRIX, proj_);
		glGetIntegerv(GL_VIEWPORT, view_);
	}

	Vec3<float> posToWorld(const Vec2<float>& pos, const float z, const GLdouble model[])
	{
		GLdouble ox, oy, oz;
#if !(TARGET_OS_IPHONE)
		gluUnProject(pos.x, pos.y, z, model, proj_, view_, &ox, &oy, &oz);
		// TODO:iPhoneでの実装

		 // world(ox, oy, oz);
#endif
		 return Vec3<float>(ox, oy, oz);
	}
};

}
