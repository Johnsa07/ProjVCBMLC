/***************************************************************************
 **
 **    MASTER INCLUDE FILE
 **
 **    Used with ARM IAR C/C++ Compiler and Assembler.
 **
 **    (c) Copyright IAR Systems 2005
 **
 **    $Revision: 39 $
 **
 ***************************************************************************/
#ifndef __INCLUDES_H
#define __INCLUDES_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <intrinsics.h>

//#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "arm_comm.h"

#include "drv_glcd_cnfg.h"
#include "drv_glcd.h"
#include "glcd_ll.h"
#include "iar_logo.h"
#include "carcontrol.h"
#include "feedback.h"
#include "machinelearning.h"

#include "i2c1_drv.h"
#include "accl_drv.h"

#endif /* __INCLUDES_H */
