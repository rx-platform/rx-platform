

/****************************************************************************
*
*  soft_logic\sl_inst.h
*
*  Copyright (c) 2018-2019 Dusan Ciric
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
*  along with rx-platform. It is also available in any rx-platform console
*  via <license> command. If not, see <http://www.gnu.org/licenses/>.
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

      ~sl_invertor();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class bit_instruction : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,1);

  public:
      bit_instruction (const bit_value& bit);

      ~bit_instruction();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      virtual bool is_through (program_context* context, bool input) = 0;

      virtual bool is_active (program_context* context) = 0;


      bit_value bit_;


  private:


};






class sl_open_contact : public bit_instruction  
{

  public:
      sl_open_contact (const bit_value& bit);

      ~sl_open_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_closed_contact : public bit_instruction  
{

  public:
      sl_closed_contact (const bit_value& bit);

      ~sl_closed_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_pos_contact : public bit_instruction  
{

  public:
      sl_pos_contact (const bit_value& bit);

      ~sl_pos_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_neg_contact : public bit_instruction  
{

  public:
      sl_neg_contact (const bit_value& bit);

      ~sl_neg_contact();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_latch : public bit_instruction  
{

  public:
      sl_latch (const bit_value& bit);

      ~sl_latch();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_unlatch : public bit_instruction  
{

  public:
      sl_unlatch (const bit_value& bit);

      ~sl_unlatch();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_coil : public bit_instruction  
{

  public:
      sl_coil (const bit_value& bit);

      ~sl_coil();


  protected:

      bool is_through (program_context* context, bool input);

      bool is_active (program_context* context);


  private:


};






class sl_closed_coil : public bit_instruction  
{

  public:
      sl_closed_coil (const bit_value& bit);

      ~sl_closed_coil();


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

      ~sl_compare();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value arguments_[2];


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

      ~sl_small_compare();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value arguments_[2];

      compare_type inst_type_;


};






class sl_mov : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,2);

  public:
      sl_mov (const register_value& result, const argument_value& argument);

      ~sl_mov();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value argument_;

      register_value result_;


};






class subprogram_instruction : public sl_instruction_base  
{

  public:
      subprogram_instruction (const string_type& sub_name, int inputs, int outputs, const std::vector<argument_value>& arguments, const std::vector<register_value>& out_arguments, const rx_uuid& guid, int height);

      ~subprogram_instruction();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);

      void initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context);

      void deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context);

      int get_inputs ();

      int get_outputs ();

      int get_ladder_size ();


      const rx_uuid get_guid () const
      {
        return guid_;
      }



  protected:

  private:


      string_type sub_name_;

      int inputs_bits_;

      int output_bits_;

      int height_;

      std::vector<argument_value> arguments_;

      rx_uuid guid_;

      std::vector<register_value> out_arguments_;


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

      ~sl_arithmetic();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value arguments_[2];

      arithmetic_type inst_type_;

      register_value result_;


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

      ~sl_logic();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      argument_value arguments_[2];

      logic_type inst_type_;

      register_value result_;


};






class sl_counter : public sl_instruction_base  
{

  public:
      sl_counter (dword number, const argument_value& preset);

      ~sl_counter();


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


      argument_value preset_;

      bool prev_up_state_;

      dword value_;

      //	broj brojaca
      dword number_;

      bool prev_down_state_;

      dword preset_val_;

      bool bit_e_;

      bool bit_f_;


};






class sl_up_counter : public sl_counter  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_up_counter (dword number, const argument_value& preset);

      ~sl_up_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_down_counter : public sl_counter  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_down_counter (dword number, const argument_value& preset);

      ~sl_down_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_up_down_counter : public sl_counter  
{
	INSTRUCTION_DATA(4,3,4);

  public:
      sl_up_down_counter (dword number, const argument_value& preset);

      ~sl_up_down_counter();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


};






class sl_inc_dec : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,1);

  public:
      sl_inc_dec (bool inc, const register_value& value);

      ~sl_inc_dec();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

  private:


      register_value value_;

      bool inc_;


};






class sl_monostabile : public sl_instruction_base  
{
	INSTRUCTION_DATA(1,1,2);

  public:
      sl_monostabile (dword number, const argument_value& preset, dword base);

      ~sl_monostabile();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      void set_results (bool use_zero, program_context* context);

      void begin (program_context* context);


  private:


      argument_value preset_;

      //	broj brojaca
      dword number_;

      //	broj brojaca
      dword base_;

      dword value_;

      dword preset_val_;

      bool prev_c_state_;

      dword started_tick_;

      bool prev_r_state_;

      bool prev_i_state_;


};






class sl_timer_inst : public sl_instruction_base  
{
	INSTRUCTION_DATA(2,2,2);

  public:
      sl_timer_inst (dword number, const argument_value& preset, dword base);

      ~sl_timer_inst();


      void process_instruction (io_program_data& io_data, program_context* context, const rx_time& now);


  protected:

      void set_results (bool use_zero, program_context* context);

      void begin (program_context* context);


  private:


      argument_value preset_;

      //	broj brojaca
      dword number_;

      //	broj brojaca
      dword base_;

      dword value_;

      dword preset_val_;

      bool prev_c_state_;

      dword started_tick_;

      bool prev_r_state_;

      bool prev_d_state_;

      dword ellapsed_;


};


} // namespace instructions_runtime
} // namespace sl_runtime



#endif
