
#pragma once

#include <iostream>
#include "co_vec3.hpp"

namespace ngs {

class Light {
	GLenum hdl_;
	GLfloat scale_;
	GLfloat amb_[4];
	GLfloat diff_[4];
	GLfloat spe_[4];
	GLfloat pos_[4];

public:
	Light() : hdl_(GL_LIGHT0), scale_(1.0) {
		amb_[0] = amb_[1] = amb_[2] = 0.0;
		amb_[3] = 1.0;

		diff_[0] = diff_[1] = diff_[2] = 0.0;
		diff_[3] = 1.0;

		spe_[0] = spe_[1] = spe_[2] = 0.0;
		spe_[3] = 1.0;

		pos_[0] = pos_[1] = pos_[2] = pos_[3] = 0.0;
	}
	~Light() {}

	void handle(const GLenum hdl)
	{
		hdl_ = hdl;
	}
	void scale(const float scale)
	{
		scale_ = scale;
	}
	
	void ambientCol(const float red, const float green, const float blue)
	{
		amb_[0] = red;
		amb_[1] = green;
		amb_[2] = blue;
	}
	
	void diffuseCol(const float red, const float green, const float blue)
	{
		diff_[0] = red;
		diff_[1] = green;
		diff_[2] = blue;
	}
	
	void specularCol(const float red, const float green, const float blue)
	{
		spe_[0] = red;
		spe_[1] = green;
		spe_[2] = blue;
	}

	void pos(const Vec3<float>& pos)
	{
		pos_[0] = pos.x;
		pos_[1] = pos.y;
		pos_[2] = pos.z;
	}

	
	void setup()
	{
		glEnable(GL_LIGHTING);
		glEnable(hdl_);

		GLfloat value[4];
		for(int i = 0; i < 3; ++i)
		{
			value[i] = amb_[i] * scale_;
		}
		glLightfv(hdl_, GL_AMBIENT, value);

		for(int i = 0; i < 3; ++i)
		{
			value[i] = diff_[i] * scale_;
		}
		glLightfv(hdl_, GL_DIFFUSE, value);

		for(int i = 0; i < 3; ++i)
		{
			value[i] = spe_[i] * scale_;
		}
		glLightfv(hdl_, GL_SPECULAR, value);

		glLightfv(hdl_, GL_POSITION, pos_);
	}

};

}
