// DrawBldgObj.hpp
//
// 建物用の描画オブジェクト

#pragma once


#ifndef STDAFX_H
#include <boost/shared_ptr.hpp>
#endif
#include "IBuilding3D.hpp"
#include "Math/Geometric.hpp"
#include "TextureAllocator.hpp"


namespace app {
  using namespace ts;
  using boost::shared_ptr;

  
  // 描画オブジェクトクラス
  struct DrawBldgObj : public DrawPrimitive {

      DrawBldgObj() {}
      DrawBldgObj(IRenderer::PrimitiveType type_)
        : DrawPrimitive(type_) {}
      DrawBldgObj(IRenderer::PrimitiveType type_, vector<Vector3D>& vertex_)
        : DrawPrimitive(type_, vertex_) {}
      virtual ~DrawBldgObj() {}


      // 各頂点にテクスチャ座標を割り当てる
      void setTextureCoord(MapDirection /*mapDir*/,
                           TextureAllocator& texAllocator) {
        if (texCoord.size() != vertex.size()) {
          texCoord.resize(vertex.size());
        }
        switch (type) {
        case IRenderer::QUADS:
        case IRenderer::TRIANGLES:
          for (size_t n = 0; n + 2 < vertex.size(); n += type == IRenderer::QUADS ? 4 : 3) {
            texAllocator.setTexCoords(&vertex[n], &texCoord[n], type == IRenderer::QUADS ? 4 : 3);

          }
          break;
        case IRenderer::TRIANGLE_FAN:
          if (vertex.size() > 0) {
            texAllocator.setTexCoords(&vertex[0], &texCoord[0], static_cast<uint32_t>(vertex.size()));
          }
          break;
        case IRenderer::TRIANGLE_STRIP:
#if 0
          if (isFrontFace(0, 1, 2, matrix)) {
            for (size_t n = 0; n < vertex.size(); ++n) {
              texCoord[n] = getTexCoords(vertex[n], matrix,  bbRect2D.getRect(), texOffset);
            }
          }
#endif
          break;
        }

      }

      void doRendering(IRenderer* gl) {
        beginPrimitive begin(gl, type);
        if (texCoord.empty()) {
          for (size_t n = 0; n < vertex.size(); ++n) {
              gl->vertex(vertex[n]);
          }
        }
        else {
          for (size_t n = 0; n < vertex.size(); ++n) {
              gl->texcoord(texCoord[n]);
              gl->vertex(vertex[n]);
          }
        }
      }
      // ワイヤーフレームでの描画
      void doRenderingWireFrame(IRenderer* gl) {
        switch (type) {
        case IRenderer::QUADS:
          for (size_t n = 0; n + 3 < vertex.size(); n += 4) {
            beginPrimitive begin(gl, IRenderer::LINE_LOOP); 
            gl->vertex(vertex[n]);
            gl->vertex(vertex[n+1]);
            gl->vertex(vertex[n+2]);
            gl->vertex(vertex[n+3]);
          }
          break;
        case IRenderer::TRIANGLES:
          for (size_t n = 0; n + 2 < vertex.size(); n += 3) {
            beginPrimitive begin(gl, IRenderer::LINE_LOOP); 
            gl->vertex(vertex[n]);
            gl->vertex(vertex[n+1]);
            gl->vertex(vertex[n+2]);
          }
          break;
        case IRenderer::TRIANGLE_FAN:
          {
            beginPrimitive begin(gl, IRenderer::LINES); 
            for (size_t n = 1; n < vertex.size(); ++n) {
              gl->vertex(vertex[0]);
              gl->vertex(vertex[n]);
            }
          }
          break;
        case IRenderer::TRIANGLE_STRIP:
#if 0
          beginPrimitive begin(gl, IRenderer::LINES); 
          for (size_t n = 1; n < vertex.size(); ++n) {
            gl->vertex(vertex[n]);
            gl->vertex(vertex[n-1]);
            if (n > 1) {
              gl->vertex(vertex[n]);
              gl->vertex(vertex[n-2]);
            }
          }
#endif
          break;
        }

      }

