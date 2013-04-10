// VRMLparser.hpp
//

#ifndef TS_UTIL_VRMLPARSER_HPP
#define TS_UTIL_VRMLPARSER_HPP

#ifndef STDAFX_H
#include <string>
#include <iostream>
#include <stack>
#include <vector>
#endif
#include <boost/mpl/list.hpp>
#include <boost/mpl/iter_fold.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/type.hpp>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

#include "libTS/Common/Common.h"
#include "util.hpp"
#include "unittest.hpp"
#include "ustring.hpp"
#include "Logger.h"

#include "../Math/old/Vector.h"
#include "../Math/old/Matrix.h"
#include "../Graphics/Vertices.hpp"
#include "../Graphics/GLCommon.h"


namespace ts { namespace vrml {

  namespace mpl = boost::mpl;

  enum CoordType {
    UnknwonCoordinate,
    Coordinate,
    TexCoordinate
  };
  enum KeyWordType {
    PROC_BEGIN, // {
    PROC_END,   // }
    ARRAY_BEGIN, // [
    ARRAY_END,   // ]
    CMD_GROUP,
    CMD_SHAPE,
    CMD_TRANSFORM,

    SUB_APPEARANCE,
    SUB_MATERIAL,
    SUB_IMAGETEXTURE,

    SUB_CHILDREN,
    SUB_GEOMETORY,
    SUB_TEXCOORD,

    VAL_POINT,
    VAL_COORDINATE,
    VAL_TEXCOORDINATE,
    VAL_COORDINDEX,
    VAL_TEXCOORDINDEX,

    ATTR_APPEARANCE,
    ATTR_COORD,

    ATTR_CCW,
    ATTR_SOLID,
    ATTR_DEF,
    ATTR_INDEXEDFACESET,
    ATTR_MATERIAL,
    ATTR_ROTATION,
    ATTR_TEXTURE,
    ATTR_TRANSLATION,
    ATTR_URL,
    PROCESS_DONE,
    INVALID_KEYWORD_TYPE
  };

  template <typename target>
  struct SharedPtrPolicy {
    typedef boost::shared_ptr<target> ptr;
  };

  // Node
  struct Node  {
    typedef SharedPtrPolicy<Node>::ptr ptr;
    Node* parent;
    Node(ptr p = ptr()) : parent(p.get()) {}
    virtual ~Node() {}
    virtual bool hasChildlen() const { return false; }
    virtual KeyWordType getType() const { return INVALID_KEYWORD_TYPE; }
  };

  struct Chindlen {
    void add(Node::ptr node) {
      childlen.push_back(node);
    }
    std::vector<Node::ptr> childlen;
  };
  
  struct Appearance {
    ColorRGB diffuseColor;
    float ambientIntensity;
    ColorRGB specularColor;
    float shininess;
    float transparency;
    mstring imageTextureUrl;
  };

  struct ImageTexture {
    mstring url;
    virtual KeyWordType getType() const { return SUB_IMAGETEXTURE; }
  };

  struct Geometory {
    bool ccw;
    bool solid;
    Geometory() : ccw(true), solid(true) {}
    virtual KeyWordType getType() const { return SUB_GEOMETORY; }
  };

  struct IndexFaceSet : Geometory {
    std::vector<Vector3D> coordinate;
    std::vector<TexCoords> textureCoord;
    std::vector<int32_t> coordIndex;
    std::vector<int32_t> texCoordIndex;
    virtual KeyWordType getType() const { return ATTR_INDEXEDFACESET; }
  };



  struct Group : Node {
    typedef SharedPtrPolicy<Group>::ptr ptr;
    Group(Node::ptr parent) : Node(parent) {}
    Chindlen children;
    virtual bool hasChildlen() const { return true; }
    virtual KeyWordType getType() const { return CMD_GROUP; }
  };


  struct Transform : Group {
    typedef SharedPtrPolicy<Transform>::ptr ptr;
    Vector3Dd translation;
    Vector3D rotateAxis; // ��]��
    float    rotateRad;  // ��]�p�x(���W�A���l)
    Transform(Node::ptr parent) : Group(parent), translation(0,0,0), rotateAxis(0,0,0), rotateRad(0) {}
    virtual bool hasChildlen() const { return true; }
    virtual KeyWordType getType() const { return CMD_TRANSFORM; }
  };


