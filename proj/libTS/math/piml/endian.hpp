/********************************************************************** -*-tab-width:4-*-

    @brief Platform Independent Math Library
    @brief 環境に依存しない数値演算ライブラリ
    @author h.godai godai@techarts.co.jp
    @note  サポートプラットフォーム
    @note  Windows (x86,x64), iPhone(ARM), Android(x86), PS-VITA(ARM), PS3(PowerPC,cell)

****************************************************************************************/




#ifndef TS_PIML_ENDIAN_HPP
#define TS_PIML_ENDIAN_HPP

#include <boost/static_assert.hpp>
//#include <boost/cstdint.hpp>

namespace piml {
  namespace endian {
  //  using boost::uint8_t;

    // 比較的高速な、変数単位でのバイトスワップ
    inline uint16_t bswap(uint16_t x) {
      return (x >> 8) | (x << 8);
    }
    inline int16_t bswap(int16_t x) {
      return static_cast<int16_t>(bswap(static_cast<uint16_t>(x)));
    }

    inline uint32_t bswap(uint32_t x) {
      x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
      return (x >> 16) | (x << 16);
    }
    inline int32_t bswap(int32_t x) {
      return static_cast<int32_t>(bswap(static_cast<uint32_t>(x)));
    }
    inline uint64_t bswap(uint64_t x) {
      union {
        uint64_t v64;
        uint32_t v32[2];
      } s, d;
      s.v64 = x;
      d.v32[0] = bswap(s.v32[1]);
      d.v32[1] = bswap(s.v32[0]);
      return d.v64;
    }
    inline int64_t bswap(int64_t x) {
      return static_cast<int64_t>(bswap(static_cast<uint64_t>(x)));
    }

    // バイトストリームストリームから エンディアンを入れ替えて値を得る
    template <int N> struct swap_bs {};
    template <> struct swap_bs<8> {
        swap_bs(const void* vsp, uint8_t* dp) {
            const uint8_t* sp = reinterpret_cast<const uint8_t*>(vsp);
            dp += 7;
            *dp = *sp;      // 0
            *--dp = *++sp;  // 1
            *--dp = *++sp;  // 2
            *--dp = *++sp;  // 3
            *--dp = *++sp;  // 4
            *--dp = *++sp;  // 5
            *--dp = *++sp;  // 6
            *--dp = *++sp;  // 7
        }
    };
    template <> struct swap_bs<4> {
      swap_bs(const void* vsp, uint8_t* dp) {
          const uint8_t* sp = reinterpret_cast<const uint8_t*>(vsp);
          dp += 3;
          *dp = *sp;      // 0
          *--dp = *++sp;  // 1
          *--dp = *++sp;  // 2
          *--dp = *++sp;  // 3
      }
    };
    template <> struct swap_bs<2> {
      swap_bs(const void* vsp, uint8_t* dp) {
          const uint8_t* sp = reinterpret_cast<const uint8_t*>(vsp);
          ++dp;
          *dp = *sp;      // 0
          *--dp = *++sp;  // 1
      }
    };
    // ポインタから変数のエンディアンを入れ替える
    template <typename VT>
    inline VT swap_value(const void* src) {
  	  union {
        VT      value_;
        uint8_t v8_[sizeof(VT)];
      } d;
      swap_bs<sizeof(VT)>(src, &d.v8_[0]);
      return d.value_;
    }

    // double用
    inline double bswap(double src)     { return swap_value<double>(&src);   }
    // float用
    inline float bswap(float src)       { return swap_value<float>(&src);    }

    // 主にenum用 4バイト系のデータ固定
    template <typename ET>
    inline ET bswap(ET src) {
      BOOST_STATIC_ASSERT(sizeof(ET) == 4);
      return static_cast<ET>(bswap(static_cast<uint32_t>(src)));
    }



    // 自分自身をbswap
    template <typename T>
    inline void sbswap(T& x) {
        x = bswap(x);
    };

    template <typename T, size_t N>
    inline void sbswap(T (&x)[N]) {
      for (size_t n = 0; n < N; ++n) {
            sbswap(x[n]);
        }
    }
    



