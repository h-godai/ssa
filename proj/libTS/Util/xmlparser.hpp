//! 簡易XMLパーサー

#ifndef TSL_XMLPARSER_HPP
#define TSL_XMLPARSER_HPP

#include <vector>
#include <algorithm>
#include <functional>

#include <boost/shared_ptr.hpp>
#include "ustring.hpp"

#ifdef WIN32
#pragma warning (disable : 4996)
#else
#include <stdint.h>
#define _T(x) x
#endif

namespace ts {
    template <typename STR>
    struct Escape {
      static STR& unEscape(STR& str) {
	str.replace_string(L"&quot;", L"\"");
	str.replace_string(L"&lt;", L"<");
	str.replace_string(L"&gt;", L">");
	str.replace_string(L"&amp;", L"&");
	return str;
      }
      static STR& escape(STR& str) {
	str.replace_string(L"\"", L"&quot;");
	str.replace_string(L"<", L"&lt;");
	str.replace_string(L">", L"&gt;");
	str.replace_string(L"&", L"&amp;");
	return str;
      }
    };
    template <>
    struct Escape<mstring> {
      static mstring& unEscape(mstring& str) {
	str.replace_string("&quot;", "\"");
	str.replace_string("&lt;", "<");
	str.replace_string("&gt;", ">");
	str.replace_string("&amp;", "&");
	return str;
      }
      static mstring& escape(mstring& str) {
	str.replace_string("\"", "&quot;");
	str.replace_string("<", "&lt;");
	str.replace_string(">", "&gt;");
	str.replace_string("&", "&amp;");
	return str;
      }
    };
     


  // エレメントの属性部
  template <typename StringT>
  struct Attribute {
    Attribute(){}
    Attribute(const StringT& name_, const StringT& value_) : name(name_), value(value_) {}
    StringT name;
    StringT value;
  };

  // エレメント
  template <typename StringT>
  struct Element {
    Element() {} 
    Element(const StringT& name_, const StringT& value_) : name(name_), value(value_) {}
    StringT name;
    StringT value;
    std::vector<Attribute<StringT> > attributes;
  };
  
  // 階層構造を持ったデータ
  template <typename StringT>
  class HierarchicalData {
  public:
    typedef boost::shared_ptr<HierarchicalData> ptr;
    typedef std::vector<ptr> PtrList;

    HierarchicalData(HierarchicalData* parent=0) : m_parent(parent) {}

    // 検索系

    // 子要素まですべてのAttributeをPredに渡す
    template <typename Pred>
    void eachAttribute(Pred pred) {
      std::for_each(m_element.attributes.begin(), m_element.attributes.end(), pred);

      struct ChildPred {
        Pred& pred_;
        ChildPred(Pred& pred) : pred_(pred) {}
        void operator() (ptr h) {
          h->eachAttribute(pred_);
        }
      };

      std::for_each(m_childlen.begin(), m_childlen.end(), ChildPred(pred));
    }

    // データ操作系
    const Element<StringT>& getElement() const { return m_element; }
    Element<StringT>& getElement() { return m_element; }

    void setElement(const Element<StringT>& element) { m_element = element; }
    void setElementName(const StringT& name) { m_element.name = name; }
    void setBody(const StringT& body) { m_element.value = body; }

    // attributeを追加
    void addAttribute(const Attribute<StringT>& attr) {
      m_element.attributes.push_back(attr);
    }

    ptr getParent() const { return m_parent; }
    void setParent(ptr parent) { m_parent = parent; }
    void addChild(ptr child) { m_childlen.push_back(child); }

    std::vector<ptr>& getChildlen() { return m_childlen; }

  private:
    Element<StringT> m_element;
    HierarchicalData* m_parent; // 親
    std::vector<ptr> m_childlen; //子供
  };


  enum XMLErrCode {
    XML_OK,
    XML_FileNotFound,
    XML_ParseError,
    XML_FileTooLarge,
    XML_FileCanNotOpen,
    XML_NoAllocMemory,

  };

  class XMLParserException : public std::exception {
  public:
    XMLParserException(XMLErrCode err, const mstring& msg) : m_errMsg(msg), m_errCode(err) {}
    virtual ~XMLParserException() throw() {} 
    XMLErrCode getErrorCode() const { return m_errCode; }
    const mstring& getErrorMessage() const { return m_errMsg; }
    const char* what() const throw() { return m_errMsg.c_str(); }
  private:
    mstring m_errMsg;
    XMLErrCode m_errCode;
  };

