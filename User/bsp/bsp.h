/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*	文件名称 : bsp.h
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 应用程序只需 #include bsp.h 即可，
*			  不需要#include 每个模块的 h 文件
*
*	Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H_
#define _BSP_H

#define STM32_V4
//#define STM32_X2

/* 检查是否定义了开发板型号 */
#if !defined (STM32_V4) && !defined (STM32_X2)
	#error "Please define the board model : STM32_X2 or STM32_V4"
#endif

/* 定义 BSP 版本号 */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()		bsp_Idle()

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#include "bsp_led.h"
#include "bsp_timer.h"
#include "bsp_key.h"

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_Idle(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
