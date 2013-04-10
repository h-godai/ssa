
#ifndef LATLONG_HPP
#define LATLONG_HPP

#ifndef STDAFX_H
#include <string>
#include <iostream>
#endif
#include <math.h>

#include "Vector.h"

namespace ts { namespace latlng {

  typedef uint32_t GcsType; // ���n�n(Geographical coordinate system)�̌^
  const GcsType InvalidGcsType = 0; // �����Ȓl
  const GcsType TokyoGcs = 9; // �������n�n

  struct {
    const _TCHAR* name;
    double lat;
    double lng;
  } const GcsTable[] = {
     { _T("�ܓx�o�x����"), 0.0, 0.0 },
     { _T("1�n(����,������)"), 33.00, 129.50 },
     { _T("2�n(����,����,�F�{,�啪,�{��,������)"), 33.00, 131.00 },
     { _T("3�n(�R��,����,�L��)"), 36.00, 132.166667},
     { _T("4�n(����,���Q,����,���m)"), 33.00, 132.166667},
     { _T("5�n(����,����,���R)"), 36.00, 134.333333},
     { _T("6�n(���s,���,����,����,�O�d,�ޗ�,�a�̎R)"), 36.00, 136.00},
     { _T("7�n(�ΐ�,�x�R,��,���m)"), 36.00, 137.166667},
     { _T("8�n(�V��,����,�R��,�É�)"), 36.00, 138.50},
     { _T("9�n(����,����,�Ȗ�,���,���,��t,�Q�n,�_�ސ�)"), 36.00, 139.8333333333 },
     { _T("10�n(�X,�H�c,�R�`,���,�{��"), 40.00, 140.833333333 },
     { _T("11�n(�k�C����)"), 44.00, 140.25 },
     { _T("12�n(�k�C������)"), 44.00, 142.24 },
     { _T("13�n(�k�C����)"), 44.00, 144.25 },
     { _T("14�n(���}��)"), 26.00, 142.00 },
     { _T("15�n(����)"),   26.00, 127.50 },
     { _T("16�n(�{��,�^�ߍ�,�Ί_,���\)"),   26.00, 124.00 },
     { _T("17�n(�哌��)"),   26.00, 131.00 },
     { _T("18�n(���m����)"), 20.00, 136.00 },
     { _T("19�n(�쒹��)"),   26.00, 154.00 }
  };

  struct LL_STR
  {
      double  degrees;
      double  minutes;
      double  seconds;
  };

  //  1�n�`19�n�̌��_�k��
  const LL_STR lat0List[] = 
            {  
                {33,00,00.0000}, {33,00,00.0000},{36,00,00.0000}, 
                {33,00,00.0000}, {36,00,00.0000},{36,00,00.0000}, 
                {36,00,00.0000}, {36,00,00.0000},{36,00,00.0000},
                {40,00,00.0000}, {44,00,00.0000},{44,00,00.0000},
                {44,00,00.0000}, {26,00,00.0000},{26,00,00.0000}, 
                {26,00,00.0000}, {26,00,00.0000},{20,00,00.0000}, 
                {26,00,00.0000} 
            };
        
  //  1�n�`19�n�̌��_���o
  const  LL_STR lng0List[] =
            {  
                {129,30,00.0000}, {131,00,00.0000}, {132,10,00.0000},
                {133,30,00.0000}, {134,20,00.0000}, {136,00,00.0000},
                {137,10,00.0000}, {138,30,00.0000}, {139,50,00.0000},
                {140,50,00.0000}, {140,15,00.0000}, {142,15,00.0000},
                {144,15,00.0000}, {142,00,00.0000}, {127,30,00.0000}, 
                {124,00,00.0000}, {131,00,00.0000}, {136,00,00.0000}, 
                {154,00,00.0000}
            };

  // �����������Ƃ��̍ŏ��P�ʁB�ԓ���Ŗ�1cm
  const double LatLongIntegerResolution = static_cast<double>(40000.0*1000.0*100.0);

  // 64bit�����l�ŕ\���ܓx�o�x
  typedef uint64_t IntegerType;

  struct LatLong
  {
      double latitude;    //  �ܓx
      double longitude;   //  �o�x


