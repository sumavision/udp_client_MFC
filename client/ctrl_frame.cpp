
#include "stdafx.h"
#include "ctrl_frame.h"

int ctrl_frame_convert_result(int state)
{
	int ret = state;
	
	switch (ret)
	{
		case CTRL_ERROR_START:
			AfxMessageBox(_T("��ʼ�ֶδ���"));
			break;
		case CTRL_ERROR_DEVID:
			AfxMessageBox(_T("�豸ID�ֶδ���"));
			break;
		case CTRL_ERROR_OACK:
			AfxMessageBox(_T("����/Ӧ���ֶδ���"));
			break;
		case CTRL_ERROR_SGET:
			AfxMessageBox(_T("����/��ѯ�ֶδ���"));
			break;
		case CTRL_ERROR_CHECK:
			AfxMessageBox(_T("У���ֶδ���"));
			break;
		case CTRL_ERROR_END:
			AfxMessageBox(_T("�����ֶδ���"));
			break;
		default :
			ret = CTRL_ANL_OK;
			break;
	}

	return ret;
}
int convert_ctrl_frame_to_string(char *MsgCF,char *MsgOutStr)
{
	CTRL_FRAME ctrl_opt;
	int Anl_Len = 0;
	int i = 0;
	int DataLen = 0;
	char TCheck = 0;
	unsigned int CheckVal = 0;

	unsigned char Val;

	char buff[100] = {0};

	char string_flag = 0;
	int count = 0;

	memset((char*)&ctrl_opt,0,sizeof(CTRL_FRAME));

	//��ʼ�ֶ�
	ctrl_opt.ctrl_start = *MsgCF;
	if (ctrl_opt.ctrl_start != FRAME_START_VAL)
	{
		return CTRL_ERROR_START;
	}
	Anl_Len            = sizeof(char);

	//�豸ID,��0x1000~0xfffe֮�䣻0x0000Ϊ�㲥��ַ��0x0001~0x0fff����
	ctrl_opt.ctrl_dev_id = *((unsigned short*)(MsgCF+Anl_Len));
	if (!((0x0fff< ctrl_opt.ctrl_dev_id) && (ctrl_opt.ctrl_dev_id < 0xffff)
		   ||(ctrl_opt.ctrl_dev_id == CF_BROADCAST_ID)))
	{
		return CTRL_ERROR_DEVID;
	}
	Anl_Len              += sizeof(unsigned short);

	//����/Ӧ��
	ctrl_opt.ctrl_order_ack = *(MsgCF+Anl_Len);
	if(ctrl_opt.ctrl_order_ack != FRAME_ACK_VAL && ctrl_opt.ctrl_order_ack != FRAME_ERROR_VAL) 
	{
		return CTRL_ERROR_OACK;
	}
	Anl_Len += sizeof(char);

	//����/��ѯ
	ctrl_opt.ctrl_set_get = *(MsgCF+Anl_Len);
	if(ctrl_opt.ctrl_set_get != FRAME_SET_VAL && ctrl_opt.ctrl_set_get != FRAME_GET_VAL 
		&&ctrl_opt.ctrl_order_ack != FRAME_ERROR_VAL) 
	{
		return CTRL_ERROR_SGET;
	}
	Anl_Len += sizeof(char);

	//����id
	ctrl_opt.ctrl_id = *((unsigned int*)(MsgCF+Anl_Len));
	Anl_Len += sizeof(unsigned int);

	if (ctrl_opt.ctrl_id == 0x0009 ||ctrl_opt.ctrl_id == 0x000a||ctrl_opt.ctrl_id == 0x000b
		||ctrl_opt.ctrl_id == 0x0407)
	{
		string_flag = 1;
	}

	//����
	memcpy(ctrl_opt.ctrl_data,(MsgCF+Anl_Len),FRAME_DATA_LEN);
	Anl_Len += FRAME_DATA_LEN;

	//����У��ֵ
	for (i = 0;i < Anl_Len;i++)
	{
		TCheck = *(MsgCF+i);
		CheckVal += TCheck;
	}

	//У��
	ctrl_opt.ctrl_check = *(MsgCF+Anl_Len);
	if (ctrl_opt.ctrl_check !=(char)(CheckVal&0xff)&&ctrl_opt.ctrl_order_ack != FRAME_ERROR_VAL)
	{
		return CTRL_ERROR_CHECK;
	}
	Anl_Len += sizeof(char);

	//�����ֶ�
	ctrl_opt.ctrl_end = *(MsgCF+Anl_Len);
	if(ctrl_opt.ctrl_end != FRAME_END_VAL)
	{
		return CTRL_ERROR_END;
	}
	Anl_Len += sizeof(char);


	//ת�����ַ������������λ��
	memset(buff,0,sizeof(buff));
	Anl_Len = 0;
	
	sprintf(buff+Anl_Len,"%02X",ctrl_opt.ctrl_start);
	Anl_Len = sizeof(char)*2;

	//�����ʾ
	ctrl_opt.ctrl_dev_id = ntohs(ctrl_opt.ctrl_dev_id);
	sprintf(buff+Anl_Len,"%04X",ctrl_opt.ctrl_dev_id);
	Anl_Len += sizeof(unsigned short)*2;

	sprintf(buff+Anl_Len,"%02X",ctrl_opt.ctrl_order_ack);
	Anl_Len += sizeof(char)*2;

	sprintf(buff+Anl_Len,"%02X",ctrl_opt.ctrl_set_get);
	Anl_Len += sizeof(char)*2;

	//�����ʾ
	ctrl_opt.ctrl_id = ntohl(ctrl_opt.ctrl_id);
	sprintf(buff+Anl_Len,"%08X",ctrl_opt.ctrl_id);
	Anl_Len += sizeof(unsigned int)*2;

	//����Ӧ���Լ� ����汾��Ӳ���汾�����кš�osd�����ַ�����ʽ ��ʾ
	if (ctrl_opt.ctrl_set_get == FRAME_SET_VAL||string_flag == 1)
	{
		DataLen = strlen(ctrl_opt.ctrl_data);
		sprintf(buff+Anl_Len,"%s",ctrl_opt.ctrl_data);
		Anl_Len += DataLen;

		for (i = 0;i< FRAME_DATA_LEN - DataLen;i++)
		{
			sprintf(buff+Anl_Len+i*2,"%02X",(unsigned char)ctrl_opt.ctrl_data[DataLen+i]);
		}
		Anl_Len += (FRAME_DATA_LEN - DataLen)*2;
	}
	else if (ctrl_opt.ctrl_set_get == FRAME_GET_VAL ||ctrl_opt.ctrl_order_ack == FRAME_ERROR_VAL)
	{
		for (i = 0;i< FRAME_DATA_LEN;i++)
		{
			sprintf(buff+Anl_Len+i*2,"%02X",(unsigned char)ctrl_opt.ctrl_data[i]);
		}
		Anl_Len += FRAME_DATA_LEN*2;
	}

	//�ֽڶ��룬��ǿ��ת��
	sprintf(buff+Anl_Len,"%02X",(unsigned char)ctrl_opt.ctrl_check);
	Anl_Len += sizeof(char)*2;
	
	sprintf(buff+Anl_Len,"%02X",(unsigned char)ctrl_opt.ctrl_end);
	Anl_Len += sizeof(char)*2;

	//ÿ�����ֽں� ��ӿո�
	int DataAsideLen = (FRAME_ID_LEN + FRAME_SGET_LEN + FRAME_OACK_LEN + FRAME_DEV_LEN + FRAME_START_LEN)*2;
	for(i = 0;i < Anl_Len;i++)
	{
		//�ַ�����ʾ�ֶβ��ӿո�
		if (DataLen != 0 && (DataAsideLen <= i&&i < (DataAsideLen+DataLen)))
		{
			strncpy((MsgOutStr+count),(buff+i),DataLen);
			i += DataLen;
			count += DataLen;

			MsgOutStr[count] = ' ';
			count++;
			continue;
		}
		MsgOutStr[count] = buff[i];
		count++;
		
		if(i%2 != 0)
		{
			MsgOutStr[count] = ' ';
			count++;
		}
	}
	
	return count;

}

