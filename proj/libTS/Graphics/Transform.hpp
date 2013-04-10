#ifndef TS_GRAPHICS_TRANSFORM_HPP
#define TS_GRAPHICS_TRANSFORM_HPP


#include "libTS/Common/Common.h"
#include "libTS/math/matrix.hpp"

namespace ts {

class Transform {
    Matrix   m_matrix;
    bool m_hasMatrix;
  public:
    Transform() : m_hasMatrix(false) {}
    Transform(const Matrix& m) : m_matrix(m), m_hasMatrix(true) {}
    void setMatrix(const Matrix& m) {
      m_matrix = m;
      m_hasMatrix = true;
    }
    void clear() {
      m_matrix.identity();
      m_hasMatrix = false; 
    }

    void preRendering(IRenderer* gl) { 
      if (m_hasMatrix) {
        gl->pushMatrix();
        gl->transform(m_matrix);
      }
    }
    void postRendering(IRenderer* gl) { 
      if (m_hasMatrix) {
        gl->popMatrix();
      }
    }

  };


} // namespace

#endif