  struct Shape : Node {
    typedef SharedPtrPolicy<Shape>::ptr ptr;
    Appearance appearance;
    IndexFaceSet geometory;
    Shape(Node::ptr parent) : Node(parent) {}
    virtual KeyWordType getType() const { return CMD_SHAPE; }
  };


  // �p�[�X���s���̃p�����[�^�f�[�^�N���X
  template <typename Iter>
  struct ParseParam {
    Iter& iter;
    Iter end;
    Group::ptr root;
    Node::ptr currentNode;
    std::stack<Node::ptr> cmdStack;
    ParseParam(Iter& i, Iter en) : iter(i), end(en), root(new Group(Node::ptr())), currentNode(root) {}
  };

  // �G���[exception
  struct ParseError : public std::exception {
    ParseError(const mstring& msg) : std::exception(msg.c_str()) {}
  };


  // �ȉ��A�p�[�X���߂̃e���v���[�g���^�v���O���~���O

  // �e�L�[���[�h�̎��ʂƐ����������s�����߂̊��N���X
  struct ProcBase {
    // �����̓p�[�T�[����������L�[���[�h
    static const char* appearance() { return "appearance"; }
    static const char* children() { return "children"; }
    static const char* geometory() { return "geometory"; }
    static const char* texCoord() { return "texCoord"; }
    static const char* beginArray() { return "["; }
    static const char* endArray() { return "]"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>&) { 
      TS_DEBUGLOG("Ignore\n");
      return true; 
    }

    // ���݂�Node��ۗL���ׂ�Group�𒲂ׂĎ擾����
    template <typename Iter>
    static Group::ptr GetGroup(ParseParam<Iter>& param) {
      if (!param.currentNode) {
        throw ParseError("GetGroup: CurrentNode is null.");
      }
      if (!param.currentNode->hasChildlen()) {
          // Childlen�������Ȃ�Node�Ȃ̂ŃG���[
        throw ParseError("GetGroup: CurrentNode not have Childlen");
      }
      Group::ptr group = boost::dynamic_pointer_cast<Group>(param.currentNode);
      if (!group) {
        throw ParseError("GetGroup: CurrentNode is not Group subclass.");
      }
      return group;
    }
  };

  // {�@�̏���
  struct BeginProc {
    static const char* keyword() { return "{"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      param.cmdStack.push(param.currentNode);
      return true;
    }
  };

  // } �̏���
  struct EndProc {
    static const char* keyword() { return "}"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      if (param.cmdStack.empty()) {
        throw ParseError("too many '}'");
      }
      param.cmdStack.pop();
      if (param.cmdStack.empty()) {
        param.currentNode = param.root;
      }
      else {
        param.currentNode = param.cmdStack.top();
      }
      return true;
    }
  };
    

  // Transform����
  struct TransformProc : ProcBase {
    static const char* keyword() { return "Transform"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Group::ptr group = GetGroup(param);
      Transform::ptr tfm(new Transform(param.currentNode));
      group->children.add(tfm);
      param.currentNode = tfm;
      return true;
    }
  };

  // translation����
  struct translationProc {
    static const char* keyword() { return "translation"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Transform::ptr tfm = boost::dynamic_pointer_cast<Transform>(param.currentNode);
      if (!tfm) {
        throw ParseError("CurrentNode is not Transform.");
      }
      tfm->translation.x = (*(++param.iter)).toDouble();
      tfm->translation.y = (*(++param.iter)).toDouble();
      tfm->translation.z = (*(++param.iter)).toDouble();
      return true;
    }
  };

  // rotation�̏���
  struct rotationProc {
    static const char* keyword() { return "rotation"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Transform::ptr tfm = boost::dynamic_pointer_cast<Transform>(param.currentNode);
      if (!tfm) {
        throw ParseError("CurrentNode is not Transform.");
      }
      tfm->rotateAxis.x = (*(++param.iter)).toFloat();
      tfm->rotateAxis.y = (*(++param.iter)).toFloat();
      tfm->rotateAxis.z = (*(++param.iter)).toFloat();
      tfm->rotateRad    = (*(++param.iter)).toFloat();
      return true;
    }
  };

  // Shape�̏���
  struct ShapeProc : public ProcBase {
    static const char* keyword() { return "Shape"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Group::ptr group = GetGroup(param);
      Shape::ptr tfm(new Shape(param.currentNode));
      group->children.add(tfm);
      param.currentNode = tfm;
      return true;
    }
  };

