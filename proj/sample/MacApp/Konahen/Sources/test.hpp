
#pragma once

#include <algorithm>
#include <cmath>
#include "co_texture.hpp"
#include "co_mouse.hpp"
#include "co_vec2.hpp"
#include "co_quat.hpp"
#include "co_matrix.hpp"
#include "co_texmng.hpp"
#include "co_graph.hpp"
#include "co_misc.hpp"
#include "nn_camera.hpp"
#include "nn_light.hpp"
#include "nn_sphere.hpp"
#include "nn_polygon.hpp"
#include "nn_misc.hpp"
#include "nn_font.hpp"
#include "nn_place.hpp"

namespace ngs {

class Test
{
	float rotY_;
	float radius_;
	Vec3<float> center_;

	Vec2<float> mpos_;
	Vec2<float> last_mpos_;
	float pinch_;
	
	Quat<float> drag_, drag__;
	Quat<float> rot_;
	float posZ_;
	bool rotate_;
	bool zoom_;
	bool isDrag_;
	float near_z_, far_z_;

	float aspect_;
	Camera camera_;
	Camera bgCamera_;
	Camera cockpit_;

	Light earthLight_;
	Light cloudLight_;
	
	GLdouble modelMtx_[16];
	Vec3<float> wpos1_, wpos2_;

	bool hit_;
	Vec3<float> hit_pos_;
	float hit_t_;
	Vec2<float> loc_;

	float scaleX_, scaleY_, rotX_;
	float frame_;

	TexMng texmng;

	Sphere space_;
	Sphere earth_;
	Sphere earth_cloud_;
	// Sphere moon_;

	Polygon<GLfloat> polygon;

	Font *font_;

	Place place_;
	
	void calcLocation()
	{
		const Vec2<int>& d_pos = Mouse::instance().d_pos();

		Vec2<float> pos(d_pos.x, d_pos.y);
		wpos1_ = camera_.posToWorld(pos, 0.0, modelMtx_);
		wpos2_ = camera_.posToWorld(pos, 1.0, modelMtx_);
		std::cout << "mouse:" << pos.x << " " << pos.y << std::endl;
		std::cout << "(" << wpos1_.x << " " << wpos1_.y << " " << wpos1_.z << ") - (" << wpos2_.x << " " << wpos2_.y << " " << wpos2_.z << ")" << std::endl;

		Vec3<float> v = wpos2_ - wpos1_;
		v.unit();
		hit_ = sphereRayCollision(radius_, center_, wpos1_, v, hit_pos_, hit_t_);
		if(hit_)
		{
			loc_ = posToLoc(hit_pos_);
			std::cout << "(" << hit_pos_.x << " " << hit_pos_.y << " " << hit_pos_.z << ")" << std::endl;
			std::cout << loc_.x * 180.0 / PI << " " << loc_.y * 180.0 / PI << std::endl;

			{
				Vec2<float> p = locToPos2d(loc_);
				std::cout << (p.x + 1.0) * 512 << " " << (p.y + 1) * 256 << std::endl;
			}

			{
				Vec3<float> p = locToPos3d(loc_);
				std::cout << p.x * radius_ << " " << p.y * radius_ << " " << p.z * radius_ << std::endl;
			}
		}
	}

public:
	Test(const int w, const int h, const std::string& path) : radius_(100.0), rotY_(), posZ_(350.0), aspect_((float)w / (float)h),
																														camera_(Camera::PERSPECTIVE), bgCamera_(Camera::PERSPECTIVE), cockpit_(Camera::ORTHOGONAL),
																														earth_(64, 32, radius_), earth_cloud_(64, 32, radius_ * 1.01), space_(32, 16, 80.0),
																														hit_(), hit_t_(), frame_(), pinch_(), rotate_(), zoom_(),
																														near_z_(150.0), far_z_(800.0),
																														place_(path + "/devdata/place/japan.json")
	{
		texmng.read(path + "/devdata/eath.png");
		texmng.read(path + "/devdata/eath_cloud.png");
		texmng.read(path + "/devdata/skymap.png");
		texmng.read(path + "/devdata/cockpit.png");

		earthLight_.ambientCol(0.05, 0.05, 0.05);
		earthLight_.diffuseCol(1.0, 1.0, 1.0);
		earthLight_.specularCol(0.5, 0.5, 0.5);
		earthLight_.pos(Vec3<float>(100.0, 0.0, 100.0));

		cloudLight_.ambientCol(0.1, 0.1, 0.1);
		cloudLight_.diffuseCol(0.8, 0.8, 0.8);
		cloudLight_.specularCol(0.1, 0.1, 0.1);
		cloudLight_.pos(Vec3<float>(100.0, 0.0, 100.0));
		
		camera_.setAngle(45.0 * PI / 180.0);
		camera_.setAspect(aspect_);
		camera_.setNearZ(10.0);
		camera_.setFarZ(1000.0);
		camera_.setDist(posZ_);

		bgCamera_.setAngle(100.0 * PI / 180.0);
		bgCamera_.setAspect(aspect_);
		bgCamera_.setNearZ(10.0);
		bgCamera_.setFarZ(100.0);
		bgCamera_.setDist(0.0);

		cockpit_.setSize(w, h);

		// ポリゴンテスト
		{
			std::vector<Vec2<GLfloat> > vtx;
			vtx.push_back(Vec2<GLfloat>(100.0, 10.0));
			vtx.push_back(Vec2<GLfloat>(20.0, 50.0));
			vtx.push_back(Vec2<GLfloat>(100.0, 100.0));
			vtx.push_back(Vec2<GLfloat>(70.0, 120.0));
			vtx.push_back(Vec2<GLfloat>(5.0, 10.0));

			polygon.setVtx(vtx);
		}

		// Font Test
		// font_ = new Font(path + "/devdata/s7_p.ttf", 25);
		font_ = new Font(path + "/devdata/tsukip.ttf", 25);
	}
	
