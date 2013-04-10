﻿//
// 4x4行列演算
//
#pragma once

#include <cmath>
#include <algorithm>
#include "co_vec3.hpp"
#include "co_vec4.hpp"
#include "co_quat.hpp"

namespace ngs {

class Matrix {
	GLfloat mtx_[4 * 4];

public:
	Matrix()
	{
		std::fill(&mtx_[0], &mtx_[4 * 4], 0.0);
		mtx_[0] = 1.0;
		mtx_[5] = 1.0;
		mtx_[10] = 1.0;
		mtx_[15] = 1.0;
	}

	void clear()
	{
		std::fill(&mtx_[0], &mtx_[4 * 4], 0.0);
	}

	void identity()
	{
		std::fill(&mtx_[0], &mtx_[4 * 4], 0.0);
		mtx_[0] = 1.0;
		mtx_[5] = 1.0;
		mtx_[10] = 1.0;
		mtx_[15] = 1.0;
	}

	const GLfloat *value() const
	{
		return mtx_;
	}

	void translate(float x, float y, float z, bool ident = true) {
		if(ident) identity();
		mtx_[12] = x, mtx_[13] = y, mtx_[14] = z;
	}

	void scale(float x, float y, float z) {
		identity();
		mtx_[0] = x, mtx_[5] = y, mtx_[10] = z;
	}

	void rotateX(float r) {
		identity();
		float sin_r = sin(r);
		float cos_r = cos(r);
		mtx_[5] = cos_r, mtx_[6] = sin_r;
		mtx_[9] = -sin_r, mtx_[10] = cos_r;
	}

	void rotateY(float r) {
		identity();
		float sin_r = sin(r);
		float cos_r = cos(r);
		mtx_[0] = cos_r, mtx_[2] = -sin_r;
		mtx_[8] = sin_r, mtx_[10] = cos_r;
	}

	void rotateZ(float r) {
		identity();
		float sin_r = sin(r);
		float cos_r = cos(r);
		mtx_[0] = cos_r, mtx_[1] = sin_r;
		mtx_[4] = -sin_r, mtx_[5] = cos_r;
	}

	void camera(Vec3<float> eye, Vec3<float> at, Vec3<float> up) {
		Vec3<float> zaxis = (at - eye);
		zaxis.unit();
		Vec3<float> xaxis = up;
		xaxis = xaxis.cross(zaxis);
		xaxis.unit();
		Vec3<float> yaxis = zaxis.cross(xaxis);
			
		mtx_[0] = xaxis.x, mtx_[1] = yaxis.x, mtx_[2] = zaxis.x, mtx_[3] = 0.0f;
		mtx_[4] = xaxis.y, mtx_[5] = yaxis.y, mtx_[6] = zaxis.y, mtx_[7] = 0.0f;
		mtx_[8] = xaxis.z, mtx_[9] = yaxis.z, mtx_[10] = zaxis.z, mtx_[11] = 0.0f;
		mtx_[12] = -xaxis.dot(eye), mtx_[13] = -yaxis.dot(eye), mtx_[14] = -zaxis.dot(eye), mtx_[15] = 1.0f;
	}

	void projection(float r, float aspect, float near_z, float far_z) {
		clear();

		float y = 1.0f / tan(r * 0.5f);
		float x = y / aspect;
		float z = far_z / (far_z - near_z);

		mtx_[0] = x;
		mtx_[5] = y;
		mtx_[10] = z, mtx_[11] = 1;
		mtx_[14] = -z * near_z;
			
	}

	void screen(float w, float h, float min_z, float max_z) {
		identity();

		float r = (max_z / (max_z - min_z)) * min_z;

		mtx_[0] = w / 2.0f;
		mtx_[5] = h / 2.0f;
		mtx_[12] = w / 2.0f, mtx_[13] = h / 2.0f, mtx_[14] = -r;
	}

