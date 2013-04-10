// ShpFile.h
//
// Shpファイルを扱うクラス
//


#ifndef APP_SHPFILE_H
#define APP_SHPFILE_H

#ifndef STDAFX_H
#include <vector>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#endif
#include "Util/bstream.hpp"


namespace app {

  using namespace ts;
  using std::vector;
  using boost::shared_ptr;

  class ShpFile {
  public:
    struct FileHeader {
      uint32_t code;       // byte:0 ファイルコード(9994)
      uint32_t byte4;     
      uint32_t byte8;     
      uint32_t byte12;    
      uint32_t byte16;
      uint32_t byte20;
      uint32_t length;     // byte:24 ファイル長
      uint32_t version;    // byte:28 バージョン
      uint32_t shapeType;  // byte:32 シェイプタイプ
      float64_t Xmin;
      float64_t Ymin;
      float64_t Xmax;
      float64_t Ymax;
      float64_t Zmin;
      float64_t Zmax;
      float64_t Mmin;
      float64_t Mmax;
      void load(bstream& bs) {
        bs >> code >> byte4 >> byte8 >> byte12 >> byte16 >> byte20 >> length >> version >> shapeType;
        bs >> Xmin >> Ymin >> Xmax >> Ymax >> Zmin >> Zmax >> Mmin >> Mmax;
        code = ntohl(code);
        length = ntohl(length);
      }
    };
    enum ShapeType {
      NullShapeType = 0,
      PointType = 1,
      PolyLineType = 3,
      PolygonType = 5,
      MultiPointType = 8,
      PointZType = 11,
      PolyLineZType = 13,
      PolygonZType = 15,
      MultiPointZType = 18,
      PointMType = 21,
      PolyLineMType = 23,
      PolygonMType = 25,
      MultiPointMType = 28,
      MultiPatchType = 31
    };

    struct RecordHeader {
      uint32_t recordNumber;  // レコード番号
      uint32_t contentLength; // コンテンツ長(ワード単位)
      uint32_t shapeType; // シェープタイプ

      virtual ~RecordHeader() {}
      virtual void load(bstream& bs) {
        bs >> recordNumber >> contentLength >> shapeType;
        recordNumber = ntohl(recordNumber);
        contentLength = ntohl(contentLength);
      }
    };
    struct NullShape : public RecordHeader {
      virtual void load(bstream& ) {
      }
    };
    struct Point : public RecordHeader {
      double x;
      double y;
      virtual void load(const bstream& bs) {
        bs >> x >> y;
      }
    };

    struct MultiPoint : public RecordHeader {
      double box[4];
      uint32_t numPoints;
      vector<Point> points; // サイズはnumPoints
      virtual void load(bstream& bs) {
        bs >> box >> numPoints;
        points.resize(numPoints);
        bs.readArray(points);
      }
    };
    struct PolyLine : public RecordHeader {
      double box[4];
      uint32_t numParts;
      uint32_t numPoints;
      vector<uint32_t> parts;
      vector<Point> points;
      virtual void load(bstream& bs) {
        bs >> box >> numParts >> numPoints;
        parts.resize(numParts);
        bs.readArray(parts);
        points.resize(numPoints);
        bs.readArray(points);
      }
    };
    struct Polygon : public RecordHeader {
      float64_t box[4];
      uint32_t numParts;
      uint32_t numPoints;
      vector<uint32_t> parts;
      vector<Point> points;
      virtual void load(bstream& bs) {
        bs >> box >> numParts >> numPoints;
        parts.resize(numParts);
        bs.readArray(parts);
        points.resize(numPoints);
        bs.readArray(points);
      }
    };
    struct PointM : public RecordHeader {
      float64_t x;
      float64_t y;
      float64_t m;
      virtual void load(bstream& bs) {
        bs >> x >> y >> m;
      }
    };
    struct MultiPointM : public RecordHeader {
      float64_t box[4];
      uint32_t numPoints;
      vector<Point> points;
      float64_t mMin;
      float64_t mMax;
      vector<float64_t> mArray;
      virtual void load(bstream& bs) {
        bs >> box >> numPoints;
        points.resize(numPoints);
        bs.readArray(points);
        bs >> mMin >> mMax;
        mArray.resize(numPoints);
        bs.readArray(mArray);
      }
    };

    ShpFile() {
    }

    ShpFile(istream& ins) {
      bstream file(ins);
      parse(file);
    }

