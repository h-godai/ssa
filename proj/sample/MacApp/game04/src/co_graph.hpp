//
// 描画基本クラス
// 
#pragma once

#include "co_vec2.hpp"
#include "co_texmng.hpp"
#include "co_misc.hpp"

namespace ngs {

	enum enmGRP_BLEND {
		GRP_BLEND_NORMAL,							// 通常のブレンディング
		GRP_BLEND_ADD,								// 加算半透明
		GRP_BLEND_REV,								// 反転表示
		GRP_BLEND_XOR,								// XOR
		GRP_BLEND_MUL,								// 乗算
		GRP_BLEND_SCREEN,							// スクリーン合成
	};

	template <typename T>
	class GrpCol {
	public:
		T r, g, b, a;
		GrpCol() : r(static_cast<T>(0)), g(static_cast<T>(0)), b(static_cast<T>(0)), a(static_cast<T>(1)) {}
		GrpCol(T red, T green, T blue, T alpha) : r(red), g(green), b(blue), a(alpha) {}

		void set(T red, T green, T blue, T alpha = static_cast<T>(1)) {
			r = red, g = green, b = blue, a = alpha;
		}
		// TODO:カラー演算
		// 整数と小数でmaxを変える
	};
	

	class GrpBase {
	protected:
		int blend_;
		Vec2<float> pos_;
		Vec2<float> center_;
		Vec2<float> scale_;
		GrpCol<float> col_;
		float rotate_;
	public:
		GrpBase() : blend_(GRP_BLEND_NORMAL),
								pos_(0.0f, 0.0f), center_(0.0f, 0.0f), scale_(1.0f, 1.0f), rotate_(0.0f) {}
		virtual ~GrpBase() {}

		void blend(const int blend) {
			blend_ = blend;
		}

		void col(const GrpCol<float> col) {
			col_ = col;
		}
		void col(const float r, const float g, const float b, const float a = 1.0f) {
			col_.set(r, g, b, a);
		}

		void pos(const Vec2<float>& pos) {
			pos_ = pos;
		}
		void pos(const float x, const float y) {
			pos_.set(x, y);
		}

		void center(const Vec2<float>& center) {
			center_ = center;
		}
		void center(const float x, const float y) {
			center_.set(x, y);
		}

		void scale(const Vec2<float>& scale) {
			scale_ = scale;
		}
		void scale(const float x, const float y) {
			scale_.set(x, y);
		}

		void rotate(const float rotate) {
			rotate_ = rotate;
		}

		void set_mtx() const {
			glLoadIdentity();
			glTranslatef(pos_.x, pos_.y, 0);
			glRotatef(Rad2Ang(rotate_), 0, 0, 1);
			glScalef(scale_.x, scale_.y, 1.0);
			glTranslatef(-center_.x, -center_.y, 0);
		}

		void set_blend() const {
			switch(blend_)
			{
			case GRP_BLEND_NORMAL:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;

			case GRP_BLEND_ADD:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;

			case GRP_BLEND_REV:
				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
				break;

			case GRP_BLEND_XOR:
				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
				break;

			case GRP_BLEND_MUL:
				glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				break;

			case GRP_BLEND_SCREEN:
				glBlendFunc(GL_DST_COLOR, GL_ONE);
				break;
			}
		}

		virtual void draw() {}
	};


	class GrpPoint : public GrpBase {
		bool smooth_;
		float size_;
	public:
		GrpPoint() : smooth_(false), size_(1.0f) {}

		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		void size(const float size) {
			size_ = size;
		}

		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glPointSize(size_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			glBegin(GL_POINTS);
			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(0.0f, 0.0f);
			glEnd();
		}
	};

	class GrpLine : public GrpBase {
		Vec2<float> ed_;
		bool smooth_;
		float width_;
	public:
		GrpLine() : smooth_(false), width_(1.0f) {}
		
		void points(const Vec2<float>& st, const Vec2<float>& ed) {
			pos_ = st;
			ed_ = ed - st;
		}
		void points(const float st_x, const float st_y, const float ed_x, const float ed_y) {
			pos_.set(st_x, st_y);
			ed_.set(ed_x - st_x, ed_y - st_y);
		}
		
		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		void width(const float width) {
			width_ = width;
		}

		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glLineWidth(width_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			glBegin(GL_LINE_STRIP);
			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(0.0f, 0.0f);
			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(ed_.x, ed_.y);
			glEnd();
		}
	};

	class GrpBox : public GrpBase {
		Vec2<float> size_;
		bool smooth_;
		float width_;
		bool fill_;
	public:
		GrpBox() : smooth_(false), width_(1.0f), fill_(false) {}
		
