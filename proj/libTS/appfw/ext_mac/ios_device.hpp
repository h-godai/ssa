/* -*-tab-width:4-*-
 *  ios_device.hpp
 *  RiseOut
 *
 *  Created by IKEDA KOHEI on 11/05/02.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TS_APPFW_IOS_DEVICE_HPP
#define TS_APPFW_IOS_DEVICE_HPP


namespace ts {
namespace appfw {
namespace mac {

// デバイスの向き
  enum device_orientation {
    unknown_orientation
  , orientation_portrait               // angle=0
  , orientation_portrait_upside_down   // angle=180
  , orientation_landscape_left         // angle=270
  , orientation_landscape_right        // angle=90
  , orientation_faceup                 // angle=0
  , orientation_facedown               // angle=0
  };
  
}}} // ts::appfw::ios
#endif