      LatLong( double lat = 0.0, double lng = 0.0 ) 
        : latitude(lat)
        , longitude(lng)
      {
      }

      template <typename FL>
      LatLong( const Vector2DT<FL>& v2d ) 
        : latitude(v2d.y)
        , longitude(v2d.x)
      {
      }

      // ���n�n�ƃI�t�Z�b�g�l(���[�g��)����ݒ肷��
      template <typename FL>
      LatLong(GcsType kei, const Vector2DT<FL>& offset) 
        : latitude(getLat(kei))
        , longitude(getLng(kei))
      {
        // 1�x������̋��������߂�
        Vector2Dd unit = getDistance(LatLong(latitude + 1.0, longitude + 1.0));
        // �ܓx�o�x�ɓx����������
        longitude -= offset.x / unit.x;
        latitude -= offset.y / unit.y;
      }
      

      LatLong(  int32_t d1, int32_t m1, double s1, 
                int32_t d2, int32_t m2, double s2 )
        : latitude(d1 + m1/60.0 + s1/3600.0)
        , longitude(d2 + m2/60.0 + s2/3600.0)
      {
      }

      LatLong(IntegerType latlng64) {
        *this = latlng64;
      }

      LatLong& operator = (IntegerType ll64) {
        uint32_t lat = static_cast<uint32_t>(ll64 >> 32);
        uint32_t lng = static_cast<uint32_t>(ll64 & 0xffffffffLL);
        latitude = (360.0 * lat * LatLongIntegerResolution) - 180.0;
        longitude = (180.0 * lng * LatLongIntegerResolution) - 90.0;
        return *this;
      }

      // �ܓx�o�x��IntegerType�Ŏ擾����
      IntegerType getInteger() const {
        uint64_t lat = static_cast<uint64_t>(LatLongIntegerResolution * (latitude + 180.0) / 360.0) & 0xffffffffLL;
        uint64_t lng = static_cast<uint64_t>(LatLongIntegerResolution * (longitude + 90.0) / 180.0) & 0xffffffffLL;
        return (lat << 32) + lng;
      }

      operator IntegerType () const {
        return getInteger();
      }

      // Vector2Dd�ɕϊ�
      Vector2Dd toVector2D() const 
      {
        return Vector2Dd(longitude, latitude);
      }

      // �Ώۂ̈ܓx�o�x�Ƃ̋��������߂�
      Vector2Dd getDistance(const LatLong& dest ) const 
      {
          return getDistance( dest.latitude, dest.longitude );
      }

      // �ܓx�o�x����߂��n����������
      GcsType searchGcsPoint() const
      {
        uint32_t min_kei = 9;
        Vector2Dd min_pos = toM( min_kei );
        float64_t min_len = min_pos.length();
        
        for( uint32_t kei=0; kei<sizeof( lat0List )/sizeof(LL_STR); ++kei )
        {
            if( kei == 8 ) continue;
            Vector2Dd pos = toM( kei+1 );
            float64_t len = pos.length();
            if( min_len > len ) 
            {
                min_len = len;
                min_kei = kei+1; 
            }
        }

        return min_kei; // tokyo
      }

      static double getLat(GcsType kei) {
          assert(kei > 0 && kei <= 19);
          return lat0List[kei-1].degrees + lat0List[kei-1].minutes/60.0 + lat0List[kei-1].seconds/3600.0;
      }
      static double getLng(GcsType kei) {
          assert(kei > 0 && kei <= 19);
          return lng0List[kei-1].degrees + lng0List[kei-1].minutes/60.0 + lng0List[kei-1].seconds/3600.0;
      }

      // �n����̋������v�Z����
      Vector2Dd toM() const {
        return toM(searchGcsPoint());
      }

      Vector2Dd toM(GcsType kei) const {
          return getDistance(getLat(kei), getLng(kei));
      }

