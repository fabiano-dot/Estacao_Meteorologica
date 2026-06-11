#ifndef FCNT_H
#define FCNT_H

void     fcnt_init();
void     fcnt_clear(   uint32_t next_value);
void     fcnt_update();
uint32_t fcnt_get_next();

#endif // FCNT_H