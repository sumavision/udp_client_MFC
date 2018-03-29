

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

//控制帧各字段长度/BYLES
#define FRAME_START_LEN             1
#define FRAME_DEV_LEN               2
#define FRAME_OACK_LEN              1
#define FRAME_SGET_LEN              1
#define FRAME_ID_LEN                4
#define FRAME_DATA_LEN              37
#define FRAME_CHECK_LEN             1
#define FRAME_END_LEN               1

#define CTRL_FRAME_LEN              48

//控制帧各字段固定值
#define FRAME_START_VAL             0x5a

#define FRAME_ORDER_VAL             0x6a
#define FRAME_ACK_VAL               0x56
#define FRAME_ERROR_VAL             0x77

#define FRAME_SET_VAL               0x01
#define FRAME_GET_VAL               0x02

#define FRAME_END_VAL               0x54

//控制帧中设备ID 广播地址
#define CF_BROADCAST_ID             0x0000


//组合参数查询/设置 参数ID
#define CF_COMBINE_PARAM_ID         0xF001

typedef enum
{
	CTRL_ERROR_START,
	CTRL_ERROR_DEVID,
	CTRL_ERROR_OACK,
	CTRL_ERROR_SGET,
	CTRL_ERROR_ID,
	CTRL_ERROR_CHECK,
	CTRL_ERROR_END,
	CTRL_ANL_OK
}Ctrl_Anl_Error;

//控制帧定义//注意对齐问题
typedef struct _CTRL_FRAME_
{
	char ctrl_start;
	unsigned short ctrl_dev_id;
	char ctrl_order_ack;
	char ctrl_set_get;
	unsigned int ctrl_id;
	char ctrl_data[FRAME_DATA_LEN];
	char ctrl_check;
	char ctrl_end;
}CTRL_FRAME;


//extern int convert_string_to_ctrl_frame(char* MsgStr,char *buff,int MsgLen);
//extern int convert_ctrl_frame_to_string(char *buff);
extern int ctrl_frame_convert_result(int state);
extern int get_snd_msg(char* MsgInputStr,char *buff,int MsgLen);
extern int get_server_msg(char *buffer,int Recvlen);
