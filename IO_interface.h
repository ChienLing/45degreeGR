#ifndef _IO_INTERFACE_H_
#define _IO_INTERFACE_H_
#include "BLMR.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int cell_coe;
void output_gds(std::string filename, BLMR* router);
#endif