/* IAudio.hpp




*/

#ifndef TS_PLATFORMDEPENDENT_IAUDIO_HPP
#define TS_PLATFORMDEPENDENT_IAUDIO_HPP

#include "libTS/Common/Common.h"

#include "libTS/Math/Vector.h"
#include <boost/any.hpp>



namespace ts {

  class ISound {
  public:
    typedef ISound* ptr;
    enum SoundType {
      Sound_3D
    , Sound_2D
    };

    virtual void play(uint32_t id, uint32_t freq=0, int32_t vol=0, uint32_t flag=0) = 0;
    virtual void play3D(uint32_t id, const Vector3D& pos, uint32_t freq=0, int32_t vol=0, uint32_t flag=0) = 0;
    virtual void setListenerPosition(const Vector3D& pos, const Vector3D& dis, const Vector3D& velocity) = 0;
    virtual void setPosition(uint32_t id, const Vector3D& pos, const Vector3D& spd);
    virtual uint32_t getVoiceMax() = 0; // ìØéûî≠ê∫êî
    virtual uint32_t load(const std::string& filename, int32_t voices, SoundType type=Sound_3D) = 0;
    virtual bool loadTo(uint32_t id, const std::string& fn, SoundType type, bool) = 0;
    virtual void stopAll() = 0;
    virtual void stop(uint32_t id) = 0;

  };

  class IMusic {
  public:
    typedef IMusic* ptr;
    virtual void play(uint32_t id, int32_t vol) = 0;
  };

  class IAudio {
  public:
    typedef IAudio* ptr;
    static ptr Create(uint32_t devnum = 0);
    static void Destroy(ptr*);

    virtual void initialize(boost::any& frame) = 0;
    virtual ISound::ptr getSound() = 0;
    virtual IMusic::ptr getMusic() = 0;




  };


} // namespace ts
#endif

