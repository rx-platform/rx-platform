

/****************************************************************************
*
*  testing\rx_test_io.h
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


#ifndef rx_test_io_h
#define rx_test_io_h 1



// rx_io
#include "lib/rx_io.h"
// rx_test
#include "testing/rx_test.h"



namespace testing {

namespace io_test {

namespace ip_test {




class tcp_test_client : public rx::io::tcp_client_socket_std_buffer  
{
	DECLARE_REFERENCE_PTR(tcp_test_client);

  public:
      tcp_test_client();

      virtual ~tcp_test_client();


  protected:

      bool readed (const void* data, size_t count, rx_thread_handle_t destination);

      void release_buffer (buffer_ptr what);


  private:


};







class tcp_client_test : public code_test  
{

  public:
      tcp_client_test();

      virtual ~tcp_client_test();


      bool do_console_test (std::istream& in, std::ostream& out, std::ostream& err, server::prog::console_program_context::smart_ptr ctx);


  protected:

  private:


};


} // namespace ip_test
} // namespace io_test
} // namespace testing



#endif
