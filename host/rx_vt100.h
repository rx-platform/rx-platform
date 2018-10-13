

/****************************************************************************
*
*  host\rx_vt100.h
*
*  Copyright (c) 2018 Dusan Ciric
*
*  
*  This file is part of rx-platform
*
*  
*  rx-platform is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  rx-platform is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with rx-platform.  If not, see <http://www.gnu.org/licenses/>.
*  
****************************************************************************/


#ifndef rx_vt100_h
#define rx_vt100_h 1






namespace host {

namespace rx_vt100 {





class vt100_transport 
{
  public:
	vt100_transport(const vt100_transport &right) = delete;
	vt100_transport & operator=(const vt100_transport &right) = delete;
	vt100_transport(vt100_transport &&right) = delete;
	vt100_transport & operator=(vt100_transport &&right) = delete;
  private:
	enum parser_state
	{
		parser_normal,
		parser_in_end_line,
		parser_had_escape,
		parser_had_bracket,
		parser_had_bracket_number
	};
	typedef std::list<string_type> history_type;

  public:
      vt100_transport();

      virtual ~vt100_transport();


      bool char_received (const char ch, bool eof, string_type& to_echo, std::function<void(string_type)> received_line_callback);

      void add_to_history (const string_type& line);


      void set_password_mode (bool value)
      {
        password_mode_ = value;
      }



  protected:

  private:

      bool move_cursor_left ();

      bool move_cursor_right ();

      bool char_received_normal (const char ch, bool eof, string_type& to_echo, std::function<void(string_type)> received_line_callback);

      bool char_received_in_end_line (char ch, string_type& to_echo, std::function<void(string_type)> received_line_callback);

      bool char_received_had_escape (const char ch, string_type& to_echo);

      bool char_received_had_bracket (char ch, string_type& to_echo);

      bool char_received_had_bracket_number (const char ch, string_type& to_echo);

      bool move_history_up (string_type& to_echo);

      bool move_history_down (string_type& to_echo);



      parser_state state_;

      string_type current_line_;

      string_type::size_type current_idx_;

      bool password_mode_;

      history_type history_;

      history_type::iterator history_it_;

      bool had_first_;

      int opened_brackets_;


};







class dummy_transport 
{
  public:
	typedef std::unique_ptr<dummy_transport> smart_ptr;

	vt100_transport trying_;

  public:
      dummy_transport();

      virtual ~dummy_transport();


      void line_received (const string_type& line);

      void do_stuff ();


  protected:

  private:


};


} // namespace rx_vt100
} // namespace host



#endif