	void mul(const Matrix mat) {
		Matrix tmp;

		tmp.mtx_[0] = mtx_[0] * mat.mtx_[0] + mtx_[1] * mat.mtx_[4] + mtx_[2] * mat.mtx_[8] + mtx_[3] * mat.mtx_[12];
		tmp.mtx_[1] = mtx_[0] * mat.mtx_[1] + mtx_[1] * mat.mtx_[5] + mtx_[2] * mat.mtx_[9] + mtx_[3] * mat.mtx_[13];
		tmp.mtx_[2] = mtx_[0] * mat.mtx_[2] + mtx_[1] * mat.mtx_[6] + mtx_[2] * mat.mtx_[10] + mtx_[3] * mat.mtx_[14];
		tmp.mtx_[3] = mtx_[0] * mat.mtx_[3] + mtx_[1] * mat.mtx_[7] + mtx_[2] * mat.mtx_[11] + mtx_[3] * mat.mtx_[15];

		tmp.mtx_[4] = mtx_[4] * mat.mtx_[0] + mtx_[5] * mat.mtx_[4] + mtx_[6] * mat.mtx_[8] + mtx_[7] * mat.mtx_[12];
		tmp.mtx_[5] = mtx_[4] * mat.mtx_[1] + mtx_[5] * mat.mtx_[5] + mtx_[6] * mat.mtx_[9] + mtx_[7] * mat.mtx_[13];
		tmp.mtx_[6] = mtx_[4] * mat.mtx_[2] + mtx_[5] * mat.mtx_[6] + mtx_[6] * mat.mtx_[10] + mtx_[7] * mat.mtx_[14];
		tmp.mtx_[7] = mtx_[4] * mat.mtx_[3] + mtx_[5] * mat.mtx_[7] + mtx_[6] * mat.mtx_[11] + mtx_[7] * mat.mtx_[15];

		tmp.mtx_[8] = mtx_[8] * mat.mtx_[0] + mtx_[9] * mat.mtx_[4] + mtx_[10] * mat.mtx_[8] + mtx_[11] * mat.mtx_[12];
		tmp.mtx_[9] = mtx_[8] * mat.mtx_[1] + mtx_[9] * mat.mtx_[5] + mtx_[10] * mat.mtx_[9] + mtx_[11] * mat.mtx_[13];
		tmp.mtx_[10] = mtx_[8] * mat.mtx_[2] + mtx_[9] * mat.mtx_[6] + mtx_[10] * mat.mtx_[10] + mtx_[11] * mat.mtx_[14];
		tmp.mtx_[11] = mtx_[8] * mat.mtx_[3] + mtx_[9] * mat.mtx_[7] + mtx_[10] * mat.mtx_[11] + mtx_[11] * mat.mtx_[15];

		tmp.mtx_[12] = mtx_[12] * mat.mtx_[0] + mtx_[13] * mat.mtx_[4] + mtx_[14] * mat.mtx_[8] + mtx_[15] * mat.mtx_[12];
		tmp.mtx_[13] = mtx_[12] * mat.mtx_[1] + mtx_[13] * mat.mtx_[5] + mtx_[14] * mat.mtx_[9] + mtx_[15] * mat.mtx_[13];
		tmp.mtx_[14] = mtx_[12] * mat.mtx_[2] + mtx_[13] * mat.mtx_[6] + mtx_[14] * mat.mtx_[10] + mtx_[15] * mat.mtx_[14];
		tmp.mtx_[15] = mtx_[12] * mat.mtx_[3] + mtx_[13] * mat.mtx_[7] + mtx_[14] * mat.mtx_[11] + mtx_[15] * mat.mtx_[15];

		*this = tmp;							// FIXME:微妙
	}

	void apply(Vec3<float>& vec) const {
		float x = vec.x * mtx_[0] + vec.y * mtx_[4] + vec.z * mtx_[8] + mtx_[12];
		float y = vec.x * mtx_[1] + vec.y * mtx_[5] + vec.z * mtx_[9] + mtx_[13];
		float z = vec.x * mtx_[2] + vec.y * mtx_[6] + vec.z * mtx_[10] + mtx_[14];
		vec.set(x, y, z);
	}

