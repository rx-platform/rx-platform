#pragma once


/**********************************************************
Urke napisao kod, zapratite ne na isnti
;)

**********************************************************/

namespace urke
{

	std::string get_base64(const std::byte* data, size_t size);
	std::vector<std::byte> get_data(const std::string& rez);

}
