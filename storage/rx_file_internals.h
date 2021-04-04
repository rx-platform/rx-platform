

/****************************************************************************
*
*  storage\rx_file_internals.h
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


#ifndef rx_file_internals_h
#define rx_file_internals_h 1






namespace storage {

namespace files {





class rx_json_file 
{
public:
    static constexpr bool string_based = true;

  public:

      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read (const string_type& data, const string_type& file_path);

      rx_result open_for_write (const string_type& file_path);

      rx_result close_read (const string_type& file_path);

      rx_result close_write (const string_type& file_path);

      rx_result get_data (string_type& data);


  protected:

  private:


      std::unique_ptr<rx_platform::serialization::json_reader> reader_;

      std::unique_ptr<rx_platform::serialization::json_writer> writer_;


};






class rx_binary_file 
{
public:
    static constexpr bool string_based = false;

  public:

      base_meta_reader& read_stream ();

      base_meta_writer& write_stream ();

      rx_result open_for_read (const string_type& file_path);

      rx_result open_for_write (const string_type& file_path);

      rx_result close_read (const string_type& file_path);

      rx_result close_write (const string_type& file_path);

      rx_result get_data (string_type& data);


  protected:

  private:


      std::unique_ptr<rx_platform::serialization::std_buffer_reader> reader_;

      std::unique_ptr<rx_platform::serialization::std_buffer_writer> writer_;


};






class rx_runtime_file 
{

  public:

      rx_result open (const string_type& file_name);


  protected:

  private:


};






class rx_simple_file 
{

  public:

      rx_result open_for_read (const string_type& file_path);

      rx_result open_for_write (const string_type& file_path);

      rx_result read_string (string_type& buff);

      rx_result write_string (const string_type& buff);


  protected:

  private:


};


} // namespace files
} // namespace storage



#endif
