

/****************************************************************************
*
*  soft_logic\soft_plc.cpp
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


// soft_plc
#include "soft_logic/soft_plc.h"

//#include "svrmain.h"
#include "sl_inst.h"



namespace sl_runtime {
#define SPACE_PER_CHANEL	2
#define CHANELS_NUMBER		32
#define SPACE_PER_MODULE	(CHANELS_NUMBER*SPACE_PER_CHANEL)

// Class sl_runtime::sl_program

sl_program::sl_program()
{
}

sl_program::sl_program(const sl_program &right)
{
	RX_ASSERT(false);
}


sl_program::~sl_program()
{
}


sl_program & sl_program::operator=(const sl_program &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_program::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
}

void sl_program::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
}

void sl_program::process_program (program_context* context, const rx_time& now, bool debug)
{
	if(debug)
		context->process_watches(now);
}

void sl_program::load (FILE* file, dword version)
{
}


// Class sl_runtime::sl_module_base

sl_module_base::sl_module_base(const sl_module_base &right)
      : _order(0)
{
	RX_ASSERT(false);
}

sl_module_base::sl_module_base (int order)
      : _order(0)
{
	  _order=order;
}


sl_module_base::~sl_module_base()
{
	for(chanels_iterator it=_chanels.begin(); it!=_chanels.end(); it++)
		delete (*it);
}


sl_module_base & sl_module_base::operator=(const sl_module_base &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_module_base::process_inputs (program_context* context, const rx_time& now)
{
	for(chanels_iterator it=_chanels.begin(); it!=_chanels.end(); it++)
	{
		if((*it))
			(*it)->process_inputs(context,_order,now);
	}
}

void sl_module_base::process_outputs (program_context* context, const rx_time& now)
{
	for(chanels_iterator it=_chanels.begin(); it!=_chanels.end(); it++)
	{
		if((*it))
			(*it)->process_outputs(context,_order,now);
	}
}


// Class sl_runtime::sl_scheme

sl_scheme::sl_scheme(const sl_scheme &right)
{
	RX_ASSERT(false);
}

sl_scheme::sl_scheme (int width, int height)
{
	  _width=width;
	  _height=height;
	  init_runtime();
}


sl_scheme::~sl_scheme()
{
	for(int i=0; i<_width; i++)
	{
		for(int j=0; j<_height; j++)
		{
			if(_runtime_schema[i][j].instruction!=NULL)
				delete _runtime_schema[i][j].instruction;
		}
	}
}


sl_scheme & sl_scheme::operator=(const sl_scheme &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_scheme::process_schema (ladder_program_context* context, const rx_time& now)
{
	// initialize schema to initial state
	for(int i=0; i<_width; i++)
	{
		for(int j=0; j<_height; j++)
		{
			_runtime_schema[i][j].active_element=false;
			_runtime_schema[i][j].active_horizontal_end=false;
			_runtime_schema[i][j].active_horizontal_start=(i==0);
			_runtime_schema[i][j].active_vertical=false;
		}
	}
	// now go and iterate throught
	int i=0;
	while(i<_width)
	{
		int j=0;
		while(j<_height)
		{
			if(_runtime_schema[i][j].instruction)
			{
				sl_instruction_base* inst=_runtime_schema[i][j].instruction;
				// instruction processing
				laddder_io_program_data io_data(_runtime_schema,j,i,inst->get_inputs(),inst->get_outputs());
				inst->process_instruction(io_data,context,now);
				j+=inst->get_ladder_size();
			}
			else
			{
				// contact processing
				if(_runtime_schema[i][j].horizontal)
					_runtime_schema[i][j].active_horizontal_end=_runtime_schema[i][j].active_horizontal_start;
				j++;
			}
		}

		// vertical processing nad horizontal pass
		if(i<(_width-1))// no vertical processing for last line
		{
			j=0;
			int first_index=-1;
			while(j<_height)
			{
				if(_runtime_schema[i][j].vertical && first_index==-1)
				{// found start
					first_index=j;
				}
				else if(!_runtime_schema[i][j].vertical && first_index!=-1)
				{// this is stop
					bool on=false;
					for(int k=first_index; k<j+1; k++)
					{
						if(_runtime_schema[i][k].active_horizontal_end)
						{
							on=true;
							break;
						}
					}
					if(on)
					{
						for(int k=first_index; k<j+1;	k++)
						{
							if(k<j)
								_runtime_schema[i][k].active_vertical=true;
							_runtime_schema[i+1][k].active_horizontal_start=true;
						}
					}
					first_index=-1;
				}
				if(_runtime_schema[i][j].active_horizontal_end && _runtime_schema[i+1][j].horizontal)
					_runtime_schema[i+1][j].active_horizontal_start=true;
				j++;
			}
			if(first_index!=-1)
			{// vertical to the end
				bool on=false;
				for(int k=first_index; k<j; k++)
				{
					if(_runtime_schema[i][k].active_horizontal_end)
					{
						on=true;
						break;
					}
				}
				if(on)
				{
					for(int k=first_index; k<j;	k++)
					{
						_runtime_schema[i][k].active_vertical=true;
						_runtime_schema[i+1][k].active_horizontal_start=true;
					}
				}
			}

		}
		i++;
	}
}

void sl_scheme::load (FILE* file, dword version)
{
}

void sl_scheme::init_runtime ()
{
	for(int i=0; i<_width; i++)
	{
		col_type col;
		_runtime_schema.push_back(col);
		for(int j=0; j<_height; j++)
		{
			schema_elem elem;
			elem.active_element=false;
			elem.active_horizontal_end=false;
			elem.active_horizontal_start=false;
			elem.active_vertical=false;
			elem.horizontal=false;
			elem.instruction=NULL;
			elem.vertical=false;
			_runtime_schema[i].push_back(elem);
		}
	}
}

void sl_scheme::get_debug_data (std::vector<bool>& data)
{
	data.reserve(_width*_height*4);
	for(int i=0; i<_width; i++)
	{
		for(int j=0; j<_height; j++)
		{
			data.push_back(_runtime_schema[i][j].active_element);
			data.push_back(_runtime_schema[i][j].active_horizontal_start);
			data.push_back(_runtime_schema[i][j].active_horizontal_end);
			data.push_back(_runtime_schema[i][j].active_vertical);
		}
	}
}

void sl_scheme::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	for(int i=0; i<_width; i++)
	{
		for(int j=0; j<_height; j++)
		{
			if(_runtime_schema[i][j].instruction!=NULL)
			{
				_runtime_schema[i][j].instruction->initialize(holder,ctx,current_context);
			}
		}
	}
}

void sl_scheme::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	for(int i=0; i<_width; i++)
	{
		for(int j=0; j<_height; j++)
		{
			if(_runtime_schema[i][j].instruction!=NULL)
			{
				_runtime_schema[i][j].instruction->deinitialize(holder,ctx,current_context);
			}
		}
	}
}

void sl_scheme::set_vertical (int x, int y)
{
	_runtime_schema[x][y].vertical=true;
}

void sl_scheme::set_horizontal (int x, int y)
{
	_runtime_schema[x][y].horizontal=true;
}

void sl_scheme::set_instruction (int x, int y, sl_instruction_base* inst)
{
	_runtime_schema[x][y].instruction=inst;
}


// Class sl_runtime::sl_ladder_program

sl_ladder_program::sl_ladder_program()
{
}


sl_ladder_program::~sl_ladder_program()
{
	for(schemas_iterator it=_schemas.begin(); it!=_schemas.end(); it++)
		delete (*it);
}



void sl_ladder_program::process_program (program_context* context, const rx_time& now, bool debug)
{
	ladder_program_context* ladder_context=static_cast<ladder_program_context*>(context);
	int segment=-1;
	ladder_context->get_debug_segment(segment);
	bool done_dbg=false;
	int current_segment=0;
	for(schemas_iterator it=_schemas.begin(); it!=_schemas.end(); it++)
	{
		current_segment++;
		(*it)->process_schema(ladder_context,now);
		if(current_segment==segment && debug)
		{// go jhony go....
			std::vector<bool> dbg_data;
			(*it)->get_debug_data(dbg_data);
			ladder_context->set_debug_data(dbg_data,now,segment);
			done_dbg=true;
		}
	}
	if(!done_dbg  && debug)
	{
		std::vector<bool> dbg_data;
		ladder_context->set_debug_data(dbg_data,now,0);
	}
	sl_program::process_program(context,now,debug);
}

program_context* sl_ladder_program::create_program_context (program_context* parent_context, sl_program_holder* holder)
{
	  return new ladder_program_context(parent_context,holder);
}

void sl_ladder_program::load (FILE* file, dword version)
{
}

void sl_ladder_program::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	sl_program::initialize(holder,ctx,current_context);
	for(schemas_iterator it=_schemas.begin(); it!=_schemas.end(); it++)
	{
		(*it)->initialize(holder,ctx,current_context);
	}
}

void sl_ladder_program::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	sl_program::deinitialize(holder,ctx,current_context);
	for(schemas_iterator it=_schemas.begin(); it!=_schemas.end(); it++)
	{
		(*it)->deinitialize(holder,ctx,current_context);
	}
}

void sl_ladder_program::add_schema (sl_scheme* sch)
{
	_schemas.push_back(sch);
}


// Class sl_runtime::program_context

program_context::program_context(const program_context &right)
      : _watch_base(NULL),
        _dbg_segment(NULL),
        _wnd_id(-1),
        _temp_wnd_id(-1),
        _debug_segment(-1)
{
	RX_ASSERT(false);
}

program_context::program_context (program_context* parent, sl_program_holder* holder)
      : _watch_base(NULL),
        _dbg_segment(NULL),
        _wnd_id(-1),
        _temp_wnd_id(-1),
        _debug_segment(-1)
  ,_parent(parent)
  , _holder(holder)
{
}


program_context::~program_context()
{
}


program_context & program_context::operator=(const program_context &right)
{
	RX_ASSERT(false);
	return *this;
}



void program_context::initialize (initialize_context* ctx)
{
	if(_parent==NULL)
	{
		_watch_base=ctx->create_watch_debugger(this);
		_dbg_segment=ctx->create_segment_debugger(this);
	}
}

void program_context::deinitialize (deinitialize_context* ctx)
{
}

bool program_context::get_bit (adr_type type, dword reg, byte bit)
{
	address_space* addresses=get_address_space(type);
	return addresses->get_bit(PURE_TYPE(type),reg,bit);
}

void program_context::set_bit (adr_type type, dword reg, byte bit, bool val)
{
	address_space* addresses=get_address_space(type);
	addresses->set_bit(PURE_TYPE(type),reg,bit,val);
}

bool program_context::get_positive_transact (adr_type type, dword reg, byte bit)
{
	address_space* addresses=get_address_space(type);
	return addresses->get_positive_transact(PURE_TYPE(type),reg,bit);
}

bool program_context::get_negative_transact (adr_type type, dword reg, byte bit)
{
	address_space* addresses=get_address_space(type);
	return addresses->get_negative_transact(PURE_TYPE(type),reg,bit);
}

void program_context::init_scan ()
{
	  _addresses.init_scan();
}

dword program_context::get_register (adr_type type, dword reg)
{
	address_space* addresses=get_address_space(type);
	return addresses->get_register(PURE_TYPE(type),reg);
}

void program_context::set_register (adr_type type, dword reg, dword val)
{
	address_space* addresses=get_address_space(type);
	addresses->set_register(PURE_TYPE(type),reg,val);
}

void program_context::set_watch_window (int wnd_id, const std::vector<watch_addr_type>& data)
{
	_wnd_id=wnd_id;
	_watches.clear();
	_watches.insert(_watches.begin(),data.begin(),data.end());
}

void program_context::process_watches (const rx_time& now)
{
	if(_parent!=NULL)
		_parent->process_watches(now,this);
	else
		process_watches(now,this);
}

address_space* program_context::get_address_space (adr_type type)
{
	if(_parent!=NULL && (IS_MAIN(type) || type==system_registers))
	{// parent context
		return &_parent->_addresses;
	}
	else
	{// local context
		return &_addresses;
	}
}

sl_program_holder* program_context::get_program_holder ()
{
	return _holder;
}

void program_context::create_debug_data (sl_program_holder* holder)
{
}

bool program_context::set_debug_segment (int segment, const rx_uuid& guid)
{
	if(guid==_guid)
	{// this is ours
		_holder->set_debug_context(this);
		_debug_segment=segment;
		return true;// done it
	}
	else
	{// sub program find it
		program_context* context=_holder->get_subcontext(guid);
		if(context==NULL)
			return false;// sub program not found
		context->set_debug_segment(segment,guid);
		return true;
	}
}

void program_context::get_debug_segment (int& segment)
{
	segment=_debug_segment;
}

void program_context::set_debug_data (const std::vector<bool>& data, const rx_time& now, int segment)
{
	if(_parent==NULL)
	{// glavni program
		set_debug_data(data,now,segment,get_guid());
	}
	else// posalji glavnom contextu
		_parent->set_debug_data(data,now,segment,get_guid());
}

void program_context::set_debug_data (const std::vector<bool>& data, const rx_time& now, int segment, const rx_uuid& guid)
{
	if(_dbg_segment)
		_dbg_segment->set_debug_data(guid,data,now,segment);
}

void program_context::process_watches (const rx_time& now, program_context* read_from)
{
	std::vector<uint32_t> data;
	data.reserve(_watches.size());
	for(watches_iterator it=_watches.begin(); it!=_watches.end(); it++)
	{
		data.push_back(read_from->get_register(it->type,it->address));
	}
	if(_watch_base)
		_watch_base->set_debug_data(_wnd_id,data,now);
}

float program_context::get_float (adr_type type, dword reg)
{
	address_space* addresses=get_address_space(type);
	return addresses->get_float(PURE_TYPE(type),reg);
}

void program_context::set_float (adr_type type, dword reg, float val)
{
	address_space* addresses=get_address_space(type);
	addresses->set_float(PURE_TYPE(type),reg,val);
}

bool program_context::get_out_bit_written (dword reg, byte bit)
{
	address_space* addresses=get_address_space(output_bits);
	return addresses->get_out_bit_written(reg,bit);
}

bool program_context::get_out_register_written (dword reg)
{
	address_space* addresses=get_address_space(output_registers);
	return addresses->get_out_register_written(reg);
}


// Class sl_runtime::ladder_program_context

ladder_program_context::ladder_program_context (program_context* parent, sl_program_holder* holder)
  : program_context(parent,holder)
{
}


ladder_program_context::~ladder_program_context()
{
}



void ladder_program_context::initialize (initialize_context* ctx)
{
	program_context::initialize(ctx);

}

void ladder_program_context::deinitialize (deinitialize_context* ctx)
{
	program_context::deinitialize(ctx);
}


// Class sl_runtime::sl_programs_collection

sl_programs_collection::sl_programs_collection()
{
}

sl_programs_collection::sl_programs_collection(const sl_programs_collection &right)
{
	RX_ASSERT(false);
}


sl_programs_collection::~sl_programs_collection()
{
}


sl_programs_collection & sl_programs_collection::operator=(const sl_programs_collection &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_programs_collection::deinitialize (deinitialize_context* ctx)
{
	for(programs_iterator it=_programs.begin(); it!=_programs.end(); it++)
	{
		(*it)->deinitialize(ctx);
		delete (*it);
	}
	_programs.clear();
}

void sl_programs_collection::start_programs ()
{
	for(programs_iterator it=_programs.begin(); it!=_programs.end(); it++)
	{
		(*it)->start_program(get_executer(*it));
	}
}

void sl_programs_collection::stop_programs ()
{
	for(programs_iterator it=_programs.begin(); it!=_programs.end(); it++)
	{
		(*it)->stop_program();
	}
}

void sl_programs_collection::add_program (sl_program_holder* prog)
{
	_programs.push_back(prog);
}

program_executer* sl_programs_collection::get_executer (sl_program_holder* holder)
{
	  return NULL;
}


// Class sl_runtime::sl_di_module

sl_di_module::sl_di_module (int order)
  : sl_module_base(order)
{
}


sl_di_module::~sl_di_module()
{
}



void sl_di_module::create_chanels (module_chanels_factory* builder)
{
	for(int i=0; i<CHANELS_NUMBER; i++)
	{
		sl_di_chanel* ch=builder->create_di_chanel(i);
		_chanels.push_back(ch);
	}
}


// Class sl_runtime::sl_chanel_base

sl_chanel_base::sl_chanel_base(const sl_chanel_base &right)
      : _order(0)
{
	RX_ASSERT(false);
}

sl_chanel_base::sl_chanel_base (int order)
      : _order(0)
{
	_order=order;
}


sl_chanel_base::~sl_chanel_base()
{
}


sl_chanel_base & sl_chanel_base::operator=(const sl_chanel_base &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_chanel_base::process_inputs (program_context* context, int module, const rx_time& now)
{
}

void sl_chanel_base::process_outputs (program_context* context, int module, const rx_time& now)
{
}

void sl_chanel_base::set_input_bit (int num, int module, bool val, program_context* context)
{
	uint32_t address=(num+_order+module*SPACE_PER_MODULE);
	uint8_t bit=(uint8_t)(address%32);
	address=address/32;
	context->set_bit(input_bits,address,bit,val);
}

bool sl_chanel_base::get_output_bit (int num, int module, program_context* context)
{
	uint32_t address=(num+_order+module*SPACE_PER_MODULE);
	uint8_t bit=(uint8_t)(address%32);
	address=address/32;
	return context->get_bit(output_bits,address,bit);
}

uint32_t sl_chanel_base::get_output_register (int num, int module, program_context* context)
{
	uint32_t address=(num+_order+module*CHANELS_NUMBER);
	return context->get_register(output_registers,address);
}

void sl_chanel_base::set_input_register (int num, int module, uint32_t val, program_context* context)
{
	uint32_t address=(num+_order+module*CHANELS_NUMBER);
	context->set_register(input_registers,address,val);
}

float sl_chanel_base::get_output_float (int num, int module, program_context* context)
{
	uint32_t address=(num+_order+module*CHANELS_NUMBER);
	return context->get_float(output_registers,address);
}

void sl_chanel_base::set_input_float (int num, int module, float val, program_context* context)
{
	uint32_t address=(num+_order+module*CHANELS_NUMBER);
	context->set_float(input_registers,address,val);
}

bool sl_chanel_base::get_out_bit_written (int num, int module, program_context* context)
{
	uint32_t address=(num+_order+module*SPACE_PER_MODULE);
	uint8_t bit=(uint8_t)(address%32);
	address=address/32;
	return context->get_out_bit_written(address,bit);
}

bool sl_chanel_base::get_out_register_written (int num, int module, program_context* context)
{
	uint32_t address=(num+_order+module*CHANELS_NUMBER);
	return context->get_out_register_written(address);
}


// Class sl_runtime::sl_di_chanel

sl_di_chanel::sl_di_chanel (int order)
  : sl_chanel_base(order)
{
}


sl_di_chanel::~sl_di_chanel()
{
}



void sl_di_chanel::set_input_bit (bool value, int module, program_context* context)
{
	sl_chanel_base::set_input_bit(0,module,value,context);
}

void sl_di_chanel::set_input_status (bool status, int module, program_context* context)
{
	sl_chanel_base::set_input_bit(CHANELS_NUMBER,module,status,context);
}


// Class sl_runtime::program_executer

program_executer::program_executer (sl_program_holder* program)
      : _program(program)
{
}


program_executer::~program_executer()
{
}



void program_executer::program_scan ()
{
	_program->program_scan();
}


// Class sl_runtime::address_space

address_space::address_space()
{
	uint32_t mask=1;
	for(int i=0; i<(sizeof(and_masks)/sizeof(and_masks[0])); i++)
	{
		and_masks[i]=mask;
		or_masks[i]=(~mask);
		if(i<31)
			mask=mask<<1;
	}
	address_range_type range;
	_prev_values[input_bits]=range;
	_prev_values[output_bits]=range;
	_prev_values[memory_bits]=range;
}

address_space::address_space(const address_space &right)
{
	RX_ASSERT(false);
}


address_space::~address_space()
{
}


address_space & address_space::operator=(const address_space &right)
{
	RX_ASSERT(false);
	return *this;
}



bool address_space::get_bit (adr_type type, dword reg, byte bit)
{
	return (((_values[type][reg])&and_masks[bit])!=0);
}

void address_space::set_bit (adr_type type, dword reg, byte bit, bool val)
{
	uint32_t tmp=_values[type][reg];
	if(val)
	{
		tmp=tmp|and_masks[bit];
	}
	else
	{
		tmp=tmp&or_masks[bit];
	}
	// transactions stuff  here
	_values[type][reg]=tmp;
	if(type==output_bits)
	{
		uint32_t ch_tmp=_changed_values[type][reg];
		_changed_values[type][reg]=ch_tmp|and_masks[bit];
	}
}

bool address_space::get_positive_transact (adr_type type, dword reg, byte bit)
{
	uint32_t tmp=_values[type][reg];
	address_space_iterator it_prev=_prev_values.find(type);
	if(it_prev!=_prev_values.end())
	{
		address_range_iterator it_adr_prev=it_prev->second.find(reg);
		if(it_adr_prev!=it_prev->second.end())
		{
			return ((tmp^it_adr_prev->second)&(tmp&and_masks[bit]))!=0;
		}
	}
	return false;
}

bool address_space::get_negative_transact (adr_type type, dword reg, byte bit)
{
	uint32_t tmp=_values[type][reg];
	address_space_iterator it_prev=_prev_values.find(type);
	if(it_prev!=_prev_values.end())
	{
		address_range_iterator it_adr_prev=it_prev->second.find(reg);
		if(it_adr_prev!=it_prev->second.end())
		{
			return ((tmp^it_adr_prev->second)&(it_adr_prev->second&and_masks[bit]))!=0;
		}
	}
	return false;
}

void address_space::init_scan ()
{
	init_prevs(input_bits);
	init_prevs(output_bits);
	init_prevs(memory_bits);

	init_changes(input_registers);
	init_changes(output_registers);
}

void address_space::init_prevs (adr_type type)
{
	address_space_iterator it_prev=_prev_values.find(type);
	address_space_iterator it_curr=_values.find(type);
	if(it_curr!=_values.end())
	{
		for(address_range_iterator it_adr=it_curr->second.begin(); it_adr!=it_curr->second.end(); it_adr++)
		{
			address_range_iterator it_adr_prev=it_prev->second.find(it_adr->first);
			if(it_adr_prev!=it_prev->second.end())
				it_adr_prev->second=it_adr->second;
			else
				it_prev->second[it_adr->first]=it_adr->second;
		}
	}
}

dword address_space::get_register (adr_type type, dword reg)
{
	return _values[type][reg];
}

void address_space::set_register (adr_type type, dword reg, dword val)
{
	_values[type][reg]=val;
	if(type==output_bits || type==output_registers)
	{
		_changed_values[type][reg]=(uint32_t)(-1);
	}
}

float address_space::get_float (adr_type type, dword reg)
{
	uint32_t dval=_values[type][reg];
	return *((float*)&dval);
}

void address_space::set_float (adr_type type, dword reg, float val)
{
	uint32_t dval=*((uint32_t*)&val);
	_values[type][reg]=dval;
	if(type==output_bits || type==output_registers)
	{
		_changed_values[type][reg]=(uint32_t)(-1);
	}
}

bool address_space::get_out_bit_written (dword reg, byte bit)
{
	adr_type type=output_bits;
	address_space_iterator it_prev=_changed_values.find(type);
	if(it_prev!=_changed_values.end())
	{
		address_range_iterator it_adr_prev=it_prev->second.find(reg);
		if(it_adr_prev!=it_prev->second.end())
		{
			return ((it_adr_prev->second)&(and_masks[bit]))!=0;
		}
	}
	return false;
}

bool address_space::get_out_register_written (dword reg)
{
	adr_type type=output_registers;
	address_space_iterator it_prev=_changed_values.find(type);
	if(it_prev!=_changed_values.end())
	{
		address_range_iterator it_adr_prev=it_prev->second.find(reg);
		if(it_adr_prev!=it_prev->second.end())
		{
			return it_adr_prev->second!=0;
		}
	}
	return false;
}

void address_space::init_changes (adr_type type)
{
	for(address_space_iterator ita=_changed_values.begin(); ita!=_changed_values.end(); ita++)
	{
		for(address_range_iterator it=ita->second.begin(); it!=ita->second.end(); it++)
			it->second=0;
	}
}


// Class sl_runtime::sl_instruction_base

sl_instruction_base::sl_instruction_base()
{
}

sl_instruction_base::sl_instruction_base(const sl_instruction_base &right)
{
	RX_ASSERT(false);
}


sl_instruction_base::~sl_instruction_base()
{
}


sl_instruction_base & sl_instruction_base::operator=(const sl_instruction_base &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_instruction_base::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
}

void sl_instruction_base::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
}


// Class sl_runtime::sl_do_module

sl_do_module::sl_do_module (int order)
  : sl_module_base(order)
{
}


sl_do_module::~sl_do_module()
{
}



void sl_do_module::create_chanels (module_chanels_factory* builder)
{
	for(int i=0; i<CHANELS_NUMBER; i++)
	{
		sl_do_chanel* ch=builder->create_do_chanel(i);
		_chanels.push_back(ch);
	}
}


// Class sl_runtime::sl_do_chanel

sl_do_chanel::sl_do_chanel (int order)
  : sl_chanel_base(order)
{
}


sl_do_chanel::~sl_do_chanel()
{
}



bool sl_do_chanel::get_output_bit (int module, program_context* context)
{
	return sl_chanel_base::get_output_bit(0,module,context);
}

void sl_do_chanel::set_done_bit (bool value, int module, program_context* context)
{
	  sl_chanel_base::set_input_bit(0,module,value,context);
}

void sl_do_chanel::set_error_status (bool status, int module, program_context* context)
{
	  sl_chanel_base::set_input_bit(CHANELS_NUMBER,module,status,context);
}

bool sl_do_chanel::get_out_bit_written (int module, program_context* context)
{
	return sl_chanel_base::get_out_bit_written(0,module,context);
}


// Class sl_runtime::sl_program_holder

sl_program_holder::sl_program_holder()
      : _executer(NULL),
        _main(NULL),
        _rate(500),
        _name("NewProgram"),
        _debug_context(NULL),
        _first_scan(true)
{
}

sl_program_holder::sl_program_holder(const sl_program_holder &right)
      : _executer(NULL),
        _main(NULL),
        _rate(500),
        _name("NewProgram"),
        _debug_context(NULL),
        _first_scan(true)
{
	RX_ASSERT(false);
}


sl_program_holder::~sl_program_holder()
{
	for(modules_iterator it=_modules.begin(); it!=_modules.end(); it++)
		delete (*it);
	delete _main;
	delete _main_context;
	for(subprograms_iterator it=_subprograms.begin(); it!=_subprograms.end(); it++)
		delete it->second;
	for(subcontexts_iterator it=_subcontexts.begin(); it!=_subcontexts.end(); it++)
		delete it->second.context;
}


sl_program_holder & sl_program_holder::operator=(const sl_program_holder &right)
{
	RX_ASSERT(false);
	return *this;
}



void sl_program_holder::initialize (initialize_context* ctx)
{
	_main_context->initialize(ctx);
	_main->initialize(this,ctx,_main_context);
	for(custom_steps_iterator it=_custom_steps.begin(); it!=_custom_steps.end(); it++)
		(*it)->initialize(this,ctx);
}

void sl_program_holder::deinitialize (deinitialize_context* ctx)
{
	_main->deinitialize(this,ctx,_main_context);
	_main_context->deinitialize(ctx);

	for(custom_steps_iterator it=_custom_steps.begin(); it!=_custom_steps.end(); it++)
		(*it)->deinitialize();
	_custom_steps.clear();
}

void sl_program_holder::program_scan ()
{
	rx::rx_time now(rx::rx_time::now());
	_main_context->init_scan();

	// custom steps before process
	for(custom_steps_iterator it=_custom_steps.begin(); it!=_custom_steps.end(); it++)
		(*it)->process_step_before(_main_context,now);

	//////////////////////////////////////////////////////
	// processing program
	process_inputs(_main_context,now);
	process_program(_main_context,now,_debug_context==_main_context);
	process_outputs(_main_context,now);

	// custom steps after process
	for(custom_steps_iterator it=_custom_steps.begin(); it!=_custom_steps.end(); it++)
		(*it)->process_step_after(_main_context,now);

	// first scan handling
	if(_first_scan)
	{
		_first_scan=false;
		_main_context->set_bit(system_registers,0,0,false);
	}

}

void sl_program_holder::process_inputs (program_context* context, const rx_time& now)
{
	for(modules_iterator it=_modules.begin(); it!=_modules.end(); it++)
	{
		(*it)->process_inputs(context,now);
	}
}

void sl_program_holder::process_outputs (program_context* context, const rx_time& now)
{
	for(modules_iterator it=_modules.begin(); it!=_modules.end(); it++)
	{
		(*it)->process_outputs(context,now);
	}
}

void sl_program_holder::process_program (program_context* context, const rx_time& now, bool debug)
{
	_main->process_program(context,now,debug);
}

void sl_program_holder::load (FILE* file, dword version)
{
	// main program
/*
	_main->load(file,version);

	// rest of the sub programs
	int sub_count=0;
	READ_SINGLE(sub_count);
	for(int i=0; i<sub_count; i++)
	{
		sl_program* one=NULL;
		READ_SINGLE(type);
		switch(type)
		{
		case 1:
			one=new sl_ladder_program;
			break;
		default:
			throw new std::exception("wrong program type");
		}
		string_type name;
		READ_STRING(name);
		one->load(file,version);
		_subprograms[name]=one;
	}*/
}

