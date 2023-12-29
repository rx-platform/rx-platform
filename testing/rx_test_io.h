

/****************************************************************************
*
*  testing\rx_test_io.h
*
*  Copyright (c) 2020-2024 ENSACO Solutions doo
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


#ifndef rx_test_io_h
#define rx_test_io_h 1


#ifndef EXCLUDE_TEST_CODE

// rx_stream_io
#include "interfaces/rx_stream_io.h"
// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace io_test {

namespace ip_test {




class tcp_test_client : public rx_internal::interfaces::ip_endpoints::tcp_client_socket_std_buffer  
{
	DECLARE_REFERENCE_PTR(tcp_test_client);

  public:
      tcp_test_client();

      ~tcp_test_client();


      bool connect_complete (sockaddr_in* addr, sockaddr_in* local_addr);


  protected:

      bool readed (const void* data, size_t count, rx_thread_handle_t destination);

      void release_buffer (buffer_ptr what);


  private:


};







class test_client_basics : public test_case  
{
	DECLARE_REFERENCE_PTR(test_client_basics)
	DECLARE_TEST_CODE_INFO(0,1,0, "\
loading a file->reading it line by line->executing.");

  public:
      test_client_basics();

      ~test_client_basics();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};







class tcp_test : public test_category  
{

  public:
      tcp_test();

      ~tcp_test();


  protected:

  private:


};


} // namespace ip_test
} // namespace io_test
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
