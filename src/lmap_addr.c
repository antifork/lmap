/*
    lmap -- IP address management

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header$
*/

#include <lmap.h>
#include <lmap_addr.h>
#include <lmap_inet.h>
#include <lmap_inet6.h>

/* prototypes */
int init_addr(struct ip_addr *sa, int type, size_t size, char *addr);
const char *print_addr(struct ip_addr *sa, char *buf, size_t size);
int get_addr_details(struct ip_addr *sa, char *buf, size_t size);

/* returns EINVALID if sa is NULL */
/* type is AF_INET or AF_INET6 */
int 
init_addr(struct ip_addr *sa, int type, size_t size, char *addr)
{
   sa->type = type;
   sa->addr_size = size;
   memcpy(&sa->addr, addr, size);
};

const char *
print_addr(struct ip_addr *sa, char *buf, size_t size)
{
   switch (sa->type) {
      case AF_INET:
         inet_ntop4(sa->addr, buf, size);
         break;
      case AF_INET6:
         inet_ntop6(sa->addr, buf, size);
         break;
   };
   
   return buf;
}

int 
get_addr_details(struct ip_addr *sa, char *buf, size_t size)
{
   switch (sa->type) {
      case AF_INET:
         break;
      case AF_INET6:
         inet_details6(sa->addr, buf, size);
         break;
   };

   return ESUCCESS;
}
         
/* EOF */

// vim:ts=3:expandtab