	~Test() {
		std::cout << "~Test()" << std::endl;
		delete font_;
	}

	void resize(const int w, const int h)
	{
			aspect_ = (float)w / (float)h;
			camera_.setAspect(aspect_);
			bgCamera_.setAspect(aspect_);
	}
	void resize(const int w, const int h, const float sx, const float sy)
	{
		camera_.scale(sx, sy);
		bgCamera_.scale(sx, sy);
		cockpit_.setSize(w, h);
	}
	
	void step()
	{
		Mouse& mouse = Mouse::instance();
		const std::vector<Vec2<int> >&pos = mouse.posAll();		
		
		if(mouse.push(MOUSE_LEFT))
		{
			mpos_.set(pos[0].x, pos[0].y);
			last_mpos_ = mpos_;
			rotate_ = true;
			isDrag_ = false;
		}
		
		if(mouse.push(MOUSE_MULTI_TOUCH))
		{
			Vec2<int> p = pos[1];
			pinch_ = p.dist(pos[0]);
			zoom_ = true;
		}

		if(rotate_)
		{
			if(mouse.pull(MOUSE_LEFT))
			{
				drag__ = drag_ * drag__;
				drag_.set(0.0, 0.0, 0.0, 1.0);
				rotate_ = false;
				if(!isDrag_)
				{
					calcLocation();
					// 移動距離が短い場合にクリックとみなす
				}
			}
			else
			{
				Vec2<float> mp(pos[0].x, pos[0].y);
				mp = mp - mpos_;
				float len = mp.length();
				if(len > 1.0) isDrag_ = true;
				float r = len * 0.00005 * (posZ_ - radius_) * 0.3;
				mp.unit();
				Vec3<float> vct(mp.y, -mp.x, 0.0);
				drag_.rotate(r, vct);
				// クオータニオンを使うとジンバルロックしない

				mp.set(pos[0].x, pos[0].y);
				mp = mp - last_mpos_;
				len = mp.length();
				r = len * 0.00005 * (posZ_ - radius_) * 0.3;
				mp.unit();
				vct.set(mp.y, -mp.x, 0.0);
				rot_.rotate(r, vct);
				last_mpos_.set(pos[0].x, pos[0].y);
			}
		}

		if(zoom_)
		{
			if(mouse.pull(MOUSE_MULTI_TOUCH))
			{
				zoom_ = false;
			}
			else
			{
				Vec2<int> p = pos[1];
				float pinch = p.dist(pos[0]);
				float dz = (pinch - pinch_) * -0.0025 * posZ_;
				if(posZ_ + dz < near_z_)
				{
					dz *= 0.12;
				}
				else
				if(posZ_ + dz > far_z_)
				{
					dz *= 0.1;
				}
				
				posZ_ += dz;
				pinch_ = pinch;

				// Vec2<float> mp(pos.x, pos.y);
				// Vec2<float> mpd = mp - mpos_;
				// posZ_ += mpd.y * 0.001 * posZ_;
				// mpos_ = mp;
			}
		}
		else
		{
			float dz;
			dz =  near_z_ - posZ_;
			if(dz > 0.0)
			{
				posZ_ = posZ_ + dz * 0.3;
			}
			dz = far_z_ - posZ_;
			if(dz < 0.0)
			{
				posZ_ = posZ_ + dz * 0.4;
			}
		}

		camera_.setDist(posZ_);

		if(!rotate_)
		{
			drag__ = rot_ * drag__;
		}
		Quat<float> rot(drag_ * drag__);
		camera_.setRot(rot);
		bgCamera_.setRot(rot);
		
		rotY_ += 0.05;
	}

	void draw()
	{
		// glEnable(GL_DEPTH_TEST);
		// glDisable(GL_CULL_FACE);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		
		bgCamera_.setup();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glCullFace(GL_BACK);

		glColor4f(1.0, 1.0, 1.0, 1.0);
		TexMng::tex_ptr skymap = texmng.get("skymap");
		skymap->bind();
		space_.draw();

		camera_.setup();
		const Matrix& mtx = camera_.getMatrix();
		Vec3<float> lpos = Vec3<float>(500.0, -800.0, 1000.0);
		mtx.apply(lpos);
		lpos.y = -lpos.y;
		earthLight_.pos(lpos);
		cloudLight_.pos(lpos);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		earthLight_.setup();

		glRotatef(rotY_, 0.0, 1.0, 0.0);

		glCullFace(GL_FRONT);
		glEnable(GL_NORMALIZE);

#ifndef TARGET_OS_IPHONE
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
#endif
		
		{
			const GLfloat dcolor[] = {0.0, 0.0, 0.0, 1.0 };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, dcolor);
		}
		
