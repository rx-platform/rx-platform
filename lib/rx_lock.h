

/****************************************************************************
*
*  lib\rx_lock.h
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


  protected:

  private:


      slim_lock_t _slim_lock;


};



//	basic class that implements slim lock mechanism that is
//	reenternat, has virtual constructor so can be inherited
//	from



class lockable 
{

  public:
      lockable();

      virtual ~lockable();


      void lock ();

      void unlock ();


  protected:

  private:


      slim_lock_t _slim_lock;


};






template <class T>
class auto_lock_t 
{

  public:
      auto_lock_t (T* who);

      ~auto_lock_t();


  protected:

  private:


      T* _p;


};







typedef auto_lock_t< lockable  > auto_lock;







typedef auto_lock_t< slim_lock  > auto_slim_lock;






class waitable 
{

  public:
      waitable();

      virtual ~waitable();


      uint32_t wait_handle (uint32_t timeout = RX_INFINITE);


  protected:

      sys_handle_t _handle;


  private:
      waitable(const waitable &right);

      waitable & operator=(const waitable &right);



};






class event : public waitable  
{

  public:
      event (bool initial);

      virtual ~event();


      void set ();


  protected:

  private:


};



//	empty lock acctualy does nothing



class empty_slim_lock 
{

  public:
      empty_slim_lock();

      virtual ~empty_slim_lock();


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

      virtual ~const_auto_lock_t();


  protected:

  private:


      T* _p;


};







typedef const_auto_lock_t< empty_slim_lock  > const_auto_no_lock;







typedef const_auto_lock_t< slim_lock  > const_auto_slim_lock;







typedef auto_lock_t< empty_slim_lock  > auto_empty_slim_lock;







typedef const_auto_lock_t< lockable  > const_auto_lock;


// Parameterized Class rx::locks::auto_lock_t 

template <class T>
auto_lock_t<T>::auto_lock_t (T* who)
  : _p(who)
{
		_p->lock();
}


template <class T>
auto_lock_t<T>::~auto_lock_t()
{
	_p->unlock();
}



// Parameterized Class rx::locks::const_auto_lock_t 

template <class T>
const_auto_lock_t<T>::const_auto_lock_t (const T* who)
	: _p(const_cast<T*>(who))
{
}


template <class T>
const_auto_lock_t<T>::~const_auto_lock_t()
{
}



} // namespace locks
} // namespace rx



#endif
