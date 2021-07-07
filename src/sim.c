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
void SIM_SetFlags(SIM_FLAGS _flags)
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
void SIM_ClearFlags(SIM_FLAGS _flags)
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
uint8_t SearchByteInArray(char CharNeedSerach, char *_string_search,uint8_t TimeAppear,uint8_t MaxLenghtBuffer)
{
    uint16_t i=0;
    uint8_t Time_=0;
    while(i<=MaxLenghtBuffer)
    {
        if(_string_search[i]==CharNeedSerach)
        {
            if(++Time_>=TimeAppear)
              return i; 
        }
        i++;  
    }
    return MaxLenghtBuffer;
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
/*
  * Ghi 1 mảng dữ liệu vào bộ đệm.
  * Trong đó: _buffer: chứa mảng dữ liệu nhận được
              Leng: Độ dài của mảng dữ liệu
*/
void SIM_InBuffer (uint8_t* _buffer,uint16_t Leng)
{
  for(uint16_t i=0;i<Leng;i++)
    SimBuffer[SIM_IndexBuffer].Buffer[i] = _buffer[i];
  
  SimBuffer[SIM_IndexBuffer].IndexByteInBuffer = Leng;
}
/*
  * Chuyển sang bộ đệm tiếp theo để ghi thêm dữ liệu mới
*/
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
//uint8_t _buf_test[50];
//uint8_t __i=0;
void SIM_ReceiveData (uint8_t _byte)
{
  //_buf_test[__i++] = _byte;
  if(_byte == 0x0D)
  {
    if(SimBuffer[SIM_IndexBuffer].IndexByteInBuffer!=0)
    {
      if(Flag_SimPacket.BIT._Flag_Detect_End_Packet==0)
        Flag_SimPacket.BIT._Flag_Detect_End_Packet=1;
      //else
        //Flag_SimPacket.BIT._Flag_Detect_End_Packet=0;
    }
  }
  else if(_byte == 0x0A)
  {
    if(Flag_SimPacket.BIT._Flag_Detect_End_Packet!=0)
    {
      Flag_SimPacket.BIT._Flag_Detect_End_Packet=0;
      SIM_NextBuffer();
    }
  }
  else
    SIM_InByte(_byte);
}
/*
  * Nhận dữ liệu RX bằng DMA (Hỗ chợ 1 số dòng chip có chế độ IDLE hoặc DMA)
*/
void SIM_ReceiveDMA (uint8_t* _buffer,uint16_t Leng)
{
  SIM_InBuffer(_buffer,Leng);
  SIM_NextBuffer();
}
/*
  * Cho tin hieu tra ve tu SIM900.
  * _respond    :    Tin hieu tra ve.
  * _time_wait  :    Thoi gian cho toi da.
  * Tra Ve      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
********************************************************************************/
SIM_STATUS SIM_WaitForRespond(char *Respond,uint16_t _time_wait)
{
    uint16_t TimeWait = 0;
    uint8_t i;  
    while((++TimeWait<_time_wait))
    {
        SIM_Function.SIM_Delay(1);
        for(i=0;i<SIM_SIZE_BUFFER;i++)
        {
          if((SimBuffer[i].Flags&SIM_BUF_PK_VALID)!=0)
          {
            if(NULL!=SearchArrayInArray(SimBuffer[i].Buffer,Respond,1,SimBuffer[i].IndexByteInBuffer))
            {
              SimBuffer[i].Flags = 0;
              return SIM_OK;
            }
          }
        }
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
void SIM_SendCommand(char *Command)
{
    SIM_ClearFlags(SIM_SEND_FLAG_OK);
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
/**
  * Khởi động lại Module Sim
  * Trong Đó: _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM_Reset(uint32_t _timeout)
{
  SIM_SendCommand("AT+CRESET");
  return (SIM_WaitForRespond("OK\r\n",_timeout));
}
/**
  * Cấu hình thông báo khi nhận được tin nhắn mới
  * Trong đó    :   _timeout: Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
static SIM_STATUS SIM_MessagesIndications(uint32_t _timeout)
{
	SIM_SendCommand("AT+CNMI=2,1");
  return (SIM_WaitForRespond("OK\r\n",_timeout));
}
/*
  * Lệnh gọi điện đến 1 số điện thoại
  * Trong đó: *PhoneNumber: Là số điện thoại cần gọi
              _timeout    : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
*/
SIM_STATUS SIM_Call(uint8_t *PhoneNumber,uint32_t _timeout)
{
  char Cmd[30];
  sprintf(Cmd,"ATD%s;",PhoneNumber);
  SIM_SendCommand(Cmd);
  return SIM_WaitForRespond("OK\r\n",_timeout);
}

/**
  * Lênh ngắt cuộc gọi đến
**/
void SIM_HangUpCall(void)
{
	SIM_SendCommand("ATH");
}

/**
  * Gửi tin nhắn văn bản tới 1 số điện thoại
  * Trong đó: *PhoneNumber: Số điện thoại cần gửi đến
              *Messages   : Nội dung tin nhắn
              _timeout    : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/

SIM_STATUS SIM_SendMessages(char *PhoneNumber, char *Messages,uint32_t _timeout)
{
  SIM_STATUS  _status;
  char Cmd[30];
  sprintf(Cmd,"AT+CMGS=\"%s\"",PhoneNumber);
  SIM_SendCommand(Cmd);
  _status = SIM_WaitForRespond("OK\r\n",_timeout);
  if(_status==SIM_OK)
  {
    SIM_Function.SIM_SendString(Messages);
    SIM_Function.SIM_Delay(100);
    SIM_Function.SIM_Sendchar(0x1A);    // Ctr+Z
    _status = SIM_WaitForRespond("OK\r\n",_timeout);
  }
  return _status;
}

/**
  * Đọc tin nhắn nhận được
  * Trong Đó: IndexInBufferSim: Vị trí của tin nhắn trong bộ nhớ
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM_ReadMessages(uint8_t IndexInBufferSim,uint32_t _timeout)
{
	char Cmd[20];
	sprintf(Cmd,"AT+CMGR=%u",IndexInBufferSim);
	SIM_SendCommand(Cmd);
  return (SIM_WaitForRespond("OK\r\n",_timeout));
}
/**
  * Xóa tin nhắn nhận được
  * Trong Đó: IndexInBufferSim: Vị trí của tin nhắn trong bộ nhớ
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM900_DeleteMessages(uint8_t IndexInBufferSim,uint32_t _timeout)
{
	char Cmd[20];
	sprintf(Cmd,"AT+CMGD=%u",IndexInBufferSim);
	SIM_SendCommand(Cmd);
  return (SIM_WaitForRespond("OK\r\n",_timeout));
}

/**
  * Lấy số điện thoại từ tin nhắn gửi đến
  * Trong đó  :   _buffer         :  Bộ đệm dữ liệu tin nhắn nhận được
                  *_sender_number :  Số điện thoại gửi tin nhắn đến
  * Trả về    :   0               :  Lấy Số Điện Thoại Không Thành Công
                  !=0             :  Lấy Số Điện Thoại Thành Công
  * Cấu Trúc: 
          +CMGR: "REC UNREAD","+841234567890",,"21:07:0310:50:55
**/
uint8_t SIM_GetSenderNumber(SIM_BUFFER* _buffer,SIM_PHONE_NUMBER*_sender_number)
{
    char* Start,*Stop,i=0;
    Start = SearchArrayInArray(_buffer->Buffer,"\",\"",1,_buffer->IndexByteInBuffer); // Tìm vị trí bắt đầu
    Stop  = SearchArrayInArray(_buffer->Buffer,"\",\"",2,_buffer->IndexByteInBuffer); // Tìm Vị trí kết thúc
    if((Start==NULL)|(Stop==NULL))
    {
        return 0;
    }
    Start+=3;// Bỏ qua các kỹ tự " , "
    while(Start<Stop)
    {
        _sender_number->PhoneNumber[i++] = *Start++; // Tách lấy số điện thoại trong khoảng Start --- Stop
    }
    _sender_number->PhoneNumber[i++] = 0;
    return 1;
}

/**
  * [Tạm thời chỉ hỗ trợ ở chế độ nhận theo DMA]
  * Lấy nội dung tin nhắn đến
  * Trong đó   :   _buffer        :   Bộ đệm dữ liệu tin nhắn gửi đến
                  *_content       :   Nội dung tin nhắn sau khi đã được xử lý
  * Trả về     :  0               :   Xử lý nội dung tin nhắn thất bại
                  !=0             :   Xử lý nội dung tin nhắn thành công
**/
uint8_t SIM_GetContent(SIM_BUFFER* _buffer,char* _content)
{
    char*Start,*Stop,i=0;
    Start = SearchArrayInArray(_buffer->Buffer,"\"\r\n",1,_buffer->IndexByteInBuffer);//Tìm "<CR><LF>
    Stop  = &(_buffer->Buffer[_buffer->IndexByteInBuffer-2]);
    if((Start==NULL)|(Stop==NULL))
    {
        return 0;
    }
    Start+=3;//Bỏ Qua "<CR><LF>;
    while(Start<Stop)
    {
        _content[i++] = *Start++;
    }
    _content[i] = 0;
    return 1;
}

/**
  * Kiểm tra chất lượng của tín hiệu
  * Trong Đó: *rssi:  Giá trị cường độ tín hiệu sim phản hồi
              *ber (Bit Error): Giá trị bit lỗi sim phản hồi
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM_QuerySignalQuality(uint8_t* _rssi,uint8_t* _ber,uint32_t _timeout)
{
  SIM_STATUS _status;
  uint8_t __rssi[5],__ber[5];
  uint16_t TimeWait = 0;
  uint8_t Start,Stop,index=0;
  uint8_t i;  

  SIM_SendCommand("AT+CSQ");
  _status = SIM_WaitForRespond("OK",_timeout); // Nếu phả hồi OK thì lấy giá trị
  if(_status==SIM_OK)
  {
    while((++TimeWait<_timeout))
    {
      SIM_Function.SIM_Delay(1);
      for(i=0;i<SIM_SIZE_BUFFER;i++)
      {
        if((SimBuffer[i].Flags&SIM_BUF_PK_VALID)!=0)
        {
          SimBuffer[i].Flags = 0;
          if(SearchArrayInArray(SimBuffer[i].Buffer,"+CSQ:",1,SimBuffer[i].IndexByteInBuffer)!=NULL)
          {
            // Tách dữ liệu RSSI
            Start = SearchByteInArray(':',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop  = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm Vị trí kết thúc
            Start+=2;// Bỏ qua các kỹ tự ": "
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __rssi[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_rssi = (__rssi[0]-0x30)*10 + (__rssi[1]-0x30);  
            // Tách dữ liệu BER
            Start = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop = SimBuffer[i].IndexByteInBuffer;
            Start+=1;// Bỏ qua các kỹ tự ","
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __ber[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_ber = (__ber[0]-0x30)*10 + (__ber[1]-0x30);
          }
        }
      }
    }
  }

  return _status;
}

/**
  * Đăng ký mạng
  * Trong Đó: _value:  Giá trị đang ký mạng
              _status : Trạng thái mạng
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/

SIM_STATUS SIM_NetworkResistration(uint8_t* _value,uint8_t* _sta_,uint32_t _timeout)
{
  SIM_STATUS _status;
  uint8_t __value[5],__sta[5];
  uint16_t TimeWait = 0;
  uint8_t Start,Stop,index=0;
  uint8_t i;  

  SIM_SendCommand("AT+CREG?");
  _status = SIM_WaitForRespond("OK",_timeout); // Nếu phả hồi OK thì lấy giá trị
  if(_status==SIM_OK)
  {
    while((++TimeWait<_timeout))
    {
      SIM_Function.SIM_Delay(1);
      for(i=0;i<SIM_SIZE_BUFFER;i++)
      {
        if((SimBuffer[i].Flags&SIM_BUF_PK_VALID)!=0)
        {
          SimBuffer[i].Flags = 0;
          if(SearchArrayInArray(SimBuffer[i].Buffer,"+CREG:",1,SimBuffer[i].IndexByteInBuffer)!=NULL)
          {
            // Tách dữ liệu RSSI
            Start = SearchByteInArray(':',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop  = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm Vị trí kết thúc
            Start+=2;// Bỏ qua các kỹ tự ": "
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __value[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_value = __value[0]-0x30;  
            // Tách dữ liệu BER
            Start = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop = SimBuffer[i].IndexByteInBuffer;
            Start+=1;// Bỏ qua các kỹ tự ","
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __sta[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_sta_ = __sta[0]-0x30;
          }
        }
      }
    }
  }
  return _status;
}

/**
  * Tạo môi trường PDP
  * Trong Đó: _value:  Giá trị đang ký mạng
              _status : Trạng thái mạng
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/

SIM_STATUS SIM_NetworkResistration(uint8_t* _value,uint8_t* _sta_,uint32_t _timeout)
{
  SIM_STATUS _status;
  uint8_t __value[5],__sta[5];
  uint16_t TimeWait = 0;
  uint8_t Start,Stop,index=0;
  uint8_t i;  

  SIM_SendCommand("AT+CREG?");
  _status = SIM_WaitForRespond("OK",_timeout); // Nếu phả hồi OK thì lấy giá trị
  if(_status==SIM_OK)
  {
    while((++TimeWait<_timeout))
    {
      SIM_Function.SIM_Delay(1);
      for(i=0;i<SIM_SIZE_BUFFER;i++)
      {
        if((SimBuffer[i].Flags&SIM_BUF_PK_VALID)!=0)
        {
          SimBuffer[i].Flags = 0;
          if(SearchArrayInArray(SimBuffer[i].Buffer,"+CREG:",1,SimBuffer[i].IndexByteInBuffer)!=NULL)
          {
            // Tách dữ liệu RSSI
            Start = SearchByteInArray(':',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop  = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm Vị trí kết thúc
            Start+=2;// Bỏ qua các kỹ tự ": "
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __value[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_value = __value[0]-0x30;  
            // Tách dữ liệu BER
            Start = SearchByteInArray(',',SimBuffer[i].Buffer,1,SimBuffer[i].IndexByteInBuffer); // Tìm vị trí bắt đầu
            Stop = SimBuffer[i].IndexByteInBuffer;
            Start+=1;// Bỏ qua các kỹ tự ","
            for(uint8_t index=0;index<Stop-Start;index++)
            {
              __sta[index] = SimBuffer[i].Buffer[index+Start]; // Tách lấy RSSI trong khoảng Start --- Stop
            }
            *_sta_ = __sta[0]-0x30;
          }
        }
      }
    }
  }
  return _status;
}

/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ====================== Cấu Hình SIM Theo Ứng Dụng Cụ Thể ===================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/**
  * Cấu hình SIM ở chế độ đơn giản, Sử dụng Call & Messenger
  * _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM_InitBasic(uint32_t _timeout)
{ 
  SIM_STATUS _status;

  SIM_SendCommand("AT+CREG=1");//Enable Network
  _status = SIM_WaitForRespond("OK\r\n",_timeout);
  if(_status == SIM_OK)
  {
    SIM_SendCommand("AT+CMGF=1"); // Cấu hình tin nhắn ở dạng Text mode
    _status = SIM_WaitForRespond("OK\r\n",_timeout);
  }
  if(_status == SIM_OK)
  {
    SIM_SendCommand("AT+CLIP=1");// Cấu hình thông báo cuộc gọi đến
    _status =SIM_WaitForRespond("OK\r\n",_timeout);
  }
  if(_status == SIM_OK)
    _status = SIM_MessagesIndications(_timeout);  // Cấu hình thông báo tin nhắn đến

  return _status;
}


/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================== Cấu Hình SIM Chạy TCP IP =============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
SIM_STATUS SIM_NetworkEnvironment (void)
{
  
}