		const GLfloat dcolor[] = { 1.0, 1.0, 1.0, 1.0 };  /* 拡散反射係数 */
		const GLfloat scolor[] = { 0.6, 0.6, 0.6, 1.0 };  /* 鏡面反射係数 */
		
		/* 材質の設定 */
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dcolor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, scolor);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

		glDisable(GL_BLEND);
		// glDisable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_2D);
		TexMng::tex_ptr texture = texmng.get("eath");
		texture->bind();		
		earth_.draw();
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMtx_);

		float scale = 1.0;
		if(posZ_ < 200.0)
		{
			scale = (posZ_ - 150.0) / 50.0;
			if(scale < 0.0) scale = 0.0;
		}
		cloudLight_.scale(scale);

		glLoadIdentity();
		cloudLight_.setup();
		glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

		glRotatef(rotY_, 0.0, 1.0, 0.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		TexMng::tex_ptr cloud = texmng.get("eath_cloud");
		cloud->bind();
		earth_cloud_.draw();

#if 0
		glDisable(GL_BLEND);
		glLoadIdentity();
		glTranslatef(200.0, 0.0, 200.0);
		TexMng::tex_ptr moon = texmng.get("moon");
		moon->bind();
		moon_.draw();
#endif

#ifndef TARGET_OS_IPHONE
		if(hit_)
		{
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glDisable(GL_BLEND);

			glPointSize(5);
			glEnable(GL_LINE_SMOOTH);
			glBegin(GL_POINTS);
			glColor4f(1.0, 0.0, 0.0, 1.0);
			glVertex3f(hit_pos_.x, hit_pos_.y, hit_pos_.z);
			glEnd();
		}
#endif

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
#if 0
		glLoadIdentity();
		glLineWidth(1);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINES);
		glVertex3f(wpos1_.x, wpos1_.y, wpos1_.z);
		glVertex3f(wpos2_.x, wpos2_.y, wpos2_.z);
		glEnd();
#endif

		// 2D表示
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		cockpit_.setup();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
#if 0
		const std::vector<Vec2<GLfloat> >& vtx = polygon.vtx();
		size_t num = vtx.size();
		for(size_t i = 0; i < num; ++i)
		{
			int idx = ((i + 1) != num) ? i + 1 : 0;
			GrpLine obj;
			obj.points(vtx[i].x, vtx[i].y, vtx[idx].x, vtx[idx].y);
			obj.col(0.0, 1.0, 0.0);
			obj.draw();
		}
		{
			Mouse& mouse = Mouse::instance();
			const Vec2<int>& mpos = mouse.pos();

			Vec2<GLfloat> pos(mpos.x, mpos.y);
			
			GrpPoint obj;
			obj.pos(mpos.x, mpos.y);
			if(polygon.intension(pos))
			{
				obj.col(1.0, 0.0, 0.0);
			}
			else
			{
				obj.col(0.0, 0.0, 1.0);
			}
			obj.size(5);
			obj.draw();
		}
#endif


#if 0
		scaleX_ = 2.0 + sin(frame_) * 1.2;
		scaleY_ = 2.0 + cos(frame_ * 0.7) * 0.5;
		rotX_ = PI * 0.1 * sin(frame_ * 0.6);
		frame_ += 0.05;
		
		{
			GrpSprite obj;
			obj.size(256, 72);
			obj.center(256 / 2, 60);
			obj.pos(0, -106);
			obj.scale(scaleX_, scaleY_);
			obj.rotate(rotX_);
			obj.texture(texmng.get("cockpit"));
			obj.uv(0, 0, 256, 72);
			obj.col(1.0, 1.0, 1.0);
			obj.draw();
		}
		{
			GrpSprite obj;
			obj.size(256, 32);
			obj.center(256 / 2, 32);
			obj.pos(0, 250);
			obj.texture(texmng.get("cockpit"));
			obj.uv(0, 72, 256, 72 + 32);
			obj.col(1.0, 1.0, 1.0);
			obj.draw();
		}
#endif

		std::wstring str_tbl[] = {
			L"『こなへん』とは日本の方言のひとつで",
			L"「だいたいこのあたりです」を言い表す",
			L"名古屋弁である",
		};

		float y = -50.0;
		// font_->height(25);															// FIXME:サイズを変えると内部キャッシュが全て破棄される
		for(int i = 0; i < elemsof(str_tbl); ++i)
		{
			const Vec2<float>& size = font_->size(str_tbl[i]);
			font_->pos(Vec2<float>(size.x / -2.0, y));
			font_->draw(str_tbl[i]);
			y += size.y * 2.2f;
		}
	}
};

}
