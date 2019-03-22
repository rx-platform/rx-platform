

/****************************************************************************
*
*  soft_logic\sl_inst.cpp
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


#include "pch.h"


// sl_instructions
#include "soft_logic/sl_inst.h"



namespace sl_runtime {

namespace instructions_runtime {

// Class sl_runtime::instructions_runtime::sl_invertor 

sl_invertor::sl_invertor()
{
}


sl_invertor::~sl_invertor()
{
}



void sl_invertor::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	  io_data.set_output(0,!io_data.get_input(0));
}


// Class sl_runtime::instructions_runtime::sl_open_contact 

sl_open_contact::sl_open_contact (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_open_contact::~sl_open_contact()
{
}



bool sl_open_contact::is_through (program_context* context, bool input)
{
	if(input)
		return bit_.get_value(context);
	else
		return false;
}

bool sl_open_contact::is_active (program_context* context)
{
	return is_through(context,true);
}


// Class sl_runtime::instructions_runtime::sl_closed_contact 

sl_closed_contact::sl_closed_contact (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_closed_contact::~sl_closed_contact()
{
}



bool sl_closed_contact::is_through (program_context* context, bool input)
{
	if(input)
		return !(bit_.get_value(context));
	else
		return false;
}

bool sl_closed_contact::is_active (program_context* context)
{
	return is_through(context,true);
}


// Class sl_runtime::instructions_runtime::bit_instruction 

bit_instruction::bit_instruction (const bit_value& bit)
      : bit_(bit)
{
}


bit_instruction::~bit_instruction()
{
}



void bit_instruction::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	  io_data.set_instruction_active(is_active(context));
	  io_data.set_output(0,is_through(context,io_data.get_input(0)));
}


// Class sl_runtime::instructions_runtime::sl_pos_contact 

sl_pos_contact::sl_pos_contact (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_pos_contact::~sl_pos_contact()
{
}



bool sl_pos_contact::is_through (program_context* context, bool input)
{
	if(input)
		return bit_.get_positive_transact(context);
	else
		return false;
}

bool sl_pos_contact::is_active (program_context* context)
{
	return is_through(context,true);
}


// Class sl_runtime::instructions_runtime::sl_neg_contact 

sl_neg_contact::sl_neg_contact (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_neg_contact::~sl_neg_contact()
{
}



bool sl_neg_contact::is_through (program_context* context, bool input)
{
	if(input)
		return bit_.get_negative_transact(context);
	else
		return false;
}

bool sl_neg_contact::is_active (program_context* context)
{
	return is_through(context,true);
}


// Class sl_runtime::instructions_runtime::sl_latch 

sl_latch::sl_latch (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_latch::~sl_latch()
{
}



bool sl_latch::is_through (program_context* context, bool input)
{
	if(input)
		bit_.set_value(context,true);
	return input;
}

bool sl_latch::is_active (program_context* context)
{
	return false;
}


// Class sl_runtime::instructions_runtime::sl_unlatch 

sl_unlatch::sl_unlatch (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_unlatch::~sl_unlatch()
{
}



bool sl_unlatch::is_through (program_context* context, bool input)
{
	if(input)
		bit_.set_value(context,false);
	return input;
}

bool sl_unlatch::is_active (program_context* context)
{
	return false;
}


// Class sl_runtime::instructions_runtime::sl_coil 

sl_coil::sl_coil (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_coil::~sl_coil()
{
}



bool sl_coil::is_through (program_context* context, bool input)
{
	bit_.set_value(context,input);
	return input;
}

bool sl_coil::is_active (program_context* context)
{
	return false;
}


// Class sl_runtime::instructions_runtime::sl_closed_coil 

sl_closed_coil::sl_closed_coil (const bit_value& bit)
  : bit_instruction(bit)
{
}


sl_closed_coil::~sl_closed_coil()
{
}



bool sl_closed_coil::is_through (program_context* context, bool input)
{
	bit_.set_value(context,!input);
	return !input;
}

bool sl_closed_coil::is_active (program_context* context)
{
	return false;
}


// Class sl_runtime::instructions_runtime::sl_compare 

sl_compare::sl_compare (const argument_value& arg1, const argument_value& arg2)
{
	arguments_[0]=arg1;
	arguments_[1]=arg2;
}


sl_compare::~sl_compare()
{
}



void sl_compare::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(!arguments_[0].is_float() && !arguments_[1].is_float())
		{// pure integers do it regular
			int val1=arguments_[0].get_value(context);
			int val2=arguments_[1].get_value(context);
			if(val1>val2)
				io_data.set_output(0,true);
			else if(val1==val2)
				io_data.set_output(1,true);
			else if(val1<val2)
				io_data.set_output(2,true);
			if(val1!=val2)
				io_data.set_output(3,true);
		}
		else
		{// pure integers do it regular
			double val1=arguments_[0].get_double(context);
			double val2=arguments_[1].get_double(context);
			if(val1>val2)
				io_data.set_output(0,true);
			else if(val1==val2)
				io_data.set_output(1,true);
			else if(val1<val2)
				io_data.set_output(2,true);
			if(val1!=val2)
				io_data.set_output(3,true);
		}
	}
}


// Class sl_runtime::instructions_runtime::sl_small_compare 

sl_small_compare::sl_small_compare (compare_type inst_type, const argument_value& arg1, const argument_value& arg2)
      : inst_type_(inst_type)
{
	arguments_[0]=arg1;
	arguments_[1]=arg2;
}


sl_small_compare::~sl_small_compare()
{
}



void sl_small_compare::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(!arguments_[0].is_float() && !arguments_[1].is_float())
		{// pure integers do it regular
			int val1=arguments_[0].get_value(context);
			int val2=arguments_[1].get_value(context);
			switch(inst_type_)
			{
				case eq_compare:
					io_data.set_output(0,(val1==val2));
					io_data.set_output(1,!(val1==val2));
					break;
				case ne_compare:
					io_data.set_output(0,(val1!=val2));
					io_data.set_output(1,!(val1!=val2));
					break;
				case ge_compare:
					io_data.set_output(0,(val1>=val2));
					io_data.set_output(1,!(val1>=val2));
					break;
				case le_compare:
					io_data.set_output(0,(val1<=val2));
					io_data.set_output(1,!(val1<=val2));
					break;
				case gt_compare:
					io_data.set_output(0,(val1>val2));
					io_data.set_output(1,!(val1>val2));
					break;
				case lt_compare:
					io_data.set_output(0,(val1<val2));
					io_data.set_output(1,!(val1<val2));
					break;
			}


		}
		else
		{
			double val1=arguments_[0].get_double(context);
			double val2=arguments_[1].get_double(context);
			switch(inst_type_)
			{
				case eq_compare:
					io_data.set_output(0,(val1==val2));
					io_data.set_output(1,!(val1==val2));
					break;
				case ne_compare:
					io_data.set_output(0,(val1!=val2));
					io_data.set_output(1,!(val1!=val2));
					break;
				case ge_compare:
					io_data.set_output(0,(val1>=val2));
					io_data.set_output(1,!(val1>=val2));
					break;
				case le_compare:
					io_data.set_output(0,(val1<=val2));
					io_data.set_output(1,(val1<=val2));
					break;
				case gt_compare:
					io_data.set_output(0,(val1>val2));
					io_data.set_output(1,!(val1>val2));
					break;
				case lt_compare:
					io_data.set_output(0,(val1<val2));
					io_data.set_output(1,!(val1<val2));
					break;
			}
		}
	}
}


// Class sl_runtime::instructions_runtime::sl_mov 

sl_mov::sl_mov (const register_value& result, const argument_value& argument)
      : argument_(argument),
        result_(result)
{
}


sl_mov::~sl_mov()
{
}



void sl_mov::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(argument_.is_float() && result_.is_float())
		{
			result_.set_float(context,argument_.get_float(context));
		}
		else if(argument_.is_float() && !result_.is_float())
		{
			result_.set_value(context,(uint32_t)argument_.get_float(context));
		}
		else if(!argument_.is_float() && result_.is_float())
		{
			result_.set_float(context,(float)argument_.get_value(context));
		}
		else// if(!argument_.is_float() && !result_.is_float())
		{
			result_.set_value(context,(uint32_t)argument_.get_value(context));
		}
		io_data.set_output(0,true);
	}
}


// Class sl_runtime::instructions_runtime::subprogram_instruction 

subprogram_instruction::subprogram_instruction (const string_type& sub_name, int inputs, int outputs, const std::vector<argument_value>& arguments, const std::vector<register_value>& out_arguments, const rx_uuid& guid, int height)
      : sub_name_(sub_name),
        inputs_bits_(inputs),
        output_bits_(outputs),
        height_(height),
        arguments_(arguments),
        guid_(guid),
        out_arguments_(out_arguments)
{
}


subprogram_instruction::~subprogram_instruction()
{
}



void subprogram_instruction::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	std::vector<bool> inputs(inputs_bits_);
	program_context* my_context=context->get_program_holder()->get_subcontext(this);
	if(my_context==NULL)
	{// error nothing here
		RX_ASSERT(false);
	}
	else
	{
		my_context->init_scan();
		// setup input bits
		for(int i=0; i<inputs_bits_; i++)
		{
			my_context->set_bit(input_bits,0,i,io_data.get_input(i));
		}
		// set input registers
		int bit_reg=1;
		for(std::vector<argument_value>::iterator it=arguments_.begin(); it!=arguments_.end(); it++)
		{
			my_context->set_register(input_bits,bit_reg++,(uint32_t)it->get_value(context));
		}
		// set output registers
		bit_reg=1;
		for(std::vector<register_value>::iterator it=out_arguments_.begin(); it!=out_arguments_.end(); it++)
		{
			my_context->set_register(output_bits,bit_reg++,(uint32_t)it->get_value(context));
		}
		// process program
		context->get_program_holder()->execute_sub(this,now);
		// process outputs
		for(int i=0; i<output_bits_; i++)
		{
			io_data.set_output(i,my_context->get_bit(output_bits,0,i));
		}
		// do output registers
		bit_reg=1;
		for(std::vector<register_value>::iterator it=out_arguments_.begin(); it!=out_arguments_.end(); it++)
		{
			it->set_value(context,my_context->get_register(output_bits,bit_reg++));
		}
		// and we're done
	}
}

void subprogram_instruction::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	sl_instruction_base::initialize(holder,ctx,current_context);
	bool ret=holder->register_subcontext(this,sub_name_,ctx,current_context);
	RX_ASSERT(ret);
}

void subprogram_instruction::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	holder->deinitialize_sub(this,ctx);
	sl_instruction_base::deinitialize(holder,ctx,current_context);
}

int subprogram_instruction::get_inputs ()
{
	return inputs_bits_;
}

int subprogram_instruction::get_outputs ()
{
	return output_bits_;
}

int subprogram_instruction::get_ladder_size ()
{
	return height_;
}


// Class sl_runtime::instructions_runtime::sl_arithmetic 

sl_arithmetic::sl_arithmetic (arithmetic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result)
      : inst_type_(inst_type),
        result_(result)
{
	arguments_[0]=arg1;
	arguments_[1]=arg2;
}


sl_arithmetic::~sl_arithmetic()
{
}



void sl_arithmetic::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{

	bool overflow=false;
	bool carry=false;
	if(io_data.get_input(0))
	{
		if(!arguments_[0].is_float() && !arguments_[1].is_float() && !result_.is_float())
		{// pure integers do it regular
			int res;
			int val1=arguments_[0].get_value(context);
			int val2=arguments_[1].get_value(context);
			switch(inst_type_)
			{
				case add_type:
					res=val1+val2;
					break;
				case sub_type:
					res=val1-val2;
					break;
				case mul_type:
					res=val1*val2;
					break;
				case div_type:
					res=val1/val2;
					break;
				case rem_type:
					res=val1%val2;
					break;
                default:
                    res=0;
			}
			result_.set_value(context,res);
		}
		else
		{// no matther what it is double stuff
			double res=0;
			double val1=arguments_[0].get_double(context);
			double val2=arguments_[1].get_double(context);
			switch(inst_type_)
			{
				case add_type:
					res=val1+val2;
					break;
				case sub_type:
					res=val1-val2;
					break;
				case mul_type:
					res=val1*val2;
					break;
				case div_type:
					res=val1/val2;
					break;
				case rem_type:
					res=((int)val1)%((int)val2);
					break;
			}
			result_.set_double(context,res);
		}
	}
	io_data.set_output(0,overflow);
	io_data.set_output(1,carry);
}


// Class sl_runtime::instructions_runtime::sl_logic 

sl_logic::sl_logic (logic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result)
      : inst_type_(inst_type),
        result_(result)
{
	arguments_[0]=arg1;
	arguments_[1]=arg2;
}


sl_logic::~sl_logic()
{
}



void sl_logic::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	int res=0;
	if(io_data.get_input(0))
	{
		int val1=arguments_[0].get_value(context);
		int val2=arguments_[1].get_value(context);
		switch(inst_type_)
		{
			case and_type:
				res=(val1&val2);
				break;
			case or_type:
				res=(val1|val2);
				break;
			case xor_type:
				res=(val1^val2);
				break;
		}
		result_.set_value(context,(dword)res);
	}
	io_data.set_output(0,io_data.get_input(0));
}


// Class sl_runtime::instructions_runtime::sl_up_counter 

sl_up_counter::sl_up_counter (dword number, const argument_value& preset)
  : sl_counter(number,preset)
{
}


sl_up_counter::~sl_up_counter()
{
}



void sl_up_counter::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	// first
	begin(context);
	set_count_up(io_data.get_input(0));
	if(io_data.get_input(1))
	{// second
		reset();
	}
	set_results(false,context);
	io_data.set_output(0,get_d(false));
	io_data.set_output(1,get_f());
}


// Class sl_runtime::instructions_runtime::sl_counter 

sl_counter::sl_counter (dword number, const argument_value& preset)
      : preset_(preset),
        prev_up_state_(false),
        value_(0),
        number_(number),
        prev_down_state_(false),
        preset_val_(0)
{
}


sl_counter::~sl_counter()
{
}



void sl_counter::set_results (bool use_zero, program_context* context)
{
	context->set_bit(counter_bits,number_,0,get_e());
	context->set_bit(counter_bits,number_,1,get_f());
	context->set_bit(counter_bits,number_,2,get_d(use_zero));
	// previous states had to be stored in context
	context->set_bit(counter_bits,number_,3,prev_up_state_);
	context->set_bit(counter_bits,number_,4,prev_down_state_);
	// register values
	context->set_register(counter_registers,number_*2,value_);
	context->set_register(counter_registers,number_*2+1,preset_val_);
}

void sl_counter::set_count_up (bool state)
{
	if(!prev_up_state_ && state)
	{
		value_++;
		if(value_==0)
			bit_f_=true;
	}
	prev_up_state_=state;
}

void sl_counter::reset ()
{
	value_=0;
	bit_e_=false;
	bit_f_=false;
}

void sl_counter::set_count_down (bool state)
{
	if(!prev_down_state_ && state)
	{
		if(value_==0)
			bit_e_=true;
		value_--;
	}
	prev_down_state_=state;
}

void sl_counter::preset ()
{
	value_=preset_val_;
	bit_e_=false;
	bit_f_=false;
}

bool sl_counter::get_d (bool use_zero)
{
	return (value_>preset_val_ || (use_zero && value_==0));
}

bool sl_counter::get_e ()
{
	return bit_e_;
}

bool sl_counter::get_f ()
{
	return bit_f_;
}

void sl_counter::begin (program_context* context)
{
	preset_val_=(uint32_t)preset_.get_value(context);
	value_=(uint32_t)context->get_register(counter_registers,number_*2);
	// restore previous states
	prev_up_state_=context->get_bit(counter_bits,number_,3);
	prev_down_state_=context->get_bit(counter_bits,number_,4);

	bit_f_=false;
	bit_e_=false;

}


// Class sl_runtime::instructions_runtime::sl_down_counter 

sl_down_counter::sl_down_counter (dword number, const argument_value& preset)
  : sl_counter(number,preset)
{
}


sl_down_counter::~sl_down_counter()
{
}



void sl_down_counter::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	// first
	begin(context);
	set_count_down(io_data.get_input(0));
	if(io_data.get_input(1))
	{// second
		preset();
	}
	set_results(true,context);
	io_data.set_output(0,get_d(true));
	io_data.set_output(1,get_e());
}


// Class sl_runtime::instructions_runtime::sl_up_down_counter 

sl_up_down_counter::sl_up_down_counter (dword number, const argument_value& preset)
  : sl_counter(number,preset)
{
}


sl_up_down_counter::~sl_up_down_counter()
{
}



void sl_up_down_counter::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	// first
	begin(context);
	set_count_up(io_data.get_input(0));
	if(io_data.get_input(1))
	{// second
		reset();
	}
	set_count_down(io_data.get_input(2));
	if(io_data.get_input(3))
	{// second
		preset();
	}
	set_results(false,context);
	io_data.set_output(0,get_d(false));
	io_data.set_output(1,get_f());
	io_data.set_output(2,get_e());
}


// Class sl_runtime::instructions_runtime::sl_inc_dec 

sl_inc_dec::sl_inc_dec (bool inc, const register_value& value)
      : value_(value),
        inc_(inc)
{
}


sl_inc_dec::~sl_inc_dec()
{
}



void sl_inc_dec::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	bool ending=false;
	if(io_data.get_input(0))
	{
		if(value_.is_float())
		{
			float val=value_.get_float(context);
			if(inc_)
			{
				val=val+1.0f;
				value_.set_float(context,val);
			}
			else
			{// dec
				val=val-1.0f;
				value_.set_float(context,val);
			}
		}
		else
		{
			uint32_t val=value_.get_value(context);
			if(inc_)
			{
				val++;
				ending=(val==OVERFLOW_UP_VALUE);
				value_.set_value(context,val);
			}
			else
			{// dec
				val--;
				ending=(val==OVERFLOW_DOWN_VALUE);
				value_.set_value(context,val);
			}
		}
	}
	io_data.set_output(0,ending);
}


// Class sl_runtime::instructions_runtime::sl_monostabile 

sl_monostabile::sl_monostabile (dword number, const argument_value& preset, dword base)
      : preset_(preset),
        number_(number),
        base_(base),
        value_(0),
        preset_val_(0),
        prev_c_state_(false),
        started_tick_(0),
        prev_r_state_(false),
        prev_i_state_(false)
{
}


sl_monostabile::~sl_monostabile()
{
}



void sl_monostabile::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	uint32_t tick=rx_get_tick_count();
	begin(context);
	if(io_data.get_input(0) && !prev_c_state_)
	{// up so do the counting
		started_tick_=tick;
		prev_r_state_=true;
	}
	if(prev_r_state_)
	{// we are counting
		value_=(tick-started_tick_)/base_+1;
		if(value_>=preset_val_)
		{// done with counting
			value_=0;
			prev_r_state_=false;
		}
	}
	prev_c_state_=io_data.get_input(0);
	set_results(true,context);
	io_data.set_output(0,prev_r_state_);
}

void sl_monostabile::set_results (bool use_zero, program_context* context)
{
	// previous states had to be stored in context
	context->set_bit(mono_bits,number_,0,prev_r_state_);
	context->set_bit(mono_bits,number_,1,prev_c_state_);
	// register values
	context->set_register(mono_registers,number_*3,value_);
	context->set_register(mono_registers,number_*3+1,preset_val_);
	context->set_register(mono_registers,number_*3+2,started_tick_);
}

void sl_monostabile::begin (program_context* context)
{
	preset_val_=(uint32_t)preset_.get_value(context);
	// restore previous states
	prev_r_state_=context->get_bit(mono_bits,number_,0);
	prev_c_state_=context->get_bit(mono_bits,number_,1);
	value_=(uint32_t)context->get_register(mono_registers,number_*3);
	started_tick_=context->get_register(mono_registers,number_*3+2);
}


// Class sl_runtime::instructions_runtime::sl_timer_inst 

sl_timer_inst::sl_timer_inst (dword number, const argument_value& preset, dword base)
      : preset_(preset),
        number_(number),
        base_(base),
        value_(0),
        preset_val_(0),
        prev_c_state_(false),
        started_tick_(0),
        prev_r_state_(false),
        prev_d_state_(false),
        ellapsed_(0)
{
}


sl_timer_inst::~sl_timer_inst()
{
}



void sl_timer_inst::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	uint32_t tick=rx_get_tick_count();
	begin(context);
	if(!io_data.get_input(1))
	{// enable is down so rest stuff
		value_=0;
		prev_d_state_=false;
		prev_r_state_=false;
		ellapsed_=0;
	}
	else if(io_data.get_input(1) && !prev_d_state_)
	{// enable is and not done
		if(io_data.get_input(0))
		{// count is on so check our state
			if(!prev_r_state_)
			{// start counting
				started_tick_=tick;
				prev_r_state_=true;
			}
			else
			{// we were counting allready
				value_=(tick-started_tick_+ellapsed_)/base_+1;
				if(value_>=preset_val_)
				{// done with counting
					value_=preset_val_;
					prev_r_state_=false;
					prev_d_state_=true;
				}
			}
		}
		else
		{// counting is off
			if(prev_r_state_)
			{// we were counting
				value_=(tick-started_tick_+ellapsed_)/base_+1;
				if(value_>=preset_val_)
				{// done with counting
					value_=preset_val_;
					prev_d_state_=true;
				}
				else
				{// not done so reneber the ellapsed value
					ellapsed_+=(tick-started_tick_);
				}
			}
			prev_r_state_=false;// we're not counting
		}
	}
	prev_c_state_=io_data.get_input(0);
	set_results(true,context);
	io_data.set_output(0,prev_d_state_);
	io_data.set_output(1,prev_r_state_);
}

void sl_timer_inst::set_results (bool use_zero, program_context* context)
{
	context->set_bit(timer_bits,number_,0,prev_r_state_);
	context->set_bit(timer_bits,number_,1,prev_d_state_);
	context->set_bit(timer_bits,number_,2,prev_c_state_);
	// register values
	context->set_register(timer_registers,number_*4,value_);
	context->set_register(timer_registers,number_*4+1,preset_val_);
	context->set_register(timer_registers,number_*4+2,started_tick_);
	context->set_register(timer_registers,number_*4+3,ellapsed_);
}

void sl_timer_inst::begin (program_context* context)
{
	preset_val_=(uint32_t)preset_.get_value(context);
	value_=(uint32_t)context->get_register(timer_registers,number_*4);
	started_tick_=(uint32_t)context->get_register(timer_registers,number_*4+2);
	ellapsed_=(uint32_t)context->get_register(timer_registers,number_*4+3);
	// restore previous states
	prev_r_state_=context->get_bit(timer_bits,number_,0);
	prev_d_state_=context->get_bit(timer_bits,number_,1);
	prev_c_state_=context->get_bit(timer_bits,number_,2);
}


} // namespace instructions_runtime
} // namespace sl_runtime

