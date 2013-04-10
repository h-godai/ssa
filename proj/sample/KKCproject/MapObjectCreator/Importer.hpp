// Importer.hpp

#ifndef APP_IMPORTER_HPP
#define APP_IMPORTER_HPP

#ifndef STDAFX_H
#include <string>
#endif

#include "Util/util.hpp"
#include "Util/ustring.hpp"

namespace app {

  using std::string;
  using namespace ts;

  class Importer {
  public:
    virtual ~Importer(){}

    static const mstring& GetRootDirectory() { return m_rootDirectory; }
  protected:
    static mstring m_rootDirectory;
  };

}

#endif;