int convert_string_to_ctrl_frame(char* MsgInputStr,char *buff )
{
	CTRL_FRAME ctrl_opt;
	int len = 0;
	int Anl_Len = 0;
	int i = 0;
	int Temp = 0;
	char TCheck = 0;
	unsigned int CheckVal = 0;
	char MsgStr[100] = {0};
	int count = 0;
#if 0
	//ɾȥ�ո�
	for(i = 0;i < MsgLen;i++)
	{
		if(MsgInputStr[i] != ' ')
		{
			MsgStr[count] = MsgInputStr[i];
			count++;
		}
	}
#endif 
	memcpy(MsgStr,MsgInputStr,CTRL_FRAME_LEN*2);
	//��ʼ�ֶ�
	sscanf((MsgStr + Anl_Len),"%2x",&Temp);
	ctrl_opt.ctrl_start = (char)(Temp&0xff);
	if (ctrl_opt.ctrl_start != FRAME_START_VAL)
	{
		return CTRL_ERROR_START;
	}
	Anl_Len = sizeof(char)*2;
	//�豸ID,��0x1000~0xfffe֮�䣻0x0000Ϊ�㲥��ַ��0x0001~0x0fff����
	sscanf((MsgStr+ Anl_Len),"%4x",&Temp);
	ctrl_opt.ctrl_dev_id = (unsigned short)(Temp&0xffff);
	//ctrl_opt.ctrl_dev_id = ntohs(ctrl_opt.ctrl_dev_id);
	if (!((0x0fff< ctrl_opt.ctrl_dev_id) && (ctrl_opt.ctrl_dev_id < 0xffff)
		   ||(ctrl_opt.ctrl_dev_id == CF_BROADCAST_ID)))
	{
		return CTRL_ERROR_DEVID;
	}
	Anl_Len +=  sizeof(unsigned short)*2;
	//����/Ӧ��
	sscanf((MsgStr+ Anl_Len),"%2x",&Temp);
	ctrl_opt.ctrl_order_ack = (char)(Temp&0xff);
	if(ctrl_opt.ctrl_order_ack != FRAME_ORDER_VAL) 
	{
		return CTRL_ERROR_OACK;
	}
	Anl_Len += sizeof(char)*2;
	//����/��ѯ
	sscanf((MsgStr+ Anl_Len),"%2x",&Temp);
	ctrl_opt.ctrl_set_get = (char)(Temp&0xff);
	if(ctrl_opt.ctrl_set_get != FRAME_SET_VAL && ctrl_opt.ctrl_set_get != FRAME_GET_VAL) 
	{
		return CTRL_ERROR_SGET;
	}
	Anl_Len += sizeof(char)*2;
	//����id
	sscanf((MsgStr+ Anl_Len),"%8x",&ctrl_opt.ctrl_id);
	//ctrl_opt.ctrl_id = ntohl(ctrl_opt.ctrl_id);
	Anl_Len += sizeof(unsigned int)*2;

	//����
	for (i = 0;i < FRAME_DATA_LEN;i++)
	{
		sscanf((MsgStr+ Anl_Len + i*2),"%2x",&Temp);
		ctrl_opt.ctrl_data[i] = (char)(Temp&0xff);
	}
	Anl_Len += FRAME_DATA_LEN*2;

	//����У��ֵ
	for (i = 0;i < Anl_Len/2;i++)
	{
		sscanf((MsgStr + i*2),"%2x",(char*)&Temp);
		TCheck = (char)(Temp&0xff);
		CheckVal += TCheck;
	}
	//У��
	sscanf((MsgStr+ Anl_Len),"%2x",&Temp);
	ctrl_opt.ctrl_check = (char)(Temp&0xff);
	if (ctrl_opt.ctrl_check != (char)(CheckVal&0xff))
	{
		return CTRL_ERROR_CHECK;
	}
	Anl_Len += sizeof(char)*2;
	//�����ֶ�
	sscanf((MsgStr+ Anl_Len),"%2x",&Temp);
	ctrl_opt.ctrl_end = (char)(Temp&0xff);
	if(ctrl_opt.ctrl_end != FRAME_END_VAL)
	{
		return CTRL_ERROR_END;
	}
	Anl_Len += sizeof(char)*2;

	//�ֽڶ��뵼�²��� ����
	//memcpy(buff,(char*)&ctrl_opt,Anl_Len/2);
	Anl_Len = 0;
	buff[Anl_Len] = ctrl_opt.ctrl_start;
	Anl_Len++;
	
	//�豸ID ��˴���
	buff[Anl_Len] = ctrl_opt.ctrl_dev_id>>8;
	Anl_Len++;
	buff[Anl_Len] = ctrl_opt.ctrl_dev_id&0xff;
	Anl_Len++;

	buff[Anl_Len] = ctrl_opt.ctrl_order_ack;
	Anl_Len++;

	buff[Anl_Len] = ctrl_opt.ctrl_set_get;
	Anl_Len++;

	//����ID ��˴���
	buff[Anl_Len] = ctrl_opt.ctrl_id>>24;
	Anl_Len++;
	buff[Anl_Len] = (ctrl_opt.ctrl_id>>16)&0xff;
	Anl_Len++;
	buff[Anl_Len] = (ctrl_opt.ctrl_id>>8)&0xff;
	Anl_Len++;
	buff[Anl_Len] = ctrl_opt.ctrl_id&0xff;
	Anl_Len++;

	memcpy((buff+Anl_Len),ctrl_opt.ctrl_data,FRAME_DATA_LEN);
	Anl_Len += FRAME_DATA_LEN;

	buff[Anl_Len] = ctrl_opt.ctrl_check;
	Anl_Len++;

	buff[Anl_Len] = ctrl_opt.ctrl_end;
	Anl_Len++;
	
	return Anl_Len;
}

 int start_sec_get(char *buff,int SFirst,int MaxSize)
{
	int i = 0;
	
	//��Ѱ��ʼ�ֶ�
	for (i = SFirst;i < MaxSize;i ++)
	{
		if (buff[i] == FRAME_START_VAL)
		{
			if (i + CTRL_FRAME_LEN > MaxSize)
			{
				return -1;
			}
			//�ж���Ӧ�������Ƿ���ȷ
			if (buff[i + CTRL_FRAME_LEN - 1] != FRAME_END_VAL)
			{
				//����ʼ��λ�ô��󣬶���
				continue;
			}
			else 
			{
				//����ʼ��λ��ok,ֹͣ��Ѱ
				break;
			}
		}
	}

	if (i == MaxSize)
	{
		return -1;
	}

	return i;
}
 