  template <typename StringT>
  class XMLParser {
  public:



#ifdef WIN32
    static HierarchicalDataPtr ParseFile(const StringT& filename) {

      uint64_t fileSize = util::getFileSize(filename);
      if (fileSize == 0) throw XMLParserException(XML_FileNotFound, filename);
      if (fileSize > 0xfffffffe) throw XMLParserException(XML_FileTooLarge, filename);

      FILE* f = _wfopen(filename.c_str(), _T("rb"));
      if (f == 0) throw XMLParserException(XML_FileCanNotOpen, filename);
     
      unsigned int fileSize32 = static_cast<unsigned int>(fileSize);
      unsigned char* buf = new unsigned char[fileSize32+1];
      if (buf == 0) throw XMLParserException(XML_NoAllocMemory, filename);
      fread(buf, fileSize32, 1, f);
      fclose(f);
      buf[fileSize32] = 0;
      ustring all; // unicodeに変換
      strutil::utf8towstring(buf, fileSize32, all.body());
      delete [] buf;

      return Parce(all);
    }
#endif

    typedef typename HierarchicalData<StringT>::ptr HierarchicalDataPtr;

    static HierarchicalDataPtr Parse(const StringT& text) {
      // パース開始
      HierarchicalDataPtr root(new HierarchicalData<StringT>);
      parseElement(text, root);
      return root;
    }


    static void parseElement(const StringT& all, HierarchicalDataPtr parent) {
      for (size_t pos = 0; pos != StringT::npos; ) {
	size_t opos = pos;
        pos = all.find(L'<', pos);
        if (pos == StringT::npos) {
	  parent->setBody(all.substr(opos).trim("\n\r"));
	  break;
	}
        switch (all[++pos]) {
          case _T('?'): // <?の場合は>まで読み飛ばす
          case _T('!'): // <!の場合は>まで読み飛ばす
            pos = all.find(_T('>'), pos); 
            continue;
          case _T('/'): // </hoge エレメント終了
            {
              throw XMLParserException(XML_ParseError, _T("invalid end tag"));
            }
            break;
          default: // <hoge エレメント開始
            {
              bool noDocument = false;
              size_t epos = all.find(_T('>'), pos);
              //size_t docendpos = 0;
              if (epos == StringT::npos) {
                throw XMLParserException(XML_ParseError, _T("> not found"));
              }
              if (all[epos-1] == _T('/')) { // <hoge />スタイル
                noDocument = true;
                --epos;
              }
              // Elementの解析
              StringT elementBody = all.substr(pos, epos-pos);
              std::vector<StringT> attrs;
              elementBody.split(attrs, _T(" \r\n\t"));
              if (attrs.size() < 1) {
                throw XMLParserException(XML_ParseError, _T("no element name"));
              }

              HierarchicalDataPtr element(new HierarchicalData<StringT>(parent.get()));
              element->setElementName(attrs[0]);
              for (size_t n = 1; n < attrs.size(); ++n) {
		StringT attnm = attrs[n];
		StringT val;
		size_t eqpos = attnm.find(_T('='), 0);
		if (eqpos != StringT::npos) {
		  val = attnm.substr(eqpos+1);
		  attnm = attnm.substr(0,eqpos);
		}
                Attribute<StringT> attr(attnm, Escape<StringT>::unEscape(val.trim(_T("\""))));
                element->getElement().attributes.push_back(attr);
              }
              parent->addChild(element);
              // document部の解析
              if (!noDocument) {
                StringT endtag(StringT(_T("</")) + element->getElement().name + _T(">"));
                size_t docend = all.find(endtag, ++epos);
                if (docend == StringT::npos) {
                  throw XMLParserException(XML_ParseError, StringT().format(_T("no end tag %s"), endtag.c_str()));
                }
                StringT docbody(all.substr(epos, docend - epos));
                parseElement(docbody, element); // docbodyをパース
                pos = docend + endtag.length();
              }
            }
            break;
        }
      }
    }

  };
















} // namespace tsl


#endif
