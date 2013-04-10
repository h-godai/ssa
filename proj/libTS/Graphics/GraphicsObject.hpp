/*


*/

#ifndef TS_GRAPHICS_GRAPHICS_H
#define TS_GRAPHICS_GRAPHICS_H

#ifndef STDAFX_H
#include <vector>
#include <functional>
#include <algorithm>
#endif
#include "libTS/Util/bstream.hpp"
#include "libTS/Common/Common.h"
#include "IRenderer.hpp"
#include "IGraphicsObject.hpp"

#include "libTS/Util/Logger.h"

#include "libTS/Graphics/Texture.hpp"
#include "libTS/Graphics/Transform.hpp"


#ifdef WIN32
#pragma warning (disable : 4996)
#endif

using namespace std;
using namespace boost;

namespace ts {
namespace gobj {



  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class GraphicsObject : public IGraphicsObject<PointerPolicyT> {

  public:

    virtual void render(IRenderer*) {
      throw GObjException("render not implement");
    }

    virtual void serialize(bstream&) {
      throw GObjException("serialize not implement");
    }
    virtual void deserialize(const bstream&) {
      throw GObjException("deserialize not implement");
    }

  };



  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class NodeT : public GraphicsObject<PointerPolicyT> {
  public:
    typedef typename PointerPolicyT<NodeT>::ptr ptr;
    typedef ptr Node_ptr;
    typedef NodeT<PointerPolicyT> Node_t;
  protected:
    NodeT()
      : m_enable(true)
      , m_transform(0) {}
    virtual ~NodeT() {
      if (m_transform != 0) {
        delete m_transform;
      }
    }

  public:
    // renderÇÕåpè≥ã÷é~
    void render(IRenderer* gl) {
      if (!isEnable()) return;
      if (needPrePostRendering()) {
        this->preRendering(gl);
        this->doRendering(gl);
        this->postRendering(gl);
      }
      else {
        this->doRendering(gl);
      }
    }


    bool isEnable() const { return m_enable; }
    void enable(bool en = true) { m_enable = en; }
    void disable() { m_enable = false; }
    bool hasTansform() const { return m_transform != 0; }
    void setTransform(const Matrix& m) {
      if (m_transform == 0) m_transform = new Transform;
      m_transform->setMatrix(m);
    }
    virtual void preRendering(IRenderer* gl) {
      if (m_transform != 0) m_transform->preRendering(gl);
    }
    virtual void postRendering(IRenderer* gl) {
      if (m_transform != 0) m_transform->postRendering(gl);
    }

  private:
    virtual bool needPrePostRendering() const { return m_transform != 0; }
    virtual void doRendering(IRenderer*) = 0;

    bool m_enable;
    Transform* m_transform;
  };

  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class GroupT : public NodeT<PointerPolicyT> {
  public:
    typedef typename PointerPolicyT<GroupT>::ptr ptr;
    typedef vector<typename Node_t::ptr> ChildList;
    GroupT()
      : m_zOffset(0.0f)
    {}
    virtual ~GroupT() {}
  public:

    void addChild(typename Node_t::ptr child) {
      m_child.push_back(child);
    }

    void removeChild(Node_ptr child) {
      ChildList::iterator found = find(m_child.begin(), m_child.end(), child);
      if (found != m_child.end()) {
        m_child.erase(found);
      }
    }

    void removeAllChildren() {
      m_child.clear();
    }

    void setZoffset(float32_t ofs) {
      m_zOffset = ofs;
    }

  private:
    struct DoRendering : public unary_function<Node_ptr, void> {
      IRenderer* gl;
      DoRendering(IRenderer* g) : gl(g) {}
      void operator () ( Node_ptr& node ) {
        node->render(gl);
      }
    };

    struct DoSerialize : public unary_function<Node_ptr, void> {
      bstream& bs;
      DoSerialize(bstream& b) : bs(b) {}
      void operator () ( Node_ptr& node ) {
        node->serialize(bs);
      }
    private:
      void operator = ( const DoSerialize& );
    };
    struct DoDeserialize : public unary_function<Node_ptr, void> {
      const bstream& bs;
      DoDeserialize(const bstream& b) : bs(b) {}
      void operator () ( Node_ptr& node ) {
        node->deserialize(bs);
      }
    private:
      void operator = ( const DoDeserialize& );
    };

  protected:
    virtual bool needPrePostRendering() const { return true; }
    // ï`âÊëOÇ…MatrixÇÃëﬁîÇ∆ê›íËÇçsÇ§
    virtual void preRendering(IRenderer* gl) {
      Node_t::preRendering(gl);
      if (m_zOffset != 0.0) {
        gl->enablePolygonOffsetFill();
        gl->polygonOffset(1.0f, m_zOffset);
      }
    }

    virtual void postRendering(IRenderer* gl) {
      if (m_zOffset != 0.0) {
        gl->enablePolygonOffsetFill(false);
      }
      Node_t::postRendering(gl);
    }
    virtual void doRendering(IRenderer* gl) {
      // éqãüÇΩÇøÇÉåÉìÉ_ÉäÉìÉO
      for_each(m_child.begin(), m_child.end(), DoRendering(gl));
    }

  public:
    virtual void serialize(bstream& bs) {
      for_each(m_child.begin(), m_child.end(), DoSerialize(bs));
    }
    virtual void deserialize(const bstream& bs) {
      for_each(m_child.begin(), m_child.end(), DoDeserialize(bs));
    }


  protected:
    ChildList m_child;
    float32_t m_zOffset;
  };