int get_server_msg(char *buffer,int Recvlen)
{
	int ConvertLen = 0;
	int count = 0;
	int TmpSize = 0;
	int StartPos = 0;
	int CmdNum = 0;
	int ret = 1;

	int DisLen = 0;

	char *pRecvBuff = NULL;
	
	char MsgCF[1024] = {0};

	pRecvBuff = buffer;
	memcpy(MsgCF,pRecvBuff,Recvlen);
	while(TmpSize < Recvlen)
	{
		StartPos = start_sec_get(MsgCF,TmpSize,Recvlen);
		if (StartPos < 0)
		{
			break;
		}
		TmpSize = StartPos;
		ConvertLen = convert_ctrl_frame_to_string((MsgCF+TmpSize),pRecvBuff);
		if(ctrl_frame_convert_result(ConvertLen) != CTRL_ANL_OK)
		{
			ret = -1;
			break;
		}

		pRecvBuff += ConvertLen;
		TmpSize += CTRL_FRAME_LEN;
		CmdNum++;

		DisLen += ConvertLen;
	}

	if (ret < 0)
	{
		return -1;
	}

	return DisLen;
}

int get_snd_msg(char* MsgInputStr,char *buff,int MsgLen)
{
	int i = 0;
	int count = 0;
	int TmpSize = 0;
	int StartPos = 0;
	int CmdNum = 0;
	int ret = 1;
	int ConvertLen = 0;
	int SndLen = 0;

	char *pSndBuff = NULL;

	char MsgStr[1024] = {0};
	
	//ɾȥ�ո�
	for(i = 0;i < MsgLen;i++)
	{
		if(MsgInputStr[i] != ' ')
		{
			MsgStr[count] = MsgInputStr[i];
			count++;
		}
	}

	pSndBuff = buff;
	while(TmpSize < count-1)
	{
		ConvertLen = convert_string_to_ctrl_frame((MsgStr+TmpSize),pSndBuff);
		if(ctrl_frame_convert_result(ConvertLen) != CTRL_ANL_OK)
		{
			ret = -1;
			break;
		}

		pSndBuff += ConvertLen;
		TmpSize += CTRL_FRAME_LEN*2;
		CmdNum++;

		SndLen += ConvertLen;
	}

	if (ret < 0)
	{
		return -1;
	}

	return SndLen;

}

