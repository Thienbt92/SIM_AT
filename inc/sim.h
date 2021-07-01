#ifndef sim_h
#define sim_h
#include "main.h"

typedef enum 
{
  SIM_OK = 0,
  SIM_TIME_OUT = 1  
}SIM_STATUS;

#define SIM_SIZE_BUFFER          10
#define SIM_SIZE_OF_DATA_BUFFER  100
#define SIM_BUF_PK_VALID         0x01


typedef struct
{
  void (*SIM_Sendchar)(char);
  void (*SIM_SendString)(char*);
  void (*SIM_Delay)(uint16_t);
}SIM_FUNCTION;
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


void SIM_ReceiveData (uint8_t _byte);
void SIM_ProcessNewPacket(SIM_BUFFER *_buffer);
#endif

