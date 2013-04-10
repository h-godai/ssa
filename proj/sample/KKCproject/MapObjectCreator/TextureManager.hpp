// TextureManager.hpp
//
// テクスチャのファイルを管理するクラス

#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#ifndef STDAFX_H
#include <map>
#include <boost/foreach.hpp>
#endif

#include "Common/Common.h"
#include "Graphics/GraphicsObject.h"
#include "Graphics/Image/ImageBuffer.hpp"
#include "Graphics/Image/JpegImage.hpp"

namespace app {

  using std::map;
  using std::string;
  using namespace ts;
  using namespace ts::gobj;

  const char DEFAULT_BUILDING_TEXTURE[] = "building.jpg";

  class TextureManager {
    TextureManager() {} // 生成禁止
  public:
    typedef map <string, gobj::Texture2D::ptr> TextureMap;
    typedef pair<string, gobj::Texture2D::ptr> TextureMapItem;

    // TextureManagerはシングルトン
    static TextureManager& GetInstance() {
      static TextureManager myInstance;
      return myInstance;
    }

    static gobj::Texture2D::ptr LoadTexture(const string& fname) {
      if (fname.empty()) {
        return GetInstance().loadTexture(DEFAULT_BUILDING_TEXTURE);
      }
      else{
        return GetInstance().loadTexture(fname);
      }
    }
    static void SetNewTexture(mstring& fname, gobj::Texture2D::ptr& tex) {
      if (fname.empty() || fname == DEFAULT_BUILDING_TEXTURE) {
        mstring newName;
        size_t n = GetInstance().m_textures.size();
        for (;;) {
          newName.format("tex%5.5d.png", n);
          if (GetInstance().m_textures.find(newName) == GetInstance().m_textures.end()) break;
          ++n;
        }
        fname = newName;
      }
      GetInstance().m_textures[fname] = tex;
    }

    static const string& GetDirectory() { return GetInstance().m_directory; }

    static void SetDirectory(const string& dir) { GetInstance().m_directory = dir; }

    static void SaveAllTexture() {
      BOOST_FOREACH(TextureMapItem t, GetInstance().m_textures) {
        GetInstance().saveTexture(t.first, t.second);
      }
    }


  private:
    gobj::Texture2D::ptr loadTexture(const string& fname) {
      TextureMap::iterator found = m_textures.find(fname);
      if (found != m_textures.end()) {
        return found->second;
      }
      gobj::Texture2D::ptr texture;
      ImageBuffer::ptr image(ImageFile::LoadFromFile(m_directory + fname, ImageBuffer::A8R8G8B8));
      if (image.get() != 0) {
        texture = shared_ptr<Texture2D>(new Texture2D);
        texture->setImageBuffer(image);
        texture->setModified(false); // 既存
        m_textures[fname] = texture;
        TRACE("loadTexture(%s)\n", fname.c_str());
      }

      return texture;
    }

    void saveTexture(const string& fname, gobj::Texture2D::ptr& tex) {
      if (tex->isModified()) {
        ImageFile::SaveToFile(m_directory + fname, tex->getImageBuffer());
        TRACE("saveTexture(%s)\n", fname.c_str());
      }

    }

  private:
    TextureMap m_textures;
    string m_directory;
  };


} // namespace

#endif

