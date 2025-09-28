/*
 * (C)2012 Michael Duane Rice All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials
 * provided with the distribution. Neither the name of the copyright holders
 * nor the names of contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <time.h>

/** Difference between the Y2K and the UNIX epochs, in seconds. To convert a Y2K
    timestamp to UNIX...
    \code
    long unix;
    time_t y2k;

    y2k = time(NULL);
    unix = y2k + UNIX_OFFSET;
    \endcode
*/
#define UNIX_OFFSET 946684800

/** Difference between the Y2K and the NTP epochs, in seconds. To convert a Y2K
    timestamp to NTP...
    \code
    unsigned long ntp;
    time_t y2k;

    y2k = time(NULL);
    ntp = y2k + NTP_OFFSET;
    \endcode
*/
#define NTP_OFFSET 3155673600

extern volatile time_t __system_time;

/**
   Initialize the system time. Examples are...

   From a Clock / Calendar type RTC:
   \code
   struct tm rtc_time;

   read_rtc(&rtc_time);
   rtc_time.tm_isdst = 0;
   set_system_time( mktime(&rtc_time) );
   \endcode

   From a Network Time Protocol time stamp:
   \code
   set_system_time(ntp_timestamp - NTP_OFFSET);
   \endcode

   From a UNIX time stamp:
   \code
   set_system_time(unix_timestamp - UNIX_OFFSET);
   \endcode

*/
void set_system_time(time_t timestamp);

/**
   Maintain the system time by calling this function at a rate of 1 Hertz.

   It is anticipated that this function will typically be called from within an
   Interrupt Service Routine, (though that is not required). It therefore includes code which
   makes it simple to use from within a 'Naked' ISR, avoiding the cost of saving and restoring
   all the cpu registers.

   Such an ISR may resemble the following example...
   \code
   ISR(RTC_OVF_vect, ISR_NAKED)
   {
       system_tick();
       reti();
   }
   \endcode
*/
void system_tick(void);
