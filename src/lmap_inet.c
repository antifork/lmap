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
#include <lmap_if.h>
#include <lmap_inet.h>

/* prototypes */
int ip_addr_init(struct ip_addr *sa, int type, char *addr);
const char *ip_addr_ntoa(struct ip_addr *sa, char *dst);
const char *ip_addr_details(struct ip_addr *sa, char *dst, size_t size);

static const char *inet_ntop4(const char *src, char *dst, size_t size);
static const char *inet_ntop6(const char *src, char *dst, size_t size);
static const char *inet_details6(const char *src, char *dst, size_t size);

/***********************************************************************/

int 
ip_addr_init(struct ip_addr *sa, int type, char *addr)
{
   sa->type = type;
   
   switch (type) {
      case AF_INET:
         sa->addr_size = IP_ADDR_LEN;
         break;
      case AF_INET6:
         sa->addr_size = IP6_ADDR_LEN;
         break;
      default:
         return EINVALID;
   }
   
   memcpy(&sa->addr, addr, sa->addr_size);
   
   return ESUCCESS;
};

const char *
ip_addr_ntoa(struct ip_addr *sa, char *dst)
{

   /* XXX make a check on dst length */
   
   switch (sa->type) {
      case AF_INET:
         inet_ntop4(sa->addr, dst, IP_ASCII_ADDR_LEN);
         return dst;
         break;
      case AF_INET6:
         inet_ntop6(sa->addr, dst, IP6_ASCII_ADDR_LEN);
         return dst;
         break;
   };
   
   return NULL;
}

const char * 
ip_addr_details(struct ip_addr *sa, char *dst, size_t size)
{
   switch (sa->type) {
      case AF_INET:
         /* DO NOTHING */
         break;
      case AF_INET6:
         inet_details6(sa->addr, dst, size);
         break;
   };

   return dst;
}
         
const char *
inet_ntop4(const char *src, char *dst, size_t size)
{
   char str[IP_ASCII_ADDR_LEN];
   int n;
   u_char *tmp = (u_char *)src;
   
	n = sprintf(str, "%u.%u.%u.%u", tmp[0], tmp[1], tmp[2], tmp[3]);
   
   str[n] = '\0';
 
   strlcpy(dst, str, size);
   
   return dst;
}

const char *
inet_ntop6(const char *src, char *dst, size_t size)
{
	/*
	 * Note that int32_t and int16_t need only be "at least" large enough
	 * to contain a value of the specified size.  On some systems, like
	 * Crays, there is no such thing as an integer variable with 16 bits.
	 * Keep this in mind if you think this function should have been coded
	 * to use pointer overlays.  All the world's not a VAX.
	 */
	char tmp[IP6_ASCII_ADDR_LEN], *tp;
	struct { int base, len; } best, cur;
	u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof words);
	for (i = 0; i < NS_IN6ADDRSZ; i += 2)
		words[i / 2] = (src[i] << 8) | src[i + 1];
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (inet_ntop4(src+12, tp, IP_ASCII_ADDR_LEN) != 0)
				return (NULL);
			tp += strlen(tp);
			break;
		}
		tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) == 
	    (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';

  	/*
	 * Check for overflow, copy, and we're done.
	 */
	if ((socklen_t)(tp - tmp) > size) {
		__set_errno (ENOSPC);
		return (NULL);
	}
   /* XXX use strlcpy instead */
	return strcpy(dst, tmp);
}

const char *inet_details6(const char *src, char *dst, size_t size)
{
   char tmp[IP6_DETAILS_SIZE];
   int n = 0;
   
   if (dst) {
      
      if ( IN6_IS_ADDR_UNSPECIFIED((SI6A *)src) )
         n = strlcpy(tmp, "unspec", size);
      if ( IN6_IS_ADDR_LOOPBACK((SI6A *)src) )
         n = strlcpy(tmp, "loopback", size);
      if ( IN6_IS_ADDR_MULTICAST((SI6A *)src) ) 
      {
         /* There may be insufficient space left
          * for multicast specific details to fit.
          * Therefore the string may result trucated.
          * It should never happen if you allocate at least
          * 16 chars for the details string.
          */
         size_t mcsiz = size - sizeof("multic ");
         
         strncpy(tmp, "multic ", size);
         
         if ( IN6_IS_ADDR_MC_NODELOCAL((SI6A *)src) )
            n += strlcpy(tmp+n, "nodeloc", mcsiz);
         if ( IN6_IS_ADDR_MC_LINKLOCAL((SI6A *)src) )
            n += strlcpy(tmp+n, "linkloc", mcsiz);
         if ( IN6_IS_ADDR_MC_SITELOCAL((SI6A *)src) )
            n += strlcpy(tmp+n, "siteloc", mcsiz);
         if ( IN6_IS_ADDR_MC_ORGLOCAL((SI6A *)src) )
            n += strlcpy(tmp+n, "orgloc", mcsiz);
         if ( IN6_IS_ADDR_MC_GLOBAL((SI6A *)src) )
            n += strlcpy(tmp+n, "global", mcsiz);
      }
         
      if ( IN6_IS_ADDR_LINKLOCAL((SI6A *)src) )
         n = strlcpy(tmp, "linkloc", size);
      if ( IN6_IS_ADDR_SITELOCAL((SI6A *)src) )
         n = strlcpy(tmp, "siteloc", size);
      if ( IN6_IS_ADDR_V4MAPPED((SI6A *)src) )
         n = strlcpy(tmp, "v4mapped", size);
      if ( IN6_IS_ADDR_V4COMPAT((SI6A *)src) )
         n = strlcpy(tmp, "v4compat", size);
   }

   tmp[n] = '\0';
   
   /* XXX use strlcpy instead */
   return strncpy(dst, tmp, size);
}

/* EOF */

// vim:ts=3:expandtab

