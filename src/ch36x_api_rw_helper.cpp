#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <iostream.h>
#include <string.h>

#include "CH367DLL.h"
#include "ch36x_api_rw_helper.H"
#pragma comment(lib,"CH367DLL.LIB")


void mShowDevVer(void) //获得驱动版本号
{
    //ULONG ver = 0;
    //char vers[8];
    //ver = CH367GetDrvVersion();
    //_ltoa(ver, vers, 16);
    //if(!SetDlgItemText(hDialog, IDC_EDTDEVVER, vers))
    //{
    //  MessageBox(hDialog, "未能成功获得驱动版本号!", "提示", MB_ICONSTOP | MB_OK);
    //}
    ULONG ver = 0;
    char vers[8];
    ver = CH367GetDrvVersion();
    printf("CH36x Driver Version :%x\n", ver);
}


void AddrRefresh(void) // 取I/O基址存储器基址和中断号
{
    ULONG oIntLine;
    UCHAR oByte,x;
    ULONG ver = 0;
    if(CH367mGetIntLine(mIndex, &oIntLine))
    {
        //UCHAR sByte[12];
        //sprintf(sByte, "%02X\x0", oIntLine);
        //SetDlgItemText(hDialog, IDC_EDTINTLINE, sByte);
        printf("IntLine %x\n", oIntLine);
    }
    //else
    //{
    //  MessageBox(hDialog, "获得中断号失败!", "提示", MB_OK | MB_ICONSTOP);
    //  SetDlgItemText(hDialog, IDC_EDTINTLINE, "");
    //}

    if(CH367mGetIoBaseAddr(mIndex, &mBaseAddr)) //取I/O基址
    {
        //UCHAR sByte[12];
        //sprintf(sByte, "%04X\x0", mBaseAddr);
        //SetDlgItemText(hDialog, IDC_EDTBASEADDR, sByte);
        printf("IO Base Address %p\n", mBaseAddr);
    }
    //else
    //{
    //  MessageBox(hDialog, "获得I/O基址失败!", "提示", MB_OK | MB_ICONSTOP);
    //  SetDlgItemText(hDialog, IDC_EDTBASEADDR, "");
    //}
    if(CH368==1)
    {
        if(CH368mGetMemBaseAddr(mIndex, &mMemAddr)) //取MEM基址
        {
            //CHAR sByte[128]="";
            //sprintf( sByte,"%04p\x0", mMemAddr );
            //SetDlgItemText(hDialog,IDC_EDTMEM,sByte);
            printf("MEM Base Address %p\n", mMemAddr);
        }
        //else
        //{
        //  MessageBox(hDialog,"获得MEM寄存器基址失败！","提示",MB_OK|MB_ICONSTOP);
        //  SetDlgItemText(hDialog,IDC_EDTMEM,"");
        //}

    }
    if(CH367mReadIoByte(mIndex, &mBaseAddr->mCH367Speed,&oByte))//显示当前脉冲宽度
    {
        //char input[10];
        x=oByte&0X0F;
        //sprintf(input,"%d\0",(x+1)*30);
        //SetDlgItemText(mSaveDialogMain,IDC_PULSHOW,input);
        printf("PLUS Width %d\n", (x+1)*30);
    }
    //else
    //{
    //  MessageBox(mSaveDialogMain,"当前脉冲读取失败","提示",MB_OK|MB_ICONSTOP);
    //}
    if(CH368==1)
    {
        if(!CH367mWriteIoByte(mIndex,&mBaseAddr->mCH367Speed,oByte|0x40))//设置CH368支持32位对IO或者存储器进行读写
        {
            //MessageBox(mSaveDialogMain,"32位IO读写设置失败","提示",MB_OK);
            printf("Config 32-Bit MEM R/W ERROR\n");
        }else
        {
            printf("Config 32-Bit MEM R/W OK\n");
        }
    }
}

BOOL read_manufacturer_device_id(unsigned char *data) // 读厂商/设备ID
{
    unsigned char buffer[8];
    unsigned long len = 0x04;
    buffer[0] = RMDI;
    buffer[1] = 0x00;
    buffer[2] = 0x00;
    buffer[3] = 0x00;
    if( CH367StreamSPI(mIndex, 0x04, buffer, &len, data) == FALSE ) return FALSE;
    return TRUE;
}

