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
        @brief        レンダリング用のインターフェイスクラス
        @note        IGraphicsのレンダリング時にレンダリングーデータを
                    受け渡すためのインターフェイスです。
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


        /// @brief        コンストラクタ
        IRenderer() {}
        /// @brief        デストラクタ
        virtual ~IRenderer() {}

        ///    @brief        初期化処理
        ///    @note        このクラスの初期化処理を行います。
        ///    @param        
        virtual void initialize(IGraphicsContext::ptr gc = IGraphicsContext::ptr()) = 0;

        ///    @brief        終了処理
        ///    @note        このクラスの終了処理を行います。
        virtual void finalize() = 0;

        /// @brief        frameの描画開始時に呼ばれます
        /// @note        
        virtual void onBeginFrame() {}

        /// @brief        frameの描画終了時に呼ばれます
        /// @note
        virtual void onEndFrame() {}

        /// @brief        個々のSceneの描画開始時に呼ばれます
        /// @note        
        virtual bool onBeginScene(IRendererScene* /*scene*/) { return true; }

        /// @brief        個々のSceneの描画終了時に呼ばれます
        /// @note
        virtual void onEndScene() {}

        /// @brief        個々のObjectの描画開始時に呼ばれます
        /// @note        
        virtual void onBeginObject(IGraphics*) {}

        /// @brief        個々のObjectの描画終了時に呼ばれます
        /// @note
        virtual void onEndObject(IGraphics*) {}


        //==============================================================================
        // カメラ用のメソッド
        //==============================================================================

        //==============================================================================
        /**
            @brief        Perspectiveを設定します
            @note        内容はOpenGLのgluPerspectiveと同じです。
            @param        fovy        視野角
            @param        aspectRatio 縦横比
            @param        nearPlane   NEARプレーン
            @param        farPlane    FARプレーン
            @retval        なし
        */
        //==============================================================================
        virtual void perspective(float64_t fovy, float64_t aspectRatio, float64_t nearPlane, float64_t farPlane) = 0;
        virtual void ortho(float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t zNear, float64_t zFar) = 0;
        virtual void frustum(float64_t x1, float64_t x2, float64_t y1, float64_t y2, float64_t znear, float64_t zfar) = 0;
        //==============================================================================
        /**
            @brief        ProjectionMatrixを設定します
            @note        ProjectinMatrixに対してMatrix値を設定します
            @param        matrix 設定するMatrix
            @retval        なし
        */
        //==============================================================================
        virtual void selectProjectionMatrix()  = 0;
        virtual void selectModelViewMatrix() = 0;

        //==============================================================================
        /**
            @brief        視点、注視点、カメラの向きを設定します。
            @note        OpenGLのgluLookAtと同じです。
            @param        eye    カメラの位置
            @param        center 注視点
            @param        up     カメラの上方向ベクトル
            @retval        なし
        */
        //==============================================================================
        virtual void lookAt(const ts::Vector3Dd& eye, const ts::Vector3Dd& center, const ts::Vector3Dd& up) = 0;

        // Matrix操作系

        virtual void pushMatrix() = 0;
        virtual void popMatrix() = 0;

        //    @brief        TransformするMatrixを設定します。
        //    @note
        virtual void transform(const ts::Matrix& transform)  = 0;
        virtual void transform(const ts::Matrixd& transform)  = 0;

        //    @brief        任意のベクトルを軸として回転する
        //    @note        (x,y,z)のベクトルを軸としてaxisだけ回転します。
        //    @input        angle  回転角度 (-360.0～360.0)
        //    @input        axis 回転軸
        //    @retval        なし
        virtual void rotate(float32_t angle , const Vector3D& axis) = 0;

        //    @brief        移動します
        //    @note        x,y,zだけオブジェクトを移動します。
        //    @param        x,y,z 移動ベクトル
        //    @retval        なし
        virtual void translation(const ts::Vector3D& t) = 0;


        //==============================================================================
        /**
            @brief        スケールを設定します。
            @note
            @param        x,y,z スケール
            @retval        なし
        */
        //==============================================================================
        virtual void scale(const ts::Vector3D& scale) = 0;


        //==============================================================================
        // マテリアル・テクスチャ関係のメソッド
        //==============================================================================

       
        //==============================================================================
        /**
            @brief        マテリアルカラーを設定します
            @note
            @param        ambientColor    Ambient色
            @param        diffuseColor    Diffuse色
            @param        emissiveColor    emissive色
            @param        specularColor    specular色
            @retval        なし
        */
        //==============================================================================
        virtual void material(const ColorRGB& ambientColor, const ColorRGBA& diffuseColor, const ColorRGB& emissiveColor, const ColorRGB& specularColor)  = 0;

        //==============================================================================
        /**
            @brief        Shininessを設定します
            @note
            @param        shininess Shininess
            @retval        なし
        */
        //==============================================================================
        virtual void shininess(float32_t /*shininess*/) {}

         //==============================================================================
        /**
            @brief        TextureImageのポインタをセットします
            @note        現在のTextureImageを設定します。実際のオブジェクトはIDで管理され
                        ます。
            @param        tex TextureImageのリファレンス
            @retval        なし
        */
        //==============================================================================
        virtual void texture2D(Texture2D& tex, const ColorRGBA& color = ColorRGBA(255,255,255,255)) = 0;

 

        //==============================================================================
        // レンダリング関係のメソッド
        //==============================================================================


        //==============================================================================
        /**
            @brief        Lineを描画する
            @note        3D座標系でラインを描画する
            @param        l1 ラインの始点
            @param        l2 ラインの終点
            @param        color ラインの色
            @param        width ラインの太さ
            @retval        なし
        */
        //==============================================================================
        virtual void drawLine3D(const Vector3D& l1, const Vector3D& l2, const ColorRGBA& color, uint32_t width=1) = 0;


        /// @brief        2DLineを描画する
        /// @note         2D座標系でラインを描画する。
        ///               ProjectionMatrix,ViewMatrixとも破壊されるので注意
        /// @param        l1 ラインの始点
        /// @param        l2 ラインの終点
        /// @param        color ラインの色
        /// @param        width ラインの太さ
        /// @retval       なし
        virtual void drawLine2D(const Vector2D& l1, const Vector2D& l2, const ColorRGBA& color, uint32_t width=1)  = 0;

        /// @brief  四角形の平面を描画する
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

        virtual void clearDepthBuffer() = 0; // DepthBufferを初期化
        virtual void enablePolygonOffsetFill(bool enable = true) = 0;
        virtual void polygonOffset(float a, float b) = 0;
        virtual void enableDepthTest(bool enable = true) = 0;
        virtual void enableAlphaBlend(bool enable = true) = 0;
        virtual void enableCullFace(bool enable = true)  = 0;
        virtual void frontCullFace(bool enable = true)  = 0;
        


    };

      // 安全なbegin-endのためのクラス
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
