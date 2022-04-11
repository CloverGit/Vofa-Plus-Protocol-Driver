/**
  * @file Vofa.c
  * @brief Vofa 上位机相关协议发送与解析
  * @author CloverGit
  * @date 2021/12/15
  */

#include "Vofa.h"
#include "string.h"

#ifdef __C51__
static const uint8_t code justFloatTail[4] = {0x00, 0x00, 0x80, 0x7f};
#else
static const uint8_t justFloatTail[4] = {0x00, 0x00, 0x80, 0x7f};
#endif
static struct Vofa_Channel* head_handle = NULL;
static void (*UARTSendByteCallback)(uint8_t) = NULL;

/**
  * @brief  通过串口发送多个字节数据
  * @param  *buffer: 数据缓冲区
  *          length: 缓冲区大小
  * @retval 无
  */
static inline void Vofa_UART_SendBytes(const uint8_t *buffer, size_t length)
{
    while(length--) UARTSendByteCallback(*buffer++);
}

/**
  * @brief  将大端浮点数据转换为小端数据
  * @param  floatValue: 大端浮点数
  *              *byte: 存储转换后的数据的数组
  * @retval 无
  */
static void Vofa_Big2Little(float floatValue, uint8_t *byte)
{
    byte[0] = ((uint8_t *)&floatValue)[3];
    byte[1] = ((uint8_t *)&floatValue)[2];
    byte[2] = ((uint8_t *)&floatValue)[1];
    byte[3] = ((uint8_t *)&floatValue)[0];
}

/**
  * @brief  Vofa 初始化函数
  * @param  Vofa_UARTSendByteCallback: 串口发送单字节的回调函数
  * @retval 无
  */
void Vofa_Init(void (*Vofa_UARTSendByteCallback)(uint8_t))
{
    UARTSendByteCallback = Vofa_UARTSendByteCallback;
}

/**
  * @brief  Vofa 添加数据通道
  * @param  handle: 通道信息结构体, 详见 Vofa.h 中的定义
  * @retval 无
  */
void Vofa_AddChannel(struct Vofa_Channel* handle)
{
    struct Vofa_Channel* prev = head_handle;

    if (head_handle == NULL) {
        memset(handle, 0, sizeof(struct Vofa_Channel));
        head_handle = handle;
        return;
    }
    if(head_handle == handle) return;       // 节点已存在
    while (prev->next) {
        if (prev->next == handle) return;   // 节点已存在
        prev = prev->next;
    }
    memset(handle, 0, sizeof(struct Vofa_Channel));
    prev->next = handle;
}

/**
  * @brief  Vofa 删除数据通道
  * @param  handle: 通道信息结构体, 详见 Vofa.h 中的定义
  * @retval 无
  */
void Vofa_DelChannel(struct Vofa_Channel* handle)
{
    struct Vofa_Channel** curr = &head_handle;
    struct Vofa_Channel* entry = NULL;

    while (*curr) {
        entry = *curr;
        if (entry->next == handle) {
            *curr = entry->next;
//            free(entry);
            return;
        }
        curr = &(entry->next);
    }
}

/**
  * @brief  Vofa 设置某通道数据
  * @param  handle: 通道信息结构体, 详见 Vofa.h 中的定义
  *          fData: 要设置的数据
  * @retval 无
  */
void Vofa_SetChannelData(struct Vofa_Channel* handle, float fData)
{
#ifdef VOFA_USE_BIG_ENDIAN
    Vofa_Big2Little(fData, handle->dat.bData);
#else
    handle->dat.fData = fData;
#endif // VOFA_USE_BIG_ENDIAN
}

/**
  * @brief 通过 JustFloat 协议直接发送一个数据
  * @param fData: 要发送的数据
  * @retval 无
  */
void Vofa_JustFloatSendData(float fData)
{
    uint8_t *dat = (uint8_t * ) &fData;

    Vofa_UART_SendBytes(dat, FLOAT_SIZE);
    Vofa_UART_SendBytes(justFloatTail, 4);
}

/**
  * @brief 通过 JustFloat 协议发送一次指定通道的数据
  * @param  handle: 通道信息结构体, 详见 Vofa.h 中的定义
  * @retval 无
  */
void Vofa_JustFloatSendChannelData(struct Vofa_Channel* handle)
{
    Vofa_UART_SendBytes(handle->dat.bData, FLOAT_SIZE);
    Vofa_UART_SendBytes(justFloatTail, 4);
}

/**
  * @brief 通过 JustFloat 协议一次发送所用通道的数据
  * @param  无
  * @retval 无
  */
void Vofa_JustFloatSendAllChannelData(void)
{
    struct Vofa_Channel* entry = head_handle;

    while (entry) {
        Vofa_UART_SendBytes(entry->dat.bData, FLOAT_SIZE);
        entry = entry->next;
    }
    Vofa_UART_SendBytes(justFloatTail, 4);
}
