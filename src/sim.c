/********************************************************************************
Tac Gia:    BUI TRONG THIEN
Phien Ban:  1.0
Ngay Thang: 01/07/2021
********************************************************************************/

#include "sim.h"    

SIM_BUFFER SimBuffer[SIM_SIZE_BUFFER];
FLAG_SIM_PACKET Flag_SimPacket;
SIM_FUNCTION SIM_Function;
volatile uint8_t SIM_IndexBuffer = 0,CurrentIndexBuffer=0;
volatile SIM_FLAGS SIM_FlagsStatus = 0;
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  =============================== Xử Lý Cờ Của SIM ===============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*
  * Kiểm tra trạng thái các cờ của SIM
  * SIM_SEND_FLAG_OK               0x01
  * SIM_FLAG_SEND_COMMAND          0x02
  * SIM_HAVE_NEW_SMS               0x04
  * SIM_SEND_SMS_OK                0x08
*/
SIM_FLAGS SIM_GetFlags(SIM_FLAGS _flags)
{
    return SIM_FlagsStatus&_flags;
}
/*
  * Thiết lập trạng thái các cờ của SIM
  * SIM_SEND_FLAG_OK               0x01
  * SIM_FLAG_SEND_COMMAND          0x02
  * SIM_HAVE_NEW_SMS               0x04
  * SIM_SEND_SMS_OK                0x08
*/
void SIM900_SetFlags(SIM_FLAGS _flags)
{
    SIM_FlagsStatus|=_flags;
}
/*
  * Xóa trạng thái các cờ của SIM
  * SIM_SEND_FLAG_OK               0x01
  * SIM_FLAG_SEND_COMMAND          0x02
  * SIM_HAVE_NEW_SMS               0x04
  * SIM_SEND_SMS_OK                0x08
*/
void SIM900_ClearFlags(SIM_FLAGS _flags)
{
    SIM_FlagsStatus&=~_flags;
}

/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================= Xử Lý Chuỗi Trong Gói Tin =============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*******************************************************************************
 * Tìm 1 chuỗi trong chuỗi nguồn.
 * Trong Đó     _string_source  :    Chuỗi nguồn
                _string_search  :    Chuỗi cần tìm
                _time           :    Số lần xuất hiện chuỗi cần tìm
                _length         :    Độ dài của chuỗi nguồn
 * Trả về       NULL            :    Nếu không tìm thấy chuỗi.
                !=NULL          :    Vị trí con trỏ của chuỗi.
********************************************************************************/
char* SearchArrayInArray(char* _string_source,char* _string_search,uint8_t _time, uint16_t _length)
{
        uint16_t IndexSource=0,IndexSearch=0;
        while(IndexSource<_length)
        {
            if(_string_source[IndexSource++]!=_string_search[IndexSearch++])
            {
                IndexSearch = 0;
                if(_string_search[IndexSearch]==_string_source[IndexSource-1])
                    IndexSearch++;
            }
            if(_string_search[IndexSearch]==0)
            {
                if(--_time!=0)
                {
                    IndexSearch = 0;
                    continue;
                }
                return &_string_source[IndexSource-IndexSearch];
            } 
        }
     return NULL;
}
/************************************************************************************
 * Tìm 1 ký tự trong chuỗi nguồn.
 * Trong đó    CharNeedSerach    :    Ký tự cần tìm
               _string_search    :    Chuỗi cần tìm (chuỗi nguồn)
               TimeAppear        :    Số lần xuất hiện của ký tự trong chuỗi nguồn
               MaxLenghtBuffer   :    Độ dài của chuỗi nguồn
 * Trả Về      =MaxLenghtBuffer  :    Khi không tìm thấy kỹ tự trong chuỗi nguồn
               <MaxLenghtBuffer  :    Vị trí ký tự trong chuỗi nguồn.
************************************************************************************/
char* SearchByteInArray(char CharNeedSerach, char *_string_search,uint8_t TimeAppear,uint8_t MaxLenghtBuffer)
{
    uint16_t i=0;
    uint8_t Time_=0;
    while(i<=MaxLenghtBuffer)
    {
        if(_string_search[i]==CharNeedSerach)
        {
            if(++Time_>=TimeAppear)
              return &_string_search[i]; 
        }
        i++;  
    }
    return NULL;
}

/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================== Xử Lý Dữ Liệu Nhận Từ SIM ============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*
  * Nhận 1 gói dữ liệu từ sim gửi xuống 
  * Mỗi 1 byte nhận được sẽ được điều vào bộ đệm để xử lý sau
  * Khi phát hiện chuỗi kỹ tự kết thúc gói "0D 0A" thì đóng gói dữ liệu vào bộ đệm
*/
void SIM_InByte (uint8_t _byte)
{
  SimBuffer[SIM_IndexBuffer].Buffer[SimBuffer[SIM_IndexBuffer].IndexByteInBuffer++] = _byte;
  if(SimBuffer[SIM_IndexBuffer].IndexByteInBuffer > SIM_SIZE_OF_DATA_BUFFER)
     SimBuffer[SIM_IndexBuffer].IndexByteInBuffer = 0;
}
void SIM_NextBuffer (void)
{
  SimBuffer[SIM_IndexBuffer].Flags |= SIM_BUF_PK_VALID;
  CurrentIndexBuffer = SIM_IndexBuffer;
  if(++SIM_IndexBuffer>=SIM_SIZE_BUFFER)
    SIM_IndexBuffer=0;
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
/*
  * Cho tin hieu tra ve tu SIM900.
  * _respond    :    Tin hieu tra ve.
  * _time_wait  :    Thoi gian cho toi da.
  * Tra Ve      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
********************************************************************************/
static SIM_STATUS SIM_WaitForRespond(char *Respond,uint16_t _time_wait)
{
    uint16_t TimeWait = 0;
    uint8_t i;  
    while((++TimeWait<_time_wait))
    {
        SIM_Function.SIM_Delay(1);
        if(NULL!=SearchArrayInArray(SimBuffer[CurrentIndexBuffer].Buffer,Respond,1,SimBuffer[CurrentIndexBuffer].IndexByteInBuffer))
           return SIM_OK;
    }
    return SIM_TIME_OUT;
}
/*
  * Reset bộ đệm chứa dữ liệu nhận được từ SIM
*/
void SIM_ResetBuffer(SIM_BUFFER* _buffer)
{
  uint8_t i;
  for(i=0;i<_buffer->IndexByteInBuffer;i++)
      _buffer->Buffer[i] = 0;
  _buffer->Flags = _buffer->IndexByteInBuffer =0;
}

/*
  * Phát hiện tìm kiếm gói tin mới nhận được và tiến hàng xử lý.
*/
void SIM_DetectPacket(void)
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
  * Gửi lệnh xuống SIM
  * Lệnh là 1 chuỗi dạng char cần bao gồm 0D và 0A
*/
static void SIM_SendCommand(char *Command)
{
    SIM900_ClearFlags(SIM_SEND_FLAG_OK);
    SIM_Function.SIM_SendString(Command);
    SIM_Function.SIM_Sendchar(0x0D);
}

/*
  * Xử lý gói tin khi nhận được.
*/

void SIM_ProcessNewPacket(SIM_BUFFER *_buffer)
{ 
  
}


/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ====================== Tập Lệnh Điều Khiển SIM Qua AT Command ===================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


