/* -*-tab-width:4-*-
   libTS Xcode stub
 */

#ifndef LIBTS_APPFW_SOUNDFW_HPP
#define LIBTS_APPFW_SOUNDFW_HPP

#include "libTS/Common/Common.h"
#include <stdint.h>
#include <iostream>
#include <string>
#include <deque>
#include <stdexcept>
//#include <boost/shared_ptr.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include "libTS/Util/Logger.h"

namespace ts {
namespace appfw {
  namespace sounds {
	typedef boost::mutex::scoped_lock scoped_lock;


	enum source_type {
	  sound_type,
	  music_type
	};
	enum command_type {
	  cmd_play
	  , cmd_stop
	  , cmd_mute
	  , cmd_pause
	  , cmd_resume // mute or pauseを解除
	};
	struct audio_param {
	  source_type type_;
	  uint32_t id_;
	  float volume_;
	  float pan_lr; // Left(+)-Right(-) pan
	  float pan_fr; // Front(+)-Rear(-) pan
	  float pan_ud; // Up(+)-Down(-) pan
	  float duration_; // msec 0 is none
	  uint32_t repeat_; // repeart count; 0 is infinity
	  command_type cmd_;
	  audio_param(source_type t, uint32_t id, command_type cmd = cmd_play, int32_t repeat = 1)
		: type_(t)
		, id_(id)
		, volume_(10.0f)
		, pan_lr(0.0f)
		, pan_fr(0.0f)
		, pan_ud(0.0f)
		, duration_(0.0f)
		, repeat_(repeat)
		, cmd_(cmd)
	  {}
	};


	// オーディオコントローラ
	class audio_ctrl {
	public:
	  enum thread_type {
		single_thread_model    // audioのAPIはシングルスレッドで呼び出す
		, thread_safe_model    // audioのAPIはスレッドセーフでマルチスレッドから直接呼び出す
	  };

	private:
	  std::deque<audio_param> queue_;
	  boost::mutex guard_;
	  audio_ctrl ()
		{}
	  ~audio_ctrl()
		{}
	public:

	  typedef void play_sound_t(const audio_param& param);

	  void initialize(std::vector<std::string> soundlist_) {
	  }

	  // サウンドを再生するリストを帰す
	  void control_hook(std::vector<audio_param>& list) {
		scoped_lock lock(guard_);
		std::copy(queue_.begin(), queue_.end(), std::back_inserter(list));
		queue_.clear();
	  }

	  void operator << (const audio_param& param) {
		scoped_lock lock(guard_);
		queue_.push_back(param);
	  }

	  // シングルトンアクセス
	  static audio_ctrl& instance() {
		static audio_ctrl ins;
		return ins;
	  }

	};


	// 効果音
	class sound_player {
	  audio_ctrl& audio_;
	public:
	  sound_player(audio_ctrl& audio)
		: audio_(audio)
	  {}
	  void play(uint32_t id) {
		audio_ << audio_param(sound_type, id);
	  }
	  void play(audio_param& param) {
		audio_ << param;
	  }
	};

	// BGM
	class music_player {
	  audio_ctrl& audio_;
	  uint32_t playingId_;
	  static const uint32_t no_play_id = 0xffffffff;
	public:
	  volatile float current_music_time_; // 現在の再生時間

	  music_player(audio_ctrl& audio)
		: audio_(audio)
		, playingId_(no_play_id)
		, current_music_time_(0.0f)
	  {}
	  enum PlayStyle {
		no_repeat
		, repeat
	  };
	  enum PlayStopStyle {
		stop_now
		, fade_out
	  };
	  void start(uint32_t musicId, PlayStyle style) {
		stop(fade_out);
		audio_ << audio_param(music_type, musicId, cmd_play, style == repeat ? 0 : 1);
		playingId_ = musicId;
	  }
	  void stop(PlayStopStyle style=stop_now, uint32_t param = 0) {
		if (playingId_ != no_play_id) {
		  audio_ << audio_param(music_type, playingId_, cmd_stop);
		  playingId_ = no_play_id;
		}
	  }
	  void pause() {
		audio_ << audio_param(music_type, playingId_, cmd_pause);
	  }
	  void resume() {
		audio_ << audio_param(music_type, playingId_, cmd_resume);
	  }
	  bool is_playing() const {
		return playingId_ != no_play_id;
	  }
	};





  }
}
} // namespace ts


// #include "libTS/appfw/appfw.hpp"
#endif