    // little endianでデータを保存するための型定義
    namespace detail {
        template <typename VT, typename C>
        struct le_operator {
            typedef VT value_type;
            const value_type operator & (value_type t) const {
                return t & value();
            }
            const value_type operator | (value_type t) const {
                return t | value();
            }
            void operator &= (value_type t) {
                value_set(value() & t);
            }
            void operator |= (value_type t) {
                value_set(value() | t);
            }
            bool operator == (value_type t) const {
                return value() == t;
            }
            bool operator != (value_type t) const {
                return value() != t;
            }
            value_type operator ++ () {
                value_set(static_cast<value_type>(value() + 1));
                return value();
            }
            value_type operator -- () {
                value_set(static_cast<value_type>(value() - 1));
                return value();
            }
            bool is_zero() const { return value() == static_cast<value_type>(0); }
            void clear() { value_set(static_cast<value_type>(0)); }
        
        private:   
            value_type value() const { return ((const C*)this)->operator value_type (); }
            void value_set(value_type v) { *(C*)this = v; }
        };

        // byte stream type
        // ストリームの中のポインタから値を設定・取得する
        template <EndianType ENDIAN, typename VT>
        struct le_bs { private: le_bs() {} };
        template <typename VT>
        struct le_bs<LittleEndian, VT> {
            typedef VT value_type;
            uint8_t value_[sizeof(VT)];
			value_type get() const { return *reinterpret_cast<value_type*>(&value_[0]); }
            operator value_type () const { return *reinterpret_cast<value_type*>(&value_[0]); }
            void set(const le_bs<LittleEndian, VT>& v) { *this = v; }
            void set(value_type v) { *reinterpret_cast<value_type*>(&value_[0]) = v; }
            void operator = (const void* ptr) {
                 *reinterpret_cast<value_type*>(&value_[0]) = *reinterpret_cast<const value_type*>(ptr);
            }
            operator const void* () const { return &value_[0]; }
        };
        template <typename VT>
        struct le_bs<BigEndian, VT> {
            typedef VT value_type;
            uint8_t value_[sizeof(VT)];
			value_type get() const { return swap_value<VT>(&value_[0]); }
            operator value_type () const { return swap_value<VT>(&value_[0]); }
            void set(const le_bs<BigEndian, VT>& v) { *this = v; }
            void set(value_type v) { swap_bs<sizeof(VT)>(&v, &value_[0]); }
            uint8_t* ptr() { return &value_[0]; }
            const uint8_t* ptr() const { return &value_[0]; }
            void operator = (const void* ptr) {
                const uint8_t* src = reinterpret_cast<const uint8_t*>(ptr);
                value_[0] = *src;
                value_[1] = *++src;
                value_[2] = *++src;
                value_[3] = *++src;
            }
            operator const void* () const { return &value_[0]; }
        };

        // no constructor type
        template <EndianType ENDIAN, typename VT>
        struct le_nc { private: le_nc() {} };
        template <typename VT>
        struct le_nc<LittleEndian, VT> {
            typedef VT value_type;
            value_type value_;
            operator value_type () const { return value_; }
            void set(const le_nc<LittleEndian, VT>& v) { value_ = v.value_; }
            void set(value_type v) { value_ = v; }
			value_type get() const { return value_; }
            void operator = (const void* ptr) {
                 value_ = *reinterpret_cast<const value_type*>(ptr);
            }
            operator const void* () const { return &value_; }
        };
        template <typename VT>
        struct le_nc<BigEndian, VT>  {
            typedef VT value_type;
            value_type value_;
            operator value_type () const { return bswap(value_); }
            void set(const le_nc<BigEndian, VT>& v) { value_ = v.value_; }
            void set(value_type v) { value_ = bswap(v); }
			value_type get() const { return bswap(value_); }
            void operator = (const void* ptr) {
                const uint8_t* src = reinterpret_cast<const uint8_t*>(ptr);
                value_[0] = *src;
                value_[1] = *++src;
                value_[2] = *++src;
                value_[3] = *++src;
            }
            operator const void* () const { return &value_; }
        };
        
