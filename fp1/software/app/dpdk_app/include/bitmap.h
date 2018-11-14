/******************************************************************************

  Copyright (C), 2014-2024, Huawei Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mem_main.c
  Version       : Initial Draft
  Author        :
  Created       : 2014-10-11
  Last Modified :
  Description   :
  History       :
  1.Date        : 2014-10-11
    Author      : tangguijin
    Modification: Created file
******************************************************************************/
//#include <stdlib.h>

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <stddef.h>

//typedef unsigned long ubitmap_t;

unsigned long bitmap_size(unsigned long bits); 
void   bitmap_set(unsigned long *map, int start, int nr);
void   bitmap_clear(unsigned long *map, int start, int nr);
unsigned long bitmap_find_next_zero_area(unsigned long *map,
					 unsigned long size,
					 unsigned long start,
					 unsigned int  nr,
					 unsigned long align_mask);
unsigned long bitmap_find_next_zero_bit(const unsigned long *map, unsigned long size,
				 unsigned long offset);
unsigned long bitmap_find_next_bit(const unsigned long *map, unsigned long size,
                            unsigned long offset);




#endif



