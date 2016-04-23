//头文件//
#pragma once

HANDLE Init_fpga_rs232(const CString& ComName);										//FPGA串口初始化
VOID Set_fpga_brate(long BrateNUM);												//设置拨特率
BOOL Set_Prober_Gain(HANDLE HComx,int Gdata);									//设置探测器增益,即APD增益
BOOL Get_Prober_Tem(HANDLE HComx);												//获取探测器温度
BOOL Laser_ON(HANDLE HComx);													//开激光
BOOL Laser_OFF(HANDLE HComx);													//关激光
BOOL Set_Vth(HANDLE HComx,int Vdata);											//设置阈值电压
BOOL Set_Tem_M(HANDLE HComx,int Mdata);											//设置温补常量
BOOL Set_Tem_N(HANDLE HComx,int Ndata);											//设置温补系数
BOOL Warm_ON(HANDLE HComx);														//打开温补工作模式
BOOL Warm_OFF(HANDLE HComx);													//关闭温补工作模式
BOOL Set_AD_ns(HANDLE HComx,int xns);											//设置AD采集峰保延时时间
BOOL JD_Switch_ON(HANDLE HComx);												//打开节电开关
BOOL JD_Switch_OFF(HANDLE HComx);												//关闭节电开关
BOOL _GNSS(HANDLE HComx);														//工控机接收GNSS数据
BOOL Set_PulseWidth(HANDLE, int Pdata);                                         //整形脉宽
BOOL Set_PulseVth1(HANDLE, int Pdata);											//整形阈值1
BOOL Set_PulseVth2(HANDLE, int Pdata);											//整形阈值2

BOOL StartGNSS(HANDLE hCom);
BOOL StopGNSS(HANDLE hCom);

BOOL GetTempAndSener( HANDLE hCom , int* pTemp , int* pSen );

//激光器相关
BOOL Get_Laser_Tem(HANDLE HComx);												//获取激光器温度
BOOL Get_Laser_I(HANDLE HComx);													//获取激光器电流
BOOL Get_Laser_F(HANDLE HComx);													//获取激光器频率
BOOL Set_Laser_Currents(HANDLE HComx,int Cdata);								//设置激光器电流