void sl_program_holder::start_program (program_executer* executer)
{
	_executer=executer;
	_debug_context=_main_context;
	_main_context->set_bit(system_registers,0,0,true);
	_executer->start_program(_rate);
}

void sl_program_holder::stop_program ()
{
	if(_executer!=NULL)
	{
		_executer->stop_program();
		_executer->delete_executer();
		_executer=NULL;
	}
}

bool sl_program_holder::register_subcontext (instructions_runtime::subprogram_instruction* inst, const string_type& name, initialize_context* ctx, program_context* current_context)
{
	subprograms_iterator it=_subprograms.find(name);
	if(it!=_subprograms.end())
	{
		subcontext_data temp;
		temp.program=it->second;
		temp.context=it->second->create_program_context(current_context,this);
		temp.context->set_guid(inst->get_guid());
		_subcontexts[inst]=temp;
		_sub_hash[inst->get_guid()]=temp;

		temp.context->initialize(ctx);
		temp.program->initialize(this,ctx,temp.context);

		return true;
	}
	return false;
}

program_context* sl_program_holder::get_subcontext (sl_instruction_base* inst)
{
	subcontexts_iterator it=_subcontexts.find(inst);
	if(it!=_subcontexts.end())
		return it->second.context;
	else
		return NULL;
}

