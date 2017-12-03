

/****************************************************************************
*
*  soft_logic\sl_inst.h
*
*  Copyright (c) 2017 Dusan Ciric
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


#ifndef sl_instructions_h
#define sl_instructions_h 1



// soft_plc
#include "soft_logic/soft_plc.h"

#define OVERFLOW_UP_VALUE 0x80000000
#define OVERFLOW_DOWN_VALUE 0x7fffffff

#define INSTRUCTION_DATA(i,o,lh) public:\
	int get_inputs() { return i; }\
	int get_outputs() { return o; }\
	int get_ladder_size() { return lh; }\




namespace sl_runtime {

namespace instructions_runtime {





class sl_invertor : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,1);

  public:
      sl_invertor();

      virtual ~sl_invertor();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class bit_instruction : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,1);

  public:
      bit_instruction (const bit_value& bit);

      virtual ~bit_instruction();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      virtual bool is_through (program_context* context, bool input) = 0;

      virtual bool is_active (program_context* context) = 0;


      bit_value _bit;


  private:


};






class sl_open_contact : public bit_instruction  
{

  public:
      sl_open_contact (const bit_value& bit);

      virtual ~sl_open_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_closed_contact : public bit_instruction  
{

  public:
      sl_closed_contact (const bit_value& bit);

      virtual ~sl_closed_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_pos_contact : public bit_instruction  
{

  public:
      sl_pos_contact (const bit_value& bit);

      virtual ~sl_pos_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_neg_contact : public bit_instruction  
{

  public:
      sl_neg_contact (const bit_value& bit);

      virtual ~sl_neg_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_latch : public bit_instruction  
{

  public:
      sl_latch (const bit_value& bit);

      virtual ~sl_latch();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_unlatch : public bit_instruction  
{

  public:
      sl_unlatch (const bit_value& bit);

      virtual ~sl_unlatch();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_coil : public bit_instruction  
{

  public:
      sl_coil (const bit_value& bit);

      virtual ~sl_coil();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_closed_coil : public bit_instruction  
{

  public:
      sl_closed_coil (const bit_value& bit);

      virtual ~sl_closed_coil();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_compare : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,4,4);

  public:
      sl_compare (const argument_value& arg1, const argument_value& arg2);

      virtual ~sl_compare();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value _arguments[2];


};


enum compare_type
{
	eq_compare,
	ne_compare,
	lt_compare,
	gt_compare,
	le_compare,
	ge_compare
};




class sl_small_compare : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,2,2);

  public:
      sl_small_compare (compare_type inst_type, const argument_value& arg1, const argument_value& arg2);

      virtual ~sl_small_compare();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value _arguments[2];

      compare_type _inst_type;


};






class sl_mov : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,2);

  public:
      sl_mov (const register_value& result, const argument_value& argument);

      virtual ~sl_mov();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value _argument;

      register_value _result;


};






class subprogram_instruction : public sl_instruction_base  
{

  public:
      subprogram_instruction (const string_type& sub_name, int inputs, int outputs, const std::vector<argument_value>& arguments, const std::vector<register_value>& out_arguments, const rx_uuid& guid, int height);

      virtual ~subprogram_instruction();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);

      void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      int get_inputs ();

      int get_outputs ();

      int get_ladder_size ();


      const rx_uuid get_guid () const
      {
        return _guid;
      }



  protected:

  private:


      string_type _sub_name;

      int _inputs_bits;

      int _output_bits;

      int _height;

      std::vector<argument_value> _arguments;

      rx_uuid _guid;

      std::vector<register_value> _out_arguments;


};


enum arithmetic_type
{
	add_type,
	sub_type,
	div_type,
	mul_type,
	rem_type
};




class sl_arithmetic : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,2,2);

  public:
      sl_arithmetic (arithmetic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result);

      virtual ~sl_arithmetic();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value _arguments[2];

      arithmetic_type _inst_type;

      register_value _result;


};


enum logic_type
{
	and_type,
	or_type,
	xor_type
};




class sl_logic : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,2);

  public:
      sl_logic (logic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result);

      virtual ~sl_logic();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value _arguments[2];

      logic_type _inst_type;

      register_value _result;


};






class sl_counter : public sl_instruction_base  
{

  public:
      sl_counter (dword number, const argument_value& preset);

      virtual ~sl_counter();


  protected:

      void set_results (bool use_zero, program_context* context);

      void set_count_up (bool state);

      void reset ();

      void set_count_down (bool state);

      void preset ();

      bool get_d (bool use_zero);

      bool get_e ();

      bool get_f ();

      void begin (program_context* context);


  private:


      argument_value _preset;

      bool _prev_up_state;

      dword _value;

      //	broj brojaca
      dword _number;

      bool _prev_down_state;

      dword _preset_val;

      bool _bit_e;

      bool _bit_f;


};






class sl_up_counter : public sl_counter  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_up_counter (dword number, const argument_value& preset);

      virtual ~sl_up_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_down_counter : public sl_counter  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_down_counter (dword number, const argument_value& preset);

      virtual ~sl_down_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_up_down_counter : public sl_counter  
{
	INSTRUCTION_DATA(4,3,4);

  public:
      sl_up_down_counter (dword number, const argument_value& preset);

      virtual ~sl_up_down_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_inc_dec : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,1);

  public:
      sl_inc_dec (bool inc, const register_value& value);

      virtual ~sl_inc_dec();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      register_value _value;

      bool _inc;


};






class sl_monostabile : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,2);

  public:
      sl_monostabile (dword number, const argument_value& preset, dword base);

      virtual ~sl_monostabile();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      void set_results (bool use_zero, program_context* context);

      void begin (program_context* context);


  private:


      argument_value _preset;

      //	broj brojaca
      dword _number;

      //	broj brojaca
      dword _base;

      dword _value;

      dword _preset_val;

      bool _prev_c_state;

      dword _started_tick;

      bool _prev_r_state;

      bool _prev_i_state;


};






class sl_timer_inst : public sl_instruction_base  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_timer_inst (dword number, const argument_value& preset, dword base);

      virtual ~sl_timer_inst();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      void set_results (bool use_zero, program_context* context);

      void begin (program_context* context);


  private:


      argument_value _preset;

      //	broj brojaca
      dword _number;

      //	broj brojaca
      dword _base;

      dword _value;

      dword _preset_val;

      bool _prev_c_state;

      dword _started_tick;

      bool _prev_r_state;

      bool _prev_d_state;

      dword _ellapsed;


};


} // namespace instructions_runtime
} // namespace sl_runtime



#endif
