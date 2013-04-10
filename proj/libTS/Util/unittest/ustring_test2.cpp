#include "stdafx.h"
#include <assert.h>


#include "../util.hpp"

using namespace ts;

bool ustring_test(std::string& log) {


	ustring str;
	str = L"ABCD";
	str.tolower();
	if (str == L"abcd") {
		log += "ustring::tolower OK\r\n";
		return true;
	}
	else {
		log += "ustring::tolower Failure!\r\n";
		return false;
	}
}