  // Appearance
  struct AppearanceProc : public ProcBase {
    static const char* keyword() { return "Appearance"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      return true; 
    }
  };

  // ImageTexture
  struct ImageTexureProc : public ProcBase {
    static const char* keyword() { return "ImageTexture"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      // url��T��
      while (*param.iter != "}" && param.iter != param.end) {
        if (*param.iter == "url") {
          if (*(++param.iter) == "[") { ++param.iter; }
          shape->appearance.imageTextureUrl = *param.iter;
        }
        ++param.iter;
      }
      if (param.iter == param.end) {
        throw ParseError("ImageTexture:: '}' not found.");
      }
      return true; 
    }
  };

  // IndexedFaceSet
  struct IndexedFaceSetProc {
    static const char* keyword() { return "IndexedFaceSet"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      return true;
    }
  };

  template <typename Iter, typename Container>
  inline void ReadArray(ParseParam<Iter>& param, Container& container) {
    typedef Container::value_type ElementType;
    bool arrayFlag = false;
    ElementType::value_type element[ElementType::Elements];
    uint32_t idx = 0;

    if (*param.iter == "[") {
      arrayFlag = true;
      ++param.iter;
    }
    while (param.iter != param.end) {
      if (*param.iter == "]") break;
      element[idx++] = (*param.iter).get();
      if (idx == ElementType::Elements) {
        idx = 0;
        container.push_back(ElementType(element));
        if (!arrayFlag) break; // �z�񂶂�Ȃ������炱���ł����܂�
        if (*++param.iter == "]") {
          break; // ����I��
        }
        else if (*param.iter != ",") {
          throw ParseError("Invalid Array Format: " + *param.iter);
        }
      }
      ++param.iter;
    }
    if (idx != 0) {
      throw ParseError("Invalid Array Format.");
    }

  }

  // Coordinate
  struct CoordinateProc {
    static const char* keyword() { return "Coordinate"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      if (*++param.iter != "{") {
        throw ParseError("Coordinate: '{' not found.");
      }
      if (*++param.iter != "point") {
        throw ParseError("Coordinate: 'point' not found.");
      }

      ++param.iter; 
      ReadArray(param, shape->geometory.coordinate);

      while (*param.iter != "}" && param.iter != param.end) ++param.iter;
      if (param.iter == param.end) {
        throw ParseError("Coordinate: '}' not found.");
      }
      return true;
    }
  };
  // TexCoordinate
  struct TexCoordinateProc {
    static const char* keyword() { return "TextureCoordinate"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      if (*++param.iter != "{") {
        throw ParseError("TextureCoordinate: '{' not found.");
      }
      if (*++param.iter != "point") {
        throw ParseError("TextureCoordinate: 'point' not found.");
      }
      ++param.iter; 
      ReadArray(param, shape->geometory.textureCoord);

      while (*param.iter != "}" && param.iter != param.end) ++param.iter;
      if (param.iter == param.end) {
        throw ParseError("TextureCoordinate:: '}' not found.");
      }
      return true;
    }
  };
  
  // coordIndex
  struct coordIndexProc {
    static const char* keyword() { return "coordIndex"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      if (*(++param.iter) == "[") { ++param.iter; }
      for (; param.iter != param.end && *param.iter != "]"; ++param.iter) {
        if (*param.iter == ",") continue;
        shape->geometory.coordIndex.push_back((*param.iter).toInt());
      }
      if (param.iter == param.end) {
        throw ParseError("coordIndex:: ']' not found.");
      }
      return true;
    }
  };

  // texCoordIndex
  struct texCoordIndexProc {
    static const char* keyword() { return "texCoordIndex"; }
    template <typename Iter>
    static bool Process(ParseParam<Iter>& param) {
      Shape::ptr shape = boost::dynamic_pointer_cast<Shape>(param.currentNode);
      if (!shape) {
        throw ParseError("CurrenNode is not Shape.");
      }
      if (*(++param.iter) == "[") { ++param.iter; }
      for (; param.iter != param.end && *param.iter != "]"; ++param.iter) {
        if (*param.iter == ",") continue;
        shape->geometory.texCoordIndex.push_back((*param.iter).toInt());
      }
      if (param.iter == param.end) {
        throw ParseError("texCoordIndex:: ']' not found.");
      }
      return true;
    }
  };

