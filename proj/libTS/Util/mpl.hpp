// Meta Programing Library  
//
// boostのmplもどき
//
// created by TSL ikeda

#ifndef TS_MPL_HPP
#define TS_MPL_HPP


namespace ts { 
  
// ここからはboostのmplもどき
namespace mpl {

  // boostのif_cもどき
  template <bool C, typename T, typename F> struct if_c            { typedef T type; };
  template <typename T, typename F >        struct if_c<false,T,F> { typedef F type; };
  
  
  // NULLクラスの定義
  class NullType {};

  // NULL値の定義
  template <typename T>
  struct null_value {
    static const T& Value() {
      static T value;
      return value;
    }
  };
  template <> struct null_value<uint64_t> { static uint64_t Value() { return 0; } };
  template <> struct null_value<int64_t> { static int64_t Value() { return 0; } };
  template <> struct null_value<uint32_t> { static uint32_t Value() { return 0; } };
  template <> struct null_value<int32_t> { static int32_t Value() { return 0; } };
  template <> struct null_value<uint16_t> { static uint16_t Value() { return 0; } };
  template <> struct null_value<int16_t> { static int16_t Value() { return 0; } };
  template <> struct null_value<uint8_t> { static uint8_t Value() { return 0; } };
  template <> struct null_value<int8_t> { static int8_t Value() { return 0; } };
  template <> struct null_value<float> { static float Value() { return 0.0f; } };
  template <> struct null_value<double> { static double Value() { return 0.0; } };



  // タイプリスト
  template <typename T, typename U>
  struct Typelist {
    typedef T Head;
    typedef U Tail;
  };

  // タイプリスト定義用のマクロ
  template <typename T1, typename T2, typename T3 = NullType
                                    , typename T4 = NullType
                                    , typename T5 = NullType
                                    , typename T6 = NullType
                                    , typename T7 = NullType
                                    , typename T8 = NullType
  >
  struct list {
    typedef Typelist<T1, Typelist<T2
                       , Typelist<T3
                       , Typelist<T4
                       , Typelist<T5
                       , Typelist<T6
                       , Typelist<T7
                       , Typelist<T8, Typelist<NullType, NullType> > > > > > > > > type;
  };

  template <typename Cnt>           struct f_ite { typedef typename Cnt::Head type;  typedef f_ite<typename Cnt::Tail> next; };
  template <typename Cnt>           struct begin { typedef f_ite<Cnt> type; };
  template <typename H, typename T> struct endL  { typedef typename endL<typename T::Head, typename T::Tail>::type type; };
  template <typename T>             struct endL<NullType, T> { typedef Typelist<NullType, T> type; };
  template <typename Cnt>           struct end   { typedef f_ite<typename endL<typename Cnt::Head, typename Cnt::Tail>::type > type; };
  template <typename Ite>           struct next  { typedef typename Ite::next type; };
  template <typename Ite>           struct deref { typedef typename Ite::type type; };

  // AとBが同じ型ならresultがtrue
  template <typename A, typename B>
  struct is_same_impl {
    enum {result = false};
  };
  template <typename A>
  struct is_same_impl<A, A> {
    enum {result = true};
  };

  // nullチェック
  template <typename A>
  struct is_nullType {
    enum {result = false};
  };
  template <>
  struct is_nullType<NullType> {
    enum {result = true};
  };

  template <typename A, typename B>
  struct is_same {
    enum { result = is_same_impl<A,B>::result};
  };

  

  // typeが一致するものをlistから探す
  template <typename List, int Match>
  struct find {
    typedef typename
    if_c<List::Head::type == Match
       , typename List::Head
       , typename find<typename List::Tail, Match>::type
        >::type type;
  };
  template <int Match, typename N>
  struct find<Typelist<NullType, N> ,Match> {
    typedef NullType type;
  };
  
      

  } // mpl

    // 配列のサイズを得る
  template <typename T, size_t N>
  inline size_t count_of(const T(&)[N]) {
    return N;
  }
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))

  // 配列の要素数をコンパイル時にチェックするマクロ
  template <typename T, size_t N, size_t SZ>
  inline void CheckArraySize(const T(&)[N]) {
    typename mpl::if_c<N == SZ, bool, void>::type a = false;
  }

  // 文字列を32bitの値にする(mpl版)
  template <int A, int B=0, int C=0, int D=0>
  struct cstr2int32 {
    enum { value = A + (B<<8) + (C<<16) + (D<<24) };
  };


  // intの値をタイプにする
  template <int N>
  struct int_type {
    typedef int_type type;
    enum { value=N };
  };


} // namespace

#endif
