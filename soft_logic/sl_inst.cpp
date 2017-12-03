

/****************************************************************************
*
*  soft_logic\sl_inst.cpp
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


#include "stdafx.h"


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
		return _bit.get_value(context);
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
		return !(_bit.get_value(context));
	else
		return false;
}

bool sl_closed_contact::is_active (program_context* context)
{
	return is_through(context,true);
}


// Class sl_runtime::instructions_runtime::bit_instruction

bit_instruction::bit_instruction (const bit_value& bit)
      : _bit(bit)
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
		return _bit.get_positive_transact(context);
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
		return _bit.get_negative_transact(context);
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
		_bit.set_value(context,true);
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
		_bit.set_value(context,false);
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
	_bit.set_value(context,input);
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
	_bit.set_value(context,!input);
	return !input;
}

bool sl_closed_coil::is_active (program_context* context)
{
	return false;
}


// Class sl_runtime::instructions_runtime::sl_compare

sl_compare::sl_compare (const argument_value& arg1, const argument_value& arg2)
{
	_arguments[0]=arg1;
	_arguments[1]=arg2;
}


sl_compare::~sl_compare()
{
}



void sl_compare::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(!_arguments[0].is_float() && !_arguments[1].is_float())
		{// pure integers do it regular
			int val1=_arguments[0].get_value(context);
			int val2=_arguments[1].get_value(context);
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
			double val1=_arguments[0].get_double(context);
			double val2=_arguments[1].get_double(context);
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
      : _inst_type(inst_type)
{
	_arguments[0]=arg1;
	_arguments[1]=arg2;
}


sl_small_compare::~sl_small_compare()
{
}



void sl_small_compare::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(!_arguments[0].is_float() && !_arguments[1].is_float())
		{// pure integers do it regular
			int val1=_arguments[0].get_value(context);
			int val2=_arguments[1].get_value(context);
			switch(_inst_type)
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
			double val1=_arguments[0].get_double(context);
			double val2=_arguments[1].get_double(context);
			switch(_inst_type)
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
      : _argument(argument),
        _result(result)
{
}


sl_mov::~sl_mov()
{
}



void sl_mov::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	if(io_data.get_input(0))
	{
		if(_argument.is_float() && _result.is_float())
		{
			_result.set_float(context,_argument.get_float(context));
		}
		else if(_argument.is_float() && !_result.is_float())
		{
			_result.set_value(context,(uint32_t)_argument.get_float(context));
		}
		else if(!_argument.is_float() && _result.is_float())
		{
			_result.set_float(context,(float)_argument.get_value(context));
		}
		else// if(!_argument.is_float() && !_result.is_float())
		{
			_result.set_value(context,(uint32_t)_argument.get_value(context));
		}
		io_data.set_output(0,true);
	}
}


// Class sl_runtime::instructions_runtime::subprogram_instruction

subprogram_instruction::subprogram_instruction (const string_type& sub_name, int inputs, int outputs, const std::vector<argument_value>& arguments, const std::vector<register_value>& out_arguments, const rx_uuid& guid, int height)
      : _sub_name(sub_name),
        _inputs_bits(inputs),
        _output_bits(outputs),
        _height(height),
        _arguments(arguments),
        _guid(guid),
        _out_arguments(out_arguments)
{
}


subprogram_instruction::~subprogram_instruction()
{
}



void subprogram_instruction::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	std::vector<bool> inputs(_inputs_bits);
	program_context* my_context=context->get_program_holder()->get_subcontext(this);
	if(my_context==NULL)
	{// error nothing here
		RX_ASSERT(false);
	}
	else
	{
		my_context->init_scan();
		// setup input bits
		for(int i=0; i<_inputs_bits; i++)
		{
			my_context->set_bit(input_bits,0,i,io_data.get_input(i));
		}
		// set input registers
		int bit_reg=1;
		for(std::vector<argument_value>::iterator it=_arguments.begin(); it!=_arguments.end(); it++)
		{
			my_context->set_register(input_bits,bit_reg++,(uint32_t)it->get_value(context));
		}
		// set output registers
		bit_reg=1;
		for(std::vector<register_value>::iterator it=_out_arguments.begin(); it!=_out_arguments.end(); it++)
		{
			my_context->set_register(output_bits,bit_reg++,(uint32_t)it->get_value(context));
		}
		// process program
		context->get_program_holder()->execute_sub(this,now);
		// process outputs
		for(int i=0; i<_output_bits; i++)
		{
			io_data.set_output(i,my_context->get_bit(output_bits,0,i));
		}
		// do output registers
		bit_reg=1;
		for(std::vector<register_value>::iterator it=_out_arguments.begin(); it!=_out_arguments.end(); it++)
		{
			it->set_value(context,my_context->get_register(output_bits,bit_reg++));
		}
		// and we're done
	}
}

void subprogram_instruction::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	sl_instruction_base::initialize(holder,ctx,current_context);
	bool ret=holder->register_subcontext(this,_sub_name,ctx,current_context);
	RX_ASSERT(ret);
}

void subprogram_instruction::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	holder->deinitialize_sub(this,ctx);
	sl_instruction_base::deinitialize(holder,ctx,current_context);
}

int subprogram_instruction::get_inputs ()
{
	return _inputs_bits;
}

int subprogram_instruction::get_outputs ()
{
	return _output_bits;
}

int subprogram_instruction::get_ladder_size ()
{
	return _height;
}


// Class sl_runtime::instructions_runtime::sl_arithmetic

sl_arithmetic::sl_arithmetic (arithmetic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result)
      : _inst_type(inst_type),
        _result(result)
{
	_arguments[0]=arg1;
	_arguments[1]=arg2;
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
		if(!_arguments[0].is_float() && !_arguments[1].is_float() && !_result.is_float())
		{// pure integers do it regular
			int res;
			int val1=_arguments[0].get_value(context);
			int val2=_arguments[1].get_value(context);
			switch(_inst_type)
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
			}
			_result.set_value(context,res);
		}
		else
		{// no matther what it is double stuff
			double res=0;
			double val1=_arguments[0].get_double(context);
			double val2=_arguments[1].get_double(context);
			switch(_inst_type)
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
			_result.set_double(context,res);
		}
	}
	io_data.set_output(0,overflow);
	io_data.set_output(1,carry);
}


// Class sl_runtime::instructions_runtime::sl_logic

sl_logic::sl_logic (logic_type inst_type, const argument_value& arg1, const argument_value& arg2, const register_value& result)
      : _inst_type(inst_type),
        _result(result)
{
	_arguments[0]=arg1;
	_arguments[1]=arg2;
}


sl_logic::~sl_logic()
{
}



void sl_logic::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	int res=0;
	if(io_data.get_input(0))
	{
		int val1=_arguments[0].get_value(context);
		int val2=_arguments[1].get_value(context);
		switch(_inst_type)
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
		_result.set_value(context,(dword)res);
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
      : _preset(preset),
        _prev_up_state(false),
        _value(0),
        _number(number),
        _prev_down_state(false),
        _preset_val(0)
{
}


sl_counter::~sl_counter()
{
}



void sl_counter::set_results (bool use_zero, program_context* context)
{
	context->set_bit(counter_bits,_number,0,get_e());
	context->set_bit(counter_bits,_number,1,get_f());
	context->set_bit(counter_bits,_number,2,get_d(use_zero));
	// previous states had to be stored in context
	context->set_bit(counter_bits,_number,3,_prev_up_state);
	context->set_bit(counter_bits,_number,4,_prev_down_state);
	// register values
	context->set_register(counter_registers,_number*2,_value);
	context->set_register(counter_registers,_number*2+1,_preset_val);
}

void sl_counter::set_count_up (bool state)
{
	if(!_prev_up_state && state)
	{
		_value++;
		if(_value==0)
			_bit_f=true;
	}
	_prev_up_state=state;
}

void sl_counter::reset ()
{
	_value=0;
	_bit_e=false;
	_bit_f=false;
}

void sl_counter::set_count_down (bool state)
{
	if(!_prev_down_state && state)
	{
		if(_value==0)
			_bit_e=true;
		_value--;
	}
	_prev_down_state=state;
}

void sl_counter::preset ()
{
	_value=_preset_val;
	_bit_e=false;
	_bit_f=false;
}

bool sl_counter::get_d (bool use_zero)
{
	return (_value>_preset_val || use_zero && _value==0);
}

bool sl_counter::get_e ()
{
	return _bit_e;
}

bool sl_counter::get_f ()
{
	return _bit_f;
}

void sl_counter::begin (program_context* context)
{
	_preset_val=(uint32_t)_preset.get_value(context);
	_value=(uint32_t)context->get_register(counter_registers,_number*2);
	// restore previous states
	_prev_up_state=context->get_bit(counter_bits,_number,3);
	_prev_down_state=context->get_bit(counter_bits,_number,4);

	_bit_f=false;
	_bit_e=false;

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
      : _value(value),
        _inc(inc)
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
		if(_value.is_float())
		{
			float val=_value.get_float(context);
			if(_inc)
			{
				val=val+1.0f;
				_value.set_float(context,val);
			}
			else
			{// dec
				val=val-1.0f;
				_value.set_float(context,val);
			}
		}
		else
		{
			uint32_t val=_value.get_value(context);
			if(_inc)
			{
				val++;
				ending=(val==OVERFLOW_UP_VALUE);
				_value.set_value(context,val);
			}
			else
			{// dec
				val--;
				ending=(val==OVERFLOW_DOWN_VALUE);
				_value.set_value(context,val);
			}
		}
	}
	io_data.set_output(0,ending);
}


// Class sl_runtime::instructions_runtime::sl_monostabile

sl_monostabile::sl_monostabile (dword number, const argument_value& preset, dword base)
      : _preset(preset),
        _number(number),
        _base(base),
        _value(0),
        _preset_val(0),
        _prev_c_state(false),
        _started_tick(0),
        _prev_r_state(false),
        _prev_i_state(false)
{
}


sl_monostabile::~sl_monostabile()
{
}



void sl_monostabile::process_instruction (io_program_data& io_data, program_context* context, const rx_time& now)
{
	uint32_t tick=rx_get_tick_count();
	begin(context);
	if(io_data.get_input(0) && !_prev_c_state)
	{// up so do the counting
		_started_tick=tick;
		_prev_r_state=true;
	}
	if(_prev_r_state)
	{// we are counting
		_value=(tick-_started_tick)/_base+1;
		if(_value>=_preset_val)
		{// done with counting
			_value=0;
			_prev_r_state=false;
		}
	}
	_prev_c_state=io_data.get_input(0);
	set_results(true,context);
	io_data.set_output(0,_prev_r_state);
}

void sl_monostabile::set_results (bool use_zero, program_context* context)
{
	// previous states had to be stored in context
	context->set_bit(mono_bits,_number,0,_prev_r_state);
	context->set_bit(mono_bits,_number,1,_prev_c_state);
	// register values
	context->set_register(mono_registers,_number*3,_value);
	context->set_register(mono_registers,_number*3+1,_preset_val);
	context->set_register(mono_registers,_number*3+2,_started_tick);
}

void sl_monostabile::begin (program_context* context)
{
	_preset_val=(uint32_t)_preset.get_value(context);
	// restore previous states
	_prev_r_state=context->get_bit(mono_bits,_number,0);
	_prev_c_state=context->get_bit(mono_bits,_number,1);
	_value=(uint32_t)context->get_register(mono_registers,_number*3);
	_started_tick=context->get_register(mono_registers,_number*3+2);
}


// Class sl_runtime::instructions_runtime::sl_timer_inst

sl_timer_inst::sl_timer_inst (dword number, const argument_value& preset, dword base)
      : _preset(preset),
        _number(number),
        _base(base),
        _value(0),
        _preset_val(0),
        _prev_c_state(false),
        _started_tick(0),
        _prev_r_state(false),
        _prev_d_state(false),
        _ellapsed(0)
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
		_value=0;
		_prev_d_state=false;
		_prev_r_state=false;
		_ellapsed=0;
	}
	else if(io_data.get_input(1) && !_prev_d_state)
	{// enable is and not done
		if(io_data.get_input(0))
		{// count is on so check our state
			if(!_prev_r_state)
			{// start counting
				_started_tick=tick;
				_prev_r_state=true;
			}
			else
			{// we were counting allready
				_value=(tick-_started_tick+_ellapsed)/_base+1;
				if(_value>=_preset_val)
				{// done with counting
					_value=_preset_val;
					_prev_r_state=false;
					_prev_d_state=true;
				}
			}
		}
		else
		{// counting is off
			if(_prev_r_state)
			{// we were counting
				_value=(tick-_started_tick+_ellapsed)/_base+1;
				if(_value>=_preset_val)
				{// done with counting
					_value=_preset_val;
					_prev_d_state=true;
				}
				else
				{// not done so reneber the ellapsed value
					_ellapsed+=(tick-_started_tick);
				}
			}
			_prev_r_state=false;// we're not counting
		}
	}
	_prev_c_state=io_data.get_input(0);
	set_results(true,context);
	io_data.set_output(0,_prev_d_state);
	io_data.set_output(1,_prev_r_state);
}

void sl_timer_inst::set_results (bool use_zero, program_context* context)
{
	context->set_bit(timer_bits,_number,0,_prev_r_state);
	context->set_bit(timer_bits,_number,1,_prev_d_state);
	context->set_bit(timer_bits,_number,2,_prev_c_state);
	// register values
	context->set_register(timer_registers,_number*4,_value);
	context->set_register(timer_registers,_number*4+1,_preset_val);
	context->set_register(timer_registers,_number*4+2,_started_tick);
	context->set_register(timer_registers,_number*4+3,_ellapsed);
}

void sl_timer_inst::begin (program_context* context)
{
	_preset_val=(uint32_t)_preset.get_value(context);
	_value=(uint32_t)context->get_register(timer_registers,_number*4);
	_started_tick=(uint32_t)context->get_register(timer_registers,_number*4+2);
	_ellapsed=(uint32_t)context->get_register(timer_registers,_number*4+3);
	// restore previous states
	_prev_r_state=context->get_bit(timer_bits,_number,0);
	_prev_d_state=context->get_bit(timer_bits,_number,1);
	_prev_c_state=context->get_bit(timer_bits,_number,2);
}


} // namespace instructions_runtime
} // namespace sl_runtime

