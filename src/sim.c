/********************************************************************************
Tac Gia:    BUI TRONG THIEN
Phien Ban:  1.0
Ngay Thang: 01/07/2021
********************************************************************************/

#include "sim.h"    

SIM_BUFFER SimBuffer[SIM_SIZE_BUFFER];
FLAG_SIM_PACKET Flag_SimPacket;
volatile uint8_t SIM900_IndexBuffer = 0;

void SIM900_SendChar (char _char)
{
	char string[2];
	string[0]=_char;
	MID_URT_Transmit(&huart2,(uint8_t*)string,1,1000);
}

void SIM900_SendString	(uint8_t* string)
{
	HAL_UART_Transmit(&huart2,string,strlen((char*)string),1000);
}

/********************** Nhận Dữ Liệu Từ SIM ***************************************/
/*
  * Nhận 1 gói dữ liệu từ sim gửi xuống 
  * Mỗi 1 byte nhận được sẽ được điều vào bộ đệm để xử lý sau
  * Khi phát hiện chuỗi kỹ tự kết thúc gói "0D 0A" thì đóng gói dữ liệu vào bộ đệm
*/
void SIM_InByte (uint8_t _byte)
{
  SimBuffer[SIM900_IndexBuffer].Buffer[SimBuffer[SIM900_IndexBuffer].IndexByteInBuffer++] = _byte;
  if(SimBuffer[SIM900_IndexBuffer].IndexByteInBuffer > SIM_SIZE_OF_DATA_BUFFER)
     SimBuffer[SIM900_IndexBuffer].IndexByteInBuffer = 0;
}
void SIM_NextBuffer (void)
{
  SimBuffer[SIM900_IndexBuffer].Flags |= SIM_BUF_PK_VALID;
  if(++SIM900_IndexBuffer>=SIM_SIZE_BUFFER)
    SIM900_IndexBuffer=0;
}
/*
  Phát hiện kết thúc chuỗi dữ liệu
*/
void SIM_ReceiveData (uint8_t _byte)
{
  SIM_InByte(_byte);
  if(_byte == 0x0D)
  {
    if(Flag_SimPacket.BIT._Flag_Detect_End_Packet==0)
      Flag_SimPacket.BIT._Flag_Detect_End_Packet=1;
    else
      Flag_SimPacket.BIT._Flag_Detect_End_Packet=0;
  }
  else if(_byte == 0x0A)
  {
    if(Flag_SimPacket.BIT._Flag_Detect_End_Packet!=0)
    {
      Flag_SimPacket.BIT._Flag_Detect_End_Packet=0;
      SIM_NextBuffer();
    }
  }
}
/********************* Xử Lý Dữ Liệu Nhận Được *********************************/
/*
  * Reset bo dem SIM900_BUFFER.
*/
void SIM_ResetBuffer(SIM_BUFFER* _buffer)
{
  uint8_t i;
  for(i=0;i<_buffer->IndexByteInBuffer;i++)
      _buffer->Buffer[i] = 0;
  _buffer->Flags = _buffer->IndexByteInBuffer =0;
}
/*

*/
void SIM_Rolling(void)
{
  uint8_t i;
  for(i=0;i<SIM_SIZE_BUFFER;i++)
  {
    if((SimBuffer[i].Flags&SIM_BUF_PK_VALID)!=0)
    {
        SIM_ProcessNewPacket(&SimBuffer[i]);
        SIM_ResetBuffer(&SimBuffer[i]);
    }
  }
} 

/*

*/

void SIM_ProcessNewPacket(SIM_BUFFER *_buffer)
{ 
  
}