        // has constructor type
        template <EndianType ENDIAN, typename VT>
        struct le { private: le() {} };
        template <typename VT>
        struct le<LittleEndian, VT>  : le_nc<LittleEndian, VT> {
            typedef VT value_type;
            using le_nc<LittleEndian, VT>::value_;
            le(value_type v = 0) { value_ = v; }
        };
        template <typename VT>
        struct le<BigEndian, VT> : le_nc<BigEndian, VT> {
            typedef VT value_type;
            le(value_type v = 0) { this->value_ = bswap(v); }
        };
        

    } // namespace detail

    template <typename VT>
    struct le : detail::le<Endian, VT> , detail::le_operator<VT, le<VT> >{
        typedef VT value_type;
        typedef detail::le<Endian, value_type> Impl;
        typedef detail::le_operator<value_type, le<value_type> > Op;
        void operator = (const le<value_type>& v) { Impl::set(v);}
        void operator = (value_type v) { Impl::set(v);}
        void operator = (const void* v) { Impl::operator = (v); }
        operator const void* () const { return Impl::operator const void* (); }
        using Op::operator & ;
        using Op::operator &= ;
        using Op::operator | ;
        using Op::operator |= ;
        using Op::operator == ;
        using Op::operator != ;
        using Op::operator ++ ;
        using Op::operator -- ;
        le(value_type v = 0) : detail::le<Endian, value_type>(v) {}
    };
    template <typename VT>
    struct le_nc : detail::le_nc<Endian, VT>, detail::le_operator<VT, le_nc<VT> > {
        typedef VT value_type;
        typedef detail::le_nc<Endian, VT> Impl;
        typedef detail::le_operator<VT, le_nc<VT> > Op;
        void operator = (const le<VT>& v) { Impl::set(v);}
        void operator = (value_type v) { Impl::set(v);}
        void operator = (const void* v) { Impl::operator = (v); }
        operator const void* () const { return Impl::operator const void* (); }
        using Op::operator & ;
        using Op::operator &= ;
        using Op::operator | ;
        using Op::operator |= ;
        using Op::operator == ;
        using Op::operator != ;
        using Op::operator ++ ;
        using Op::operator -- ;
    };

    template <typename VT>
    struct le_bs : detail::le_bs<Endian, VT>, detail::le_operator<VT, le_bs<VT> >  {
        typedef VT value_type;
        typedef detail::le_bs<Endian, VT> Impl;
        typedef detail::le_operator<VT, le_bs<VT> > Op;
        void operator = (const le<VT>& v) { Impl::set(v);}
        void operator = (value_type v) { Impl::set(v);}
        void operator = (const void* v) { Impl::operator = (v); }
        operator const void* () const { return Impl::operator const void* (); }
        using Op::operator & ;
        using Op::operator &= ;
        using Op::operator | ;
        using Op::operator |= ;
        using Op::operator == ;
        using Op::operator != ;
        using Op::operator ++ ;
        using Op::operator -- ;

    };

    // 配列用
    template <typename VT, size_t N>
    struct le_array {
        le_nc<VT> value_[N];
        //const VT operator [] (size_t n) const { return value_[n].get(); }
        le_nc<VT>& operator [] (size_t n) { return value_[n]; }
        const le_nc<VT>& operator [] (size_t n) const { return value_[n]; }
        //operator VT* () { return &value_[0]; }
        //operator const VT* () const { return &value_[0]; }
        void operator = (const VT (&ar)[N]) { for(unsigned n = 0; n < N; ++n) { value_[n] = ar[n]; } }
        void clear(VT v = 0) { for(unsigned n = 0; n < N; ++n) { value_[n] = v; } }
        void copy_to(VT (&dst)[N]) const {
            for(unsigned n = 0; n < N; ++n) dst[n] = value_[n];
        }
    };


  } // namespace endian

  template<typename ET>
  struct enum8 {
	  int8_t value_;
	  enum8(int8_t v = 0) : value_(v) {}
	  enum8(ET v) : value_(v) {}
	  void operator = (ET v) { value_ = static_cast<uint8_t>(v); }
	  ET get() const { return static_cast<ET>(value_); }
	  operator ET () const { return get(); }
  };
} // namespace piml



#endif
