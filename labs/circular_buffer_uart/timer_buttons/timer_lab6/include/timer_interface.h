/** 
 
  @File Name
    TMR_interface.h
 
  @Summary
    This is the generated header file for Timer module interfaces.
 
  @Description
    This header file provides interfaces to Timer driver APIs.
*/

#ifndef TMR_INTERFACE_H
#define TMR_INTERFACE_H

/**
 * @brief This file contains API prototypes and other datatypes for Timer-0 module.
 * @defgroup timer_interface Timer Interface
 * @{
 */

#include<stddef.h>
        
/**
 @ingroup timer_interface
 @typedef struct TMR_INTERFACE
 @brief This structure contains the interfaces to Timer module
 */
 
struct TMR_INTERFACE
{
    void (*Initialize)(void);
    void (*Start)(void);
    void (*Stop)(void);
    void (*PeriodCountSet)(size_t count);
    void (*TimeoutCallbackRegister)(void (* CallbackHandler)(void));
    void (*Tasks)(void);
};
/**
 * @}
 */
#endif //TMR_INTERFACE_H