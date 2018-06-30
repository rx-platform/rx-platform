

/****************************************************************************
*
*  interfaces\rx_interfaces.h
*
*  Copyright (c) 2018 Dusan Ciric
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


#ifndef rx_interfaces_h
#define rx_interfaces_h 1






namespace interfaces {





class rx_ip_address 
{

  public:
      rx_ip_address();

      rx_ip_address(const rx_ip_address &right);

      virtual ~rx_ip_address();

      rx_ip_address & operator=(const rx_ip_address &right);

	  rx_ip_address & operator=(rx_ip_address &&right);

  protected:

  private:


};






class rx_ethernet_card 
{
	typedef std::vector<rx_ip_address> rx_ip_address;

  public:
      rx_ethernet_card();

      virtual ~rx_ethernet_card();

	  rx_ethernet_card & operator=(rx_ethernet_card &&right);

  protected:

  private:
      rx_ethernet_card(const rx_ethernet_card &right);

      rx_ethernet_card & operator=(const rx_ethernet_card &right);



      rx_ip_address *addresses_;


};


} // namespace interfaces



#endif
