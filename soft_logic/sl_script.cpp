

/****************************************************************************
*
*  soft_logic\sl_script.cpp
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


#include "pch.h"


// sl_script
#include "soft_logic/sl_script.h"



namespace sl_runtime {

namespace sl_script {

// Class sl_runtime::sl_script::script_program_context 

script_program_context::script_program_context (program_context* parent, sl_program_holder* holder, std::streambuf* out_buffer, std::streambuf* error_buffer)
      : current_line_(0),
        out_std_(out_buffer),
        err_std_(error_buffer),
        run_again_(true),
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

size_t script_program_context::next_line ()
{
	current_line_++;
	return current_line_;
}

std::ostream& script_program_context::get_stdout ()
{
	return out_std_;
}

std::ostream& script_program_context::get_stderr ()
{
	return err_std_;
}

bool script_program_context::should_run_again ()
{
	if (!run_again_)
	{
		run_again_ = true;
		return false;
	}
	else
	{
		return !error_;
	}
}

void script_program_context::raise_error ()
{
	error_ = true;
}

void script_program_context::stop_execution ()
{
	run_again_ = false;
}

bool script_program_context::get_result () const
{
    return !error_;
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

	while (ctx->should_run_again() && ctx->get_current_line() < total_lines)
	{
		if (!parse_line(lines_[current_line], out, err, context))
			ctx->raise_error();
	}
}

void sl_script_program::load (FILE* file, dword version)
{
}

void sl_script_program::initialize (sl_program_holder* holder, initialize_context* ctx, program_context* current_context)
{
	sl_program::initialize(holder, ctx, current_context);
}

void sl_script_program::deinitialize (sl_program_holder* holder, deinitialize_context* ctx, program_context* current_context)
{
	sl_program::deinitialize(holder, ctx, current_context);
}

void sl_script_program::load (const string_type& file, dword version)
{
}

void sl_script_program::load (string_type&& file, dword version)
{
}

void sl_script_program::parse (const string_type& file, dword version)
{
}

void sl_script_program::parse (string_type&& file, dword version)
{
}

void sl_script_program::load (string_type&& line)
{
	lines_.emplace_back(std::move(line));
}

void sl_script_program::load (const string_type& line)
{
	lines_.emplace_back(line);
}

void sl_script_program::clear ()
{
    lines_.clear();
}


} // namespace sl_script
} // namespace sl_runtime

