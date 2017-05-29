// slide_selector.hpp -*-tab-width:4-*-
//
//
//

#ifndef TS_APPFW_GADGET_SLIDE_SELECTOR_HPP
#define TS_APPFW_GADGET_SLIDE_SELECTOR_HPP

#include "libTS/appfw/gl/glfw.hpp"
#include "libTS/appfw/gl/button.hpp"
#include "libTS/appfw/gl/effect.hpp"
#include "libTS/math/math.hpp"

namespace ts {
namespace appfw {
  using namespace std;
  using namespace boost;

  struct slide_selector {
	gl::glfw& gr_;
    vector<gl::button2d> btns_;
    vector<float> oldscale_;
	vector<float> movepos_;
    int32_t current_;
    float ypos_;
    float scalebase_;
    bool selected_;
    float dragvec_;
	int32_t depthlayer_;
    slide_selector(gl::glfw& gr, float y, float sc, int32_t dep)
      : gr_(gr)
      , current_(0)
      , ypos_(y)
      , scalebase_(sc)
      , selected_(false)
      , dragvec_(0.0f)
	  , depthlayer_(dep)
    {
    }

	void set_current(int32_t cur) {
	  assert(cur < static_cast<int>(btns_.size()));
	  current_ = cur;
	}
	void set_current(const string& name) {
	  for (uint32_t cur = 0; cur < btns_.size(); ++cur) {
		if (btns_[cur].name() == name) {
		  current_ = cur;
		  return;
		}
	  }
	}

    void add(const gl::button2d& b) {
      btns_.push_back(b);
	  btns_.back().set_disable_tapeffect();
      oldscale_.push_back(1.0f);
      show_first(btns_.size()-1);
	  movepos_.resize(btns_.size());
	  *movepos_.rbegin() = 0.0f;
    }

    void show_first(size_t i) {
	  using namespace gl;
	  button2d& b =  btns_[i];
      gr_.show_object(b
					 , positioning(align_center_h, 1024)
					 , depthlayer_
					 , scale_to(0.5f,0.5f,0.1f)|transparent(1.0f));
      oldscale_[i] = 0.5f;
      show(i);
    }

    void show_all() {
      for (uint32_t i = 0; i < btns_.size(); ++i) {
		show(i);
      }
    }

    void show(int32_t i) {
	  using namespace gl;
      button2d& b =  btns_[i];
      int32_t p = abs(i - current_);
      // 0 - 1.0     1/1 + 0.2
      // 1 - 0.7     1/2 + 0.2
      // 2 - 0.53    1/3 + 0.2
      // 3 - 0.45    1/4 + 0.2
      float w = gr_.screen_size().width_;
      float scale = p == 0 ? 1.0f : (0.2f + (1.0f / (p+1)));
      float xpos = w/2.0f - ((1.0f-scale) * w/1.3f);
	  float btnw = b.image().rect().width();
      if  (i > current_) xpos = w - xpos;
      gr_.effect_image(b.name()
					   , scale_to(oldscale_[i], scale*scalebase_, 0.4f)
					   | move_to3(pos3_f(xpos - btnw/2.0f, ypos_, float(p)/4.0f), 0.3f)
					   | transparent(scale)
					   );
      oldscale_[i] = scale*scalebase_;
    }


    bool drag_check() {
      bool result = false;
	  bool neg = false;
      //dragvec_ = 0.0f;
      for (uint32_t i = 0; i < btns_.size(); ++i) {
        math::vec2_f d = btns_[i].get_drag_vec();
		movepos_[i] += d.x_;
		if (fabs(movepos_[i])  > 30.0f) {
		  TS_DEBUGLOG("dragvec:%f\n",movepos_[i]);
		  result = true;
		  neg = sign(dragvec_) != sign(movepos_[i]);
		  dragvec_= movepos_[i];
		  btns_[i].clear_drag_vec();
		  if (fabs(movepos_[i]) > 2000.0f) neg = true;
		  movepos_[i] = 0.0f;
		  break;
		}
      }
      if (result) {
		//TS_DEBUGLOG("progress=%f\n", gr_.get_effect(btns_[current_].name())->progress(0));
		if (neg || gr_.get_effect(btns_[current_].name())->progress(0) > 0.40f) {
		  if (dragvec_ > 0) ++current_; else --current_;
		  if (current_ < 0) current_ = 0;
		  if (current_ >= static_cast<int>(btns_.size())) current_ = static_cast<int>(btns_.size())-1;
		}
	  }
      return result;
    }


    bool hit_check(const string& eventname) {
	  using namespace gl;
      bool hit = false;
      selected_ = false;

      for (int32_t i = 0; i < static_cast<int>(btns_.size()); ++i) {
		button2d& b =  btns_[i];
		if (eventname == b.name().name_) {
		  if (i != current_) {
			if (i > current_) ++current_; else --current_;
			hit = true;
			break;
		  }
		  else {
			// 選択された！
			selected_ = true;
			return true;
		  }
		}
      }
      return hit;
    }

	bool is_selected() const { return selected_; }

	void hide_all(gl::effect sel_ef = gl::effect(), gl::effect unsel_ef = gl::effect()) {
	  for (int32_t j = 0; j < static_cast<int>(btns_.size()); ++j) {
		if (j == current_) {
		  gr_.hide_image(btns_[j].name(), sel_ef);
		}
		else {
		  if (unsel_ef)
			gr_.hide_image(btns_[j].name(), unsel_ef);
		  else
			gr_.hide_image(btns_[j].name(), sel_ef);
		}
	  }
	}

  };

}} // namespace
#endif
