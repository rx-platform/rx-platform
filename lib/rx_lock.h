

/****************************************************************************
*
*  lib\rx_lock.h
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


#ifndef rx_lock_h
#define rx_lock_h 1






namespace rx {

namespace locks {


//	basic class that implements slim lock mechanism that is
//	reenternat, has no virtual constructor so it is smaller



class slim_lock 
{

  public:
      slim_lock();

      ~slim_lock();


      void lock ();

      void unlock ();

      slim_lock(slim_lock&&) = delete;
      slim_lock(const slim_lock&) = delete;
  protected:

  private:


      slim_lock_t slim_lock_;


};



//	basic class that implements slim lock mechanism that is
//	reenternat, has virtual constructor so can be inherited
//	from



class lockable 
{

  public:
      lockable();

      ~lockable();


      void lock ();

      void unlock ();


  protected:

  private:


      slim_lock_t slim_lock_;


};






template <class T>
class auto_lock_t 
{

  public:
      auto_lock_t (T* who);

      ~auto_lock_t();


  protected:

  private:


      T* p_;


};







typedef auto_lock_t< lockable  > auto_lock;







typedef auto_lock_t< slim_lock  > auto_slim_lock;






class waitable 
{

  public:
      waitable();

      virtual ~waitable();


      uint32_t wait_handle (uint32_t timeout = RX_INFINITE);

      uint32_t wait_handle_us (uint64_t timeout = RX_INFINITE);

      sys_handle_t get_handle () const;


  protected:

      sys_handle_t handle_;


  private:
      waitable(const waitable &right);

      waitable & operator=(const waitable &right);



};






class event : public waitable  
{

  public:
      event (bool initial);

      ~event();


      void set ();


  protected:

  private:


};



//	empty lock acctualy does nothing



class empty_slim_lock 
{

  public:
      empty_slim_lock();

      ~empty_slim_lock();


      void lock ();

      void unlock ();


  protected:

  private:


};







typedef auto_lock_t< empty_slim_lock  > auto_no_lock;






template <class T>
class const_auto_lock_t 
{

  public:
      const_auto_lock_t (const T* who);

      ~const_auto_lock_t();


  protected:

  private:


      T* p_;


};







typedef const_auto_lock_t< empty_slim_lock  > const_auto_no_lock;







typedef const_auto_lock_t< slim_lock  > const_auto_slim_lock;







typedef auto_lock_t< empty_slim_lock  > auto_empty_slim_lock;







typedef const_auto_lock_t< lockable  > const_auto_lock;


// Parameterized Class rx::locks::auto_lock_t 

template <class T>
auto_lock_t<T>::auto_lock_t (T* who)
  : p_(who)
{
		p_->lock();
}


template <class T>
auto_lock_t<T>::~auto_lock_t()
{
	p_->unlock();
}



// Parameterized Class rx::locks::const_auto_lock_t 

template <class T>
const_auto_lock_t<T>::const_auto_lock_t (const T* who)
	: p_(const_cast<T*>(who))
{
}


template <class T>
const_auto_lock_t<T>::~const_auto_lock_t()
{
}



} // namespace locks
} // namespace rx



#endif