  class Fog {
  };
  class CompositingMode {
  };

  // É|ÉäÉSÉìÇÃï`âÊÉÇÅ[Éh
  class PolygonMode {
  };

  // É|ÉäÉSÉìÇÃï`âÊèÓïÒ
  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  struct MaterialT : PointerPolicyT<MaterialT<PointerPolicyT> > {
    ColorRGB ambient_;
    ColorRGBA diffuse_;
    ColorRGB emissive_;
    ColorRGB specular_;
    float shininess_;
    bool vertexColorTrackingEnabled_;
    virtual void doRendering(IRenderer* gl) {

    }
  };
  typedef MaterialT<> Material;




  // ï`âÊÇ∑ÇÈÉ|ÉäÉSÉìÇ…Ç¬Ç¢ÇƒÇÃèÓïÒÇèWñÒ
  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  struct AppearanceT : PointerPolicyT<AppearanceT<PointerPolicyT> > {
    uint32_t layer_;
    CompositingMode	compositingMode_;
    Fog fog_;
    PolygonMode polygonMode_;
    Material material_;
    vector<Texture2D::ptr> textures_;
  };
  typedef AppearanceT<> Appearance;

  // SurfaceÇ∆AppearanceÇ©ÇÁê¨ÇÈÇ–Ç∆Ç¬ÇÃï®ëÃ
  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class ShapeT : PointerPolicyT<ShapeT<PointerPolicyT> > {
  public:
    struct Body {
      typename AppearanceT<PointerPolicyT>::ptr appearance_;
      typename NodeT<PointerPolicyT>::ptr surface_;
      Body() {}
      Body(typename AppearanceT<PointerPolicyT>::ptr a
         , typename NodeT<PointerPolicyT>::ptr s)
        : appearance_(a)
        , surface_(s)
      {}
    };
    std::vector<Body> body_;

   protected:
    virtual void doRendering(IRenderer* gl) {
      BOOST_FOREACH(Body& s, body_) {
        s.appearance_.doRendering(gl);
        s.surface_.doRendering(gl);
      }
    }
  };
  typedef ShapeT<> Shape;


