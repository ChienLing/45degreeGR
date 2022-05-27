#ifndef _IO_INTERFACE_H_
#define _IO_INTERFACE_H_
#include "GR.h"
extern int MIN_SPACING;
extern int WIRE_WIDTH;
extern int cell_coe;
extern int coarse_x_length;
extern int coarse_y_length;
extern int fine_x_length;
extern int fine_y_length;
void output_gds(std::string filename, GR* router);
#endif