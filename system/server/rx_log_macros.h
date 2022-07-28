

/****************************************************************************
*
*  system\server\rx_log_macros.h
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


#ifndef rx_log_macros_h
#define rx_log_macros_h 1




//////////////////////////////////////////////////////////////////////////////
// log macros defined here

#define RX_LOG_CONFIG_NAME "log"
#define RX_LOG_SELF_PRIORITY 9999
#define RELEASE_CODE_INFO ""

/* if*/
#ifdef RX_LOG_SOURCE_INFO

#define LOG_CODE_INFO ___code_name.c_str()
#define LOG_CODE_PREFIX { std::string ___code_name(_rx_func_);___code_name+="@" __FILE__"\r\n";
#define LOG_CODE_POSTFIX  ; }

#else
#define LOG_CODE_INFO RELEASE_CODE_INFO
#define LOG_CODE_PREFIX 
#define LOG_CODE_POSTFIX
#endif


#define RX_LOG_TEST(msg,cb)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::trace, RX_LOG_CONFIG_NAME, RX_LOG_CONFIG_NAME, 0,LOG_CODE_INFO,cb,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_INFO(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::info, lib, src, lvl,LOG_CODE_INFO,nullptr,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_WARNING(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::warning, lib, src, lvl,LOG_CODE_INFO,nullptr,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_ERROR(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::error, lib, src, lvl,LOG_CODE_INFO,nullptr,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_CRITICAL(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::critical, lib, src, lvl,LOG_CODE_INFO,nullptr,(msg))\
 LOG_CODE_POSTFIX
#define RX_LOG_DEBUG(lib,src,lvl,msg)\
 if(rx_is_debug_instance()) {\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::debug, lib, src, lvl,LOG_CODE_INFO,nullptr, (msg)\
) LOG_CODE_POSTFIX;\
}
#define RX_TRACE(lib,src,lvl,msg)\
 LOG_CODE_PREFIX\
 rx_platform::log::log_object::instance().log_event_fast(rx_platform::log::log_event_type::trace, lib, src, lvl,LOG_CODE_INFO,nullptr,(msg))\
 LOG_CODE_POSTFIX
//////////////////////////////////////////////////////////////////////////////




#endif