  // ���̃��[�h�Ƌ��ɖ�������
  struct Ignore1 : ProcBase {
    static const char* def() { return "DEF"; } // ATTR_DEF
    template <typename Iter>
    static bool Process(ParseParam<Iter>& pp) {
      TS_DEBUGLOG("Ignore %s %s\n", pp.iter->c_str(), (pp.iter +1)->c_str());
      ++pp.iter;
      return true; // ��������
    }
  };

  // 2��̃��[�h�Ƌ��ɖ�������
  struct Ignore2 : ProcBase {
    template <typename Iter>
    static bool Process(ParseParam<Iter>& pp) {
      ++pp.iter; ++pp.iter;
      return true; // ��������
    }
  };


  // �eProc�����s���邽�߂̃R�}���h�N���X


  struct VrmlCmdBase {
  };
  // �T�u�N���X�̃t�@�N�V���������z�I�ɌĂт������߂̃x�[�X�N���X
  // virtual���g�킸�ɉ��z��������
  template <class SubClass>
  struct VrmlCmdBaseT : VrmlCmdBase {
    const char* getKeyWord() const { return SubClass::GetKeyWord(); }
    KeyWordType getType() const { return SubClass::GetType(); }
  };

  // �R�}���h�N���X�̎���
  template <int Type, typename Proc, const char* (*GetKW)() = &Proc::keyword>
  struct VrmlCmd : VrmlCmdBaseT<VrmlCmd<Type, Proc, GetKW> > {
    enum { type = Type };
    typedef Proc proc;
    static const char* GetKeyWord() { return (*GetKW)(); }
    static KeyWordType GetType() { return static_cast<KeyWordType>(type); }
  };

  // �R�}���h���X�g�e�[�u��


  typedef VrmlCmd<PROC_BEGIN, BeginProc> BeginProcCmd;
  typedef VrmlCmd<PROC_END, EndProc> EndProcCmd;
  typedef VrmlCmd<CMD_SHAPE, ShapeProc> ShapeCmd;
  typedef VrmlCmd<CMD_TRANSFORM, TransformProc> TransformCmd;
  typedef VrmlCmd<ATTR_TRANSLATION, translationProc> translationCmd;
  typedef VrmlCmd<ATTR_ROTATION, rotationProc> rotationCmd;
  typedef VrmlCmd<SUB_IMAGETEXTURE, ImageTexureProc> ImageTextureCmd;
  typedef VrmlCmd<VAL_COORDINATE, CoordinateProc> CoordinateCmd;
  typedef VrmlCmd<VAL_TEXCOORDINATE, TexCoordinateProc> TexCoordinateCmd;
  typedef VrmlCmd<VAL_COORDINDEX, coordIndexProc> CoordIndexCmd;
  typedef VrmlCmd<VAL_TEXCOORDINDEX, texCoordIndexProc> TexCoordIndexCmd;
  

  // �p�[�T�[����������L�[���[�h
  typedef VrmlCmd<SUB_APPEARANCE, ProcBase, &ProcBase::appearance> appearanceCmd;
  typedef VrmlCmd<SUB_CHILDREN, ProcBase, &ProcBase::children> childrenCmd;
  typedef VrmlCmd<SUB_GEOMETORY, ProcBase, &ProcBase::geometory> geometoryCmd;
  typedef VrmlCmd<SUB_TEXCOORD, ProcBase, &ProcBase::texCoord> texCoordCmd;
  typedef VrmlCmd<ARRAY_BEGIN, ProcBase, &ProcBase::beginArray> BeginArrayCmd;
  typedef VrmlCmd<ARRAY_END, ProcBase, &ProcBase::endArray> EndArrayCmd;

  // 1���[�h��������t�@���N�V����
  typedef VrmlCmd<ATTR_DEF, Ignore1, &Ignore1::def> DefCmd;


