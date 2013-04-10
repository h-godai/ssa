/** IRenderer.h  -*-tab-width:8;C++-*-





  Copyright 2006 TECHNICAL ARTS h.godai
  
*/

#ifndef IRenderer_H
#define IRenderer_H

#include "libTS/Common/Common.h"
#include "libTS/Graphics/IGraphicsContext.hpp"
#include "libTS/math/geometric.hpp"
#include "libTS/Graphics/Texture.hpp"
#include "libTS/Image/Color.hpp"
#include "libTS/Graphics/Vertices.hpp"

namespace ts {

  class IRendererScene;
    class IGraphics;


    //==============================================================================
    /**
        @brief        �����_�����O�p�̃C���^�[�t�F�C�X�N���X
        @note        IGraphics�̃����_�����O���Ƀ����_�����O�[�f�[�^��
                    �󂯓n�����߂̃C���^�[�t�F�C�X�ł��B
   */
    //==============================================================================
    class IRenderer {
    public:
        enum PrimitiveType {
          NOTHING,
          TRIANGLE_STRIP,
          LINE,
          LINE_LOOP,
          TRIANGLES,
          QUADS,
          QUAD_STRIP,
          LINES,
          TRIANGLE_FAN
        };


        /// @brief        �R���X�g���N�^
        IRenderer() {}
        /// @brief        �f�X�g���N�^
        virtual ~IRenderer() {}

        ///    @brief        ����������
        ///    @note        ���̃N���X�̏������������s���܂��B
        ///    @param        
        virtual void initialize(IGraphicsContext::ptr gc = IGraphicsContext::ptr()) = 0;

        ///    @brief        �I������
        ///    @note        ���̃N���X�̏I���������s���܂��B
        virtual void finalize() = 0;

        /// @brief        frame�̕`��J�n���ɌĂ΂�܂�
        /// @note        
        virtual void onBeginFrame() {}

        /// @brief        frame�̕`��I�����ɌĂ΂�܂�
        /// @note
        virtual void onEndFrame() {}

        /// @brief        �X��Scene�̕`��J�n���ɌĂ΂�܂�
        /// @note        
        virtual bool onBeginScene(IRendererScene* /*scene*/) { return true; }

        /// @brief        �X��Scene�̕`��I�����ɌĂ΂�܂�
        /// @note
        virtual void onEndScene() {}

        /// @brief        �X��Object�̕`��J�n���ɌĂ΂�܂�
        /// @note        
        virtual void onBeginObject(IGraphics*) {}

        /// @brief        �X��Object�̕`��I�����ɌĂ΂�܂�
        /// @note
        virtual void onEndObject(IGraphics*) {}


        //==============================================================================
        // �J�����p�̃��\�b�h
        //==============================================================================

        //==============================================================================
        /**
            @brief        Perspective��ݒ肵�܂�
            @note        ���e��OpenGL��gluPerspective�Ɠ����ł��B
            @param        fovy        ����p
            @param        aspectRatio �c����
            @param        nearPlane   NEAR�v���[��
            @param        farPlane    FAR�v���[��
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void perspective(float64_t fovy, float64_t aspectRatio, float64_t nearPlane, float64_t farPlane) = 0;
        virtual void ortho(float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t zNear, float64_t zFar) = 0;
        virtual void frustum(float64_t x1, float64_t x2, float64_t y1, float64_t y2, float64_t znear, float64_t zfar) = 0;
        //==============================================================================
        /**
            @brief        ProjectionMatrix��ݒ肵�܂�
            @note        ProjectinMatrix�ɑ΂���Matrix�l��ݒ肵�܂�
            @param        matrix �ݒ肷��Matrix
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void selectProjectionMatrix()  = 0;
        virtual void selectModelViewMatrix() = 0;

        //==============================================================================
        /**
            @brief        ���_�A�����_�A�J�����̌�����ݒ肵�܂��B
            @note        OpenGL��gluLookAt�Ɠ����ł��B
            @param        eye    �J�����̈ʒu
            @param        center �����_
            @param        up     �J�����̏�����x�N�g��
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void lookAt(const ts::Vector3Dd& eye, const ts::Vector3Dd& center, const ts::Vector3Dd& up) = 0;

        // Matrix����n

        virtual void pushMatrix() = 0;
        virtual void popMatrix() = 0;

        //    @brief        Transform����Matrix��ݒ肵�܂��B
        //    @note
        virtual void transform(const ts::Matrix& transform)  = 0;
        virtual void transform(const ts::Matrixd& transform)  = 0;

        //    @brief        �C�ӂ̃x�N�g�������Ƃ��ĉ�]����
        //    @note        (x,y,z)�̃x�N�g�������Ƃ���axis������]���܂��B
        //    @input        angle  ��]�p�x (-360.0�`360.0)
        //    @input        axis ��]��
        //    @retval        �Ȃ�
        virtual void rotate(float32_t angle , const Vector3D& axis) = 0;

        //    @brief        �ړ����܂�
        //    @note        x,y,z�����I�u�W�F�N�g���ړ����܂��B
        //    @param        x,y,z �ړ��x�N�g��
        //    @retval        �Ȃ�
        virtual void translation(const ts::Vector3D& t) = 0;


        //==============================================================================
        /**
            @brief        �X�P�[����ݒ肵�܂��B
            @note
            @param        x,y,z �X�P�[��
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void scale(const ts::Vector3D& scale) = 0;


        //==============================================================================
        // �}�e���A���E�e�N�X�`���֌W�̃��\�b�h
        //==============================================================================

       
        //==============================================================================
        /**
            @brief        �}�e���A���J���[��ݒ肵�܂�
            @note
            @param        ambientColor    Ambient�F
            @param        diffuseColor    Diffuse�F
            @param        emissiveColor    emissive�F
            @param        specularColor    specular�F
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void material(const ColorRGB& ambientColor, const ColorRGBA& diffuseColor, const ColorRGB& emissiveColor, const ColorRGB& specularColor)  = 0;

        //==============================================================================
        /**
            @brief        Shininess��ݒ肵�܂�
            @note
            @param        shininess Shininess
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void shininess(float32_t /*shininess*/) {}

