
class urke_command : public terminal::commands::server_command
{
	DECLARE_REFERENCE_PTR(urke_command);
	DECLARE_CONSOLE_CODE_INFO(1, 0, 0, "\
Neko sranje.\r\n\
\
");
public:
	urke_command()
		: server_command("urke")
	{
	}

	bool do_console_command(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
	{
		out << "\r\n"
			"Uros Ciric Urke...\r\n"
			"===================\r\n"
			"Pisao kod za:\r\n"
			"   - base64\r\n"
			"   - parser komandne linije " ANSI_COLOR_RED ANSI_COLOR_BOLD "KOJI JOS UVEK NE RADI!!!" ANSI_COLOR_RESET
			"\r\n\r\n";
		return true;
	}
};
