

/****************************************************************************
*
*  soft_logic\sl_script.h
*
*  Copyright (c) 2020-2021 ENSACO Solutions doo
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


#ifndef sl_script_h
#define sl_script_h 1



// soft_plc
#include "soft_logic/soft_plc.h"



namespace sl_runtime {

namespace sl_script {





class script_program_context : public program_context  
{

  public:
      script_program_context (program_context* parent, sl_program_holder* holder);

      ~script_program_context();


      void initialize (initialize_context* ctx);

      void deinitialize (deinitialize_context* ctx);

      void raise_error ();

      bool get_result () const;

      virtual std::ostream& get_stdout () = 0;

      virtual std::ostream& get_stderr () = 0;

      void set_waiting ();

      void reset_waiting ();

      void continue_scan ();

      void init_scan ();


      const size_t get_current_line () const;


  protected:

  private:

      size_t next_line ();

      virtual void send_results (bool result, bool done) = 0;



      size_t current_line_;

      std::atomic_bool waiting_;

      std::atomic_bool error_;


    friend class sl_script_program;
};







class sl_script_program : public sl_program  
{

  public:
      sl_script_program();

      ~sl_script_program();


      void process_program (program_context* context, const rx_time& now, bool debug);

      void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      void load (const string_type& lines);

      void clear ();


  protected:

      virtual bool parse_line (const string_type& line, std::ostream& out, std::ostream& err, program_context* context) = 0;


  private:


      string_array lines_;


};


} // namespace sl_script
} // namespace sl_runtime



#endif
