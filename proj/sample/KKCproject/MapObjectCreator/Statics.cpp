#include "stdafx.h"


#include "Importer.hpp"
#include "BuildingObject.hpp"
#include "MapObjectCreator.hpp"

namespace app {


  uint32_t BuildingObject::m_lastId = 0;
  mstring Importer::m_rootDirectory;


  IUserInterfaceHandler& MainApp() {
    return MapObjectCreator::getInstance();
  }


}
