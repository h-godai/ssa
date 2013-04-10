
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "picojson.hpp"
#include "co_vec2.hpp"
#include "nn_polygon.hpp"

namespace ngs {

class Place {
	std::vector<Polygon<float> > area;
public:
	Place() {}
	Place(const std::string& file)
	{
		std::fstream fstr;
		fstr.open(file.c_str(), std::ios::in);
		if(fstr.is_open())
		{
			picojson::value json;
			fstr >> json;
			fstr.close();

			picojson::array array = json.get<picojson::array>();
			for(picojson::array::iterator it = array.begin(); it != array.end(); ++it)
			{
				Polygon<float> polygon;
				picojson::array& vtx = (*it).get<picojson::array>();
				for(picojson::array::iterator it = vtx.begin(); it != vtx.end(); ++it)
				{
					picojson::array& v = (*it).get<picojson::array>();
					polygon.setVtx(Vec2<float>(v[0].get<double>(), v[1].get<double>()));
				}
				area.push_back(polygon);
			}
		}

	}
	~Place() {}
};

}
