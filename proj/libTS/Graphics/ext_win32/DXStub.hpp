// -*-tab-width:4-*-
//
// DirectXÇ∆å›ä∑ê´Çï€Ç¬ÇΩÇﬂÇÃÉXÉ^Éu

#ifndef TS_GRAPHICS_EXTWIN32_DXSTUB_HPP
#define TS_GRAPHICS_EXTWIN32_DXSTUB_HPP

#include "libTS/Image/Color.hpp"
#include "libTS/Graphics/GraphicsObject.hpp"
#include "libTS/Math/Matrix.h"
#include "libTS/Graphics/vertices.hpp"

namespace ts {
namespace dx {

  typedef float D3DVALUE;
  struct D3DCOLORVALUE {
	union {
	  D3DVALUE r;
	  D3DVALUE dvR;
	};
	union {
	  D3DVALUE g;
	  D3DVALUE dvG;
	};
	union {
	  D3DVALUE b;
	  D3DVALUE dvB;
	};
	union {
	  D3DVALUE a;
	  D3DVALUE dvA;
	};


    void operator = (const ts::ColorRGBA& c) { r = c.getRf(); g = c.getGf(); b = c.getBf(); a = c.getAf(); }
    void operator = (const ts::ColorRGB& c) { r = c.getRf(); g = c.getGf(); b = c.getBf(); }
    operator ts::ColorRGBA () const { return ts::ColorRGBA(r,g,b,a); }
    operator ts::ColorRGB () const { return ts::ColorRGB(r,g,b); }
  };

  struct D3DMATERIAL7 {
    union {
      D3DCOLORVALUE   diffuse;        /* Diffuse color RGBA */
      D3DCOLORVALUE   dcvDiffuse;
    };
    union {
      D3DCOLORVALUE   ambient;        /* Ambient color RGB */
      D3DCOLORVALUE   dcvAmbient;
    };
    union {
      D3DCOLORVALUE   specular;       /* Specular 'shininess' */
      D3DCOLORVALUE   dcvSpecular;
    };
    union {
      D3DCOLORVALUE   emissive;       /* Emissive color RGB */
      D3DCOLORVALUE   dcvEmissive;
    };
    union {
      D3DVALUE        power;          /* Sharpness if specular highlight */
      D3DVALUE        dvPower;
    };
    D3DMATERIAL7() {}
    D3DMATERIAL7(const ts::gobj::Material& ma) {
      diffuse = ma.diffuse_;
      ambient = ma.ambient_;
      specular = ma.specular_;
      emissive = ma.emissive_;
      power = ma.shininess_;
    }
    operator ts::gobj::Material () {
      ts::gobj::Material ma;
      ma.diffuse_ = diffuse;
      ma.ambient_ = ambient ;
      ma.specular_ = specular;
      ma.emissive_ = emissive;
      ma.shininess_ = power;
      return ma;
    }
      
  };
  typedef D3DMATERIAL7* LPD3DMATERIAL7;
  
  
  struct D3DVERTEX {
    enum { type =  VertexType_D3DVERTEX };
    union {
	  D3DVALUE     x;             /* Homogeneous coordinates */
	  D3DVALUE     dvX;
    };
    union {
	  D3DVALUE     y;
	  D3DVALUE     dvY;
    };
    union {
	  D3DVALUE     z;
	  D3DVALUE     dvZ;
    };
    union {
	  D3DVALUE     nx;            /* Normal */
	  D3DVALUE     dvNX;
    };
    union {
	  D3DVALUE     ny;
	  D3DVALUE     dvNY;
    };
    union {
	  D3DVALUE     nz;
	  D3DVALUE     dvNZ;
    };
    union {
	  D3DVALUE     tu;            /* Texture coordinates */
	  D3DVALUE     dvTU;
    };
    union {
	  D3DVALUE     tv;
	  D3DVALUE     dvTV;
    };

    operator ts::Vector3D () const {
      return ts::Vector3D(x,y,z);
    }
  };


  struct D3DMATRIX {
    union {
       struct {
            D3DVALUE        _11, _12, _13, _14;
            D3DVALUE        _21, _22, _23, _24;
            D3DVALUE        _31, _32, _33, _34;
            D3DVALUE        _41, _42, _43, _44;

        };
        D3DVALUE m[4][4];
    };
    D3DMATRIX() { }
    D3DMATRIX(const ts::Matrix& mat) {
      ts::Matrix cm(mat);
      cm.majorConvert();
      ::memcpy(m, cm.m, sizeof(D3DVALUE)*4*4);
    }

    void toMatrix(ts::Matrix& cm) {
      ::memcpy(cm.m, m, sizeof(float)*4*4);
      cm.majorConvert();
    }


    D3DVALUE& operator()(int iRow, int iColumn) { return m[iRow][iColumn]; }
    const D3DVALUE& operator()(int iRow, int iColumn) const { return m[iRow][iColumn]; }
  };

  enum D3DLIGHTTYPE {
    D3DLIGHT_POINT          = 1,
    D3DLIGHT_SPOT           = 2,
    D3DLIGHT_DIRECTIONAL    = 3,
    D3DLIGHT_PARALLELPOINT  = 4,
    D3DLIGHT_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
  };

  struct D3DLIGHT7 {
    D3DLIGHTTYPE    dltType;            /* Type of light source */
    D3DCOLORVALUE   dcvDiffuse;         /* Diffuse color of light */
    D3DCOLORVALUE   dcvSpecular;        /* Specular color of light */
    D3DCOLORVALUE   dcvAmbient;         /* Ambient color of light */
    D3DVECTOR       dvPosition;         /* Position in world space */
    D3DVECTOR       dvDirection;        /* Direction in world space */
    D3DVALUE        dvRange;            /* Cutoff range */
    D3DVALUE        dvFalloff;          /* Falloff */
    D3DVALUE        dvAttenuation0;     /* Constant attenuation */
    D3DVALUE        dvAttenuation1;     /* Linear attenuation */
    D3DVALUE        dvAttenuation2;     /* Quadratic attenuation */
    D3DVALUE        dvTheta;            /* Inner angle of spotlight cone */
    D3DVALUE        dvPhi;              /* Outer angle of spotlight cone */
  };

  struct BITMAPINFOHEADER {
          DWORD      biSize;
          LONG       biWidth;
          LONG       biHeight;
          WORD       biPlanes;
          WORD       biBitCount;
          DWORD      biCompression;
          DWORD      biSizeImage;
          LONG       biXPelsPerMeter;
          LONG       biYPelsPerMeter;
          DWORD      biClrUsed;
          DWORD      biClrImportant;
  };


  
}} // namespace

#endif
