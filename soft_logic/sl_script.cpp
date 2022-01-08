

/****************************************************************************
*
*  soft_logic\sl_script.cpp
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


#include "pch.h"


// sl_script
#include "soft_logic/sl_script.h"

#include "terminal/ansi_codes.h"
#define RX_ANSI_PROGRAM_LINE ANSI_COLOR_YELLOW ANSI_COLOR_BOLD "{"
#define RX_ANSI_PROGRAM_LINE_END "}" ANSI_COLOR_RESET


namespace sl_runtime {

namespace sl_script {

// Class sl_runtime::sl_script::script_program_context 

script_program_context::script_program_context (program_context* parent, sl_program_holder* holder)
      : current_line_(0),
        waiting_(false),
        error_(false)
	, program_context(parent, holder)
{
}


script_program_context::~script_program_context()
{
}



void script_program_context::initialize (initialize_context* ctx)
{
	program_context::initialize(ctx);
}

void script_program_context::deinitialize (deinitialize_context* ctx)
{
	program_context::deinitialize(ctx);
}

void script_program_context::raise_error ()
{
	if (!error_)
	{
		reset_waiting();
		error_ = true;
		get_stderr() << "\r\nError in line " RX_ANSI_PROGRAM_LINE << current_line_ + 1 << RX_ANSI_PROGRAM_LINE_END "\r\n";
	}
	else
	{
		// this should not be happening
		// anyway just do the waiting stuff as code,
		// as bad as it is, depends on it
		RX_ASSERT(false);
		reset_waiting();
	}
}

bool script_program_context::get_result () const
{
    return !error_;
}

void script_program_context::set_waiting ()
{
	waiting_ = true;
}

void script_program_context::reset_waiting ()
{
	waiting_ = false;
}

size_t script_program_context::next_line ()
{
	current_line_++;
	return current_line_;
}

void script_program_context::continue_scan ()
{
	waiting_ = false;
	program_context::continue_scan();
}

void script_program_context::init_scan ()
{
	current_line_ = 0;
	error_ = false;
	program_context::init_scan();
}


const size_t script_program_context::get_current_line () const
{
  return current_line_;
}


// Class sl_runtime::sl_script::sl_script_program 

sl_script_program::sl_script_program()
{
}


sl_script_program::~sl_script_program()
{
}



void sl_script_program::process_program (program_context* context, const rx_time& now, bool debug)
{
	script_program_context* ctx = static_cast<script_program_context*>(context);
	size_t total_lines = lines_.size();
	size_t current_line = ctx->get_current_line();
	string_type label;

	std::ostream& out(ctx->get_stdout());
	std::ostream& err(ctx->get_stderr());

	while (!ctx->waiting_ && ctx->get_current_line() < total_lines)
	{
		if (!parse_line(lines_[current_line], out, err, context))
		{
			ctx->raise_error();
			break;
		}
		else
		{
			ctx->next_line();
		}
	}
	ctx->send_results(ctx->get_result(), !ctx->waiting_);
}

void sl_script_program::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	sl_program::initialize(holder, ctx, current_context);
}

void sl_script_program::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	sl_program::deinitialize(holder, ctx, current_context);
}

void sl_script_program::load (const string_type& lines)
{
	size_t count = lines.size();
	size_t idx1 = 0;
	size_t idx2 = 0;
	while (idx2<count && idx2 != string_type::npos)
	{
		idx1 = idx2;
		idx2 = lines.find_first_of("\r\n", idx1);
		if (idx2 != string_type::npos)
		{			
			if(idx2>idx1+1)
				lines_.emplace_back(lines.substr(idx1, (idx2 - idx1)));
			idx2++;
		}
		
	}
	if (idx1 == 0 && idx2 == string_type::npos)
		lines_.emplace_back(lines);
	else if (idx2 != string_type::npos && idx2 + 1 < count)
		lines_.emplace_back(lines.substr(idx2 + 1));
}

void sl_script_program::clear ()
{
    lines_.clear();
}


} // namespace sl_script
} // namespace sl_runtime

