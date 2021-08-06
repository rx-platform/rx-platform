
#include "terminal/rx_con_programs.h"
#include "terminal/rx_commands.h"

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
			"HCF...\r\n"
			"===================\r\n"
			"   " ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "still not implemented!!!" ANSI_COLOR_RESET "\r\n"
			"\r\n\r\n";
		return true;
	}
};
