
class xena_command : public terminal::commands::server_command
{
	DECLARE_REFERENCE_PTR(xena_command);
	DECLARE_CONSOLE_CODE_INFO(1, 0, 0, "\
Neko sranje.\r\n\
\
");
public:
	xena_command()
		: server_command("xena")
	{
	}

	bool do_console_command(std::istream& in, std::ostream& out, std::ostream& err, console_context_ptr ctx)
	{
		out << "\r\n"
			"Kad si mlad onda si glup,"
			"Kad se opametis onda vise nisi mlad,"
			"Ceo zivot si jurio sopstveni rep,"
			"I onda riknes..."
			"\r\n\r\n";
		return true;
	}
};