		void size(const Vec2<float>& size) {
			size_ = size;
		}
		void size(const float x, const float y) {
			size_.set(x, y);
		}
		
		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		void width(const float width) {
			width_ = width;
		}
		void fill(const bool fill) {
			fill_ = fill;
		}

		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glLineWidth(width_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			glBegin(fill_ ? GL_QUADS : GL_LINE_LOOP);
			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(0.0f, 0.0f);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(0.0f, size_.y);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(size_.x, size_.y);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glVertex2f(size_.x, 0.0f);
			glEnd();
		}
	};

	class GrpCircle : public GrpBase {
		Vec2<float> radius_;
		bool smooth_;
		float width_;
		bool fill_;
		int div_;
	public:
		GrpCircle() : smooth_(false), width_(1.0f), fill_(false), div_(10) {}

		void radius(const Vec2<float>& radius) {
			radius_ = radius;
		}
		void radius(const float x, const float y) {
			radius_.set(x, y);
		}

		void div(int div) {
			div_ = div;
		}
		void width(const float width) {
			width_ = width;
		}
		void fill(const bool fill) {
			fill_ = fill;
		}
		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		
		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glLineWidth(width_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			if(fill_)
			{
				glBegin(GL_TRIANGLE_FAN);
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(0.0f, 0.0f);

				for(int i = 0; i <= div_; ++i)
				{
					float r;
					r = (PI * 2.0f * (float)i) / div_;
					glColor4f(col_.r, col_.g, col_.b, col_.a);
					glVertex2f(radius_.x * sinf(r), radius_.y * cosf(r));
				}
				glEnd();
			}
			else
			{
				glBegin(GL_LINE_LOOP);
				for(int i = 0; i < div_; ++i)
				{
					float r;
					r = (PI * 2.0f * (float)i) / div_;
					glColor4f(col_.r, col_.g, col_.b, col_.a);
					glVertex2f(radius_.x * sinf(r), radius_.y * cosf(r));
				}
				glEnd();
			}
		}
	};

	class GrpTri : public GrpBase {
		Vec2<float> points_[3];
	public:
		GrpTri() {}
		
		void points(const Vec2<float>& p1, const Vec2<float>& p2, const Vec2<float>& p3) {
			points_[0] = p1;
			points_[1] = p2;
			points_[2] = p3;
		}

		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			set_mtx();

