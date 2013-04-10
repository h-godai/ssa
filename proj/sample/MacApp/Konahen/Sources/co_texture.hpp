
#pragma once

#include <string>
#include <fstream>
#include "co_vec2.hpp"
#include "co_png.hpp"
#include "co_filepath.hpp"
#include "co_misc.hpp"


namespace ngs {
	
class Texture {
public:
	struct Pixcel {
		u_char r, g, b, a;
	};
	struct PixcelRGB {
		u_char r, g, b;
	};

private:
	Vec2<int> size_gl_;																// OpenGLで管理するサイズ
	GLuint id_;
	Vec2<int> size_;
	Vec2<float> uv_;
	std::string name_;

	void copy32rgb(ngs::Png& png, int width, int height)
	{
		PixcelRGB *png_img;
		Pixcel *image;

		png_img = (PixcelRGB *)png.image;
		image = new Pixcel[width * height];
		for(int h = 0; h < png.size.y; ++h)
		{
			for(int i = 0; i < png.size.x; ++i)
			{
				PixcelRGB *pixcel = png_img + h * width + i;
				Pixcel pix = { pixcel->r, pixcel->g, pixcel->b, 255};
				image[h * width + i] = pix;
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		delete[] image;
	}

	void copy32rgba(ngs::Png& png, int width, int height) {
		Pixcel *png_img;
		Pixcel *image;
		Pixcel matPixcel = { 0, 0, 0, 0 };

		png_img = (Pixcel *)png.image;
		image = new Pixcel[width * height];
		for(int h = 0; h < png.size.y; ++h)
		{
			Pixcel *pixcel = image + h * width;
			std::memcpy(pixcel, png_img + h * png.size.x, png.size.x * sizeof(Pixcel));
			for(int i = 0; i < png.size.x; i += 1)
			{
				if(pixcel->a == 0)
				{
					*pixcel = matPixcel;
				}
				pixcel += 1;
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		delete[] image;
	}

	void copy32i(ngs::Png& png, int width, int height) {
		Pixcel *image;

		image = new Pixcel[width * height];
		for(int h = 0; h < png.size.y; ++h)
		{
			for(int i = 0; i < png.size.x; ++i)
			{
				Palette& pal = png.pal[*(png.image + i + h * png.size.x)];

				Pixcel p;
				p.r = pal.red;
				p.g = pal.green;
				p.b = pal.blue;
				p.a = pal.alpha;
					
				image[i + h * width] = p;
				// FIXME:最適化
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		delete[] image;
	}
		
	Texture() {}
public:
	Texture(const std::string& path) {
		glGenTextures(1, &id_);
		name_ = getFileName(path);
		
		std::fstream fstr;
		fstr.open(path.c_str(), std::ios::in | std::ios::binary);
		if(fstr.is_open())
		{
			fstr.seekg(0, std::ios::end);
			std::streampos pos = fstr.tellg();
			fstr.seekg(0, std::ios::beg);

			char *buf = new char[pos];
			fstr.read(buf, pos);
			fstr.close();

			ngs::Png png_obj((png_bytep)buf, pos);
			delete[] buf;

			glBindTexture(GL_TEXTURE_2D, id_);
			size_ = png_obj.size;
			size_gl_.set(ngs::int2pow(size_.x), ngs::int2pow(size_.y));
			uv_.set((float)size_.x / (float)size_gl_.x, (float)size_.y / (float)size_gl_.y);

			GLint pack, unpack;
			glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
			glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			if(png_obj.type == PNG_COLOR_TYPE_PALETTE)
			{
				copy32i(png_obj, size_gl_.x, size_gl_.y);
			}
			else
			if(png_obj.type == PNG_COLOR_TYPE_RGB)
			{
				copy32rgb(png_obj, size_gl_.x, size_gl_.y);
			}
			else
			{
				copy32rgba(png_obj, size_gl_.x, size_gl_.y);
			}
				
			glPixelStorei(GL_PACK_ALIGNMENT, pack);
			glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);
		}
	}
	~Texture() {
		std::cout << "~Texture()" << std::endl;
		glDeleteTextures(1, &id_);
	}

	const Vec2<int>& size() const { return size_; }
	const Vec2<float>& uv() const { return uv_; }
	const std::string& name() const { return name_; }

	void bind() const {
		glBindTexture(GL_TEXTURE_2D, id_);

		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
};

}
