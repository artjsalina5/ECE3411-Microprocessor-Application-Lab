#ifndef UI_OPAMP_H_
#define UI_OPAMP_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Launch and run the OPAMP Lab
void opamp_lab_init(void);
void opamp_lab_process(void);
void opamp_lab_show_welcome(void);

// State helpers
bool opamp_lab_is_active(void);
void opamp_lab_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_OPAMP_H_ */