bool sl_program_holder::execute_sub (sl_instruction_base* inst, const rx_time& now)
{
	subcontexts_iterator it=_subcontexts.find(inst);
	if(it!=_subcontexts.end())
	{
		it->second.program->process_program(it->second.context,now,_debug_context==it->second.context);
		return true;
	}
	RX_ASSERT(false);
	return false;
}

program_context* sl_program_holder::get_subcontext (const rx_uuid& guid)
{
	subcontexts_hash_iterator it=_sub_hash.find(guid);
	if(it!=_sub_hash.end())
		return it->second.context;
	else
		return NULL;
}

void sl_program_holder::set_debug_context (program_context* context)
{
	  _debug_context=context;
}

void sl_program_holder::set_main_program (sl_program* main)
{
	_main=main;
	_main_context=_main->create_program_context(NULL,this);
}

void sl_program_holder::add_sub_program (const string_type& name, sl_program* prog)
{
	_subprograms[name]=prog;
}

void sl_program_holder::add_custom_step (custom_program_step* step)
{
	  _custom_steps.push_back(step);
}

void sl_program_holder::add_module (sl_module_base* module)
{
	  _modules.push_back(module);
}

program_context* sl_program_holder::get_main_context ()
{
	return _main_context;
}

void sl_program_holder::deinitialize_sub (sl_instruction_base* inst, deinitialize_context* ctx)
{
	subcontexts_iterator it=_subcontexts.find(inst);
	if(it!=_subcontexts.end())
	{
		it->second.program->deinitialize(this,ctx,it->second.context);
		it->second.context->deinitialize(ctx);
	}
}


