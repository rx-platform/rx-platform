

/****************************************************************************
*
*  runtime_internal\rx_filters.h
*
*  Copyright (c) 2020-2025 ENSACO Solutions doo
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
// rx_value_point
#include "runtime_internal/rx_value_point.h"

namespace rx_internal {
namespace sys_runtime {
namespace filters {
class calculation_filter;

} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal




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






class calcualtion_point : public data_source::value_point_impl  
{

  public:
      calcualtion_point (calculation_filter* my_filter);


  protected:

  private:

      void value_changed (const rx_value& val);



      calculation_filter *my_filter_;


};






class calculation_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of calculation filter.\r\n\
General expression filter.");

    DECLARE_REFERENCE_PTR(calculation_filter);

  public:
      calculation_filter();

      ~calculation_filter();


      rx_result initialize_filter (runtime::runtime_init_context& ctx);

      rx_result start_filter (runtime::runtime_start_context& ctx);

      rx_result stop_filter (runtime::runtime_stop_context& ctx);

      void value_changed (const rx_value& val, calcualtion_point* whose);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      rx_result filter_output (rx_simple_value& val);

      rx_result connect_input (const string_type& path);

      void disconnect_input ();

      rx_result connect_output (const string_type& path);

      void disconnect_output ();

      void input_value_changed (const rx_value& val);

      void output_value_changed (const rx_value& val);



      calcualtion_point input_point_;

      calcualtion_point output_point_;


      runtime::local_value<string_type> input_path_;

      runtime::local_value<string_type> output_path_;

      char token_buffer_[64];


};






class cumulative_speed_filter : public rx_platform::runtime::blocks::filter_runtime  
{
    DECLARE_CODE_INFO("rx", 1, 0, 0, "\
Implementation of cumulative speed filter.\r\n\
");

    DECLARE_REFERENCE_PTR(cumulative_speed_filter);

  public:

      rx_result initialize_filter (runtime::runtime_init_context& ctx);

      rx_result start_filter (runtime::runtime_start_context& ctx);

      rx_result stop_filter (runtime::runtime_stop_context& ctx);

      rx_result deinitialize_filter (runtime::runtime_deinit_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      bool supports_output () const;

      void timer_tick ();



      uint64_t last_value_;

      uint64_t current_value_;

      rx_timer_ticks_t last_tick_;

      rx_value my_value_;

      rx_timer_ptr timer_;

      runtime::local_value<uint32_t> period_;


};






class round_filter : public rx_platform::runtime::blocks::filter_runtime  
{

  public:
      round_filter ();


      rx_result initialize_filter (runtime::runtime_init_context& ctx);


  protected:

  private:

      rx_result filter_input (rx_value& val);

      bool supports_output () const;



      runtime::local_value<int> decimal_places_;

      double multiply_arg_;


};


} // namespace filters
} // namespace sys_runtime
} // namespace rx_internal



#endif
