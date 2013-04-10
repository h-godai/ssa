/*!	@file	factory.hpp
	@brief	ファクトリクラステンプレート
  @author h.godai

*/
#ifndef TS_FACTORY_H
#define TS_FACTORY_H

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>
#include <cstdio>

#include "ustring.hpp"

namespace ts {

  template <class Product, typename IdentifireType>
  class Factory {
  private:
    typedef typename Factory* ptr;
    typedef typename Product::ptr productPtr;
    ~Factory(){} // 他人によるdelete禁止

  public:
    static ptr Instance() {
      static Factory self;
      return &self;
    }
    typedef productPtr (*Creator)();
    struct Property {
      IdentifireType typeId; // 識別用のID
      Creator creator;
    };

  private:
    typedef std::map<uint32_t, const Property* const> ProductList;

  public:


    struct NotFoundException : public std::exception {
      NotFoundException(const char* const& msg) : std::exception(msg) {}
    };

 
    productPtr Get(IdentifireType type) {
      ProductList::iterator i = m_productList.find(type);
      if (i != m_productList.end()) {
        return i->second->creator();
      }
      throw NotFoundException("Factory::Get");
    }

    template <typename Pred>
    productPtr Get(IdentifireType type, Pred cmp) {
      ProductList::iterator i = m_productList.find(type);
      if (i != m_productList.end()) {
        return i->second->creator();
      }
      BOOST_FOREACH(ProductList::value_type& v, m_productList) {
        if (cmp(v.second->creator())) {
          return v.second->creator();
        }
      }
      throw NotFoundException("Factory::Get");
    }

    template <typename Container, typename Getter>
    void enumlateProducts(Container& container, Getter getter) {
                 std::back_insert_iterator<Container> ite(container);

      BOOST_FOREACH(ProductList::value_type& v, m_productList) {
        container.push_back(getter(v.second));
      }
    }

    bool Register(const Property* const prop) {
      if (m_productList.find(prop->typeId) != m_productList.end()) {
        // 二重登録！
        return false;
      }
      m_productList.insert(ProductList::value_type(prop->typeId, prop));
      return true;
    }


  private:
    ProductList m_productList;
  };

} // namespace

#endif
