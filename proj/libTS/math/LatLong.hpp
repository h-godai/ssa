
#ifndef LATLONG_HPP
#define LATLONG_HPP

#ifndef STDAFX_H
#include <string>
#include <iostream>
#endif
#include <math.h>

#include "Vector.h"

namespace ts { namespace latlng {

  typedef uint32_t GcsType; // 測地系(Geographical coordinate system)の型
  const GcsType InvalidGcsType = 0; // 無効な値
  const GcsType TokyoGcs = 9; // 東京測地系

  struct {
    const _TCHAR* name;
    double lat;
    double lng;
  } const GcsTable[] = {
     { _T("緯度経度入力"), 0.0, 0.0 },
     { _T("1系(長崎,鹿児島)"), 33.00, 129.50 },
     { _T("2系(福岡,佐賀,熊本,大分,宮崎,鹿児島)"), 33.00, 131.00 },
     { _T("3系(山口,島根,広島)"), 36.00, 132.166667},
     { _T("4系(香川,愛媛,徳島,高知)"), 33.00, 132.166667},
     { _T("5系(兵庫,鳥取,岡山)"), 36.00, 134.333333},
     { _T("6系(京都,大阪,福井,滋賀,三重,奈良,和歌山)"), 36.00, 136.00},
     { _T("7系(石川,富山,岐阜,愛知)"), 36.00, 137.166667},
     { _T("8系(新潟,長野,山梨,静岡)"), 36.00, 138.50},
     { _T("9系(東京,福島,栃木,茨城,埼玉,千葉,群馬,神奈川)"), 36.00, 139.8333333333 },
     { _T("10系(青森,秋田,山形,岩手,宮城"), 40.00, 140.833333333 },
     { _T("11系(北海道西)"), 44.00, 140.25 },
     { _T("12系(北海道中央)"), 44.00, 142.24 },
     { _T("13系(北海道東)"), 44.00, 144.25 },
     { _T("14系(小笠原)"), 26.00, 142.00 },
     { _T("15系(沖縄)"),   26.00, 127.50 },
     { _T("16系(宮古,与那国,石垣,西表)"),   26.00, 124.00 },
     { _T("17系(大東島)"),   26.00, 131.00 },
     { _T("18系(沖ノ鳥島)"), 20.00, 136.00 },
     { _T("19系(南鳥島)"),   26.00, 154.00 }
  };

  struct LL_STR
  {
      double  degrees;
      double  minutes;
      double  seconds;
  };

  //  1系〜19系の原点北緯
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
        
  //  1系〜19系の原点東経
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

  // 製数化したときの最小単位。赤道上で約1cm
  const double LatLongIntegerResolution = static_cast<double>(40000.0*1000.0*100.0);

  // 64bit製数値で表す緯度経度
  typedef uint64_t IntegerType;

  struct LatLong
  {
      double latitude;    //  緯度
      double longitude;   //  経度


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

      // 測地系とオフセット値(メートル)から設定する
      template <typename FL>
      LatLong(GcsType kei, const Vector2DT<FL>& offset) 
        : latitude(getLat(kei))
        , longitude(getLng(kei))
      {
        // 1度あたりの距離を求める
        Vector2Dd unit = getDistance(LatLong(latitude + 1.0, longitude + 1.0));
        // 緯度経度に度数を加える
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

      // 緯度経度をIntegerTypeで取得する
      IntegerType getInteger() const {
        uint64_t lat = static_cast<uint64_t>(LatLongIntegerResolution * (latitude + 180.0) / 360.0) & 0xffffffffLL;
        uint64_t lng = static_cast<uint64_t>(LatLongIntegerResolution * (longitude + 90.0) / 180.0) & 0xffffffffLL;
        return (lat << 32) + lng;
      }

      operator IntegerType () const {
        return getInteger();
      }

      // Vector2Ddに変換
      Vector2Dd toVector2D() const 
      {
        return Vector2Dd(longitude, latitude);
      }

      // 対象の緯度経度との距離を求める
      Vector2Dd getDistance(const LatLong& dest ) const 
      {
          return getDistance( dest.latitude, dest.longitude );
      }

      // 緯度経度から近い系を検索する
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

      // 系からの距離を計算する
      Vector2Dd toM() const {
        return toM(searchGcsPoint());
      }

      Vector2Dd toM(GcsType kei) const {
          return getDistance(getLat(kei), getLng(kei));
      }

      // 系からの距離を計算する
      Vector2Dd getDistance( double  decLat0, double  decLng0 ) const
      {
          double  decLat  = latitude;
          double  decLng  = longitude;

          double  pai      = 3.141592;
          double  radLat   = decLat  * pai / 180.0;
          double  radLng   = decLng  * pai / 180.0;
          double  radLat0  = decLat0 * pai / 180.0;
          double  radLng0  = decLng0 * pai / 180.0;

          double  a  = 6378137;                                         // 長半径（世界測地系）
          double  f  = 1.0 /  298.257222101;                            // 扁平率（世界測地系）
          double  e1 = sqrt( 2.0 * f - pow( f, 2.0 )  );                // 第一離心率
          double  e2 = sqrt( 2.0 * 1.0 / f - 1.0) / (1.0 / f - 1.0);    // 第二離心率

            //  赤道から指定緯度までの子午線弧長を求める
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

          //  赤道から指定緯度までの子午線弧長を求める
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

          //  赤道から指定緯度までの子午線弧長を求める
          //  赤道から座標系の原点の緯度までの子午線弧長
          double s0 = b1 * radLat0 + b2 * sin(2.0 * radLat0) + 
                      b3 * sin(4.0 * radLat0)  + b4 * sin(6.0 * radLat0) + 
                      b5 * sin(8.0 * radLat0)  + b6 * sin(10.0 * radLat0) + 
                      b7 * sin(12.0 * radLat0) + b8 * sin(14.0 * radLat0) + 
                      b9 * sin(16.0 * radLat0);

          //  赤道から入力緯度までの子午線弧長
          double s  = b1 * radLat + b2 * sin(2.0 * radLat) + 
                      b3 * sin(4.0 * radLat)  + b4 * sin(6.0 * radLat) + 
                      b5 * sin(8.0 * radLat)  + b6 * sin(10.0 * radLat) + 
                      b7 * sin(12.0 * radLat) + b8 * sin(14.0 * radLat) + 
                      b9 * sin(16.0 * radLat);

          //  経度の差（東方を正にとる）
          double  deltaLambda  = radLng - radLng0;
          double  eta2         = pow( e2, 2.0 )  * pow( cos( radLat ), 2.0 );
          double  t            = tan( radLat );
          //  座標系の原点における縮尺係数
          double  m0           = 0.9999;
          // 卯酉線曲率半径
          double  w            = sqrt( 1.0 - pow( e1, 2.0 ) * pow( sin(radLat), 2.0 ) );
          double  n            = a / w;

          // ＸＹ座標を求める
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