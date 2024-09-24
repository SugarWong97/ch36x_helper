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


int ch36xMemConfig32BitRW(int enable)
{
    if(CH368==1)
    {
        UCHAR oByte;
        CH367mReadIoByte(mIndex, &mBaseAddr->mCH367Speed,&oByte);//控制IO寄存器的speed位实现32位支持
        if(enable)//IsDlgButtonChecked(hDialog,IDC_CB32)==BST_CHECKED)
        {
            CH367mWriteIoByte(mIndex,&mBaseAddr->mCH367Speed,oByte|0x40);
            //SendDlgItemMessage(hDialog,IDC_CBMEMMODEL,CB_INSERTSTRING,1,"双字");
            printf("Mem 32-Bit Config Enable\n");
        }
        else
        {
            CH367mWriteIoByte(mIndex,&mBaseAddr->mCH367Speed,oByte&0x3F);
            printf("Mem 32-Bit Config Disable\n");
            //SendDlgItemMessage(hDialog,IDC_CBMEMMODEL,CB_DELETESTRING,1,0);
        }
        return true;
    }
    return false;
}


/*============================= MEM读写 ==============================*/
enum CH36xMemModel
{
     CH36xMemModel_BYTE = 0;
     CH36xMemModel_DWORD;
};


void ch36xMemRead(enum CH36xMemModel MemModel)//MEM读操作
{
    mVAR_TYPE mVarType;
    UCHAR MemAddr[5],MemLen[5];
    UCHAR mBuf[mMAX_BUFFER_LENGTH]="";
    ULONG mLen=0,mAddr;
    mAddr=GetDlgItemText(hDialog,IDC_EDITMEMADD,MemAddr,5);
    if(!mCheckInput(MemAddr))
    {
        MessageBox(hDialog,"您输入的字符有误，请输入0~9,a~f,A~F之间的十六进制数!","提示",MB_OK|MB_ICONERROR);
        return;
    }
    mVarType = mCharToVar(MemAddr,mAddr, 0);
    mAddr = mVarType.sVar;//获得输入地址
    mLen=GetDlgItemText(hDialog,IDC_EDITMEMLEN,MemLen,5);
    if(!mCheckInput(MemLen))
    {
        MessageBox(hDialog,"您输入的字符有误，请输入0~9,a~f,A~F之间的十六进制数!","提示",MB_OK|MB_ICONERROR);
        return;
    }
    mLen=mStrToBcd(MemLen);//获得输入长度
    if(mLen>=0x8000)
    {
        MessageBox(hDialog,"请输入小于等于0x8000的长度!","提示",MB_OK|MB_ICONERROR);
        printf("Len too long\n");
        return;
    }
    //if(GetDlgItemText(hDialog,IDC_EDITMEMADD,MemAddr,5)==0||GetDlgItemText(hDialog,IDC_EDITMEMLEN,MemAddr,5)==0)
    //{
    //  MessageBox(hDialog,"请输入地址和数据长度","提示",MB_OK|MB_ICONSTOP);
    //  return;
    //}
    if(mAddr+mLen>0x7FFF)
    {
        MessageBox(hDialog,"您输入的地址+长度>0x7FFF发生错误,请查阅CH367mAccessBlock的相关说明","提示",MB_OK|MB_ICONSTOP);
        return;
    }
    if(MemModel == CH36xMemModel_BYTE)
    {
        if(!CH367mAccessBlock(mIndex,mFuncReadMemByte,&mMemAddr->mCH368MemPort[mAddr],mBuf,mLen))
        {
            MessageBox(hDialog,"MEM读取失败","提示",MB_OK|MB_ICONERROR);
            return;
        }
        else
        {
            char buffer[mMAX_BUFFER_LENGTH*3];
            ULONG i,j=0;
            for(i=0;i<mLen;i++)
            {
                sprintf(&buffer[j],"%2x ",mBuf[i]);    //两位十六进制数加一个空格
                if (mBuf[i]<16 )                      //一位十六进制字符前面加0
                    buffer[j]=48;
                if (buffer[j]>=97 && buffer[j]<=122) //小写字母转为大写字母
                    buffer[j]=buffer[j]-32;
                if (buffer[j+1]>=97 && buffer[j+1]<=122) //小写字母转为大写字母
                    buffer[j+1]=buffer[j+1]-32;
                j += 3;
            }
            buffer[j]='\0';
            SetDlgItemText(hDialog,IDC_EDITMEMDATA,buffer);
        }
    }
    else
    {
        if(!mCheckWord((PVOID)mAddr, 8)||!mCheckWord((PVOID)mLen, 8))
        {
            MessageBox(hDialog, "请输入能存储双字的起始地址且数据长度为4的倍数", "提示", MB_OK | MB_ICONSTOP);
            return;
        }
        if(!CH367mAccessBlock(mIndex,mFuncReadMemDword,&mMemAddr->mCH368MemPort[mAddr],mBuf,mLen))
        {
            MessageBox(hDialog,"MEM读取失败","提示",MB_OK);
            return;
        }
        else
        {
            char buffer[mMAX_BUFFER_LENGTH*3];
            ULONG i,j=0;
            for(i=0;i<mLen;i++)
            {
                sprintf(&buffer[j],"%2x ",mBuf[i]);    //两位十六进制数加一个空格
                if (mBuf[i]<16 )                      //一位十六进制字符前面加0
                    buffer[j]=48;
                if (buffer[j]>=97 && buffer[j]<=122) //小写字母转为大写字母
                    buffer[j]=buffer[j]-32;
                if (buffer[j+1]>=97 && buffer[j+1]<=122) //小写字母转为大写字母
                    buffer[j+1]=buffer[j+1]-32;
                j += 3;
            }
            buffer[j]='\0';
            SetDlgItemText(hDialog,IDC_EDITMEMDATA,buffer);
        }
    }
}
/*
void mMemWrite(HWND hDialog,int MemModel)
{
    mVAR_TYPE mVarType;
    UCHAR MemAddr[5],MemLen[5];
    UCHAR mBuf[mMAX_BUFFER_LENGTH*2]="";
    UCHAR bufferFilter[mMAX_BUFFER_LENGTH*2]="";
    UCHAR buffer[mMAX_BUFFER_LENGTH]="";
    UCHAR inputStr[mMAX_BUFFER_LENGTH*2]="";
    ULONG mLen=0,mAddr,strLen,i,j=0;
    mAddr=GetDlgItemText(hDialog,IDC_EDITMEMADD,MemAddr,5);
    if(!mCheckInput(MemAddr))
    {
        MessageBox(hDialog,"您输入的字符有误，请输入0~9,a~f,A~F之间的十六进制数!","提示",MB_OK|MB_ICONERROR);
        return;
    }
    mVarType = mCharToVar(MemAddr,mAddr, 0);
    mAddr = mVarType.sVar;//获取输入地址
    mLen=GetDlgItemText(hDialog,IDC_EDITMEMLEN,MemLen,5);
    if(!mCheckInput(MemLen))
    {
        MessageBox(hDialog,"您输入的字符有误，请输入0~9,a~f,A~F之间的十六进制数!","提示",MB_OK|MB_ICONERROR);
        return;
    }
    mLen=mStrToBcd(MemLen);//获取输入长度
    if(mLen>=0x8000)
    {
        MessageBox(hDialog,"请输入小于等于0x8000的长度!","提示",MB_OK|MB_ICONERROR);
        return;
    }
    strLen=GetDlgItemText(hDialog,IDC_EDITMEMDATA,inputStr,mMAX_BUFFER_LENGTH);//获取输入数据
    for(i=0;i<strLen;i++)
    {
        if(inputStr[i]!=' ')
            bufferFilter[j++]=inputStr[i];
    }
    if(mAddr+mLen>0x7FFF)
    {
        MessageBox(hDialog,"您输入的地址+长度>0x7FFF发生错误,请查阅CH367mAccessBlock的相关说明","提示",MB_OK|MB_ICONERROR);
        return;
    }
    if(GetDlgItemText(hDialog,IDC_EDITMEMADD,MemAddr,5)==0||GetDlgItemText(hDialog,IDC_EDITMEMLEN,MemLen,5)==0||GetDlgItemText(hDialog,IDC_EDITMEMDATA,inputStr,mMAX_BUFFER_LENGTH)==0)
    {
        MessageBox(hDialog,"请输入地址,数据长度和写入数据内容","提示",MB_OK|MB_ICONSTOP);
        return;
    }
    if(mLen>j/2)//在输入长度和数据长度中取较小的值
    {
        mLen=j/2;
    }
    memcpy(mBuf,bufferFilter,mLen*2);
    memcpy(buffer,mStrtoVal(mBuf,mLen*2),mLen);
    if(MemModel==0)//以字节的方式读写MEM
    {
        if(!CH367mAccessBlock(mIndex,mFuncWriteMemByte,&mMemAddr->mCH368MemPort[mAddr],buffer,mLen))
        {
            MessageBox(NULL,"写失败","提示",MB_OK|MB_ICONERROR);
            return;
        }
        else
        {
            MessageBox(NULL,"写成功","提示",MB_OK);
        }
    }
    else//以双字的方式读写MEM
    {
        if(!mCheckWord((PVOID)mAddr, 8))
        {
            MessageBox(hDialog, "请输入能存储双字的起始地址且数据长度为4的倍数", "提示", MB_OK | MB_ICONSTOP);
            return;
        }
        if(!CH367mAccessBlock(mIndex,mFuncWriteMemDword,&mMemAddr->mCH368MemPort[mAddr],buffer,mLen))
        {
            MessageBox(NULL,"写失败","提示",MB_OK|MB_ICONERROR);
            return;
        }
        else
        {
            MessageBox(NULL,"写成功","提示",MB_OK);
        }
    }
}
*/
int main(int argc, char *argv[])
{
    int index = 0;
    ch36xOpenDevice(index);

    ch36xMemConfig32BitRW(true);

    ch36xCloseDevice(index);

    return 0;
}
