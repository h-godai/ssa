// VrmlExporter.hpp

#ifndef APP_VRMLEXPORTER_HPP
#define APP_VRMLEXPORTER_HPP

#ifndef STDAFX_H
#include <string>
#include <iostream>
#endif

#include "Util/util.hpp"
#include "Util/ustring.hpp"
#include "Exporter.hpp"
#include "BuildingObject.hpp"

namespace app {

  using std::string;
  using namespace ts;

  class VrmlExporter : public Exporter {
  public:
    virtual ~VrmlExporter(){}


    // ƒrƒ‹1ŒÂ‚ðVRML‚Éo—Í‚·‚é
    void exportBuilding(const string& filename, BuildingObject::ptr building) {
      mstring imageFileName(filename + ".png");
      ofstream ofs(filename.c_str(), ios_base::binary);
      exportBuilding(ofs, building, imageFileName);
      ImageFile::SaveToFile(imageFileName, building->getTexture2D()->getImageBuffer());
    }

    void exportBuilding(ostream& os, BuildingObject::ptr building, const mstring& imageFileName) {

      vector<DrawPrimitive> objs;
      building->getDrawPrimitives(objs);
      mstring points("point [");
      mstring texcoord("point [");

      mstring coordIndex("coordIndex [");
      uint32_t coord = 0;

      BOOST_FOREACH(DrawPrimitive& obj, objs) {
        switch (obj.type) {
        case IRenderer::QUADS:
          for (size_t n = 0; n + 3 < obj.vertex.size(); n += 4) {
             mstring str;
             str.format(" %f %f %f, %f %f %f, %f %f %f, %f %f %f", 
               obj.vertex[n].x, obj.vertex[n].y, obj.vertex[n].z, 
               obj.vertex[n+1].x, obj.vertex[n+1].y, obj.vertex[n+1].z, 
               obj.vertex[n+2].x, obj.vertex[n+2].y, obj.vertex[n+2].z, 
               obj.vertex[n+3].x, obj.vertex[n+3].y, obj.vertex[n+3].z);
             if (coord != 0) points += ",";
             points += str.body();
             str.format(" %f %f, %f %f, %f %f, %f %f", 
               obj.texCoord[n].u,   1.0f - obj.texCoord[n].v,
               obj.texCoord[n+1].u, 1.0f - obj.texCoord[n+1].v,
               obj.texCoord[n+2].u, 1.0f - obj.texCoord[n+2].v,
               obj.texCoord[n+3].u, 1.0f - obj.texCoord[n+3].v);
             if (coord != 0) texcoord += ",";
             texcoord += str.body();
             str.format(" %d %d %d %d -1 ", coord, coord+1, coord+2, coord+3);
             coordIndex += str;
             coord += 4;
          }
          break;
        case IRenderer::TRIANGLES:
          for (size_t n = 0; n + 2 < obj.vertex.size(); n += 3) {
             mstring str;
             str.format(" %f %f %f, %f %f %f, %f %f %f", 
               obj.vertex[n].x, obj.vertex[n].y, obj.vertex[n].z, 
               obj.vertex[n+1].x, obj.vertex[n+1].y, obj.vertex[n+1].z, 
               obj.vertex[n+2].x, obj.vertex[n+2].y, obj.vertex[n+2].z);
             if (coord != 0) points += ",";
             points += str.body();
             str.format(" %f %f, %f %f, %f %f", 
               obj.texCoord[n].u,   1.0f - obj.texCoord[n].v,
               obj.texCoord[n+1].u, 1.0f - obj.texCoord[n+1].v,
               obj.texCoord[n+2].u, 1.0f - obj.texCoord[n+2].v);
             if (coord != 0) texcoord += ",";
             texcoord += str.body();
             str.format(" %d %d %d -1 ", coord, coord+1, coord+2);
             coordIndex += str;
             coord += 3;
          }
          break;
        }
      }
      points += "]";
      texcoord += "]";
      coordIndex += "]";

      os << "#VRML V2.0 utf8" << std::endl;
      os << "Transform {" << std::endl;
      os << "  children [" << std::endl;
      os << "    Shape {" << std::endl;
      os << "      geometry IndexedFaceSet {" << std::endl;
      os << "        coord Coordinate {" << std::endl;
      os << "          " << points.c_str() << std::endl;
      os << "        }" << std::endl;
      os << "        texCoord TextureCoordinate {" << std::endl;
      os << "          " << texcoord.c_str() << std::endl;
      os << "        }" << std::endl;
      os << "        " << coordIndex.c_str() << std::endl;
      os << "      }" << std::endl;
      os << "      appearance Appearance {" << std::endl;
      os << "        texture ImageTexture { url [\"" << imageFileName.c_str() << "\"] }" << std::endl;
      os << "      }" << std::endl;
      os << "    }" << std::endl;
      os << "  ]" << std::endl;
      os << "  translation " << building->getOrigin().x << " " << building->getOrigin().y << " 0" << std::endl;
      os << "}" << std::endl;



    }


  };

}

#endif;