//
// テクスチャ管理
//
#pragma once

#include <iostream>
#include <string>
#include <map>
#include <memory>
#include "co_texture.hpp"


namespace ngs {

	class TexMng {
	public:
		typedef std::tr1::shared_ptr<Texture> tex_ptr;

	private:
		std::map<std::string, tex_ptr> tex_obj;
		
	public:
		TexMng() {
			std::cout << "TexMng()" << std::endl;
		}
		~TexMng() {
			std::cout << "~TexMng()" << std::endl;
		}

		tex_ptr read(const std::string& path) {
			std::string name = getFileName(path);
			std::map<std::string, tex_ptr>::iterator it = tex_obj.find(name);
			if(it == tex_obj.end())
			{
				tex_ptr obj(new Texture(path));
				tex_obj[name] = obj;
				return obj;
			}
			else
			{
				return it->second;
			}
		}

		tex_ptr set(const std::string& name, Texture *texture)
		{
			std::map<std::string, tex_ptr>::iterator it = tex_obj.find(name);
			if(it == tex_obj.end())
			{
				tex_ptr obj(texture);
				tex_obj[name] = obj;
				return obj;
			}
			else
			{
				return it->second;
			}
		}

		tex_ptr get(const std::string& name) {
			std::map<std::string, tex_ptr>::iterator it = tex_obj.find(name);
			if(it == tex_obj.end())
			{
				std::cout << "No texture obj:" << name << std::endl;
				tex_ptr dummy;
				return dummy;
			}
			else
			{
				return it->second;
			}
		}

		void destroy(tex_ptr obj) {
			const std::string& name = obj->name();
			std::map<std::string, tex_ptr>::iterator it = tex_obj.find(name);
			if(it == tex_obj.end())
			{
				std::cout << "No texture obj:" << obj->name() << std::endl;
			}
			else
			{
				tex_obj.erase(it);
			}
		}
	};

}
