
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
			"   Trag\r\n\r\n"
			"   volim tvoju necujnu senku kad sklizne iz sobe,\r\n"
			"   volim mirisni trag koji tada ostavis za sobom,\r\n"
			"   nedostajes mi danas, nedostajaces mi i sutra,\r\n"
			"   lepo je ceznuti za tvojim mirisnim tragom,\r\n"
			"   radovati se tvom prisustvu,\r\n"
			"   ziveti za momenat,\r\n"
			"   lepo je voleti te...\r\n\r\n\r\n"
			"                           jednoj Kseniji,\r\n"
			"                           28.10.2022.\r\n"
			"\r\n\r\n";
		return true;
	}
};
