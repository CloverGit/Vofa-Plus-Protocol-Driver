# Vofa Plus Protocol Driver
## 简介
这是为 MCS-51(8051) 系列单片机及其兼容性单片机编写的 VOFA+ 串口上位机软件的通信协议驱动，能够支持数据通道的无限扩展，目前实现了对 JustFloat 协议的发送支持

## 使用方法
1. 先申请数据通道结构

```c
struct Vofa_Channel Vofa_CH0 = {0};
```

2. 绑定串口发送单字节数据的接口
```c
Vofa_Init(UART1_SendOneChar);
```

3. 将数据通道加入到发送队列当中
```c
Vofa_AddChannel(&Vofa_CH0);
```

4. 使用 `Vofa_SetChannelData()` 设定通道数据
```c
Vofa_SetChannelData(&Vofa_CH0, sin(testData));
```

5. 使用 `Vofa_JustFloatSendAllChannelData()` 一次发送全部通道数据



## Example

## MCS-51

``` c
#include "VofaPlus.h"
void UART1_SendByte(uint8_t dat)
{
    ...
}

void main(void)
{
    float fDataCH0 = 0;
    float fDataCH1 = 0;
	struct Vofa_Channel Vofa_CH0 = {0};
	struct Vofa_Channel Vofa_CH1 = {0};
    ...
    Vofa_Init(UART1_SendByte);
    Vofa_AddChannel(&Vofa_CH0);
    Vofa_AddChannel(&Vofa_CH1);
    while (1) {
        for(i = 0; i < 1000; i++) {
            fDataCH0 += 0.1;
            fDataCH1 = fDataCH0 + 3.141592;
            Vofa_SetChannelData(&Vofa_CH0, sin(fDataCH0));
            Vofa_SetChannelData(&Vofa_CH1, sin(fDataCH1));

            Vofa_JustFloatSendAllChannelData();
        }
}

```

## STM32

基于 HAL 库的 128 条数据通道测试

``` c
/* USER CODE BEGIN Includes */
#include "VofaPlus.h"
/* USER CODE END Includes */
...
int main(void)
{
    /* USER CODE BEGIN 2 */
    struct Vofa_Channel Vofa_CH0 = {0};
    struct Vofa_Channel Vofa_CHx[128];

    Vofa_Init(HAL_UART_TransmitByte);
    for (uint16_t i = 0; i < 128; i++)
        Vofa_AddChannel(&Vofa_CHx[i]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1) {
    static float fDataTest = 0;
    for (uint16_t i = 0; i < 128; i++) {
        fDataTest += (float)0.1;
        Vofa_CHx[i].dat.fData = (float)sin((double)fDataTest);
    }
    HAL_Delay(10);
    Vofa_JustFloatSendAllChannelData();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
...
/* USER CODE BEGIN 4 */
void HAL_UART1_Transmit(uint8_t dat)
{
    HAL_UART_TransmitByte(&huart1, (uint8_t*)&dat, 1, HAL_MAX_DELAY);
}
/* USER CODE END 4 */

```