      // 平面での分割
      // p1,p2,p3 分割する平面
      // o1,o2 分割後のオブジェクト
      // crossVertex 断面の頂点集合
      void splitSurface(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3,
                        vector<DrawBldgObj>& o1,
                        vector<DrawBldgObj>& o2,
                        Vertex3DSet& crossVertex)
      {
        DrawBldgObj triangles1(IRenderer::TRIANGLES);
        DrawBldgObj triangles2(IRenderer::TRIANGLES);
        DrawBldgObj quads1(IRenderer::QUADS);
        DrawBldgObj quads2(IRenderer::QUADS);

        switch (type) {
        case IRenderer::QUADS:
          for (size_t n = 0; n + 3 < vertex.size(); n += 4) {
            const Vector3D& v1 = vertex[n];
            const Vector3D& v2 = vertex[n+1];
            const Vector3D& v3 = vertex[n+2];
            const Vector3D& v4 = vertex[n+3];
            Vector3D c12, c23, c34, c41;
            bool r1,r2,r3,r4;
            r1 = math::LinePlaneCrossPoint(v1, v2, p1, p2, p3, c12, true);
            r2 = math::LinePlaneCrossPoint(v2, v3, p1, p2, p3, c23, true);
            r3 = math::LinePlaneCrossPoint(v3, v4, p1, p2, p3, c34, true);
            r4 = math::LinePlaneCrossPoint(v4, v1, p1, p2, p3, c41, true);
            if (r4 && r1) {
              // v4-v1とv1-v2に面があるので、triangleとfanに分割される
              DrawBldgObj fan(IRenderer::TRIANGLE_FAN);
              fan.vertex.push_back(v3);
              fan.vertex.push_back(v4);
              fan.vertex.push_back(c41);
              fan.vertex.push_back(c12);
              fan.vertex.push_back(v2);
              DrawBldgObj* t;
              if (SideCheck3D(v1, p1, p2, p3) > 0) {
                o2.push_back(fan);
                t = &triangles1;
              }
              else {
                o1.push_back(fan);
                t = &triangles2;
              }
              t->vertex.push_back(v1);
              t->vertex.push_back(c12);
              t->vertex.push_back(c41);
              crossVertex.insert(c12);
              crossVertex.insert(c41);

            }
            else if (r1 && r2) {
              // v1-v2とv2-v3に面があるので、triangleとfanに分割される
              DrawBldgObj fan(IRenderer::TRIANGLE_FAN);
              fan.vertex.push_back(v4);
              fan.vertex.push_back(v1);
              fan.vertex.push_back(c12);
              fan.vertex.push_back(c23);
              fan.vertex.push_back(v3);
              DrawBldgObj* t;
              if (SideCheck3D(v2, p1, p2, p3) > 0) {
                o2.push_back(fan);
                t = &triangles1;
              }
              else {
                o1.push_back(fan);
                t = &triangles2;
              }
              t->vertex.push_back(v2);
              t->vertex.push_back(c23);
              t->vertex.push_back(c12);
              crossVertex.insert(c12);
              crossVertex.insert(c23);
            }
            else if (r2 && r3) {
              // v2-v3とv3-v4に面があるので、triangleとfanに分割される
              DrawBldgObj fan(IRenderer::TRIANGLE_FAN);
              fan.vertex.push_back(v1);
              fan.vertex.push_back(v2);
              fan.vertex.push_back(c23);
              fan.vertex.push_back(c34);
              fan.vertex.push_back(v4);
              DrawBldgObj* t;
              if (SideCheck3D(v3, p1, p2, p3) > 0) {
                o2.push_back(fan);
                t = &triangles1;
              }
              else {
                o1.push_back(fan);
                t = &triangles2;
              }
              t->vertex.push_back(v3);
              t->vertex.push_back(c34);
              t->vertex.push_back(c23);
              crossVertex.insert(c34);
              crossVertex.insert(c23);
            }
            else if (r3 && r4) {
              // v3-v4とv4-v1に面があるので、triangleとfanに分割される
              DrawBldgObj fan(IRenderer::TRIANGLE_FAN);
              fan.vertex.push_back(v2);
              fan.vertex.push_back(v3);
              fan.vertex.push_back(c34);
              fan.vertex.push_back(c41);
              fan.vertex.push_back(v1);
              DrawBldgObj* t;
              if (SideCheck3D(v4, p1, p2, p3) > 0) {
                o2.push_back(fan);
                t = &triangles1;
              }
              else {
                o1.push_back(fan);
                t = &triangles2;
              }
              t->vertex.push_back(v4);
              t->vertex.push_back(c41);
              t->vertex.push_back(c34);
              crossVertex.insert(c41);
              crossVertex.insert(c34);
            }
            else if (r1 & r3) {
              // v1-v2 と v3 - v4に面があるので2つのquadsに分割される
              DrawBldgObj* q1;
              DrawBldgObj* q2;
              if (SideCheck3D(v1, p1, p2, p3) > 0) {
                q1 = &quads1;
                q2 = &quads2;
              }
              else {
                q1 = &quads2;
                q2 = &quads1;
              }
              q1->vertex.push_back(v1);
              q1->vertex.push_back(c12);
              q1->vertex.push_back(c34);
              q1->vertex.push_back(v4);
              q2->vertex.push_back(v3);
              q2->vertex.push_back(c34);
              q2->vertex.push_back(c12);
              q2->vertex.push_back(v2);
              crossVertex.insert(c12);
              crossVertex.insert(c34);
            }
            else if (r2 && r4) {
              // v2-v3 と v4 - v1に面があるので2つのquadsに分割される
              DrawBldgObj* q1;
              DrawBldgObj* q2;
              if (SideCheck3D(v1, p1, p2, p3)) {
                q1 = &quads1;
                q2 = &quads2;
              }
              else {
                q1 = &quads2;
                q2 = &quads1;
              }
              q1->vertex.push_back(v1);
              q1->vertex.push_back(v2);
              q1->vertex.push_back(c23);
              q1->vertex.push_back(c41);
              q2->vertex.push_back(v3);
              q2->vertex.push_back(v4);
              q2->vertex.push_back(c41);
              q2->vertex.push_back(c23);
              crossVertex.insert(c23);
              crossVertex.insert(c41);
            }
            else
            {
              // 分割はされない
              if (SideCheck3D(v1, p1, p2, p3) > 0) {
                quads1.vertex.push_back(v1);
                quads1.vertex.push_back(v2);
                quads1.vertex.push_back(v3);
                quads1.vertex.push_back(v4);
              }
              else {
                quads2.vertex.push_back(v1);
                quads2.vertex.push_back(v2);
                quads2.vertex.push_back(v3);
                quads2.vertex.push_back(v4);
              }
            }
          }
          break;
        case IRenderer::TRIANGLES:
          for (size_t n = 0; n + 2 < vertex.size(); n += 3) {
            const Vector3D& v1 = vertex[n];
            const Vector3D& v2 = vertex[n+1];
            const Vector3D& v3 = vertex[n+2];
            Vector3D c12, c23, c31;
            bool r1,r2,r3;
            r1 = math::LinePlaneCrossPoint(v1, v2, p1, p2, p3, c12, true);
            r2 = math::LinePlaneCrossPoint(v2, v3, p1, p2, p3, c23, true);
            r3 = math::LinePlaneCrossPoint(v3, v1, p1, p2, p3, c31, true);
            if (r1 && r2) {
              // v1-v2とv2-v3に面がある
              DrawBldgObj* t1;
              DrawBldgObj* q2;
              if (SideCheck3D(v2, p1, p2, p3) > 0) {
                t1 = &triangles1;
                q2 = &quads2;
              }
              else {
                t1 = &triangles2;
                q2 = &quads1;
              }
              t1->vertex.push_back(v2);
              t1->vertex.push_back(c23);
              t1->vertex.push_back(c12);
              q2->vertex.push_back(v1);
              q2->vertex.push_back(c12);
              q2->vertex.push_back(c23);
              q2->vertex.push_back(v3);
              crossVertex.insert(c12);
              crossVertex.insert(c23);
            }
            else if (r2 && r3) {
              // v2-v3とv3-v1に面がある
              DrawBldgObj* t1;
              DrawBldgObj* q2;
              if (SideCheck3D(v3, p1, p2, p3) > 0) {
                t1 = &triangles1;
                q2 = &quads2;
              }
              else {
                t1 = &triangles2;
                q2 = &quads1;
              }
              t1->vertex.push_back(v3);
              t1->vertex.push_back(c31);
              t1->vertex.push_back(c23);
              q2->vertex.push_back(v2);
              q2->vertex.push_back(c23);
              q2->vertex.push_back(c31);
              q2->vertex.push_back(v1);
              crossVertex.insert(c23);
              crossVertex.insert(c31);
            }
            else if (r3 && r1) {
              // v3-v1とv1-v2に面がある
              DrawBldgObj* t1;
              DrawBldgObj* q2;
              if (SideCheck3D(v1, p1, p2, p3) > 0) {
                t1 = &triangles1;
                q2 = &quads2;
              }
              else {
                t1 = &triangles2;
                q2 = &quads1;
              }
              t1->vertex.push_back(v1);
              t1->vertex.push_back(c12);
              t1->vertex.push_back(c31);
              q2->vertex.push_back(v3);
              q2->vertex.push_back(c31);
              q2->vertex.push_back(c12);
              q2->vertex.push_back(v2);
              crossVertex.insert(c12);
              crossVertex.insert(c31);
            }
            else 
            {
              // 分割はされない
              if (SideCheck3D(v1, p1, p2, p3) > 0) {
                triangles1.vertex.push_back(v1);
                triangles1.vertex.push_back(v2);
                triangles1.vertex.push_back(v3);
              }
              else {
                triangles2.vertex.push_back(v1);
                triangles2.vertex.push_back(v2);
                triangles2.vertex.push_back(v3);
              }
            }
          }
          break;
        case IRenderer::TRIANGLE_FAN:
          {
            SideType st = OnSurface;
            for (uint32_t n = 0; n < vertex.size(); ++n) {
              if (st == OnSurface) {
                st = SideCheck3D(vertex[n], p1, p2, p3);
              }
              else {
                SideType s = SideCheck3D(vertex[n], p1, p2, p3);
                if (s != OnSurface && s != st) {
                  // 違う面があるのでtriangleに分解する
                  DrawBldgObj tri(IRenderer::TRIANGLES);
                  for (uint32_t i = 1; i < vertex.size() - 1; ++i) {
                    tri.vertex.push_back(vertex[0]);
                    tri.vertex.push_back(vertex[i]);
                    tri.vertex.push_back(vertex[i+1]);
                  }
                  tri.splitSurface(p1, p2, p3, o1, o2, crossVertex);
                  return;
                }
              }
            }
            if (st != BackSide) {
              o1.push_back(*this);
            }
            else {
              o2.push_back(*this);
            }
            return;    
          }
          break;
        default:
        case IRenderer::TRIANGLE_STRIP:
          if (SideCheck3D(vertex[0], p1, p2, p3) > 0) {
            o1.push_back(*this);
          }
          else {
            o2.push_back(*this);
          }
          return;
        }
        o1.push_back(triangles1);
        o1.push_back(quads1);
        o2.push_back(triangles2);
        o2.push_back(quads2);
      }

    };

}

// bstream用のオペレーター定義
namespace ts {
  template <> inline    const bstream& operator >> (const bstream& bs, DrawBldgObj& s) {
    bs >> s.type;
    bs >> s.vertex;
    bs >> s.texCoord;
    bs >> s.normal;
    return bs; 
  }
  template <> inline    bstream& operator << (bstream& bs, const DrawBldgObj& s) {
    bs << static_cast<uint32_t>(s.type);
    bs << s.vertex;
    bs << s.texCoord;
    bs << s.normal;
    return bs; 
  }
}