      // �n����̋������v�Z����
      Vector2Dd getDistance( double  decLat0, double  decLng0 ) const
      {
          double  decLat  = latitude;
          double  decLng  = longitude;

          double  pai      = 3.141592;
          double  radLat   = decLat  * pai / 180.0;
          double  radLng   = decLng  * pai / 180.0;
          double  radLat0  = decLat0 * pai / 180.0;
          double  radLng0  = decLng0 * pai / 180.0;

          double  a  = 6378137;                                         // �����a�i���E���n�n�j
          double  f  = 1.0 /  298.257222101;                            // �G�����i���E���n�n�j
          double  e1 = sqrt( 2.0 * f - pow( f, 2.0 )  );                // ��ꗣ�S��
          double  e2 = sqrt( 2.0 * 1.0 / f - 1.0) / (1.0 / f - 1.0);    // ��񗣐S��

            //  �ԓ�����w��ܓx�܂ł̎q�ߐ��ʒ������߂�
          double  pA =  1.0 + 3.0 / 4.0 * pow( e1, 2.0 ) + 45.0 / 64.0 * pow( e1, 4.0 ) + 175.0 / 256.0 * pow( e1, 6.0 ) + 
                        11025.0 / 16384.0 * pow( e1, 8.0 ) + 43659.0 / 65536.0 * pow( e1, 10.0 ) +
                        693693.0 / 1048576.0 * pow( e1, 12.0 ) + 19324305.0 / 29360128.0 * pow( e1, 14.0 ) +
                        4927697775.0 / 7516192768.0 * pow( e1, 16.0 );
          double  pB =  3.0 / 4.0 * pow( e1, 2.0 ) + 15.0 / 16.0 * pow( e1, 4.0 ) + 525.0 / 512.0 * pow( e1, 6.0 ) +
                        2205.0 / 2048.0 * pow( e1, 8.0 ) + 72765.0 / 65536.0 * pow( e1, 10.0 ) + 
                        297297.0 / 262144.0 * pow( e1, 12.0 ) + 135270135.0 / 117440512.0 * pow( e1, 14.0 ) + 
                        547521975.0 / 469762048.0 * pow( e1, 16.0 );
          double  pC =  15.0 / 64.0 * pow( e1, 4.0 ) + 105.0 / 256.0 * pow( e1, 6.0 ) + 2205.0 / 4096.0 * pow( e1, 8.0 ) + 
                        10395.0 / 16384.0 * pow( e1, 10.0 ) + 1486485.0 / 2097152.0 * pow( e1, 12.0 ) + 
                        45090045.0 / 58720256.0 * pow( e1, 14.0 ) + 766530765.0 / 939524096.0 * pow( e1, 16.0 );
          double  pD =  35.0 / 512.0 * pow( e1, 6.0 ) + 315.0 / 2048.0 * pow( e1, 8.0 ) + 
                        31185.0 / 131072.0 * pow( e1, 10.0 ) + 165165.0 / 524288.0 * pow( e1, 12.0 ) +
                        45090045.0 / 117440512.0 * pow( e1, 14.0 )  + 209053845.0 / 469762048.0 * pow( e1, 16.0 );
          double  pE =  315.0 / 16384.0 * pow( e1, 8.0 ) + 3465.0 / 65536.0 * pow( e1, 10.0 ) + 
                        99099.0 / 1048576.0 * pow( e1, 12.0 ) + 4099095.0 / 29360128.0 * pow( e1, 14.0 ) + 
                        348423075.0 / 1879048192.0 * pow( e1, 16.0 );
          double  pF =  693.0 / 131072.0 * pow( e1, 10.0 ) + 9009.0 / 524288.0 * pow( e1, 12.0 ) + 
                        4099095.0 / 117440512.0 * pow( e1, 14.0 ) + 26801775.0 / 469762048.0 * pow( e1, 16.0 );
          double  pG =  3003.0 / 2097152.0 * pow( e1, 12.0 ) + 315315.0 / 58720256.0 * pow( e1, 14.0 ) + 
                        11486475.0 / 939524096.0 * pow( e1, 16.0 );
          double  pH =  45045.0 / 117440512.0 * pow( e1, 14.0 ) + 765765.0 / 469762048.0 * pow( e1, 16.0 );
          double  pI =  765765.0 / 7516192768.0 * pow( e1, 16.0 );

          //  �ԓ�����w��ܓx�܂ł̎q�ߐ��ʒ������߂�
          double bCoef  = a * ( 1.0 - pow( e1, 2.0 ) );
          double b1     = bCoef * pA;
          double b2     = bCoef * -pB / 2.0;
          double b3     = bCoef * pC / 4.0;
          double b4     = bCoef * -pD / 6.0;
          double b5     = bCoef * pE / 8.0;
          double b6     = bCoef * -pF / 10.0;
          double b7     = bCoef * pG / 12.0;
          double b8     = bCoef * -pH / 14.0;
          double b9     = bCoef * pI / 16.0;

          //  �ԓ�����w��ܓx�܂ł̎q�ߐ��ʒ������߂�
          //  �ԓ�������W�n�̌��_�̈ܓx�܂ł̎q�ߐ��ʒ�
          double s0 = b1 * radLat0 + b2 * sin(2.0 * radLat0) + 
                      b3 * sin(4.0 * radLat0)  + b4 * sin(6.0 * radLat0) + 
                      b5 * sin(8.0 * radLat0)  + b6 * sin(10.0 * radLat0) + 
                      b7 * sin(12.0 * radLat0) + b8 * sin(14.0 * radLat0) + 
                      b9 * sin(16.0 * radLat0);

          //  �ԓ�������͈ܓx�܂ł̎q�ߐ��ʒ�
          double s  = b1 * radLat + b2 * sin(2.0 * radLat) + 
                      b3 * sin(4.0 * radLat)  + b4 * sin(6.0 * radLat) + 
                      b5 * sin(8.0 * radLat)  + b6 * sin(10.0 * radLat) + 
                      b7 * sin(12.0 * radLat) + b8 * sin(14.0 * radLat) + 
                      b9 * sin(16.0 * radLat);

          //  �o�x�̍��i�����𐳂ɂƂ�j
          double  deltaLambda  = radLng - radLng0;
          double  eta2         = pow( e2, 2.0 )  * pow( cos( radLat ), 2.0 );
          double  t            = tan( radLat );
          //  ���W�n�̌��_�ɂ�����k�ڌW��
          double  m0           = 0.9999;
          // �K�ѐ��ȗ����a
          double  w            = sqrt( 1.0 - pow( e1, 2.0 ) * pow( sin(radLat), 2.0 ) );
          double  n            = a / w;

          // �w�x���W�����߂�
          double x =  ((s - s0) + 1.0 / 2.0 * n * pow( cos(radLat), 2.0 ) * t * pow( deltaLambda, 2.0 ) + 
                      1.0 / 24.0 * n * pow( cos(radLat), 4.0 ) * t * 
                      (5.0 - pow( t, 2.0 ) + 9.0 * eta2 + 4.0 * pow( eta2, 2.0 ) ) * pow( deltaLambda, 4.0 ) - 
                      1.0 / 720.0 * n * pow( cos(radLat), 6.0 ) * t * 
                      (-61.0 + 58.0 * pow( t, 2.0 ) - pow( t, 4.0 ) - 270.0 * eta2 + 330.0 * pow( t, 2.0 ) * eta2) * 
                      pow( deltaLambda, 6.0 ) - 
                      1.0 / 40320.0 * n * pow( cos(radLat), 8.0 ) * t * 
                      (-1385.0 + 3111.0 * pow( t, 2.0 ) - 543.0 * pow( t, 4.0 ) + pow( t, 6.0) ) * pow( deltaLambda, 8.0 ) ) * m0;

          double y =  (n * cos(radLat) * deltaLambda - 
                      1.0 / 6.0 * n * pow( cos(radLat), 3.0 ) * (-1.0 + pow( t, 2.0 ) - eta2) * 
                      pow( deltaLambda, 3.0 ) - 
                      1.0 / 120.0 * n * pow( cos(radLat), 5.0 ) * 
                      (-5.0 + 18.0 * pow( t, 2.0 ) - pow( t, 4.0 ) - 14.0 * eta2 + 58.0 * pow( t, 2.0 ) * eta2) * 
                      pow( deltaLambda, 5.0 ) - 
                      1.0 / 5040.0 * n * pow( cos(radLat), 7.0 ) * 
                      (-61.0 + 479.0 * pow( t, 2.0 ) - 179.0 * pow( t, 4.0 ) + pow( t, 6.0 ) ) * pow( deltaLambda, 7.0 ) ) * m0;

          return Vector2Dd( y, x );
     }

  };

}} // namespace

#endif;