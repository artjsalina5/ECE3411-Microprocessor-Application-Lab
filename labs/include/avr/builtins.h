/* Copyright (c) 2008 Anatoly Sokolov
   Copyright (c) 2010 Joerg Wunsch
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id$ */

/*
   avr/builtins.h - Intrinsic functions built into the compiler
 */
 
#ifndef _AVR_BUILTINS_H_
#define _AVR_BUILTINS_H_

#ifndef __HAS_DELAY_CYCLES
#define __HAS_DELAY_CYCLES 1
#endif

/** \file */
/** \defgroup avr_builtins <avr/builtins.h>: GCC builtins
    \code #include <avr/builtins.h> \endcode

    This header file declares AVR builtins.
    All the functions documented here are built into the
    compiler, and cause it to emit the corresponding assembly
    code instructions.
*/

/**
    \ingroup avr_builtins

    Emits a \c NOP instruction.  */
extern void __builtin_avr_nop(void);

/**
    \ingroup avr_builtins

    Enables interrupts by setting the global interrupt mask.  */
extern void __builtin_avr_sei(void);

/**
    \ingroup avr_builtins

    Disables all interrupts by clearing the global interrupt mask.  */
extern void __builtin_avr_cli(void);

/**
    \ingroup avr_builtins

    Emits a \c SLEEP instruction.  */

extern void __builtin_avr_sleep(void);

/**
    \ingroup avr_builtins

    Emits a WDR (watchdog reset) instruction.  */
extern void __builtin_avr_wdr(void);

/**
    \ingroup avr_builtins

    Emits a SWAP (nibble swap) instruction on __b.  */
extern uint8_t __builtin_avr_swap(uint8_t __b);

/**
    \ingroup avr_builtins

    Emits an FMUL (fractional multiply unsigned) instruction.  */
extern uint16_t __builtin_avr_fmul(uint8_t __a, uint8_t __b);

/**
    \ingroup avr_builtins

    Emits an FMUL (fractional multiply signed) instruction.  */
extern int16_t __builtin_avr_fmuls(int8_t __a, int8_t __b);

/**
    \ingroup avr_builtins

    Emits an FMUL (fractional multiply signed with unsigned) instruction.  */
extern int16_t __builtin_avr_fmulsu(int8_t __a, uint8_t __b);

#if __HAS_DELAY_CYCLES || defined(__DOXYGEN__)
/**
    \ingroup avr_builtins

    Emits a sequence of instructions causing the CPU to spend
    \c __n cycles on it.  */
extern void __builtin_avr_delay_cycles(uint32_t __n);
#endif

/**
    \ingroup avr_builtins

    Insert bits from \c __bits into \c __val and return the resulting value.
    The nibbles of \c __map determine how the insertion is performed:

    Let X be the n-th nibble of \c __map
    \li If X is 0xf, then the n-th bit of \c __val is returned unaltered.
    \li If X is in the range 0…7, then the n-th result bit is set to the X-th bit
    of \c __bits
    \li If X is in the range 8…0xe, then the n-th result bit is undefined.

    One typical use case for this built-in is adjusting input and output values
    to non-contiguous port layouts. Some examples:

    \code{.c}
    // same as val, bits is unused
    __builtin_avr_insert_bits (0xffffffff, bits, val)
    \endcode

    \code{.c}
    // same as bits, val is unused
    __builtin_avr_insert_bits (0x76543210, bits, val)
    \endcode

    \code{.c}
    // same as rotating bits by 4
    __builtin_avr_insert_bits (0x32107654, bits, 0)
    \endcode

    \code{.c}
    // high nibble of result is the high nibble of val
    // low nibble of result is the low nibble of bits
    __builtin_avr_insert_bits (0xffff3210, bits, val)
    \endcode

    \code{.c}
    // reverse the bit order of bits
    __builtin_avr_insert_bits (0x01234567, bits, 0)
    \endcode
  */
extern uint8_t __builtin_avr_insert_bits(uint32_t __map, uint8_t __bits,
                                         uint8_t __val);

/* Enable builtins that are non compliant to MISRA rules, if strict MISRA
   compliant (__XC_STRICT_MISRA) is not enforced.
   Examples:
   - Non standard C extensions (__memx)
*/
#ifndef __XC_STRICT_MISRA

/**
    \ingroup avr_builtins

    This built-in takes a byte address to the 24-bit address space __memx and
    returns the number of the flash segment (the 64 KiB chunk) where the
    address points to. Counting starts at 0. If the address does not point to
    flash memory, return -1.  */
#ifdef __BUILTIN_AVR_FLASH_SEGMENT
extern int8_t __builtin_avr_flash_segment(const __memx void *__a);
#endif

#endif

#endif /* _AVR_BUILTINS_H_ */
