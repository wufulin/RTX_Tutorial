/**
  ******************************************************************************
  * @file    main.c
  * @author  wufulin
  * @version V1.0
  * @date    2018-07-20
  * 
  * Copyright (C), 2018-2019, wufulin
  ******************************************************************************
  */
#include "bsp.h" /* 底层硬件驱动 */
#include "bsp_rtt.h"
#include "RTL.h"

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_ALL (BIT_0 | BIT_1)

/**
  ******************************************************************************
  *                                 变量声明                                     
  ******************************************************************************
  */
static uint64_t AppTaskPrintfStk[256]; /* 任务栈 */
static uint64_t AppTaskStartStk[256];
static uint64_t AppTaskScanStk[256];
static uint64_t AppTaskSemStk[256];

static OS_SEM semaphore; /* 信号量 */
static OS_MUT mutex;     /* 互斥信号量 */

OS_TID HandleTaskPrintf = NULL; /* 任务句柄 */
OS_TID HandleTaskSem = NULL;
OS_TID HandleTaskScan = NULL;

/**
  ******************************************************************************
  *                                 函数声明                                     
  ******************************************************************************
  */
__task void AppTaskStart(void);
__task void AppTaskScan(void);
__task void AppTaskPrintf(void);
__task void AppTaskSem(void);
static void AppTaskCreate(void);
static void TIM_CallBack1(void);

/**
  ******************************************************************************
  *  函 数 名: AppTaskSem
  *  功能说明: 信号量任务，优先级1
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
__task void AppTaskSem(void)
{
  OS_RESULT xResult;
  const uint16_t usMaxBlockTime = 1000;

  while (1)
  {
    xResult = os_sem_wait(&semaphore, usMaxBlockTime);
    switch (xResult)
    {
    case OS_R_OK:
      SEGGER_RTT_printf(0, "Sem No Wait\r\n");
      break;
    case OS_R_SEM:
      SEGGER_RTT_printf(0, "Sem Ok in BlockTime\r\n");
      break;
    case OS_R_TMO:
      SEGGER_RTT_printf(0, "Sem Timeout\r\n");
      break;
    default:
      break;
    }
  }
}

/**
  ******************************************************************************
  *  函 数 名: AppTaskScan
  *  功能说明: 扫描任务，优先级1
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
__task void AppTaskScan(void)
{
  const uint16_t usFrequency = 500;

  /* 设置延迟周期 */
  os_itv_set(usFrequency);

  while (1)
  {
    os_mut_wait(&mutex, 0xffff);
    SEGGER_RTT_printf(0, "Hello App Task Scan\n");
    SEGGER_RTT_printf(0, "%d\r\n", os_time_get());
    os_mut_release(&mutex);
    // os_dly_wait(100);
    /* os_itv_wait 是绝对延迟， os_dly_wait 是相对延迟 */
    bsp_StartHardTimer(1, 50000, (void *)TIM_CallBack1);
    os_sem_send(&semaphore);
    os_itv_wait();
  }
}

/**
  ******************************************************************************
  *  函 数 名: AppTaskStart
  *  功能说明: 启动任务，优先级2
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
__task void AppTaskStart(void)
{
  /* 创建信号量计数值是0， 用于任务同步 */
  os_sem_init(&semaphore, 0);

  /* 创建互斥信号量 */
  os_mut_init(&mutex);

  AppTaskCreate();

  while (1)
  {
    os_dly_wait(2000);
    // os_evt_set(BIT_ALL, HandleTaskPrintf);
    tsk_lock();
    SEGGER_RTT_printf(0, "Hello App Task Start\n");
    tsk_unlock();
  }
}

/**
  ******************************************************************************
  *  函 数 名: AppTaskPrintf
  *  功能说明: SEGGER_RTT 打印任务，优先级1
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
__task void AppTaskPrintf(void)
{
  OS_RESULT xResult;
  const uint16_t usMaxBlockTime = 1500;
  const uint16_t usFrequency = 200;

  /* 设置延迟周期 */
  os_itv_set(usFrequency);

  while (1)
  {
    /* os_itv_wait 是绝对延迟， os_dly_wait 是相对延迟 */
    // os_itv_wait();
    xResult = os_evt_wait_and(BIT_ALL, usMaxBlockTime);
    switch (xResult)
    {
    case OS_R_EVT:
      SEGGER_RTT_printf(0, "Hello App Task Printf\n");
      break;
    case OS_R_TMO:
      SEGGER_RTT_printf(0, "Time out\r\n");
      break;
    default:
      break;
    }
  }
}

/**
  ******************************************************************************
  *  函 数 名: TIM_CallBack1
  *  功能说明: 定时器中断的回调函数，此函数被bsp_StartHardTimer所调用
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
static void TIM_CallBack1(void)
{
  /* 发送事件标志， 设置bit0和bit1 */
  SEGGER_RTT_printf(0, "TIM2 Interrupt\r\n");
  isr_evt_set(BIT_ALL, HandleTaskPrintf);
}

/**
  ******************************************************************************
  *  函 数 名: AppTaskCreate
  *  功能说明: 创建应用任务
  *  形    参: 无
  *  返 回 值: 无
  ******************************************************************************
  */
static void AppTaskCreate(void)
{
  HandleTaskPrintf = os_tsk_create_user(AppTaskPrintf,             /* 任务函数 */
                                        1,                         /* 任务优先级 */
                                        &AppTaskPrintfStk,         /* 任务栈 */
                                        sizeof(AppTaskPrintfStk)); /* 任务栈大小 */

  HandleTaskSem = os_tsk_create_user(AppTaskSem,                   /* 任务函数 */
                                     1,                            /* 任务优先级 */
                                     &AppTaskSemStk,               /* 任务栈 */
                                     sizeof(AppTaskSemStk));       /* 任务栈大小 */

  HandleTaskScan = os_tsk_create_user(AppTaskScan,                 /* 任务函数 */
                                      1,                           /* 任务优先级 */
                                      &AppTaskScanStk,             /* 任务栈 */
                                      sizeof(AppTaskScanStk));     /* 任务栈大小 */

  // if (os_tsk_prio(HandleTaskPrintf, 3) == OS_R_OK)
  // {
  //   SEGGER_RTT_printf(0, "Modify Task Priority Success\r\n");
  // }
  // else
  // {
  //   SEGGER_RTT_printf(0, "Modify Task Priority Failed\r\n");
  // }
}

/**
  ******************************************************************************
  *  函 数 名: main
  *  功能说明: c程序入口
  *  形    参: 无
  *  返 回 值: 错误代码(无需处理)
  ******************************************************************************
  */
int main(void)
{
  bsp_Init();     /* 初始化外设 */
  bsp_RTT_Init(); /* 初始化SEGGER_RTT */

  /* 创建启动任务 */
  os_sys_init_user(AppTaskStart,             /* 任务函数 */
                   2,                        /* 任务优先级 */
                   &AppTaskStartStk,         /* 任务栈 */
                   sizeof(AppTaskStartStk)); /* 任务栈大小 */

  /* 进入主程序循环体 */
  while (1)
    ;
}