// Class sl_runtime::argument_value

argument_value::argument_value()
{
}

argument_value::argument_value(const argument_value &right)
{
	_addr=right._addr;
	_type=right._type;
	_value=right._value;
}

argument_value::argument_value (float value)
{
	_type=const_float_value;
	*((float*)&_value)=value;
}

argument_value::argument_value (int value)
{
	_type=const_value;
	_value=value;
}

argument_value::argument_value (adr_type type, dword addr)
{
	_type=type;
	_addr=addr;
}


argument_value::~argument_value()
{
}


argument_value & argument_value::operator=(const argument_value &right)
{
	_addr=right._addr;
	_type=right._type;
	_value=right._value;
	return *this;
}



int argument_value::get_value (program_context* context)
{
	if(_type==const_value || _type==const_float_value)
		return _value;
	else
		return (int)context->get_register(_type,_addr);
}

float argument_value::get_float (program_context* context)
{
	if(_type==const_value || _type==const_float_value)
		return *((float*)&_value);
	else
		return context->get_float(_type,_addr);
}

bool argument_value::is_float ()
{
	return IS_FLOAT(_type);
}

double argument_value::get_double (program_context* context)
{
	return is_float() ? get_float(context) : get_value(context);
}


// Class sl_runtime::register_value

register_value::register_value()
{
}

