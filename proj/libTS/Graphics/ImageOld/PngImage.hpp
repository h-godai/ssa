// PngImage.hpp
//
// PNG�摜�t�@�C���������N���X



#ifndef TSL_PLATFORMDEPENDENT_PNGIMAGE_HPP
#define TSL_PLATFORMDEPENDENT_PNGIMAGE_HPP

#ifndef STDAFX_H
#include <string>
#endif
#include "ImageFile.hpp"

namespace ts {
  using std::string;


  class PngImage : public ImageFile {
  public:
    PngImage() {};
    virtual ~PngImage() {};

    // virtual method
    virtual ImageFormatType getImageFormat() const { return ImageFile::PNG; }

    // �t�@�C���ɏ�������
    virtual bool compress(const string& fileName, const ImageBuffer::ptr& input);
 
    // �^����ꂽ�o�b�t�@�̃t�H�[�}�b�g�ɓǂݍ���

    // �摜�C���[�W�̃t�H�[�}�b�g�̂܂ܓǂݍ���
    virtual ImageBuffer::ptr deCompress(const string& fileName, ImageBuffer::PixelFormat fmt = ImageBuffer::A8R8G8B8);
  };


} // namespace

#endif

