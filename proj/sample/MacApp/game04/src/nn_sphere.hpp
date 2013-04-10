
#pragma once

#include <vector>

namespace ngs {


struct Vertex {
	GLfloat x, y, z;
};

struct Uv {
	GLfloat u, v;
};

struct Face {
	GLushort v1, v2, v3;
};


class Sphere {
	std::vector<Vertex> vertex_;
	std::vector<Vertex> normal_;
	std::vector<Uv> uv_;
	std::vector<Face> face_;

	GLuint vertices_;
	GLuint faces_;
	GLuint points_;
	
	Sphere() {}
public:
	Sphere(int slices, int stacks, float radius) : vertices_((slices + 1) * (stacks + 1)), faces_(slices * stacks * 2), points_(faces_ * 3) {
		vertex_.reserve(vertices_);
		normal_.reserve(vertices_);
		uv_.reserve(vertices_);
		face_.reserve(faces_);

		for(int j = 0; j <= stacks; ++j)
		{
			float ph = PI * (float)j / (float)stacks;
			float y = radius * cos(ph);
			float r = radius * sin(ph);
			for(int i = 0; i <= slices; ++i)
			{
				Vertex vtx;
				
				float th = 2.0 * PI * (float)i / (float)slices;
				float x = r * cos(th);
				float z = r * sin(th);
				vtx.x = x;
				vtx.y = y;
				vtx.z = z;
				vertex_.push_back(vtx);

				float l = sqrt(x * x + y * y + z * z);
				vtx.x = x / l;
				vtx.y = y / l;
				vtx.z = z / l;
				normal_.push_back(vtx);

				float u = (float)i / (float)slices;
				float v = (float)j / (float)stacks;
				Uv uv = { u, v };
				uv_.push_back(uv);
			}
		}

		for(int j = 0; j < stacks; ++j)
		{
			for(int i = 0; i < slices; ++i)
			{
				int count = (slices + 1) * j + i;

				Face face;

				face.v1 = count;
				face.v2 = count + 1;
				face.v3 = count + slices + 2;
				face_.push_back(face);
				
				face.v1 = count;
				face.v2 = count + slices + 2;
				face.v3 = count + slices + 1;
				face_.push_back(face);
				// GL_TRIANGLESで描画するのでfaceを２つづつ生成
			}
		}
	}
	
	~Sphere() {}

	void draw()
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, static_cast<GLvoid *>(&vertex_[0]));
		glNormalPointer(GL_FLOAT, 0, static_cast<GLvoid *>(&normal_[0]));
		glTexCoordPointer(2, GL_FLOAT, 0, static_cast<GLvoid *>(&uv_[0]));
		glDrawElements(GL_TRIANGLES, points_, GL_UNSIGNED_SHORT, static_cast<GLvoid *>(&face_[0]));

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
};

}