  class Primitive {
  };


  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class Line3DT : public NodeT<PointerPolicyT> {
  public:
    Line3DT() : m_color(0,0,0,255), m_width(1)
    {}
    Line3DT(const Vector3D& v1, const Vector3D& v2, const ColorRGBA& col, uint32_t width=1)
      : m_color(col),
        m_width(width) {
        m_vertex[0] = v1;
        m_vertex[1] = v2;
    }
    virtual ~Line3DT() {}

    template <typename vT>
    void setVertexes(vT& v) {
        m_vertex[0] = *v;
        m_vertex[1] = *(v+1);
    }

    void setVertexes(const Vector3D& v1, const Vector3D& v2) {
      m_vertex[0] = v1;
      m_vertex[1] = v2;
    }

    void setColor(const ColorRGBA& col) {
      m_color = col;
    }

  protected:
    virtual void doRendering(IRenderer* gl) {
      //DEBUGLOG("Line3D::doRendering\n");
      gl->drawLine3D(m_vertex[0], m_vertex[1], m_color, m_width);
    }
 
  private:
    Vector3D m_vertex[2];
    ColorRGBA m_color;
    uint32_t m_width;

  };

  // éläpå`
  template <typename VertexT
         ,  template<class Me> class PointerPolicyT = ts::default_pointer_policy >

  class RectangleT : public NodeT<PointerPolicyT> {
  public:
    typedef shared_ptr<RectangleT> ptr;
    typedef VerticesNT<4, VertexT> Vertices4;
    enum VertexPos {
      RT = 0, // 0: right top
      LT,     // 1: left top
      LB,     // 2: left bottom
      RB,     // 3: right bottom;
    };


    RectangleT() 
      : m_color(255,255,255,255)
      , m_borderColor(255,255,255,255)
      , m_width(1.0f)
      , m_drawTexture(false)
      , m_drawBorder(true)
    {
      initialize();
    }

    RectangleT(const std::vector<VertexT>& v, ColorRGBA& col, float32_t width=1.0f)
      : m_color(col)
      , m_width(width)
      , m_drawTexture(false)
      , m_drawBorder(true)
    {
      setVertexes(v.begin());
      initialize();
    }

    virtual ~RectangleT() {}

    void initialize() {
      m_texCoords[0].u = 1.0f;
      m_texCoords[0].v = 0.0f;
      m_texCoords[1].u = 0.0f;
      m_texCoords[1].v = 0.0f;
      m_texCoords[2].u = 0.0f;
      m_texCoords[2].v = 1.0f;
      m_texCoords[3].u = 1.0f;
      m_texCoords[3].v = 1.0f;
      m_vertex[0] = Vector3D( 1.0f, 1.0f, 0.0f);
      m_vertex[1] = Vector3D(-1.0f, 1.0f, 0.0f);
      m_vertex[2] = Vector3D(-1.0f,-1.0f, 0.0f);
      m_vertex[3] = Vector3D( 1.0f,-1.0f, 0.0f);
    }

    void setVertexes(const RectF32& rect) {
      m_vertex[RT].x = rect.left + rect.width;
      m_vertex[RT].y = rect.top;
      m_vertex[LT].x = rect.left;
      m_vertex[LT].y = rect.top;
      m_vertex[LB].x = rect.left;
      m_vertex[LB].y = rect.top + rect.height;
      m_vertex[RB].x = rect.left + rect.width;
      m_vertex[RB].y = rect.top + rect.height;
    }

    void setVertexes(typename std::vector<VertexT>::const_iterator v) {
      m_vertex[RT] = *v;
      m_vertex[LT] = *++v;
      m_vertex[LB] = *++v;
      m_vertex[RB] = *++v;
    }

    void setVertexes(const Vertices4& v) {
      m_vertex = v;
    }

    void setVertex(size_t n, const VertexT& v) {
      m_vertex[n] = v;
    }
 
    void setColor(const ColorRGBA& col) {
      m_color = col;
      if (m_texture.get() != 0) {
         m_texture->setColor(m_color);
      }
    }
    void setBorderColor(const ColorRGBA& col) {
      m_borderColor = col;
    }
    void setTexCoord(const TexCoords* texCoords) {
      m_texCoords[0] = texCoords[0];
      m_texCoords[1] = texCoords[1];
      m_texCoords[2] = texCoords[2];
      m_texCoords[3] = texCoords[3];
    }

