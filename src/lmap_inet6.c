/*
    lmap -- IPv6 inet stuff

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
#include <lmap_inet6.h>
#include <lmap_inet.h>

/* prototypes */
   
const char *inet_ntop6(const char *src, char *dst, size_t size);
const char *inet_details6(const char *src, char *dst, size_t size);

/* Original author Paul Vixie.
 * Slightly modified by Daniele Milan <tinybyte at box it>
 *
 * You may pass NULL to details if you don't need them.
 * All the other agruments MUST be provided by the caller.
 *
 * Returns NULL and sets ENOSPC in errno if dst isn't big enoguh.
 *  
 * NOTE: you should allocate at least 16 bytes for the details string
 *       to be sure it will not be truncated.
 */
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
			if (pa_ntoa_r(*(src+12), tp) != 0)
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
	return strcpy(dst, tmp);
}

const char *inet_details6(const char *src, char *dst, size_t size)
{
   char tmp[IP6_DETAILS_SIZE];

   if (dst) {
      
      if ( IN6_IS_ADDR_UNSPECIFIED((SI6A *)src) )
         strncpy(tmp, "unspec", size);
      if ( IN6_IS_ADDR_LOOPBACK((SI6A *)src) )
         strncpy(tmp, "loopback", size);
      if ( IN6_IS_ADDR_MULTICAST((SI6A *)src) ) 
      {
         /* There may be insufficient space left
          * for multicast specific details to fit.
          * Therefore the string may result trucated.
          * It should never happen if you allocate at least
          * 16 chars for the details string.
          */
         size_t mcsiz = size - 7;
         
         strncpy(tmp, "multic", size);
         
         if ( IN6_IS_ADDR_MC_NODELOCAL((SI6A *)src) )
            strncat(tmp, " nodeloc", mcsiz);
         if ( IN6_IS_ADDR_MC_LINKLOCAL((SI6A *)src) )
            strncat(tmp, " linkloc", mcsiz);
         if ( IN6_IS_ADDR_MC_SITELOCAL((SI6A *)src) )
            strncat(tmp, " siteloc", mcsiz);
         if ( IN6_IS_ADDR_MC_ORGLOCAL((SI6A *)src) )
            strncat(tmp, " orgloc", mcsiz);
         if ( IN6_IS_ADDR_MC_GLOBAL((SI6A *)src) )
            strncat(tmp, " global", mcsiz);
      }
         
      if ( IN6_IS_ADDR_LINKLOCAL((SI6A *)src) )
         strncpy(tmp, "linkloc", size);
      if ( IN6_IS_ADDR_SITELOCAL((SI6A *)src) )
         strncpy(tmp, "siteloc", size);
      if ( IN6_IS_ADDR_V4MAPPED((SI6A *)src) )
         strncpy(tmp, "v4mapped", size);
      if ( IN6_IS_ADDR_V4COMPAT((SI6A *)src) )
         strncpy(tmp, "v4compat", size);
   }
   return strncpy(dst, tmp, size);
}

/* EOF */

// vim:ts=3:expandtab