register_value::register_value(const register_value &right)
{
	_addr=right._addr;
	_type=right._type;
}

register_value::register_value (adr_type type, dword addr)
{
	_addr=addr;
	_type=type;
}


register_value::~register_value()
{
}


register_value & register_value::operator=(const register_value &right)
{
	_addr=right._addr;
	_type=right._type;
	return *this;
}



dword register_value::get_value (program_context* context)
{

	return context->get_register(_type,_addr);
}

void register_value::set_value (program_context* context, dword val)
{
	  context->set_register(_type,_addr,val);
}

uint64_t register_value::make_cache_value ()
{
	  return _addr|(((uint64_t)_type)<<32);
}

float register_value::get_float (program_context* context)
{

	return context->get_float(_type,_addr);
}

void register_value::set_float (program_context* context, float val)
{

	context->set_float(_type,_addr,val);
}

bool register_value::is_float ()
{
	return IS_FLOAT(_type);
}

double register_value::get_double (program_context* context)
{
	return is_float() ? get_float(context) : get_value(context);
}

void register_value::set_double (program_context* context, double val)
{
	is_float() ? set_float(context,(float)val) : set_value(context,(int)val);
}


// Class sl_runtime::bit_value

bit_value::bit_value()
      : _addr(0),
        _bit(0)
{
}

