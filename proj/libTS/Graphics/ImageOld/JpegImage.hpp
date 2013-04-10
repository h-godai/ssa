// JpegImege.hpp



#ifndef TSL_PLATFORMDEPENDENT_JPEGIMAGE_HPP
#define TSL_PLATFORMDEPENDENT_JPEGIMAGE_HPP

#include <string>
#include "ImageFile.hpp"

namespace ts {
  using std::string;


  class JpegImage : public ImageFile {
  public:
    JpegImage();
    virtual ~JpegImage();

    // virtual method
    // virtual method
    virtual ImageFormatType getImageFormat() const { return ImageFile::JPEG; }
    // �t�@�C���ɏ�������
    virtual bool compress(const string& fileName, const ImageBuffer::ptr& input);
    // �摜�C���[�W�̃t�H�[�}�b�g�̂܂ܓǂݍ���
    virtual ImageBuffer::ptr deCompress(const string& fileName, ImageBuffer::PixelFormat fmt = ImageBuffer::A8R8G8B8);
    // �p�ӂ����C���[�W�f�[�^�ɓǂݍ���
    bool deCompress2(const string& fileName, ImageBuffer& output);

  };


} // namespace

#endif