    void setTexCoord(const RectF32& rect) {
      m_texCoords[0].u = rect.left + rect.width;
      m_texCoords[0].v = rect.top;
      m_texCoords[1].u = rect.left;
      m_texCoords[1].v = rect.top;
      m_texCoords[2].u = rect.left;
      m_texCoords[2].v = rect.top + rect.height;
      m_texCoords[3].u = rect.left + rect.width;
      m_texCoords[3].v = rect.top + rect.height;
    }

    void setTexCoord(uint32_t n, const TexCoords& texCoords) {
      m_texCoords[n] = texCoords;
    }

    void setTexture2D(shared_ptr<Texture2D> texture) {
      m_texture = texture;
      m_drawTexture = true;
      m_drawBorder = false;
      if (texture.get() != 0) {
        m_texture->setColor(m_color);
      }
    }

    void refreshTexture() { m_texture->deleteTextures(); }
    void enableTexture(bool enable = true) { m_drawTexture = enable; }
    void enableBorder(bool enable = true) { m_drawBorder = enable; }

  
    virtual void doRendering(IRenderer* gl) {
      if (m_texture.get() != 0 && m_drawTexture) {
        // ÉeÉNÉXÉ`ÉÉÇ≈éläpå`Çï`âÊ
        gl->texture2D(*m_texture, m_texture->blendColor());
        beginPrimitive begin(gl, IRenderer::TRIANGLE_STRIP);
        gl->texcoord(m_texCoords[0]);
        gl->vertex(m_vertex[0]);
        gl->texcoord(m_texCoords[1]);
        gl->vertex(m_vertex[1]);
        gl->texcoord(m_texCoords[3]);
        gl->vertex(m_vertex[3]);
        gl->texcoord(m_texCoords[2]);
        gl->vertex(m_vertex[2]);
      }
      if (m_drawBorder) {
        // ÉâÉCÉìÇ≈éläpå`Çï`âÊ
        gl->disableTexture2D();
        gl->color(m_borderColor);
        gl->lineWidth(m_width);
        beginPrimitive begin(gl, IRenderer::LINE_LOOP);
        gl->vertex(m_vertex[0]);
        gl->vertex(m_vertex[1]);
        gl->vertex(m_vertex[2]);
        gl->vertex(m_vertex[3]);
      }
    }
    void setLineWidth(float32_t lw) { m_width = lw; }
    const VertexT& getVertex(size_t n) { return m_vertex[n]; }
    const Vertices4& getRectVertex() { return m_vertex; }

  private:
    Vertices4 m_vertex;
    ColorRGBA m_color;
    ColorRGBA m_borderColor;
    float32_t m_width;
    TexCoords m_texCoords[4];
    shared_ptr<Texture2D> m_texture;
    bool m_drawTexture;
    bool m_drawBorder;

  };

  typedef RectangleT<Vector2D> Rectangle2D;
  typedef RectangleT<Vector3D> Rectangle3D;
  typedef RectangleT<Vector2Dd> Rectangle2Dd;
  typedef RectangleT<Vector3Dd> Rectangle3Dd;


 
 template < typename VT
          , template<class Me> class PointerPolicyT = ts::default_pointer_policy >
 class Mesh : public NodeT<PointerPolicyT> {
  public:
    typedef typename PointerPolicyT<Mesh<VT> >::ptr ptr;
    IRenderer::PrimitiveType primitiveType_;
    vector<VT> vertex_;
    vector<uint32_t> indices_;
  public:
    Mesh(IRenderer::PrimitiveType t=IRenderer::NOTHING): primitiveType_(t) {}
    virtual ~Mesh() {}
  protected:
    struct RenderXYZ {
      enum { type =  VertexType_XYZ };
      template <typename V> static void draw(IRenderer* gl, const V& v) {
        gl->vertex(Vector3D(v.x,v.y,v.z));
      }
    };
    struct RenderXYZUV {
      enum { type =  VertexType_XYZUV };
      template <typename V> static void draw(IRenderer* gl, const V& v) {
        gl->texcoord(v.texcoords);
        gl->vertex(Vector3D(v.x,v.y,v.z));
      }
    };
    struct RenderXYZN {
      enum { type =  VertexType_XYZN };
      template <typename V> static void draw(IRenderer* gl, const V& v) {
        gl->normal(v.normal);
        gl->vertex(Vector3D(v.x,v.y,v.z));
      }
    };
    struct RenderXYZUVN {
      enum { type =  VertexType_XYZUVN };
      template <typename V> static void draw(IRenderer* gl, const V& v) {
        gl->normal(v.normal);
        gl->texcoord(v.texcoords);
        gl->vertex(Vector3D(v.x,v.y,v.z));
      }
    };
    struct RenderD3DVERTEX {
      enum { type =  VertexType_D3DVERTEX };
      template <typename V> static void draw(IRenderer* gl, const V& v) {
        gl->normal(Vector3D(v.nx,v.ny,v.nz));
        gl->texcoord(TexCoords(v.tu,v.tv));
        gl->vertex(Vector3D(v.x,v.y,v.z));
      }
    };
    typedef typename ts::mpl::list<RenderXYZ,RenderXYZN,RenderXYZUV,RenderXYZUVN,RenderD3DVERTEX>::type renderList;
    typedef typename ts::mpl::find<renderList, VT::type>::type myRender;