    ShpFile(bstream& file) {
      parse(file);
    }


    void parse(bstream& file) {
      FileHeader fileHeader;
      fileHeader.load(file);
      while (file.getLeft() > 0) {
        RecordHeader rh;
        rh.load(file);
        RecordHeader* obj = 0;
        switch (rh.shapeType) {
        case NullShapeType:
          obj = new NullShape();
          break;
        case PointType:
          obj = new Point();
          break;
        case PolyLineType:
          obj = new PolyLine();
          break;
        case PolygonType:
          obj = new Polygon();
          break;
        case MultiPointType:
        case PointZType:
        case PolyLineZType:
        case PolygonZType:
        case MultiPointZType:
          break;
        case PointMType:
          obj = new PointM();
          break;
        case PolyLineMType:
        case PolygonMType:
        case MultiPointMType:
        case MultiPatchType:
          break;
        }
        assert(obj != 0);
        *obj = rh;
        obj->load(file);
        m_record.push_back(shared_ptr<RecordHeader>(obj));
      }

    }

    typedef vector< shared_ptr<RecordHeader> > RecordData;
    typedef RecordData::iterator iterator;

    RecordData& getRecordData() { return m_record; };


  private:
    RecordData m_record;


  };

  inline
  const bstream& operator >> (const bstream& b, ShpFile::Point& n) { n.load(b); return b; }

  // shpファイルとペアで使うdbfファイルの定義
  class DBaseFile {
  public:
    typedef vector<string> DataList;
    typedef shared_ptr<DataList> DataListPtr;

    struct Header {
      uint8_t version;
      uint8_t date[3];
      uint32_t recordCount;
      uint16_t headerSize;
      uint16_t recordSize;
      uint8_t  tflag;
      uint8_t  cryptFlag;
      uint8_t  mdxFlag;
      uint8_t  languageID;
      void load(const bstream& bs) {
        bs >> version >> date >> recordCount >> headerSize >> recordSize;
        bs.skip(2);
        bs >> tflag >> cryptFlag;
        bs.skip(12);
        bs >> mdxFlag >> languageID;
        bs.skip(2);
      }
    };
    struct Field {
      char name[11];
      uint8_t type;
      uint8_t reserved[4];
      uint8_t length;
      uint8_t fraction;
      uint8_t reserved2[2];
      uint8_t workID;
      uint8_t reserved3[10];
      uint8_t mdxFlag;
      void load(const bstream& bs) {
        bs >> name >> type >> reserved >> length >> fraction;
        bs >> reserved2 >> workID >> reserved3 >> mdxFlag;
      }
    };

    DBaseFile(istream& ins) {
      bstream file(ins);
      parse(file);
    }

    DBaseFile(const bstream& file) {
      parse(file);
    }

    void parse(const bstream& file) {
      m_header.load(file);
      assert(file.getPos() == 32);
      char ch;
      for(;;) {
        file >> ch;
        if (ch == 0x0d) break; // フィールドの終わり
        file.skip(-1);
        Field field;
        field.load(file);
        TRACE("%s,", field.name);
        m_fieldIdx[field.name] = static_cast<int>(m_field.size());
        m_field.push_back(field);
      }
      TRACE("\n");
      for (;;) {
        file >> ch;
        if (ch == 0x2a) continue; // 削除フィールド
        if (ch != 0x20) break; // EOF
        shared_ptr<vector<string> > rec(new vector<string>);
        for (vector<Field>::iterator col = m_field.begin(); col != m_field.end(); ++col) {
          rec->push_back(string(file.getBuffer<char*>(), col->length));
          file.skip(col->length);
          //TRACE("%s,", rec->back().c_str());
        }
        //TRACE("\n");
        m_record.push_back(rec);
      }


    }

    int getIdx(const string& name) {
      return m_fieldIdx[name];
    }

    string& get(int rec, const string& name) {
      DataListPtr& rd = m_record[rec];
      return rd->at(getIdx(name));
    }
    DataListPtr getRecord(size_t rec) {
      return m_record[rec];
    }

    const Field& getField(size_t idx) {
      return m_field[idx];
    }
    size_t getFieldCount() { return m_field.size(); }

  private:
    Header m_header;
    vector<Field> m_field;
    map<string, int> m_fieldIdx;
    vector<DataListPtr> m_record;
  };

} // namespace

#endif

