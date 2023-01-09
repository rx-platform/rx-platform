

/****************************************************************************
*
*  runtime_internal\rx_filters.h
*
*  Copyright (c) 2020-2023 ENSACO Solutions doo
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


#ifndef rx_filters_h
#define rx_filters_h 1



// rx_blocks
#include "system/runtime/rx_blocks.h"



namespace rx_internal {

namespace sys_runtime {

namespace filters {
rx_result register_filter_constructors();





class linear_scaling_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of linear scaling for input and output.");

    DECLARE_REFERENCE_PTR(linear_scaling_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      runtime::local_value<double> hi_raw_;

      runtime::local_value<double> low_raw_;

      runtime::local_value<double> hi_eu_;

      runtime::local_value<double> low_eu_;


};






class cutoff_scaling : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of cutoff scaling for input and output.");

    DECLARE_REFERENCE_PTR(cutoff_scaling);

  public:
      cutoff_scaling (bool lo_cutoff);


      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      runtime::local_value<double> input_cutoff_;

      runtime::local_value<double> output_cutoff_;

      bool lo_cutoff_;


};






class limit_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of limit filter for input and output.");

    DECLARE_REFERENCE_PTR(limit_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      runtime::local_value<double> hi_input_;

      runtime::local_value<double> low_input_;

      runtime::local_value<double> hi_output_;

      runtime::local_value<double> low_output_;


};






class quality_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of quality filter that provides pure bit value.");

    DECLARE_REFERENCE_PTR(quality_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      bool supports_output () const;



      bool good_value_;


};






class ascii_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 1, 0, "\
Implementation of UTF8 to ASCII conversion.\r\n\
Conversion is always UTF8 to ASCII for both input and output");

    DECLARE_REFERENCE_PTR(ascii_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);

      string_type wrap (const string_type& what);



      char invalid_char_;

      uint8_t columns_;

      uint8_t max_len_;


};






class hex2decimal : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of hex to decimal string filter.\r\n\
Can treat empty string as 0.");

    DECLARE_REFERENCE_PTR(hex2decimal);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);



      bool empty_is_zero_;


};






class latch_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation latch filter.\r\n\
Supports both string and numeric values");

    DECLARE_REFERENCE_PTR(latch_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);

      rx_result start_filter (runtime::runtime_start_context& ctx);

      rx_result stop_filter (runtime::runtime_stop_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);



      runtime::local_value<bool> unlatch_;

      values::rx_value latched_;

      runtime::local_value<uint32_t> timeout_;

      rx_timer_ptr timer_;


};


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal



#endif
