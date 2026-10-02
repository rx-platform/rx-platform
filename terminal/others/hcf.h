
#include "terminal/rx_con_programs.h"
#include "terminal/rx_commands.h"
#include "terminal/ansi_codes.h"

class hcf_command : public rx_internal::terminal::commands::server_command
{
	DECLARE_REFERENCE_PTR(hcf_command);
	DECLARE_CONSOLE_CODE_INFO(1, 0, 0, "\
HCF command , MC68000 0x9D 0xDD.\r\n\
\
");
public:
	hcf_command()
		: server_command("hcf")
	{
	}

	bool do_console_command(std::istream& in, std::ostream& out, std::ostream& err, rx_internal::terminal::console_context_ptr ctx)
	{

		out << "\r\n"
			" " ANSI_COLOR_BOLD ANSI_COLOR_RED "H" ANSI_COLOR_RESET "alt and " ANSI_COLOR_BOLD ANSI_COLOR_RED "C" ANSI_COLOR_RESET "atch " ANSI_COLOR_BOLD ANSI_COLOR_RED "F" ANSI_COLOR_RESET "ire\r\n"
			" ===================\r\n";
		out << "Computer machine code instruction that causes the computer's\r\n"
			<< "central processing unit (CPU) to cease meaningful operation.\r\n"
			<< ANSI_COLOR_BOLD ANSI_COLOR_GREEN
			<< "MC68000 0x9D 0xDD\r\n"
			<< "I80286  0x0F 0x04" ANSI_COLOR_RESET;
		out << "\r\n\r\n" ANSI_COLOR_MAGENTA " Well this is just for fun...\r\n " ANSI_COLOR_RESET;
		
		out << ANSI_COLOR_WHITE ANSI_COLOR_BOLD;

		out << "\x1b(0\r\n";

		out << "                                             " ANSI_COLOR_BOLD ANSI_COLOR_RED "//" ANSI_COLOR_YELLOW "//" ANSI_COLOR_GREEN "//" ANSI_COLOR_CYAN "//" ANSI_COLOR_RESET "\r\n";
		out << "lqqqqqqqxlqqqqqqklqqqqqqqxqqqqqqqkxlqqqqqqq " ANSI_COLOR_BOLD ANSI_COLOR_RED "//" ANSI_COLOR_YELLOW "//" ANSI_COLOR_GREEN "//" ANSI_COLOR_CYAN "//" ANSI_COLOR_RESET "\r\n";
		out << "mqqqqqqkxx      xx       xlqqqqqquxx       " ANSI_COLOR_BOLD ANSI_COLOR_RED "//" ANSI_COLOR_YELLOW "//" ANSI_COLOR_GREEN "//" ANSI_COLOR_CYAN "//" ANSI_COLOR_RESET "\r\n";
		out << "qqqqqqqjxx      xmqqqqqqqxmqqqqqqjxx      " ANSI_COLOR_BOLD ANSI_COLOR_RED "//" ANSI_COLOR_YELLOW "//" ANSI_COLOR_GREEN "//" ANSI_COLOR_CYAN "//" ANSI_COLOR_RESET "\r\n";

		/*out << "\x1b#3lqqqqqqq lqqqqqqklqqqqqqqxqqqqqqqk lqqqqqqq\r\n";
		out << "\x1b#4lqqqqqqqxlqqqqqqklqqqqqqqxqqqqqqqkxlqqqqqqq\r\n";
		out << "\x1b#3mqqqqqqkxx      xx       xlqqqqqquxx       \r\n";
		out << "\x1b#4mqqqqqqkxx      xx       xlqqqqqquxx       \r\n";
		out << "\x1b#3qqqqqqqjxx      xmqqqqqqqxmqqqqqqjxx       \r\n";
		out << "\x1b#4qqqqqqqj         mqqqqqqq mqqqqqqj         \r\n";*/

		out << "\x1b(B" ANSI_COLOR_RESET;

		/*out << "\r\n\x1b#3";
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED "/" ANSI_COLOR_YELLOW "/" ANSI_COLOR_GREEN "/" ANSI_COLOR_CYAN "/" ANSI_COLOR_RESET
			<< "  ZX Spectrum";

		out << "\r\n\x1b#4";
		out << ANSI_COLOR_BOLD ANSI_COLOR_RED "/" ANSI_COLOR_YELLOW "/" ANSI_COLOR_GREEN "/" ANSI_COLOR_CYAN "/" ANSI_COLOR_RESET
			<< "  ZX Spectrum";*/

		out << "ZX Spectrum\r\n";
		out << "\r\nSo, I will not Halt and Catch Fire, at least not now ;)  ...\r\n";

		return true;
	}
};
