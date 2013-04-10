// -*-tab-width:4-*-
//
// enum class

namespace ts { 

  namespace detail {
    // inheritance_enumの前方宣言
    template <typename B, template <class B> class Sub> struct inheritance_enum;

    // 継承後のenumクラスとなる型(2世代目)
    template <typename Base, typename Sub, int Lv>
    struct inheritance_impl : Sub {
      enum { has_base_type = 1 };
      typedef typename Base::type BaseType;
      typedef typename Sub::type SubType;
      typedef typename Base::value_type value_type;
      inheritance_impl() {}
      inheritance_impl(BaseType e) {  value_ = static_cast<value_type>(e); }
      inheritance_impl(SubType e) {  value_ = static_cast<value_type>(e); }
      void operator = (BaseType e) { value_ = static_cast<value_type>(e); }
      void operator = (SubType e) { value_ = static_cast<value_type>(e); }
      bool operator == (BaseType e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (SubType e) const { return value_ == static_cast<value_type>(e); }
      bool operator != (BaseType e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (SubType e) const { return value_ != static_cast<value_type>(e); }
      template <template <class B> class Sub>
      struct append {
        typedef typename inheritance_enum<inheritance_impl, Sub>::type type;
      };
    };
    // 継承後のenumクラスとなる型(3世代目)
    template <typename Base, typename Sub>
    struct inheritance_impl<Base, Sub, 1> : Sub {
      enum { has_base_type = 2 };
      typedef typename Base::type BaseType;
      typedef typename Base::BaseType BaseType2;
      typedef typename Sub::type SubType;
      typedef typename Base::value_type value_type;
      inheritance_impl() {}
      inheritance_impl(BaseType2 e) {  value_ = static_cast<value_type>(e); }
      inheritance_impl(BaseType e) {  value_ = static_cast<value_type>(e); }
      inheritance_impl(SubType e) {  value_ = static_cast<value_type>(e); }
      void operator = (BaseType2 e) { value_ = static_cast<value_type>(e); }
      void operator = (BaseType e) { value_ = static_cast<value_type>(e); }
      void operator = (SubType e) { value_ = static_cast<value_type>(e); }
      bool operator == (BaseType2 e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (BaseType e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (SubType e) const { return value_ == static_cast<value_type>(e); }
      bool operator != (BaseType2 e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (BaseType e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (SubType e) const { return value_ != static_cast<value_type>(e); }
    };
    // 継承後のenumクラスとなる型(4世代目)
    template <typename Base, typename Sub>
    struct inheritance_impl<Base, Sub, 2> : Sub {
      enum { has_base_type = 3 };
      typedef typename Base::type BaseType;
      typedef typename Base::BaseType BaseType2;
      typedef typename Base::BaseType2 BaseType3;
      typedef typename Sub::type SubType;
      typedef typename Base::value_type value_type;
      inheritance_impl() {}
      explicit inheritance_impl(BaseType3 e) {  value_ = static_cast<value_type>(e); }
      explicit inheritance_impl(BaseType2 e) {  value_ = static_cast<value_type>(e); }
      explicit inheritance_impl(BaseType e) {  value_ = static_cast<value_type>(e); }
      explicit inheritance_impl(SubType e) {  value_ = static_cast<value_type>(e); }
      void operator = (BaseType3 e) { value_ = static_cast<value_type>(e); }
      void operator = (BaseType2 e) { value_ = static_cast<value_type>(e); }
      void operator = (BaseType e) { value_ = static_cast<value_type>(e); }
      void operator = (SubType e) { value_ = static_cast<value_type>(e); }
      bool operator == (BaseType3 e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (BaseType2 e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (BaseType e) const { return value_ == static_cast<value_type>(e); }
      bool operator == (SubType e) const { return value_ == static_cast<value_type>(e); }
      bool operator != (BaseType3 e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (BaseType2 e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (BaseType e) const { return value_ != static_cast<value_type>(e); }
      bool operator != (SubType e) const { return value_ != static_cast<value_type>(e); }
    };


    // enum_classを継承するためのクラス
    // 継承enumは、以下の書式で定義する
    // template <typename Super>
    // struct EnumSubClass : Super {
    //   enum type {
    //     Element1 = Super::END_ENUM,
    //     Element2,
    //     ...
    //     END_ENUM
    //   };
    // };
    template <typename Base, template <class B> class Sub>
    struct inheritance_enum {
      struct sub_enum : Sub<Base> {
        typedef typename Sub<Base>::type type;
        sub_enum() {}
        explicit sub_enum(type e) { value_ = e; }
        void operator = (sub_enum e) { value_ = e.value_; }
      };
      typedef inheritance_impl<Base, sub_enum, Base::has_base_type> type;
    };

    // enumクラスの基底のなるクラス   
    template <typename ET, typename VT>
    struct enum_class : ET {
      enum { has_base_type = 0 };
      typedef typename ET::type type;
      typedef VT value_type;
      value_type value_;
      enum_class() : value_(0) {}
      enum_class(type e) : value_(static_cast<value_type>(e)) {}
      value_type get() const { return value_; }
      operator ET () const { return static_cast<type>(value_); }
      bool operator == (type e) const { return e == static_cast<type>(value_); }
      bool operator != (type e) const { return e != static_cast<type>(value_); }
      void operator = (enum_class e) { value_ = e.value_; }
      
      template <template <class B> class Sub>
      struct append {
        typedef typename inheritance_enum<enum_class<ET,VT>, Sub>::type type;
      };
    };
  } // detail

  // 8bit,16bit,32bitでenumを定義する
  // enum_の引数となるクラスは、以下の定義とする
  // struct XXX {
  //   enum type {   Element1,...., END_ENUM };
  // };
  // enumの名称は'type'、enumの最後のエントリーは'END_ENUM'とする
  template <typename ET> struct enum8  { typedef detail::enum_class<ET, uint8_t> type; };
  template <typename ET> struct enum16 { typedef detail::enum_class<ET, uint16_t> type; };
  template <typename ET> struct enum32 { typedef detail::enum_class<ET, uint32_t> type; };
}
// 以下 exsample
#ifdef SAMPLECODE_OF_TS_ENUM_CLASS
  // enumeration for base 
  struct Hoge {
    enum type {
      Hoge1,
      Hoge2,
      Hoge3,
      END_ENUM
    };
  };
  typedef enum8<Hoge>::type EnumHoge;

  // enumration for subclass
  template <typename Super>
  struct Fuga : Super {
    enum type {
      Fuga1 = Super::END_ENUM,
      Fuga2,
      Fuga3,
      END_ENUM
    };
  };
  // enumration for subclass
  template <typename Super>
  struct Mago : Super {
    enum type {
      Mago1 = Super::END_ENUM,
      Mago2,
      Mago3,
      END_ENUM
    };
  };
  typedef EnumHoge::append<Fuga>::type EnumFuga;
  typedef EnumFuga::append<Mago>::type EnumMago;
#endif

