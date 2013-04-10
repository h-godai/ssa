/*
 *  AppMain.h
 *  OpenGLES-test2
 *
 *  Created by IKEDA KOHEI on 11/04/06.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef __cplusplus
#include <stdlib.h>
#include <stdint.h>
#include "libTS/appfw/appfw.hpp"
#include "libTS/appfw/drawframe_app.hpp"

namespace app {
  using namespace ts::appfw;
  inline drawframe_app& app_instance() { return drawframe_app::app_instance(); }
}

#endif
