#pragma once
#include "pch.h"

namespace urke
{

	namespace parser
	{

		struct parser_option
		{
			char short_option = '\0';
			const char* long_option = nullptr;
			const char* help_text = nullptr;
		};

		struct bit_option : public parser_option
		{
			bool* value;
		};

		struct int_option : public parser_option
		{
			int* value;
		};

		struct uint_option : public parser_option
		{
			unsigned int* value;
		};

		struct float_option : public parser_option
		{
			double* value;
		};

		struct string_option : public parser_option
		{
			string_type* value;
		};

		//////////////////////////////////////////////////////////////////

		class parser3000
		{

			std::vector<bit_option> bit_options;
			std::vector<int_option> int_options;
			std::vector<uint_option> uint_options;
			std::vector<float_option> float_options;
			std::vector<string_option> string_options;

		public:
			// full options
			void add_bit_option(const bit_option& opt);
			void add_int_option(const int_option& opt);
			void add_uint_option(const uint_option& opt);
			void add_float_option(const float_option& opt);
			void add_string_option(const string_option& opt);
			// fast bit options
			////////////////////////////////////
			void add_bit_option(char opt, bool* value, const char* help = nullptr);
			void add_bit_option(const char* opt, bool* value, const char* help = nullptr);
			void add_bit_option(char opt, const char* opt_long, bool* value, const char* help = nullptr);

			void add_int_option(char opt, int* value, const char* help = nullptr);
			void add_int_option(const char* opt, int* value, const char* help = nullptr);
			void add_int_option(char opt, const char* opt_long, int* value, const char* help = nullptr);

			void add_uint_option(char opt, unsigned int* value, const char* help = nullptr);
			void add_uint_option(const char* opt, unsigned int* value, const char* help = nullptr);
			void add_uint_option(char opt, const char* opt_long, unsigned int* value, const char* help = nullptr);

			void add_float_option(char opt, double* value, const char* help = nullptr);
			void add_float_option(const char* opt, double* value, const char* help = nullptr);
			void add_float_option(char opt, const char* opt_long, double* value, const char* help = nullptr);

			void add_string_option(char opt, string_type* value, const char* help = nullptr);
			void add_string_option(const char* opt, string_type* value, const char* help = nullptr);
			void add_string_option(char opt, const char* opt_long, string_type* value, const char* help = nullptr);
			////////////////////////////////////

			void print_help(const string_type& str, std::ostream& help);

			bool parse(std::istream& in, std::ostream& err);

		private:
			bool to_int_parser(const string_type& what, int& val);
			bool to_uint_parser(const string_type& what, unsigned int& val);
			bool to_float_parser(const string_type& what, double& val);


		};


	} // parser

} // urke
