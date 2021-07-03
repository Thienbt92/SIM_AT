#ifndef sim_h
#define sim_h
#include "main.h"

typedef enum 
{
  SIM_OK = 0,
  SIM_TIME_OUT = 1  
}SIM_STATUS;

typedef enum
{
  CALL_OK        = 0,
  CME_ERROR,
  NO_DIALTONE,
  BUSY,
  NO_CARRIER,
  NO_ANSWER,
  CONNECT,
}CALL_STATUS;

typedef struct
{
    char PhoneNumber[15];
}SIM_PHONE_NUMBER;

typedef struct
{
    uint8_t Flags;
    SIM_PHONE_NUMBER Phone;   
}MASTER_PHONE_NUMBER;

#define SIM_SIZE_BUFFER          10
#define SIM_SIZE_OF_DATA_BUFFER  100
#define SIM_BUF_PK_VALID         0x01


typedef struct
{
  void (*SIM_Sendchar)(char);
  void (*SIM_SendString)(char*);
  void (*SIM_Delay)(uint32_t);
}SIM_FUNCTION;
extern SIM_FUNCTION SIM_Function;

typedef struct
{
    uint8_t Flags;
    uint8_t IndexByteInBuffer;
    char Buffer[SIM_SIZE_OF_DATA_BUFFER];
}SIM_BUFFER;

typedef  union
{
  uint8_t BYTE;
  struct
  {
    uint32_t _Flag_Detect_End_Packet:1;  // Phát hiện kết thúc gói tin
  }BIT;
}FLAG_SIM_PACKET;
typedef struct
{
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
    uint8_t Day;
    uint8_t Date;
    uint8_t Month;
    uint8_t Year;
}SIM_TIME;

#define MASTER_FLAGS_ADMIN               0x80
#define MASTER_FLAGS_MOD                 0x40
#define MASTER_FLAGS_SUB_MOD             0x20
#define MASTER_FLAGS_NOMAL               0x10



#define SIM_FLAGS                      uint8_t
#define SIM_SEND_FLAG_OK               0x01
#define SIM_FLAG_SEND_COMMAND          0x02
#define SIM_HAVE_NEW_SMS               0x04
#define SIM_SEND_SMS_OK                0x08


/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  =============================== Xử Lý Cờ Của SIM ===============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
SIM_FLAGS SIM_GetFlags(SIM_FLAGS _flags);
void SIM_SetFlags(SIM_FLAGS _flags);
void SIM_ClearFlags(SIM_FLAGS _flags);
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================= Xử Lý Chuỗi Trong Gói Tin =============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
char* SearchArrayInArray(char* _string_source,char* _string_search,uint8_t _time, uint16_t _length);
char* SearchByteInArray(char CharNeedSerach, char *_string_search,uint8_t TimeAppear,uint8_t MaxLenghtBuffer);
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================== Xử Lý Dữ Liệu Nhận Từ SIM ============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
void SIM_InByte (uint8_t _byte);
void SIM_InBuffer (uint8_t* _buffer,uint16_t Leng);
void SIM_NextBuffer (void);
void SIM_ReceiveData (uint8_t _byte);
void SIM_ReceiveDMA (uint8_t* _buffer,uint16_t Leng);
SIM_STATUS SIM_WaitForRespond(char *Respond,uint16_t _time_wait);
void SIM_ResetBuffer(SIM_BUFFER* _buffer);
void SIM_DetectPacket(void);
void SIM_SendCommand(char *Command);
void SIM_ProcessNewPacket(SIM_BUFFER *_buffer);

/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ====================== Tập Lệnh Điều Khiển SIM Qua AT Command ===================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
SIM_STATUS SIM_Call(uint8_t *PhoneNumber,uint32_t _timeout);
void SIM_HangUpCall(void);
SIM_STATUS SIM_SendMessages(char *PhoneNumber, char *Messages,uint32_t _timeout);
SIM_STATUS SIM_ReadMessages(uint8_t IndexInBufferSim,uint32_t _timeout);
SIM_STATUS SIM900_DeleteMessages(uint8_t IndexInBufferSim,uint32_t _timeout);
uint8_t SIM_GetSenderNumber(SIM_BUFFER* _buffer,SIM_PHONE_NUMBER*_sender_number);
uint8_t SIM_GetContent(SIM_BUFFER* _buffer,char* _content);
#endif

