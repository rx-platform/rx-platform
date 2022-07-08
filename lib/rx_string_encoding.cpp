

/****************************************************************************
*
*  lib\rx_string_encoding.cpp
*
*  Copyright (c) 2020-2022 ENSACO Solutions doo
*  Copyright (c) 2018-2019 Dusan Ciric
*
*  
*  This file is part of {rx-platform} 
*
*  
*  {rx-platform} is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  {rx-platform} is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License  
*  along with {rx-platform}. It is also available in any {rx-platform} console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#include "pch.h"


// rx_string_encoding
#include "lib/rx_string_encoding.h"

namespace rx {


struct conv_entry
{
	uint16_t utf8;
	uint8_t ascii;
};
struct conv_entry conv_table[] = {
	{ 160, 32 }
	,{ 161, 33 }
	,{ 162, 99 }
	,{ 164, 36 }
	,{ 165, 89 }
	,{ 166, 124 }
	,{ 169, 67 }
	,{ 170, 97 }
	,{ 171, 60 }
	,{ 173, 45 }
	,{ 174, 82 }
	,{ 178, 50 }
	,{ 179, 51 }
	,{ 183, 46 }
	,{ 184, 44 }
	,{ 185, 49 }
	,{ 186, 111 }
	,{ 187, 62 }
	,{ 192, 65 }
	,{ 193, 65 }
	,{ 194, 65 }
	,{ 195, 65 }
	,{ 196, 65 }
	,{ 197, 65 }
	,{ 198, 65 }
	,{ 199, 67 }
	,{ 200, 69 }
	,{ 201, 69 }
	,{ 202, 69 }
	,{ 203, 69 }
	,{ 204, 73 }
	,{ 205, 73 }
	,{ 206, 73 }
	,{ 207, 73 }
	,{ 208, 68 }
	,{ 209, 78 }
	,{ 210, 79 }
	,{ 211, 79 }
	,{ 212, 79 }
	,{ 213, 79 }
	,{ 214, 79 }
	,{ 216, 79 }
	,{ 217, 85 }
	,{ 218, 85 }
	,{ 219, 85 }
	,{ 220, 85 }
	,{ 221, 89 }
	,{ 224, 97 }
	,{ 225, 97 }
	,{ 226, 97 }
	,{ 227, 97 }
	,{ 228, 97 }
	,{ 229, 97 }
	,{ 230, 97 }
	,{ 231, 99 }
	,{ 232, 101 }
	,{ 233, 101 }
	,{ 234, 101 }
	,{ 235, 101 }
	,{ 236, 105 }
	,{ 237, 105 }
	,{ 238, 105 }
	,{ 239, 105 }
	,{ 241, 110 }
	,{ 242, 111 }
	,{ 243, 111 }
	,{ 244, 111 }
	,{ 245, 111 }
	,{ 246, 111 }
	,{ 248, 111 }
	,{ 249, 117 }
	,{ 250, 117 }
	,{ 251, 117 }
	,{ 252, 117 }
	,{ 253, 121 }
	,{ 255, 121 }
	,{ 256, 65 }
	,{ 257, 97 }
	,{ 258, 65 }
	,{ 259, 97 }
	,{ 260, 65 }
	,{ 261, 97 }
	,{ 262, 67 }
	,{ 263, 99 }
	,{ 264, 67 }
	,{ 265, 99 }
	,{ 266, 67 }
	,{ 267, 99 }
	,{ 268, 67 }
	,{ 269, 99 }
	,{ 270, 68 }
	,{ 271, 100 }
	,{ 272, 68 }
	,{ 273, 100 }
	,{ 274, 69 }
	,{ 275, 101 }
	,{ 276, 69 }
	,{ 277, 101 }
	,{ 278, 69 }
	,{ 279, 101 }
	,{ 280, 69 }
	,{ 281, 101 }
	,{ 282, 69 }
	,{ 283, 101 }
	,{ 284, 71 }
	,{ 285, 103 }
	,{ 286, 71 }
	,{ 287, 103 }
	,{ 288, 71 }
	,{ 289, 103 }
	,{ 290, 71 }
	,{ 291, 103 }
	,{ 292, 72 }
	,{ 293, 104 }
	,{ 294, 72 }
	,{ 295, 104 }
	,{ 296, 73 }
	,{ 297, 105 }
	,{ 298, 73 }
	,{ 299, 105 }
	,{ 300, 73 }
	,{ 301, 105 }
	,{ 302, 73 }
	,{ 303, 105 }
	,{ 304, 73 }
	,{ 305, 105 }
	,{ 308, 74 }
	,{ 309, 106 }
	,{ 310, 75 }
	,{ 311, 107 }
	,{ 313, 76 }
	,{ 314, 108 }
	,{ 315, 76 }
	,{ 316, 108 }
	,{ 317, 76 }
	,{ 318, 108 }
	,{ 321, 76 }
	,{ 322, 108 }
	,{ 323, 78 }
	,{ 324, 110 }
	,{ 325, 78 }
	,{ 326, 110 }
	,{ 327, 78 }
	,{ 328, 110 }
	,{ 332, 79 }
	,{ 333, 111 }
	,{ 334, 79 }
	,{ 335, 111 }
	,{ 336, 79 }
	,{ 337, 111 }
	,{ 338, 79 }
	,{ 339, 111 }
	,{ 340, 82 }
	,{ 341, 114 }
	,{ 342, 82 }
	,{ 343, 114 }
	,{ 344, 82 }
	,{ 345, 114 }
	,{ 346, 83 }
	,{ 347, 115 }
	,{ 348, 83 }
	,{ 349, 115 }
	,{ 350, 83 }
	,{ 351, 115 }
	,{ 352, 83 }
	,{ 353, 115 }
	,{ 354, 84 }
	,{ 355, 116 }
	,{ 356, 84 }
	,{ 357, 116 }
	,{ 358, 84 }
	,{ 359, 116 }
	,{ 360, 85 }
	,{ 361, 117 }
	,{ 362, 85 }
	,{ 363, 117 }
	,{ 364, 85 }
	,{ 365, 117 }
	,{ 366, 85 }
	,{ 367, 117 }
	,{ 368, 85 }
	,{ 369, 117 }
	,{ 370, 85 }
	,{ 371, 117 }
	,{ 372, 87 }
	,{ 373, 119 }
	,{ 374, 89 }
	,{ 375, 121 }
	,{ 376, 89 }
	,{ 377, 90 }
	,{ 378, 122 }
	,{ 379, 90 }
	,{ 380, 122 }
	,{ 381, 90 }
	,{ 382, 122 }
	,{ 384, 98 }
	,{ 393, 68 }
	,{ 401, 70 }
	,{ 402, 102 }
	,{ 407, 73 }
	,{ 410, 108 }
	,{ 415, 79 }
	,{ 416, 79 }
	,{ 417, 111 }
	,{ 427, 116 }
	,{ 430, 84 }
	,{ 431, 85 }
	,{ 432, 117 }
	,{ 438, 122 }
	,{ 461, 65 }
	,{ 462, 97 }
	,{ 463, 73 }
	,{ 464, 105 }
	,{ 465, 79 }
	,{ 466, 111 }
	,{ 467, 85 }
	,{ 468, 117 }
	,{ 469, 85 }
	,{ 470, 117 }
	,{ 471, 85 }
	,{ 472, 117 }
	,{ 473, 85 }
	,{ 474, 117 }
	,{ 475, 85 }
	,{ 476, 117 }
	,{ 478, 65 }
	,{ 479, 97 }
	,{ 484, 71 }
	,{ 485, 103 }
	,{ 486, 71 }
	,{ 487, 103 }
	,{ 488, 75 }
	,{ 489, 107 }
	,{ 490, 79 }
	,{ 491, 111 }
	,{ 492, 79 }
	,{ 493, 111 }
	,{ 496, 106 }
	,{ 609, 103 }
	,{ 697, 39 }
	,{ 698, 34 }
	,{ 700, 39 }
	,{ 708, 94 }
	,{ 710, 94 }
	,{ 712, 39 }
	,{ 715, 96 }
	,{ 717, 95 }
	,{ 732, 126 }
	,{ 768, 96 }
	,{ 770, 94 }
	,{ 771, 126 }
	,{ 782, 34 }
	,{ 817, 95 }
	,{ 818, 95 }
};


int search_conv_table(int l, int r, uint16_t x)
{
	if (r >= l)
	{
		int mid = l + (r - l) / 2;

		if (conv_table[mid].utf8 == x)
			return mid;
		if (conv_table[mid].utf8 > x)
			return search_conv_table(l, mid - 1, x);

		return search_conv_table(mid + 1, r, x);
	}
	return -1;
}


char utf8_to_ascii(uint16_t x, char invalid_char)
{
	int l = 0;
	int r = (int)(sizeof(conv_table) / sizeof(conv_table[0])) - 1;

	int idx = search_conv_table(l, r, x);
	if (idx < 0)
		return invalid_char;
	else
		return conv_table[idx].ascii;
}

string_type utf8_to_ascii(string_view_type in, char invalid_char)
{
	string_type ret;
	if (!in.empty())
	{
		ret.reserve(in.size());
		auto it = in.begin();
		while (it != in.end())
		{
			char temp = *it;
			if (temp & 0x80)
			{
				if ((temp & 0xe0) == 0xc0)
				{
					it++;
					if (it != in.end())
					{
						char temp2 = *it;
						if ((temp2 & 0xc0) == 0x80)
						{
							uint16_t res = (temp2 & 0x3f) | ((temp & 0x1f) << 6);
							ret.push_back(utf8_to_ascii(res, invalid_char));
						}
						else
						{
							ret.push_back(invalid_char);
						}
					}
				}
				else if ((temp & 0xf0) == 0xe0)
				{
					it++;
					if (it != in.end())
					{
						it++;
						if (it != in.end())
							ret.push_back(invalid_char);
					}
				}
				else if ((temp & 0xf8) == 0xf0)
				{
					it++;
					if (it != in.end())
					{
						it++;
						if (it != in.end())
						{
							it++;
							if (it != in.end())
								ret.push_back(invalid_char);
						}
					}
				}
			}
			else
			{
				ret.push_back(temp);
			}
			it++;
		}
	}
	return ret;
}




}// namespace rx
