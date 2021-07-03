
============= Xử Lý Cờ Của SIM ===========

SIM_FLAGS SIM_GetFlags(SIM_FLAGS _flags);
void SIM_SetFlags(SIM_FLAGS _flags);
void SIM_ClearFlags(SIM_FLAGS _flags);

======== Xử Lý Chuỗi Trong Gói Tin =====

char* SearchArrayInArray(char* _string_source,char* _string_search,uint8_t _time, uint16_t _length);
char* SearchByteInArray(char CharNeedSerach, char *_string_search,uint8_t TimeAppear,uint8_t MaxLenghtBuffer);

======= Xử Lý Dữ Liệu Nhận Từ SIM =======

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


===== Tập Lệnh Điều Khiển SIM Qua AT Command =====

SIM_STATUS SIM_Call(uint8_t *PhoneNumber,uint32_t _timeout);
void SIM_HangUpCall(void);
SIM_STATUS SIM_SendMessages(char *PhoneNumber, char *Messages,uint32_t _timeout);
SIM_STATUS SIM_ReadMessages(uint8_t IndexInBufferSim,uint32_t _timeout);
SIM_STATUS SIM900_DeleteMessages(uint8_t IndexInBufferSim,uint32_t _timeout);
uint8_t SIM_GetSenderNumber(SIM_BUFFER* _buffer,SIM_PHONE_NUMBER*_sender_number);
uint8_t SIM_GetContent(SIM_BUFFER* _buffer,char* _content);