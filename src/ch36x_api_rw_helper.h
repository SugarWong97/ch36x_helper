#ifndef		_DEBUG368_H
#define		_DEBUG368_H

#ifdef __cplusplus
extern "C" {
#endif

HINSTANCE mSaveInstance; /* 实例 */
HWND mSaveDialogMain; /* 主对话框 */
HWND mSaveDialogI2c; /* I2C对话框 */
HWND mSaveDialogHelp; /* HELP对话框 */

CHAR mCaptionInform[] = "提示";
UINT mIndex = 0; /* 设备序号 */
CHAR flag_open = 0; /* 设备开关标志 */
mPCH367_IO_REG mBaseAddr; /* I/O空间寄存器 */
mPCH368_MEM_REG mMemAddr;/*Mem空间寄存器*/

char buffer_open[16];
char buffer_close[16];
int CH367=0;
int CH368=0;
int intType;//判断是电平中断（1）还是边沿中断（2）
int Devices[5][2];//用于存储机器中的已有设备

WNDPROC WindowProcedureI2cDid,WindowProcedureI2cVid,WindowProcedureI2cRid,WindowProcedureI2cSdid,\
		WindowProcedureI2cSvid,WindowProcedureIoAdd,WindowProcedureIoLen,WindowProcedureIoData,\
		WindowProcedureMemAdd,WindowProcedureMemLen,WindowProcedureMemData,WindowProcedureI2CAdd,\
		WindowProcedureI2cData,WindowProcedureConAdd,WindowProcedureConData;

typedef struct _VAR_TYPE {
	union {
		UCHAR cVar;
		UINT iVar;
		USHORT sVar;
		ULONG lVar;
	};
}mVAR_TYPE, *mpVAR_TYPE;

/* 命令码常量定义,请查阅25F512芯片说明书 */
#define WREN		0x06 /* 写使能指令 */
#define WRDI		0x04 /* 写禁止指令 */
#define EWSR		0x50 /* 写状态寄存器使能指令 */
#define RDSR		0x05 /* 读状态寄存器指令 */
#define WRSR		0x01 /* 写状态寄存器指令,WEL为0不允许写 */
#define READ		0x03 /* 读字节指令 */
#define FAST_READ	0x0B /* 快读指令 */
#define BP			0x02 /* 字节写指令 */
#define AAI			0xAF /* 连接写指令 */
#define SE			0x20 /* 扇区擦除指令 */
#define BE			0xD8 /* 块擦除指令 */
//#define CE			0xC7 /* 芯片擦除指令*/
#define CE			0x60 /* 芯片擦除指令*/
#define RMDI        0x90 /* 读厂商/设备ID指令 */

#define SPI_BUFFER_LENGTH 0x1000 /* 每次读4096个字节 */
CHAR SpiFilePath[256] = ""; /* 保存打开文件路径 */


unsigned char qbyte = 0;
#define CH367_GPO_SET do{CH367mReadIoByte(mIndex, &mBaseAddr->mCH367IoPort[0xF1], &qbyte); CH367mWriteIoByte(mIndex, &mBaseAddr->mCH367IoPort[0xF1], qbyte |= 0x80);}while(0)
#define CH367_GPO_CLR do{CH367mReadIoByte(mIndex, &mBaseAddr->mCH367IoPort[0xF1], &qbyte); CH367mWriteIoByte(mIndex, &mBaseAddr->mCH367IoPort[0xF1], qbyte &= 0x7F);}while(0)
#define WM_INTNOTIFY (WM_USER + 1)
char irq_buf[100] = "";
ULONG mCount = 0;//电平中断计数
ULONG mCount1=0;//边沿中断计数
UCHAR PulWidth[]={0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e};//脉冲宽度数组
UCHAR CurPulWidth;//当前脉冲宽度

LRESULT CALLBACK mDialogMain(HWND hDialog, UINT uMessage, WPARAM wParam, LPARAM lParam); // 主对话框事件 
LRESULT CALLBACK mDialogI2c(HWND hDialog, UINT uMessage, WPARAM wParam, LPARAM lParam); // I2C对话框事件 
LRESULT CALLBACK mDialogHelp(HWND hDialog, UINT uMessage, WPARAM wParam, LPARAM lParam); // HELP对话框事件 
void CALLBACK InterruptEvent(void); // 设备中断通知消息 
void AddrRefresh(HWND hDialog); // 重新取基址
void mSetI2C(HWND hDialog); // I2C读写
void mIoWrite(HWND hDialog,int IoModel,int IoAddr);//IO读操作
void mIoRead(HWND hDialog,int IoModel,int IoAddr);//IO写操作
void mMemRead(HWND hDialog,int MemModel);//MEM读操作
void mMemWrite(HWND hDialog,int MemModel);//MEM读操作
void mConRead(HWND hDialog);//配置空间字节读
void mConWrite(HWND hDialog);//配置空间字节写
void mI2CRead(HWND hDialog);//I2C读
void mI2CWrite(HWND hDialog);//I2C写
void mInitCheckBox(HWND hDialog); // 初始化默认选中单选框 
void mShowDevVer(HWND hDialog); //显示驱动版本号


void mPreVerify(HWND hwnd);

int ch36xCloseDevice(int index);

#ifdef __cplusplus
}
#endif

#endif // _DEBUG367_H