bool ch36xOpenDevice(int index)
{
    char buffer[mMAX_BUFFER_LENGTH*3];
    mIndex = index;
    UCHAR mByte;
    mCount=0;
    mCount1=0;
    if(flag_open)
    {
        ch36xCloseDevice(mIndex);
    }

    //mIndex = (UCHAR)SendDlgItemMessage(hDialog, IDC_DEVICE_NUMBER, CB_GETCURSEL, 0, 0); // 获得要打开的设备号
    printf("Index is %d\n", mIndex);
    if(CH367mOpenDevice(mIndex, FALSE, TRUE, 0x00) == INVALID_HANDLE_VALUE) //  使能中断, 电平中断模式, 低电平触发
    {
        //sprintf(buffer_open, "无法打开设备%d,请确保设备已经插入", mIndex);
        //MessageBox(hDialog, buffer_open, mCaptionInform, MB_ICONSTOP | MB_OK);
        //return( TRUE );
        return false;
    }

    CH367mReadConfig(mIndex,(PVOID)0x02, &mByte);//读取设备的配置空间用以确定是什么设备
    if(mByte==0x34)////0x34表示CH368设备,0x31或者0x30表示CH367,如果您更改了设备标识,即设备的配置寄存器中的02H空间，请将程序中更改这里的标识
    {
        CH368=1;
        CH367=0;
        printf("Ch36x Open 368 at %d\n", mIndex);
    }
    else if(mByte==0x31||mByte==0x30)
    {
        CH367=1;
        CH368=0;
        printf("Ch36x Open 367 at %d\n", mIndex);
    }
    else
    {
        //MessageBox(hDialog,"未能识别您的设备,如果您更改了设备标识,即设备的配置寄存器中的02H空间,请将程序中相应的代码更改一下","提示",MB_OK);
        //CH367mCloseDevice(mIndex);
        //return FALSE;
        printf("Ch36x Open Unknown at %d\n", mIndex);
        ch36xCloseDevice(mIndex);
        return false;
    }

    if(CH368==1)//如果是CH368设备则重新打开支持存储器读写
    {
        CH367mCloseDevice(mIndex);
        CH367mOpenDevice(mIndex, TRUE, TRUE, 0x00);//重新打开CH368设备并设置支持Mem
    }
    flag_open = 0x01;
    mShowDevVer(); // 显示驱动版本号
    AddrRefresh();

    //CH367mSetIntRoutine(mIndex, InterruptEvent); // 设置中断服务程序
    CH367mSetIntRoutine(mIndex, NULL); // 设置中断服务程序
    //if(CH367==1)
    //{
    //  sprintf(buffer_open, "CH367设备%d已经打开", mIndex);
    //}
    //if(CH368==1)
    //{
    //  sprintf(buffer_open, "CH368设备%d已经打开", mIndex);
    //}
    //SetDlgItemText(hDialog, IDC_STATUS, buffer_open);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNPU), TRUE); // 按键可见
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNIOCL), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNIORD), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNIOWR), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPIRD), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPIWR), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNI2CWR), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNI2CRD), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNI2C_PROGRAM), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPICL), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPIRD), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPIWR), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNCONRD), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNCONWR), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNINT1), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNINT2), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNCLOSE),TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNSPISET), TRUE);
    //EnableWindow(GetDlgItem(hDialog, IDC_BTNOPEN), FALSE);
    //EnableWindow(GetDlgItem(hDialog, IDC_DEVICE_NUMBER), FALSE);
    read_manufacturer_device_id((unsigned char*)buffer);//检测FLASH是否正常
    if(buffer[0] == 0xBF && buffer[1] == 0x48 && buffer[2] == 0xBF && buffer[3] == 0x48)
    {
        printf("Flash OK\n");
        //SetDlgItemText(hDialog,IDC_EDITFLASH,"FLASH状态正常");
        //EnableWindow(GetDlgItem(hDialog,IDC_FLASH_READ),TRUE);
        //EnableWindow(GetDlgItem(hDialog,IDC_FLASH_WRITE),TRUE);
        //EnableWindow(GetDlgItem(hDialog,IDC_FLASH_ERASE),TRUE);
    }
    return true;
}

int ch36xCloseDevice(int index)
{
    mIndex = index;
    CH367mCloseDevice(mIndex);
    return true;
}

int main(int argc, char *argv[])
{
    int index = 0;
    ch36xOpenDevice(index);
    ch36xCloseDevice(index);

    return 0;
}
