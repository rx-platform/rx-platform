

/****************************************************************************
*
*  terminal\rx_con_programs.h
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


#ifndef rx_con_programs_h
#define rx_con_programs_h 1



// rx_mem
#include "lib/rx_mem.h"
// sl_script
#include "soft_logic/sl_script.h"

#include "lib/security/rx_security.h"
#include "system/server/rx_ns.h"
#include "rx_terminal_style.h"


namespace rx_internal {

namespace terminal {

namespace console {

namespace script {


//	This class implements cancel, current directory and
//	instruction data




class console_program_context : public sl_runtime::sl_script::script_program_context  
{
public:
    typedef console_program_context* smart_ptr;
    typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;
    typedef std::map<size_t, rx_struct_ptr > instructions_data_type;
    typedef int pending_jobs_type;

  public:
      console_program_context (program_context* parent, sl_runtime::sl_program_holder* holder, const string_type& current_directory);

      ~console_program_context();


      void set_instruction_data (rx_struct_ptr data);

      bool is_canceled ();

      void cancel_execution ();

      virtual api::rx_context create_api_context () = 0;


      string_type get_current_directory ();
      void set_current_directory (string_type value);

      const int get_terminal_width () const
      {
        return terminal_width_;
      }

      void set_terminal_width (int value)
      {
        terminal_width_ = value;
      }


      const int get_terminal_height () const
      {
        return terminal_height_;
      }

      void set_terminal_height (int value)
      {
        terminal_height_ = value;
      }


      template<typename T>
      pointers::reference<T> get_instruction_data()
      {
          /*auto it = instructions_data_.find(get_possition<T>());
          if (it != instructions_data_.end())
          {
              return it->second.cast_to<pointers::reference<T> >();
          }
          else*/
          {
              return pointers::reference<T>::null_ptr;
          }
      }
      template<typename T>
      void raise_error(const T& result)
      {
          for (const auto& one : result.errors())
              get_stderr() << ANSI_RX_ERROR_LIST ">>" ANSI_COLOR_RESET << one << "\r\n";
          script_program_context::raise_error();
      }
  protected:

  private:


      string_type current_directory_;

      instructions_data_type instructions_data_;

      std::atomic_bool canceled_;

      int terminal_width_;

      int terminal_height_;


};







class console_program : public sl_runtime::sl_script::sl_script_program  
{
    typedef memory::std_strbuff<memory::std_vector_allocator>::smart_ptr buffer_ptr;

  public:
      console_program ();

      ~console_program();


      std::unique_ptr<sl_runtime::program_context> create_program_context (sl_runtime::program_context* parent_context, sl_runtime::sl_program_holder* holder);

      void process_program (sl_runtime::program_context* context, const rx_time& now, bool debug);


  protected:

      bool parse_line (const string_type& line, std::ostream& out, std::ostream& err, sl_runtime::program_context* context);


  private:


};


} // namespace script
} // namespace console
} // namespace terminal
} // namespace rx_internal



#endif
