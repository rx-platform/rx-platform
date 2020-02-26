
class asteroid_command : public terminal::commands::server_command
{
	DECLARE_REFERENCE_PTR(asteroid_command);
	DECLARE_CONSOLE_CODE_INFO(1, 0, 0, "\
Zoran Modli (22.4.1948 - 23.2.2020.)\r\n\
\
");
public:
	asteroid_command()
		: server_command("asteroid")
	{
	}

	bool do_console_command(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
	{
		out << "\r\n"
			"2/23/2020 somewhere from internet:\r\n"
			"\r\n\r\n"
			"\"Danas je u Beogradu preminuo Zoran Modli.\r\n"
			" Po suncanom i vedrom vremenu, njegov avion je krenuo u besmrtni let.\"\r\n"
			"\r\n\r\n"
			"<http://www.modli.rs>\r\n"
			"\r\n\r\n"
			"Zoran Modli (22.4.1948 - 23.2.2020.)\r\n"
			"\r\n\r\n";
		return true;
	}
};
