

/****************************************************************************
*
*  testing\rx_test_runtime.h
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


#ifndef rx_test_runtime_h
#define rx_test_runtime_h 1


#ifndef EXCLUDE_TEST_CODE

// rx_test
#include "testing/rx_test.h"

#include "runtime_internal/rx_subscription.h"


namespace testing {

namespace basic_tests {

namespace runtime_test {





class runtime_structure_test : public test_case  
{
	 DECLARE_REFERENCE_PTR(runtime_structure_test)
	 DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing runtime capabilities.");

  public:
      runtime_structure_test();

      ~runtime_structure_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};







class runtime_test_category : public test_category  
{

  public:
      runtime_test_category();

      ~runtime_test_category();


  protected:

  private:


};






class runtime_transaction_test : public test_case  
{
	DECLARE_REFERENCE_PTR(runtime_transaction_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing transaction model.");

  public:
      runtime_transaction_test();

      ~runtime_transaction_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


};






class runtime_connect_test : public test_case  
{
	DECLARE_REFERENCE_PTR(runtime_connect_test)
	DECLARE_TEST_CODE_INFO(0, 1, 0, "\
basic testing subscription connect/disconnect.");

	class internal_callback : public rx_internal::sys_runtime::subscriptions::rx_subscription_callback
	{
	public:
		runtime_connect_test* parent = nullptr;
		void items_changed(const std::vector<runtime::update_item>& items)
		{
			printf("\r\n**********Items changed fired!!!");
			for (const auto& one : items)
			{
				printf("Handle = %d  Value = %s\r\n", (int)one.handle, one.value.get_storage().to_string().c_str());
			}
		}
		void transaction_complete(runtime_transaction_id_t transaction_id, rx_result result, std::vector<runtime::update_item>&& items)
		{
			printf("\r\n**********Transaction complete fired!!!\r\n");
		}
        void write_completed(runtime_transaction_id_t transaction_id, std::vector<std::pair<runtime_handle_t, rx_result> > results)
        {
            printf("\r\n**********Write complete fired!!!\r\n");
        }
	};

  public:
      runtime_connect_test();

      ~runtime_connect_test();


      bool run_test (std::istream& in, std::ostream& out, std::ostream& err, test_program_context::smart_ptr ctx);


  protected:

  private:


      internal_callback callback_;


};


} // namespace runtime_test
} // namespace basic_tests
} // namespace testing

#endif //EXCLUDE_TEST_CODE


#endif
