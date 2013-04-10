
#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include "co_vec2.hpp"
#include "co_mouse.hpp"
#include "test.hpp"

namespace ngs {

class App {
	int width_, height_;
	int width_org_, height_org_;
	float aspect_;
	Test *test_;
	std::string path_;
	
	App() : test_() {
		std::cout << "App()" << std::endl;
	}

	void size(const int w, const int h)
	{
		width_ = w;
		height_ = h;
		aspect_ = (float)w / (float)h;
	}
	
public:
	static App& instance() {
		static App app;
		return app;
	}
	~App() {
		std::cout << "~App()" << std::endl;
		if(test_) delete test_;
	}

	void initialize(const int w, const int h, const std::string& path)
	{
		size(w, h);
		width_org_ = w;
		height_org_ = h;
		path_ = path;
	}
	
	void resize(const int w, const int h)
	{
		float sx = (float)w / (float)width_;
		float sy = (float)h / (float)height_;

		size(w, h);
		glViewport(0, 0, w, h);

		Mouse& mouse = Mouse::instance();
		mouse.resize(Vec2<int>(w, h), Vec2<int>(0, 0), Vec2<int>(w, h));

		test_->resize(w, h, sx, sy);
	}
	
	void resizeFit(const int w, const int h)
	{
		glViewport(0, 0, w, h);
		
		Mouse& mouse = Mouse::instance();
		mouse.resize(Vec2<int>(width_, height_), Vec2<int>(0, 0), Vec2<int>(w, h));

		test_->resize(width_, height_);
	}
	
	void resizeKeepAspect(const int w, const int h)
	{
		bool a = ((float)w / (float)h) < aspect_;

		int width = a ? w : (float)h * aspect_ + 0.5;
		int height = a ? (float)w / aspect_ + 0.5 : h;

		int ofs_x = (w - width) / 2;
		int ofs_y = (h - height) / 2;
		glViewport(ofs_x, ofs_y, width, height);

		Mouse& mouse = Mouse::instance();
		mouse.resize(Vec2<int>(width_, height_), Vec2<int>(ofs_x, ofs_y), Vec2<int>(w, h));

		test_->resize(width, height);
	}
	
	void step()
	{
		test_->step();
	}
	
	void draw()
	{
		test_->draw();
	}

	void reset()
	{
		if(test_) delete test_;
		test_ = new Test(width_org_, height_org_, path_);
		size(width_org_, height_org_);									// 内部的に初期サイズに戻す
	}

	void close()
	{
		if(test_)
		{
			delete test_;
			test_ = 0;
		}
	}

	const std::string& path() const
	{
		return path_;
	}
};

}
