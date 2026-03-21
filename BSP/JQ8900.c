#include "JQ8900.h"
#include "string.h"
#include "delay.h"
//可根据需求更换串口
extern UART_HandleTypeDef huart2;
extern uint8_t music_flag;
extern struct Info cradle;
struct Record_Info rec;

#define JQ8900_UART   huart2

void UART2_SendData(const uint8_t *str, uint8_t len)
{
  HAL_UART_Transmit(&JQ8900_UART, str, len, 100);
}


void  JQ8x00_Command_Data(UartCommandData Command,uint8_t DATA)
{
	uint8_t Buffer[6] ={0xaa};
    uint8_t DataLen = 0;
    Buffer[1] = Command;       //指令类型
    if((Command != AppointTrack) && (Command != SetCycleCount) && (Command != SelectTrackNoPlay) && (Command != AppointTimeBack) && (Command != AppointTimeFast))        //只含一个数据指令    
    {
        Buffer[2] = 1;			//数据长度
        Buffer[3] = DATA;       //数据
        Buffer[4] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3];            //校验和
        DataLen = 5;
    }
    else                                                                                        //含两个数据指令 
    {
        Buffer[2] = 2;			//数据长度
        Buffer[3] = DATA/256;       //数据
        Buffer[4] = DATA%256;       //数据
        Buffer[5] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3] + Buffer[4];  
        DataLen = 6;
    }
	UART2_SendData(Buffer,DataLen);
}

void JQ8x00_RandomPathPlay(JQ8X00_Symbol symbol,char *DATA)
{
    uint8_t Buffer[52] ={0xaa,0x08};
    uint8_t i,j;
    Buffer[2] = 1 + strlen(DATA) + 4;       //长度,1为盘符，4为*？？？
    Buffer[3] = symbol;        //盘符
    i = 4;
    while(*DATA)
    {
       Buffer[i++] =  *DATA;
        DATA++;
    }
    Buffer[i++] = '*';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    for(j=0;j<i;j++)
    {
        Buffer[i] = Buffer[i] + Buffer[j];      //计算校验合
    }
    i++;
    UART2_SendData(Buffer,i);
}

void JQ8900_Stop(void)
{
  uint8_t Data[4] = {0xAA, 0x04, 0x00, 0xAE};
  UART2_SendData(Data,4);
  
}

void JQ8900_Play_Recording(struct Record_Info *rec)
{
    JQ8x00_Command_Data(SetVolume,rec->volume);         //设置音量
    Delay_ms(10);                                       //连续发送指令，加入一定的延时等待模块处理完成上一条指令
    JQ8x00_Command_Data(AppointTrack, rec->index);      //播放曲目
    JQ8x00_RandomPathPlay(JQ8X00_FLASH, rec->path);     
    Delay_ms(1000); 
}

void Cradle_Open(void)
{
      rec.index = 1;
      strcpy(rec.path, "\"00001\"");
      rec.volume  = 15;
      JQ8900_Play_Recording(&rec);
}

void Cradle_Close(void)
{
      JQ8900_Stop();
}