bit_value::bit_value(const bit_value &right)
      : _addr(0),
        _bit(0)
{
	_bit=right._bit;
	_addr=right._addr;
	_type=right._type;
}

bit_value::bit_value (adr_type type, dword addr, byte bit)
      : _addr(0),
        _bit(0)
{
	_bit=bit;
	_addr=addr;
	_type=type;
}


bit_value::~bit_value()
{
}


bit_value & bit_value::operator=(const bit_value &right)
{
	_bit=right._bit;
	_addr=right._addr;
	_type=right._type;
	return *this;
}



bool bit_value::get_value (program_context* context) const
{
	return context->get_bit(_type,_addr,_bit);
}

void bit_value::set_value (program_context* context, bool val)
{
	context->set_bit(_type,_addr,_bit,val);
}

adr_type bit_value::get_type () const
{
	return _type;
}

dword bit_value::get_addr () const
{
	return _addr;
}

byte bit_value::get_bit () const
{
	return _bit;
}

bool bit_value::get_positive_transact (program_context* context) const
{
	return context->get_positive_transact(_type,_addr,_bit);
}

bool bit_value::get_negative_transact (program_context* context) const
{
	return context->get_negative_transact(_type,_addr,_bit);
}


// Class sl_runtime::sl_ai_module

sl_ai_module::sl_ai_module (int order)
  : sl_module_base(order)
{
}


