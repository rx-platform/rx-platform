

/****************************************************************************
*
*  lib\rx_crypto.h
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


#ifndef rx_crypto_h
#define rx_crypto_h 1



// rx_ptr
#include "lib/rx_ptr.h"



namespace rx {

namespace crypto {





class cryptography_key : public pointers::reference_object  
{

  public:
      cryptography_key();

      ~cryptography_key();


      bool is_valid () const;


  protected:

      crypt_key_t hkey_;


  private:


};






class cryptography_hash : public pointers::reference_object  
{

  public:
      cryptography_hash();

      ~cryptography_hash();


      bool is_valid () const;


  protected:

      crypt_hash_t hhash_;


  private:


};






class symetric_key : public cryptography_key  
{

  public:
      symetric_key (const void* data, size_t size, int alg, int mode);

      ~symetric_key();


  protected:

  private:


};


} // namespace crypto
} // namespace rx



#endif