	void apply(Vec4<float>& vec) const {
		float x = vec.x * mtx_[0] + vec.y * mtx_[4] + vec.z * mtx_[8] + vec.w * mtx_[12];
		float y = vec.x * mtx_[1] + vec.y * mtx_[5] + vec.z * mtx_[9] + vec.w * mtx_[13];
		float z = vec.x * mtx_[2] + vec.y * mtx_[6] + vec.z * mtx_[10] + vec.w * mtx_[14];
		float w = vec.x * mtx_[3] + vec.y * mtx_[7] + vec.z * mtx_[11] + vec.w * mtx_[15];
		vec.set(x, y, z, w);
	}

	void reverse() {
		Matrix tmp;

		tmp.mtx_[0] = mtx_[5] * (mtx_[10] * mtx_[15] - mtx_[11] * mtx_[14]) + mtx_[6] * (mtx_[11] * mtx_[13] - mtx_[9] * mtx_[15]) + mtx_[7] * (mtx_[9] * mtx_[14] - mtx_[10] * mtx_[13]);
		tmp.mtx_[1] = mtx_[9] * (mtx_[14] * mtx_[3] - mtx_[15] * mtx_[2]) + mtx_[10] * (mtx_[15] * mtx_[1] - mtx_[13] * mtx_[3]) + mtx_[11] * (mtx_[13] * mtx_[2] - mtx_[14] * mtx_[1]);
		tmp.mtx_[2] = mtx_[13] * (mtx_[2] * mtx_[7] - mtx_[3] * mtx_[6]) + mtx_[14] * (mtx_[3] * mtx_[5] - mtx_[1] * mtx_[7]) + mtx_[15] * (mtx_[1] * mtx_[6] - mtx_[2] * mtx_[5]);
		tmp.mtx_[3] = mtx_[1] * (mtx_[6] * mtx_[11] - mtx_[7] * mtx_[10]) + mtx_[2] * (mtx_[7] * mtx_[9] - mtx_[5] * mtx_[11]) + mtx_[3] * (mtx_[5] * mtx_[10] - mtx_[6] * mtx_[9]);

		tmp.mtx_[4] = mtx_[6] * (mtx_[11] * mtx_[12] - mtx_[8] * mtx_[15]) + mtx_[7] * (mtx_[8] * mtx_[14] - mtx_[10] * mtx_[12]) + mtx_[4] * (mtx_[10] * mtx_[15] - mtx_[11] * mtx_[14]);
		tmp.mtx_[5] = mtx_[10] * (mtx_[15] * mtx_[0] - mtx_[12] * mtx_[3]) + mtx_[11] * (mtx_[12] * mtx_[2] - mtx_[14] * mtx_[0]) + mtx_[8] * (mtx_[14] * mtx_[3] - mtx_[15] * mtx_[2]);
		tmp.mtx_[6] = mtx_[14] * (mtx_[3] * mtx_[4] - mtx_[0] * mtx_[7]) + mtx_[15] * (mtx_[0] * mtx_[6] - mtx_[2] * mtx_[4]) + mtx_[12] * (mtx_[2] * mtx_[7] - mtx_[3] * mtx_[6]);
		tmp.mtx_[7] = mtx_[2] * (mtx_[7] * mtx_[8] - mtx_[4] * mtx_[11]) + mtx_[3] * (mtx_[4] * mtx_[10] - mtx_[6] * mtx_[8]) + mtx_[0] * (mtx_[6] * mtx_[11] - mtx_[7] * mtx_[10]);

		tmp.mtx_[8] = mtx_[7] * (mtx_[8] * mtx_[13] - mtx_[9] * mtx_[12]) + mtx_[4] * (mtx_[9] * mtx_[15] - mtx_[11] * mtx_[13]) + mtx_[5] * (mtx_[11] * mtx_[12] - mtx_[8] * mtx_[15]);
		tmp.mtx_[9] = mtx_[11] * (mtx_[12] * mtx_[1] - mtx_[13] * mtx_[0]) + mtx_[8] * (mtx_[13] * mtx_[3] - mtx_[15] * mtx_[1]) + mtx_[9] * (mtx_[15] * mtx_[0] - mtx_[12] * mtx_[3]);
		tmp.mtx_[10] = mtx_[15] * (mtx_[0] * mtx_[5] - mtx_[1] * mtx_[4]) + mtx_[12] * (mtx_[1] * mtx_[7] - mtx_[3] * mtx_[5]) + mtx_[13] * (mtx_[3] * mtx_[4] - mtx_[0] * mtx_[7]);
		tmp.mtx_[11] = mtx_[3] * (mtx_[4] * mtx_[9] - mtx_[5] * mtx_[8]) + mtx_[0] * (mtx_[5] * mtx_[11] - mtx_[7] * mtx_[9]) + mtx_[1] * (mtx_[7] * mtx_[8] - mtx_[4] * mtx_[11]);

		tmp.mtx_[12] = mtx_[4] * (mtx_[9] * mtx_[14] - mtx_[10] * mtx_[13]) + mtx_[5] * (mtx_[10] * mtx_[12] - mtx_[8] * mtx_[14]) + mtx_[6] * (mtx_[8] * mtx_[13] - mtx_[9] * mtx_[12]);
		tmp.mtx_[13] = mtx_[8] * (mtx_[13] * mtx_[2] - mtx_[14] * mtx_[1]) + mtx_[9] * (mtx_[14] * mtx_[0] - mtx_[12] * mtx_[2]) + mtx_[10] * (mtx_[12] * mtx_[1] - mtx_[13] * mtx_[0]);
		tmp.mtx_[14] = mtx_[12] * (mtx_[1] * mtx_[6] - mtx_[2] * mtx_[5]) + mtx_[13] * (mtx_[2] * mtx_[4] - mtx_[0] * mtx_[6]) + mtx_[14] * (mtx_[0] * mtx_[5] - mtx_[1] * mtx_[4]);
		tmp.mtx_[15] = mtx_[0] * (mtx_[5] * mtx_[10] - mtx_[6] * mtx_[9]) + mtx_[1] * (mtx_[6] * mtx_[8] - mtx_[4] * mtx_[10]) + mtx_[2] * (mtx_[4] * mtx_[9] - mtx_[5] * mtx_[8]);

		*this = tmp;							// FIXME:微妙
	}