sl_ai_module::~sl_ai_module()
{
}



void sl_ai_module::create_chanels (module_chanels_factory* builder)
{
	for(int i=0; i<CHANELS_NUMBER; i++)
	{
		sl_ai_chanel* ch=builder->create_ai_chanel(i);
		_chanels.push_back(ch);
	}
}


// Class sl_runtime::sl_ao_module

sl_ao_module::sl_ao_module (int order)
  : sl_module_base(order)
{
}


sl_ao_module::~sl_ao_module()
{
}



void sl_ao_module::create_chanels (module_chanels_factory* builder)
{
	for(int i=0; i<CHANELS_NUMBER; i++)
	{
		sl_ao_chanel* ch=builder->create_ao_chanel(i);
		_chanels.push_back(ch);
	}
}


// Class sl_runtime::sl_ai_chanel

sl_ai_chanel::sl_ai_chanel (int order)
  : sl_chanel_base(order)
{
}


sl_ai_chanel::~sl_ai_chanel()
{
}



void sl_ai_chanel::set_input_reg (dword value, int module, program_context* context)
{
	set_input_register(0,module,value,context);
}

void sl_ai_chanel::set_input_status (bool status, int module, program_context* context)
{
	set_input_bit(0,module,status,context);
}

void sl_ai_chanel::set_input_float (float value, int module, program_context* context)
{
	sl_chanel_base::set_input_float(0,module,value,context);
}


// Class sl_runtime::sl_ao_chanel

sl_ao_chanel::sl_ao_chanel (int order)
  : sl_chanel_base(order)
{
}


sl_ao_chanel::~sl_ao_chanel()
{
}



dword sl_ao_chanel::get_output_register (int module, program_context* context)
{
	return sl_chanel_base::get_output_register(0,module,context);
}

float sl_ao_chanel::get_output_float (int module, program_context* context)
{
	return sl_chanel_base::get_output_float(0,module,context);
}

bool sl_ao_chanel::get_out_register_written (int module, program_context* context)
{
	return sl_chanel_base::get_out_register_written(0,module,context);
}

void sl_ao_chanel::set_done_bit (bool value, int module, program_context* context)
{
	  sl_chanel_base::set_input_bit(0,module,value,context);
}

