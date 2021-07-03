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
SIM_FLAGS SIM_GetFlags(SIM_FLAGS _flags);

/*
  * Thiết lập trạng thái các cờ của SIM
  * SIM_SEND_FLAG_OK               0x01
  * SIM_FLAG_SEND_COMMAND          0x02
  * SIM_HAVE_NEW_SMS               0x04
  * SIM_SEND_SMS_OK                0x08
*/
void SIM_SetFlags(SIM_FLAGS _flags);

/*
  * Xóa trạng thái các cờ của SIM
  * SIM_SEND_FLAG_OK               0x01
  * SIM_FLAG_SEND_COMMAND          0x02
  * SIM_HAVE_NEW_SMS               0x04
  * SIM_SEND_SMS_OK                0x08
*/
void SIM_ClearFlags(SIM_FLAGS _flags);


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
char* SearchArrayInArray(char* _string_source,char* _string_search,uint8_t _time, uint16_t _length);

/************************************************************************************
 * Tìm 1 ký tự trong chuỗi nguồn.
 * Trong đó    CharNeedSerach    :    Ký tự cần tìm
               _string_search    :    Chuỗi cần tìm (chuỗi nguồn)
               TimeAppear        :    Số lần xuất hiện của ký tự trong chuỗi nguồn
               MaxLenghtBuffer   :    Độ dài của chuỗi nguồn
 * Trả Về      =MaxLenghtBuffer  :    Khi không tìm thấy kỹ tự trong chuỗi nguồn
               <MaxLenghtBuffer  :    Vị trí ký tự trong chuỗi nguồn.
************************************************************************************/
char* SearchByteInArray(char CharNeedSerach, char *_string_search,uint8_t TimeAppear,uint8_t MaxLenghtBuffer);

/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ========================== Xử Lý Dữ Liệu Nhận Từ SIM ============================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*
  * Nhận 1 gói dữ liệu từ sim gửi xuống 
  * Mỗi 1 byte nhận được sẽ được điều vào bộ đệm để xử lý sau
  * Khi phát hiện chuỗi kỹ tự kết thúc gói "0D 0A" thì đóng gói dữ liệu vào bộ đệm
*/
void SIM_InByte (uint8_t _byte);

/*
  * Ghi 1 mảng dữ liệu vào bộ đệm.
  * Trong đó: _buffer: chứa mảng dữ liệu nhận được
              Leng: Độ dài của mảng dữ liệu
*/
void SIM_InBuffer (uint8_t* _buffer,uint16_t Leng);

/*
  * Chuyển sang bộ đệm tiếp theo để ghi thêm dữ liệu mới
*/
void SIM_NextBuffer (void);

/*
  Phát hiện kết thúc chuỗi dữ liệu
*/
void SIM_ReceiveData (uint8_t _byte);
/*
  * Nhận dữ liệu RX bằng DMA (Hỗ chợ 1 số dòng chip có chế độ IDLE hoặc DMA)
*/
void SIM_ReceiveDMA (uint8_t* _buffer,uint16_t Leng);

/*
  * Cho tin hieu tra ve tu SIM900.
  * _respond    :    Tin hieu tra ve.
  * _time_wait  :    Thoi gian cho toi da.
  * Tra Ve      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
********************************************************************************/
SIM_STATUS SIM_WaitForRespond(char *Respond,uint16_t _time_wait);

/*
  * Reset bộ đệm chứa dữ liệu nhận được từ SIM
*/
void SIM_ResetBuffer(SIM_BUFFER* _buffer);

/*
  * Phát hiện tìm kiếm gói tin mới nhận được và tiến hàng xử lý.
*/
void SIM_DetectPacket(void);

/*
  * Gửi lệnh xuống SIM
  * Lệnh là 1 chuỗi dạng char cần bao gồm 0D và 0A
*/
void SIM_SendCommand(char *Command);

/*
  * Xử lý gói tin khi nhận được.
*/

void SIM_ProcessNewPacket(SIM_BUFFER *_buffer);


/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  ====================== Tập Lệnh Điều Khiển SIM Qua AT Command ===================
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/**
  * Cấu hình thông báo khi nhận được tin nhắn mới
  * Trong đó    :   _timeout: Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
static SIM_STATUS SIM_MessagesIndications(uint32_t _timeout);
/*
  * Lệnh gọi điện đến 1 số điện thoại
  * Trong đó: *PhoneNumber: Là số điện thoại cần gọi
              _timeout    : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
*/
SIM_STATUS SIM_Call(uint8_t *PhoneNumber,uint32_t _timeout);

/**
  * Lênh ngắt cuộc gọi đến
**/
void SIM_HangUpCall(void);

/**
  * Gửi tin nhắn văn bản tới 1 số điện thoại
  * Trong đó: *PhoneNumber: Số điện thoại cần gửi đến
              *Messages   : Nội dung tin nhắn
              _timeout    : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/

SIM_STATUS SIM_SendMessages(char *PhoneNumber, char *Messages,uint32_t _timeout);

/**
  * Đọc tin nhắn nhận được
  * Trong Đó: IndexInBufferSim: Vị trí của tin nhắn trong bộ nhớ
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM_ReadMessages(uint8_t IndexInBufferSim,uint32_t _timeout);

/**
  * Xóa tin nhắn nhận được
  * Trong Đó: IndexInBufferSim: Vị trí của tin nhắn trong bộ nhớ
              _timeout        : Thời gian chờ phản hồi từ sim
  * Trả về      :   SIM900_OK          :    Có respond trả về
                    SIM900_TIME_OUT    :    Quá thời gian _time_wait mà không có respond trả về
**/
SIM_STATUS SIM900_DeleteMessages(uint8_t IndexInBufferSim,uint32_t _timeout);

/**
  * Lấy số điện thoại từ tin nhắn gửi đến
  * Trong đó  :   _buffer         :  Bộ đệm dữ liệu tin nhắn nhận được
                  *_sender_number :  Số điện thoại gửi tin nhắn đến
  * Trả về    :   0               :  Lấy Số Điện Thoại Không Thành Công
                  !=0             :  Lấy Số Điện Thoại Thành Công
  * Cấu Trúc: 
          +CMGR: "REC UNREAD","+841234567890",,"21:07:0310:50:55
**/
uint8_t SIM_GetSenderNumber(SIM_BUFFER* _buffer,SIM_PHONE_NUMBER*_sender_number);
/**
  * [Tạm thời chỉ hỗ trợ ở chế độ nhận theo DMA]
  * Lấy nội dung tin nhắn đến
  * Trong đó   :   _buffer        :   Bộ đệm dữ liệu tin nhắn gửi đến
                  *_content       :   Nội dung tin nhắn sau khi đã được xử lý
  * Trả về     :  0               :   Xử lý nội dung tin nhắn thất bại
                  !=0             :   Xử lý nội dung tin nhắn thành công
**/
uint8_t SIM_GetContent(SIM_BUFFER* _buffer,char* _content)