    virtual void doRendering(IRenderer* gl) {
      if (primitiveType_ != IRenderer::NOTHING) {
        beginPrimitive begin(gl, primitiveType_);
        if (indices_.empty()) {
          for (uint32_t i = 0; i < indices_.size(); ++i) {
            myRender::draw(gl, vertex_[i]);
          }
        }
        else {
          for (uint32_t i = 0; i < indices_.size(); ++i) {
            myRender::draw(gl, vertex_[indices_[i]]);
          }
        }
      }
    }

  };


  template < template<class Me> class PointerPolicyT = ts::default_pointer_policy >
  class LightT : public NodeT<PointerPolicyT> {
  public:
    typedef typename PointerPolicyT<LightT>::ptr ptr;

    enum LightType {
      Point          = 1,
      Spot           = 2,
      Directional    = 3,
      ParallelPoint  = 4,
      UnknownType = 0x7fffffff
    };

    LightType lightType_;       /* Type of light source */
    ColorRGBA diffuse_;         /* Diffuse color of light */
    ColorRGBA specular_;        /* Specular color of light */
    ColorRGBA ambient_;         /* Ambient color of light */
    Vector3D  position_;        /* Position in world space */
    Vector3D  direction_;       /* Direction in world space */
    float32_t range_;           /* Cutoff range */
    float32_t falloff_;         /* Falloff */
    float32_t attenuation0_;    /* Constant attenuation */
    float32_t attenuation1_;    /* Linear attenuation */
    float32_t attenuation2_;    /* Quadratic attenuation */
    float32_t theta_;           /* Inner angle of spotlight cone */
    float32_t phi_;             /* Outer angle of spotlight cone */

    LightT()
      : lightType_(Point)
      , diffuse_(ColorRGBA(1.0f,1.0f,1.0f))
      , specular_(ColorRGBA(1.0f,1.0f,1.0f))
      , ambient_(ColorRGBA(1.0f,1.0f,1.0f))
      , position_(Vector3D(0.0f,0.0f,0.0f))
      , direction_(Vector3D(0.0f,-1.0f,0.0f))
      , range_(1000.0f)
      , falloff_(1000.0f)
      , attenuation0_(0.0f)
      , attenuation1_(0.0f)
      , attenuation2_(0.0f)
      , theta_(0.0f)
      , phi_(0.0f)
    {}
    virtual ~LightT() {}

  protected:

    virtual void doRendering(IRenderer*) {
    }


  };

  // default_pointer_policyÇ≈égÇ§èÍçáÇÃtypedef
  typedef NodeT<default_pointer_policy> Node;
  typedef GroupT<default_pointer_policy> Group;
  //typedef MeshT<default_pointer_policy> Mesh;
  typedef LightT<default_pointer_policy> Light;
  typedef Line3DT<default_pointer_policy> Line3D;

}} // namespace



#endif