         //==============================================================================
        /**
            @brief        TextureImage�̃|�C���^���Z�b�g���܂�
            @note        ���݂�TextureImage��ݒ肵�܂��B���ۂ̃I�u�W�F�N�g��ID�ŊǗ�����
                        �܂��B
            @param        tex TextureImage�̃��t�@�����X
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void texture2D(Texture2D& tex, const ColorRGBA& color = ColorRGBA(255,255,255,255)) = 0;

 

        //==============================================================================
        // �����_�����O�֌W�̃��\�b�h
        //==============================================================================


        //==============================================================================
        /**
            @brief        Line��`�悷��
            @note        3D���W�n�Ń��C����`�悷��
            @param        l1 ���C���̎n�_
            @param        l2 ���C���̏I�_
            @param        color ���C���̐F
            @param        width ���C���̑���
            @retval        �Ȃ�
        */
        //==============================================================================
        virtual void drawLine3D(const Vector3D& l1, const Vector3D& l2, const ColorRGBA& color, uint32_t width=1) = 0;


        /// @brief        2DLine��`�悷��
        /// @note         2D���W�n�Ń��C����`�悷��B
        ///               ProjectionMatrix,ViewMatrix�Ƃ��j�󂳂��̂Œ���
        /// @param        l1 ���C���̎n�_
        /// @param        l2 ���C���̏I�_
        /// @param        color ���C���̐F
        /// @param        width ���C���̑���
        /// @retval       �Ȃ�
        virtual void drawLine2D(const Vector2D& l1, const Vector2D& l2, const ColorRGBA& color, uint32_t width=1)  = 0;

        /// @brief  �l�p�`�̕��ʂ�`�悷��
        virtual void drawSolidRectangle(const Vertices4_3D& rect, const ColorRGBA& color) = 0;

        virtual void setGraphicContext(IGraphicsContext::ptr& gc) = 0;

        virtual void begin(PrimitiveType primitive) = 0;
        virtual void end() = 0;
        virtual void vertex(const Vector3D&) = 0;
        virtual void vertex(const Vector2D&) = 0;
        virtual void texcoord(const TexCoords& ) = 0;
        virtual void color(const ColorRGBA& color) = 0;
        virtual void normal(const Vector3D&) = 0;

        virtual void enableTexture2D() = 0;
        virtual void disableTexture2D() = 0;
        virtual void lineWidth(float32_t) = 0;
        virtual void loadIdentity() = 0;

        virtual void getProjectionMatrix(Matrix& m) = 0;
        virtual void getModelViewMatrix(Matrix& m) = 0;

        virtual void clearDepthBuffer() = 0; // DepthBuffer��������
        virtual void enablePolygonOffsetFill(bool enable = true) = 0;
        virtual void polygonOffset(float a, float b) = 0;
        virtual void enableDepthTest(bool enable = true) = 0;
        virtual void enableAlphaBlend(bool enable = true) = 0;
        virtual void enableCullFace(bool enable = true)  = 0;
        virtual void frontCullFace(bool enable = true)  = 0;
        


    };

      // ���S��begin-end�̂��߂̃N���X
      template <typename oT, typename pT>
      class beginPrimitiveT {
      public:
        beginPrimitiveT(oT* gl, pT pt) : m_gl(gl) {
          m_gl->begin(pt);
        }
        ~beginPrimitiveT() {
          m_gl->end();
        }
      private:
        oT* m_gl;
      };

      typedef beginPrimitiveT<IRenderer, IRenderer::PrimitiveType> beginPrimitive;


} // namespace
#endif
