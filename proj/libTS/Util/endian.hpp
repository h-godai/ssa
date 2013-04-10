
#ifndef TS_UTIL_ENDIAN_HPP
#define TS_UTIL_ENDIAN_HPP

#include "libTS/Common/Common.h"


namespace ts {
namespace util {

  // define endian value
  const int BIG_ENDIAN_VALUE = 1;
  const int LITTLE_ENDIAN_VALUE = 0;
#if defiend(TS_BIG_ENDIAN)
  const int ENDIAN = BIG_ENDIAN_VALUE;
#else
  const int ENDIAN = LITTLE_ENDIAN_VALUE;
#endif


  template <int Endian, uint32_t SZ, typename VT>
  struct le_data {};

	// for little endian
	template <uint32_t SZ, typename VT>
	struct le_data<LITTLE_ENDIAN_VALUE, SZ, VT> {
		enum { Size = SZ };
		VT value_;
		le_data() : value_(0) {}
		le_data(const VT& v) : value_(v) {}
		operator VT () const {
			return value_;
		}
	};
	// for big endian
	template <typename VT>
	struct le_data<BIG_ENDIAN_VALUE, 2, VT> {
		enum { Size = 2 };
		union {
			uint8_t data_[2];
			VT value_;
		};
		le_data() : value_(0) {}
		le_data(const VT& v) {
			data_[0] = static_cast<u16>(v) & 0xff;
			data_[1] = static_cast<u16>(v) >> 8;
		}
		operator VT () const {
			return static_cast<VT>((data_[1] << 8) + data_[0]);
		}
	};

	template <typename VT>
	struct le_data<BIG_ENDIAN_VALUE, 4, VT> {
		union {
			u8 data_[4];
			VT value_;
		};
		le_data() : value_(0) {}
		le_data(const VT& v) {
			data_[0] = static_cast<u32>(v) & 0xff;
			data_[1] = (static_cast<u32>(v) >> 8) & 0xff;
			data_[2] = (static_cast<u32>(v) >> 16) & 0xff;
			data_[3] = (static_cast<u32>(v) >> 24) & 0xff;
		}
		operator VT () const {
			return static_cast<VT>((data_[3] << 24) + (data_[2] << 16) + (data_[1] << 8) + data_[0]);
		}
	};
#if 0
	template <typename VT>
	struct le_data<BIG_ENDIAN_VALUE, 8, VT> {
		uint64_t value_;
		le_data() : value_(0) {}
		le_data(const VT& v) : value_(ga::common::GASwap64(static_cast<uint64_t>(value_)))
		{}
		operator VT () const {
			return static_cast<VT>(ga::common::GASwap64((uint64_t)value_));
		}
	};
#endif

	// little endianÇ≈ÉfÅ[É^Çäiî[Ç∑ÇÈ
	template <typename VT>
	struct le : le_data<ENDIAN, sizeof(VT), VT> {
		typedef VT value_type;
		typedef le_data<ENDIAN, sizeof(VT), VT> data_type;
		le() : data_type() {}
	    le(const value_type& v) : data_type(v) {}
		void operator = (const value_type& v) {
			*this = le(v);
		}

	};

}}


#endif
