// Exporter.hpp

#ifndef APP_EXPORTER_HPP
#define APP_EXPORTER_HPP

#ifndef STDAFX_H
#include <string>
#endif

#include "Util/util.hpp"
#include "Util/ustring.hpp"

namespace app {

  using std::string;
  using namespace ts;

  class Exporter {
  public:
    virtual ~Exporter(){}

  };

}

#endif;