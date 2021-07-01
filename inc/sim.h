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

void SIM_ReceiveData (uint8_t _byte);
void SIM_ProcessNewPacket(SIM_BUFFER *_buffer);
#endif