			glBegin(GL_TRIANGLES);
			for(int i = 0; i < 3; ++i)
			{
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(points_[i].x, points_[i].y);
			}
			glEnd();
		}
	};

	class GrpFan : public GrpBase {
		Vec2<float> radius_;
		bool smooth_;
		float width_;
		bool fill_;
		int div_;
		float angle_;
	public:
		GrpFan() : smooth_(false), width_(1.0f), fill_(false), div_(10) {}

		void radius(const Vec2<float>& radius) {
			radius_ = radius;
		}
		void radius(const float x, const float y) {
			radius_.set(x, y);
		}

		void div(int div) {
			div_ = div;
		}
		void angle(float angle) {
			angle_ = angle;
		}
		void width(const float width) {
			width_ = width;
		}
		void fill(const bool fill) {
			fill_ = fill;
		}
		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		
		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glLineWidth(width_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			if(fill_)
			{
				glBegin(GL_TRIANGLE_FAN);
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(0.0f, 0.0f);

				for(int i = 0; i <= div_; ++i)
				{
					float r = (angle_ * (float)i) / (float)div_;
					glColor4f(col_.r, col_.g, col_.b, col_.a);
					glVertex2f(radius_.x * sinf(r), radius_.y * cosf(r));
				}
				glEnd();
			}
			else
			{
				glBegin(GL_LINE_LOOP);
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(0.0f, 0.0f);

				for(int i = 0; i < div_; ++i)
				{
					float r = (angle_ * (float)i) / (float)div_;
					glColor4f(col_.r, col_.g, col_.b, col_.a);
					glVertex2f(radius_.x * sinf(r), radius_.y * cosf(r));
				}
				glEnd();
			}
		}
	};

	class GrpDounut : public GrpBase {
		Vec2<float> radius_, hole_;
		int div_;
	public:
		GrpDounut() : div_(10) {}

		void radius(const Vec2<float>& radius) {
			radius_ = radius;
		}
		void radius(const float x, const float y) {
			radius_.set(x, y);
		}
		void hole(const Vec2<float>& hole) {
			hole_ = hole;
		}
		void hole(const float x, const float y) {
			hole_.set(x, y);
		}

		void div(int div) {
			div_ = div;
		}
		
		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			set_mtx();

			glBegin(GL_TRIANGLE_STRIP);
			for(int i = 0; i <= div_; i += 1)
			{
				float r = (PI * 2.0f * (float)i) / div_;
				float sin_r = sinf(r);
				float cos_r = cosf(r);
				
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(radius_.x * sin_r, radius_.y * cos_r);
				
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(hole_.x * sin_r, hole_.y * cos_r);
			}
			glEnd();
		}
	};

	class GrpSprite : public GrpBase {
		Vec2<float> size_;
		Vec2<float> uv_top_, uv_bottom_;
		bool flip_v_, flip_h_;
		bool filter_;
		TexMng::tex_ptr tex_;
	public:
		GrpSprite() : flip_v_(false), flip_h_(false), filter_(true) {}

		void size(const Vec2<float>& size) {
			size_ = size;
		}
		void size(const float x, const float y) {
			size_.set(x, y);
		}

		void texture(const TexMng::tex_ptr tex) {
			tex_ = tex;
		}
		void uv(const Vec2<float>& top, const Vec2<float>& bottom) {
			uv_top_ = top;
			uv_bottom_ = bottom;
		}
		void uv(const float top_x, const float top_y, const float bottom_x, const float bottom_y) {
			uv_top_.set(top_x, top_y);
			uv_bottom_.set(bottom_x, bottom_y);
		}
		void flip(const bool h, const bool v) {
			flip_h_ = h;
			flip_v_ = v;
		}
		void smooth(const bool smooth) {
			filter_ = smooth;
		}

		virtual void draw() {
			glEnable(GL_TEXTURE_2D);
			tex_->bind();
			GLint filter = filter_ ? GL_LINEAR : GL_NEAREST;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			set_blend();
			set_mtx();

			glBegin(GL_QUADS);

			float top_u, top_v;
			float bottom_u, bottom_v;
			const Vec2<int>& size = tex_->size();
			const Vec2<float>& uv = tex_->uv();
			
			
			top_u = uv_top_.x * uv.x / size.x;
			top_v = uv_top_.y * uv.y / size.y;
			bottom_u = uv_bottom_.x * uv.x / size.x;
			bottom_v = uv_bottom_.y * uv.y / size.y;
			if(flip_h_)
			{
				float a = top_u;
				top_u = bottom_u;
				bottom_u = a;
			}
			if(flip_v_)
			{
				float a = top_v;
				top_v = bottom_v;
				bottom_v = a;
			}
			Vec2<float> uv_top(top_u, top_v);
			Vec2<float> uv_bottom(bottom_u, bottom_v);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glTexCoord2f(top_u, top_v);
			glVertex2f(0.0f, 0.0f);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glTexCoord2f(top_u, bottom_v);
			glVertex2f(0.0f, size_.y);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glTexCoord2f(bottom_u, bottom_v);
			glVertex2f(size_.x, size_.y);

			glColor4f(col_.r, col_.g, col_.b, col_.a);
			glTexCoord2f(bottom_u, top_v);
			glVertex2f(size_.x, 0.0f);

			glEnd();
			
		}
	};

	class GrpBezierLine : public GrpBase {
		Bezier bx_, by_;
		float div_;
		bool smooth_;
		float width_;
	public:
		GrpBezierLine() : div_(5.0f), smooth_(false), width_(1.0f) {}
		
		void smooth(const bool smooth) {
			smooth_ = smooth;
		}
		void width(const float width) {
			width_ = width;
		}
		void div(float div) {
			div_ = div;
		};
		void set(float x0, float y0, float x1, float y1, float x_p1, float y_p1, float x_p2, float y_p2) {
			pos_.set(x0, y0);
			bx_.set(0.0f, x1 - x0, x_p1 - x0, x_p2 - x0);
			by_.set(0.0f, y1 - y0, y_p1 - y0, y_p2 - y0);
		}
		void set(Vec2<float>& st, Vec2<float>& ed, Vec2<float>& p1, Vec2<float>& p2) {
			pos_ = st;
			bx_.set(0.0f, ed.x - st.x, p1.x - st.x, p2.x - st.x);
			by_.set(0.0f, ed.y - st.y, p1.y - st.y, p2.y - st.y);
		}

		virtual void draw() {
			glDisable(GL_TEXTURE_2D);
			set_blend();
			glLineWidth(width_);
			if(smooth_) glEnable(GL_LINE_SMOOTH);
			else		glDisable(GL_LINE_SMOOTH);
			set_mtx();

			float len = BezierLength(bx_, by_, 8);
			float dt = div_ / len;
			if(dt > 1.0f) dt = 1.0f;
			float t = 0.0f;

			glBegin(GL_LINE_STRIP);
			while(t < 1.0f)
			{
				float x0 = bx_.pos(t);
				float x1 = bx_.pos(t + dt);
				float y0 = by_.pos(t);
				float y1 = by_.pos(t + dt);
				
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(x0, y0);
				glColor4f(col_.r, col_.g, col_.b, col_.a);
				glVertex2f(x1, y1);
				
				t += dt;
			}
			glEnd();
		}
	};

}
