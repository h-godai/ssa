#ifndef TS_UTIL_LINKED_LIST_HPP
#define TS_UTIL_LINKED_LIST_HPP

namespace ts { namespace util {

      // linked list interface class
    // support last(), next(), and append() method
    // Cls must have a "Cls* next_" instance;
  template <typename Cls, typename size_type = std::size_t>
    struct linked_list {
      typedef Cls* ptr;
      ptr last() const {
        Cls* first = this;
        while(first->next_ != 0) first = first->next_;
        return first->next_;
      };
      ptr next(size_type n) const {
	Cls* first = this;
	while (n > 0) {
	  first = first->next_;
	  if (first == 0) return 0;
	}
      }
      
      static void append(ptr& src, ptr add) {
	if (src == 0) 
	  src = add; 
	else 
	  src->last() = add;
      }
      void set_last(Cls* p) {
	Cls* first = static_cast<Cls*>(this);
	while(first->next_ != 0) first = first->next_;
	first->next_ = p;;
      }
	
    };




}} // namespace


#endif