  // �L�[���[�h���T�[�`����t�@���N�V�����̃��X�g
  typedef mpl::list<BeginProcCmd
                  , EndProcCmd
                  , BeginArrayCmd
                  , EndArrayCmd
                  , ShapeCmd
                  , TransformCmd
                  , translationCmd
                  , rotationCmd
                  , appearanceCmd
                  , ImageTextureCmd
                  , CoordinateCmd
                  , TexCoordinateCmd
                  , CoordIndexCmd
                  , TexCoordIndexCmd
                  , childrenCmd
                  , geometoryCmd
                  , texCoordCmd
                  , DefCmd
                  > VrmlCmdList;
  typedef mpl::begin<VrmlCmdList>::type VrmlCmdListBegin;
  typedef mpl::end<VrmlCmdList>::type VrmlCmdListEnd;


  // �L�[���[�h���������Ď��s����
  template <typename Begin, typename End>
  struct FindKeywordT {
    typedef typename mpl::deref<Begin>::type Type;
    template <typename Param>
    static KeyWordType execute(Param& param) {
      if (*param.iter == Type::GetKeyWord()) {
        // ��������
        if (Type::proc::Process(param)) {
          // ��������
          return PROCESS_DONE;
        }
        else {
          return Type::GetType();
        }
      }
      else {
        return FindKeywordT<typename mpl::next<Begin>::type, End>::execute(param);
      }
    }
  };

  // FindKeyword�̓��ꉻ�B���X�g�̍Ō�̎��݂̂����炪���s�����
  template <typename End>
  struct FindKeywordT<End, End> {
    template <typename Iter>
    static KeyWordType execute(Iter& /*kw*/) {
      return INVALID_KEYWORD_TYPE; // �L�[���[�h��������Ȃ�����
    }
  };

  typedef FindKeywordT<VrmlCmdListBegin, VrmlCmdListEnd> FindVrmlCmd;

    struct NullLogger {
      const NullLogger& operator << (const std::string& ) const {
        return *this;
      }
      static NullLogger& Instance() { static NullLogger ins; return ins; }
    };

  } // namespace vrml

  class VRMLparser {
  public:

    static vrml::Group::ptr Parse(std::istream& inf) {
       return Parse(inf, vrml::NullLogger::Instance());
    }

    template <typename Logger>
    static vrml::Group::ptr Parse(std::istream& inf, Logger& logger) {
      using namespace vrml;
      using namespace std;

      // �p�[�X�̑O���� �R�����g�s�Ɖ��s�R�[�h�̍폜
      vector<mstring> body;
      while (!inf.eof()) {
        mstring line;
        getline(inf, line);
        if (line.empty()) continue;
        line.trim(" \t\r\n");
        if (line[0] == '#') {
          // �R�����g�s
          logger << line << "\n";
          continue;
        }
        vector<mstring> words;
        line.split(words, " \t"); // �󔒂ŋ�؂����P��̏W���̂ɂ���
        BOOST_FOREACH(mstring& ws, words) {
          ws.trim(" \r\t");
          if (!ws.empty()) {
            if (ws.length() == 1) {
              body.push_back(ws);
            }
            else {
              char left = ws[0];
              char right = ws[ws.length()-1];
              if (left == '[' || left == '{' || left == ',') {
                body.push_back(ws.substr(0, 1));
                ws = ws.substr(1);
              }
              if (ws.length() > 1 && (right == ']' || right == ',' || right == '}' || right == '{' || right == '[')) {
                body.push_back(ws.substr(0, ws.length() - 1));
                body.push_back(ws.substr(ws.length()-1, 1));
              }
              else {
                body.push_back(ws);
              }
            }
          }
        }
      }

      return Parse(body, logger);
    }

    // @throw ParseError
    template <typename Logger>
    static vrml::Group::ptr Parse(std::vector<mstring>& body, Logger& logger) {
      using namespace vrml;
      using namespace std;

      // �p�[�X�J�n
      vector<mstring>::iterator w = body.begin();
      ParseParam<vector<mstring>::iterator> param(w, body.end());

      for (; w != body.end(); ++w) {
        logger << w->c_str() << ": ";
        vrml::KeyWordType type = ts::vrml::FindVrmlCmd::execute(param);
        if (type == vrml::PROCESS_DONE) { 
          logger << "Process done.\n";
          continue; 
        }
        if (type == vrml::INVALID_KEYWORD_TYPE) {
          logger << "Unknown Keyword '" << *w << "'\n";
          continue; 
        }
        logger << "process failed:" << *w << "\n";
        continue;
      }
      return param.root;
    }
  
    //! �P�̎����p���\�b�h
    POINTCUT_UNITTEST_METHOD;

  };


  
} // namespace

#endif