	Matrix operator*(const Matrix mat) const {
		Matrix dst = *this;
		dst.mul(mat);
		return dst;
	}

	Vec3<float> operator*(Vec3<float> vec) {
		apply(vec);
		return vec;
	}

	Vec4<float> operator*(Vec4<float> vec) {
		apply(vec);
		return vec;
	}

	void rotate(const Quat<float>& quat) {
		mtx_[0] = 1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z;
		mtx_[1] = 2.0f * quat.x * quat.y + 2.0f * quat.w * quat.z;
		mtx_[2] = 2.0f * quat.x * quat.z - 2.0f * quat.w * quat.y;
		mtx_[3] = 0.0f;

		mtx_[4] = 2.0f * quat.x * quat.y - 2.0f * quat.w * quat.z;
		mtx_[5] = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z;
		mtx_[6] = 2.0f * quat.y * quat.z + 2.0f * quat.w * quat.x;
		mtx_[7] = 0.0f;

		mtx_[8] = 2.0f * quat.x * quat.z + 2.0f * quat.w * quat.y;
		mtx_[9] = 2.0f * quat.y * quat.z - 2.0f * quat.w * quat.x;
		mtx_[10] = 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y;
		mtx_[11] = 0.0f;

		mtx_[12] = mtx_[13] = mtx_[14] = 0.0f;
		mtx_[15] = 1.0f;
	}

	void set(const GLfloat mtx[])
	{
		for(int i = 0; i < 16; ++i)
		{
			mtx_[i] = mtx[i];
		}
	}
};
	
}
