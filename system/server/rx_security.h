
//	  %X% %Q% %Z% %W%



#ifndef rx_security_h
#define rx_security_h 1



// rx_lib
#include "lib/rx_lib.h"



namespace server {

namespace security {





class security_context : public rx::pointers::reference_object  
{

  public:
      security_context();

      virtual ~security_context();


      virtual const string_type& get_user_name () const = 0;


  protected:

  private:


};







class security_aware_object : public rx::pointers::virtual_reference_object  
{

  public:
      security_aware_object();

      security_aware_object(const security_aware_object &right);

      virtual ~security_aware_object();

      security_aware_object & operator=(const security_aware_object &right);


  protected:

  private:


};


} // namespace security
} // namespace server



#endif