void sl_ao_chanel::set_error_status (bool status, int module, program_context* context)
{
	sl_chanel_base::set_input_bit(CHANELS_NUMBER,module,status,context);
}


// Class sl_runtime::watch_base

watch_base::watch_base(const watch_base &right)
{
	RX_ASSERT(false);
}

watch_base::watch_base (program_context* context)
	: _context(context)
{
}


watch_base::~watch_base()
{
}


watch_base & watch_base::operator=(const watch_base &right)
{
	RX_ASSERT(false);
	return *this;
}



program_context* watch_base::get_context ()
{
	  return _context;
}


// Class sl_runtime::debug_segment_base

debug_segment_base::debug_segment_base (program_context* context)
      : _context(context)
{
}


debug_segment_base::~debug_segment_base()
{
}



program_context* debug_segment_base::get_context ()
{
	return _context;
}


// Class sl_runtime::initialize_context

initialize_context::initialize_context()
{
}

initialize_context::initialize_context(const initialize_context &right)
{
	RX_ASSERT(false);
}


initialize_context::~initialize_context()
{
}


initialize_context & initialize_context::operator=(const initialize_context &right)
{
	RX_ASSERT(false);
	return *this;
}



// Class sl_runtime::custom_program_step

custom_program_step::custom_program_step()
{
}

custom_program_step::custom_program_step(const custom_program_step &right)
{
	RX_ASSERT(false);
}


custom_program_step::~custom_program_step()
{
}


custom_program_step & custom_program_step::operator=(const custom_program_step &right)
{
	RX_ASSERT(false);
	return *this;
}



void custom_program_step::process_step_before (program_context* context, const rx_time& now)
{
}

void custom_program_step::process_step_after (program_context* context, const rx_time& now)
{
}

void custom_program_step::initialize (sl_program_holder* holder, initialize_context* ctx)
{
}

void custom_program_step::deinitialize ()
{
}


// Class sl_runtime::deinitialize_context

deinitialize_context::deinitialize_context()
{
}


deinitialize_context::~deinitialize_context()
{
}



// Class sl_runtime::io_program_data

io_program_data::io_program_data()
{
}


io_program_data::~io_program_data()
{
}



// Class sl_runtime::laddder_io_program_data

laddder_io_program_data::laddder_io_program_data (schema_type& schema, int& row, int& col, int inputs, int outputs)
      : _schema(schema),
        _row(row),
        _col(col),
        _inputs(inputs),
        _outputs(outputs)
{
}


laddder_io_program_data::~laddder_io_program_data()
{
}



bool laddder_io_program_data::get_input (int idx)
{
	RX_ASSERT(idx<_inputs);
	if(idx<_inputs)
		return _schema[_col][_row+idx].active_horizontal_start;
	else
		return false;
}

void laddder_io_program_data::set_output (int idx, bool val)
{
	RX_ASSERT(idx<_outputs);
	if(idx<_outputs)
		_schema[_col][_row+idx].active_horizontal_end=val;
}

void laddder_io_program_data::set_instruction_active (bool val)
{
	_schema[_col][_row].active_element=val;
}


namespace builders {

// Class sl_runtime::builders::program_holder_bulder

program_holder_bulder::program_holder_bulder(const program_holder_bulder &right)
{
	RX_ASSERT(false);
}

program_holder_bulder::program_holder_bulder (module_chanels_factory* chanels_factory)
  : _chanels_factory(chanels_factory)
{
}


program_holder_bulder::~program_holder_bulder()
{
	delete _chanels_factory;
}


program_holder_bulder & program_holder_bulder::operator=(const program_holder_bulder &right)
{
	RX_ASSERT(false);
	return *this;
}



sl_program_holder* program_holder_bulder::build_program_holder ()
{
	sl_program_holder* ret=new sl_program_holder;
	// build modules
	sl_module_base* module=NULL;
	int order=1;
	while(module=get_next_module(order))
	{
		module->create_chanels(_chanels_factory);
		ret->add_module(module);
		order++;
	}
	program_builder* main_builder=get_main_program_builder();
	ret->set_main_program(main_builder->build_program());
	delete main_builder;
	program_builder* sub_builder=NULL;
	string_type sub_name;
	while(sub_builder=get_sub_program_builder(sub_name))
	{
		sl_program* sub=sub_builder->build_program();
		ret->add_sub_program(sub_name,sub);
		delete sub_builder;
	}
	return ret;
}


// Class sl_runtime::builders::module_chanels_factory

module_chanels_factory::module_chanels_factory()
{
}

module_chanels_factory::module_chanels_factory(const module_chanels_factory &right)
{
	RX_ASSERT(false);
}


module_chanels_factory::~module_chanels_factory()
{
}


module_chanels_factory & module_chanels_factory::operator=(const module_chanels_factory &right)
{
	RX_ASSERT(false);
	return *this;
}



// Class sl_runtime::builders::program_builder

program_builder::program_builder()
{
}

program_builder::program_builder(const program_builder &right)
{
	RX_ASSERT(false);
}


program_builder::~program_builder()
{
}


program_builder & program_builder::operator=(const program_builder &right)
{
	RX_ASSERT(false);
	return *this;
}



// Class sl_runtime::builders::ladder_program_builder

ladder_program_builder::ladder_program_builder()
{
}


ladder_program_builder::~ladder_program_builder()
{
}



sl_program* ladder_program_builder::build_program ()
{
	sl_ladder_program* ret=new sl_ladder_program;
	sl_scheme* scheme=NULL;
	while(scheme=get_next_schema())
	{
		ret->add_schema(scheme);
	}
	return ret;
}


} // namespace builders
} // namespace sl_runtime

