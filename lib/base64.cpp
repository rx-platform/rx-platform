#include "pch.h"
#include "base64.h"


using namespace std;


namespace urke
{

	char tabel_base64[] = {
		'A', 'B', 'C', 'D',
		'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X',
		'Y', 'Z', 'a', 'b',
		'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r',
		's', 't', 'u', 'v',
		'w', 'x', 'y', 'z',
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', '+', '/'
	};

	std::byte getFrom(char ch)
	{
		for (size_t i = 0; i < sizeof(tabel_base64) / sizeof(tabel_base64[0]); i++)
		{
			if (ch == tabel_base64[i])
			{
				return (std::byte)i;
			}
		}
		*((int*)0) = 13;
		return std::byte{ 0 };
	}

	string get_base64(const std::byte* data, size_t size)
	{
		string result;

		int bonus = 0;

		if (size % 3 == 1)
		{
			bonus = 2;
			size += 2;
		}
		else if (size % 3 == 2)
		{
			bonus = 1;
			size += 1;
		}

		result.reserve(size * 8 / 6);

		for (size_t i = 0; i < size; i++)
		{
			std::byte between;

			std::byte first = data[i] & std::byte{ 0xfc };
			first = first >> 2;

			//////////////////////////////////////////////////

			std::byte second = data[i] & std::byte{ 0x03 };
			second = second << 4;
			i++;
			between = data[i] & std::byte{ 0xf0 };
			between = between >> 4;
			if (bonus == 2 && i >= size - 2) between = std::byte{ 0 };
			second = second | between;

			//////////////////////////////////////////////////

			std::byte third = data[i] & std::byte{ 0x0f };
			third = third << 2;
			if ((bonus == 1 || bonus == 2) && i >= size - 1) third = std::byte{ 0 };
			i++;
			between = data[i] & std::byte{ 0xc0 };
			between = between >> 6;
			if ((bonus == 1 || bonus == 2) && i >= size - 1) between = std::byte{ 0 };
			third = third | between;

			//////////////////////////////////////////////////

			std::byte fourth = data[i] & std::byte{ 0x3f };
			if ((bonus == 1 || bonus == 2) && i >= size - 1) fourth = std::byte{ 0 };

			result += tabel_base64[std::to_integer<unsigned char>(first)];
			result += tabel_base64[std::to_integer<unsigned char>(second)];
			result += tabel_base64[std::to_integer<unsigned char>(third)];
			result += tabel_base64[std::to_integer<unsigned char>(fourth)];

		}

		if (bonus == 1) result[result.length() - 1] = '=';
		else if (bonus == 2)
		{
			result[result.length() - 2] = '=';
			result[result.length() - 1] = '=';
		}

		return result;
	}


	vector<std::byte> reverse(vector<std::byte>& back, int bonus);

	vector<std::byte> get_data(const string& rez)
	{

		vector<std::byte> converted;

		if (rez.empty())
			return converted;

		int bonus = 0;
		size_t len = rez.length();

		for (size_t i = 0; i < rez.length(); i++)
		{
			if (rez[i] != '=')
				converted.push_back(getFrom(rez[i]));
		}

		if (rez[len - 2] == '=')
		{
			bonus=2;
		}
		else if (rez[len - 1] == '=')
		{
			bonus = 1;
		}

		converted = reverse(converted,bonus);

		return converted;

	}

	vector<std::byte> reverse(vector<std::byte>& back, int bonus)
	{
		std::byte between;

		vector<std::byte> end;

		if (bonus == 1)
		{
			back.push_back(std::byte{ 0 });
		}
		else if (bonus == 2)
		{
			back.push_back(std::byte{ 0 });
			back.push_back(std::byte{ 0 });
		}

		for (size_t i = 0; i < back.size(); i++)
		{
			std::byte first = back[i] << 2;
			i++;
			between = back[i] >> 4;
			first = first | between;

			/////////////////////////////////////////////

			std::byte second = back[i] << 4;
			i++;
			between = back[i] >> 2;
			second = second | between;

			/////////////////////////////////////////////

			std::byte third = back[i] << 6;
			i++;
			between = back[i];
			third = third | between;

			end.push_back(first);
			end.push_back(second);
			end.push_back(third);
		}

		if (bonus == 1) end.pop_back();
		else if (bonus == 2)
		{
			end.pop_back();
			end.pop_back();
		}

		return end;
	}

}
