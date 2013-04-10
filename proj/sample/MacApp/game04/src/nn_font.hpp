//
// FTBitmapFont
// FTBufferFont
// FTExtrudeFont
// FTOutlineFont
// FTPixmapFont
// FTPolygonFont
// FTTextureFont
// 

#pragma once

#define FTGL_LIBRARY_STATIC
#if (TARGET_OS_IPHONE)
#include <FTGL/ftgles.h>
#else
#include <FTGL/ftgl.h>
#endif
#include <string>
#include "co_vec2.hpp"


#if defined (_MSC_VER)
#ifdef _DEBUG
#pragma comment (lib, "freetype246MTd.lib")
#pragma comment (lib, "ftglMTd.lib")
#else
#pragma comment (lib, "freetype246MT.lib")
#pragma comment (lib, "ftglMT.lib")
#endif
#endif

namespace ngs {

template <typename FT>
class Font {
	FTFont *font_;

	int blend_;
	Vec2<float> pos_;
	Vec2<float> center_;
	Vec2<float> scale_;
	GrpCol<float> col_;
	float rotate_;
	
	void set_blend(const int blend) const {
		switch(blend)
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
	
	Font() {}
public:
	Font(const std::string& name, const int height) : scale_(1, 1), rotate_(0), col_(1, 1, 1, 1)
	{
		font_ = new FT(name.c_str());
		font_->FaceSize(height);
		font_->UseDisplayList(false);
	}
	~Font()
	{
		std::cout << "~Font()" << std::endl;
		delete font_;
	}

	void blend(const int mode) { blend_ = mode; }
	void pos(const Vec2<float>& val) { pos_ = val; }
	void center(const Vec2<float>& val) { center_ = val; }
	void scale(const Vec2<float>& val) {scale_ = val; }
	void col(GrpCol<float>& val) {col_ = val; }
	void rotate(const float rot) {rotate_ = rot; }
	void height(const int value) {
		font_->FaceSize(value);
	}

	template <typename T>
	const Vec2<float> size(const T& str)
	{
		float llx, lly, llz;
		float urx, ury, urz;
		font_->BBox(str.c_str(), llx, lly, llz, urx, ury, urz);
		return Vec2<float>(urx - llx, ury - lly);
	}
	
	template <typename T>
	void draw(const T& str)
	{
		glLoadIdentity();
		glTranslatef(pos_.x, pos_.y, 0);
		glRotatef(Rad2Ang(rotate_), 0, 0, 1);
		glScalef(scale_.x, scale_.y, 1.0);
		glTranslatef(-center_.x, -center_.y, 0);
		glScalef(1.0, -1.0, 1.0);
		set_blend(blend_);
		glColor4f(col_.r, col_.g, col_.b, col_.a);
		
		font_->Render(str.c_str());
	}
};

}
