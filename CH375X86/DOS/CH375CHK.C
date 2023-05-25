/* TC 2.0 */

#define		MY_CH375_IO_ADDR		0x0260		/* 假定I/O地址为260H,该值应该与CONFIG.SYS文件中指定的I/O地址相同 */
/*#define		MY_CARD_IS_SERIAL		1*/		/* 如果是串口连接请恢复此定义 */

#define		DELAY_PORT_ADDR		0x61			/* 用于读I/O模拟延时的I/O端口地址,如果存在冲突可以另选地址 */
/*#define		DELAY_PORT_ADDR		( MY_CH375_IO_ADDR + 2 )*/	/* 如果是并口连接并且译码提供状态端口那么请恢复此定义 */

unsigned short PortBaseAddr;					/* 全局变量,I/O端口地址 */
/* 对于并口连接为CH375并口硬件I/O端口地址,+0为数据端口,+1为命令端口; 对于串口连接为串口控制芯片16C550的I/O端口地址 */


/* I/O操作延时 */
void	DelayuS( unsigned short Delay )
{
	unsigned short c;
	c = Delay * 3;
	do {  /* 至少延时一个I/O操作 */
		inportb( DELAY_PORT_ADDR );  /* 通过读I/O端口模拟延时 */
	} while ( c -- );
}


#ifdef	MY_CARD_IS_SERIAL

/* 等待串口发送完成 */
void	SerialWaitSend( void )
{
	unsigned short i;
	unsigned char  d;
	for ( i = 0; i < 10000; i ++ ) {  /* 10000uS超时 */
		DelayuS( 1 );
		d = inportb( PortBaseAddr + 0x05 );  /* SERIAL_LSR */
		if ( ( d & 0x60 ) == 0x60 ) break;  /* SERIAL_LSR_THRE | SERIAL_LSR_TSRE */
	}
	DelayuS( 0 );
}

/* 清除串口已经接收的数据 */
void	SerialClearRecv( void )
{
	unsigned short i;
	for ( i = 0; i < 100; i ++ ) {
		inportb( PortBaseAddr + 0x05 );  /* SERIAL_LSR */
		DelayuS( 0 );
		inportb( PortBaseAddr + 0x00 );  /* SERIAL_DIO */
	}
	DelayuS( 0 );
}

/* 通过串口向CH375发出命令码 */
void	CH375WriteCmd( unsigned char cmd )
{
	SerialWaitSend( );  /* 等待之前的发送完成 */
	SerialClearRecv( );  /* 清除之前的接收 */
	outportb( PortBaseAddr + 0x03, 0x2B );  /* SERIAL_LCR, (8位数据,MARK,1位停止位) */
	DelayuS( 0 );
	outportb( PortBaseAddr + 0x00, cmd );  /* SERIAL_DIO */
	SerialWaitSend( );  /* 等待发送完成 */
	outportb( PortBaseAddr + 0x03, 0x3B );  /* SERIAL_LCR, (8位数据,SPACE,1位停止位) */
	DelayuS( 0 );
}

/* 通过串口向CH375发出数据 */
void	CH375WriteData( unsigned char dat )
{
	unsigned char  d;
	outportb( PortBaseAddr + 0x00, dat );  /* SERIAL_DIO */
	do {
		DelayuS( 0 );
		d = inportb( PortBaseAddr + 0x05 );  /* SERIAL_LSR */
	} while ( ( d & 0x20 ) == 0 );  /* SERIAL_LSR_THRE */
	DelayuS( 0 );
}

/* 通过串口从CH375接收数据 */
unsigned char	CH375ReadData( void )
{
	unsigned short i;
	unsigned char  d;
	for ( i = 0; i < 10000; i ++ ) {
		d = inportb( PortBaseAddr + 0x05 );  /* SERIAL_LSR */
		if ( d & 0x1F ) break;
	}
	if ( d & 0x01 ) {
		d = inportb( PortBaseAddr + 0x00 );  /* SERIAL_DIO */
		return( d );
	}
	else {
		DelayuS( 2000 );  /* 延时2mS */
		SerialClearRecv( );  /* 清除接收 */
		return( 0x80 );
	}
}

#else

/* 通过并口向CH375发出命令码 */
void	CH375WriteCmd( unsigned char cmd )
{
	DelayuS( 2 );
	outportb( PortBaseAddr + 1, cmd );  /* 写命令口 */
	DelayuS( 2 );
}

/* 通过并口向CH375发出数据 */
void	CH375WriteData( unsigned char dat )
{
	outportb( PortBaseAddr + 0, dat );  /* 写数据口 */
	DelayuS( 1 );
}

/* 通过并口从CH375接收数据 */
unsigned char	CH375ReadData( void )
{
	unsigned char d;
	DelayuS( 1 );
	d = inportb( PortBaseAddr + 0 );  /* 读数据口 */
	return( d );
}

#endif


/* 读CH375寄存器 */
unsigned char	CH375ReadReg( unsigned char RegAddr )
{
	unsigned char d;
/*	disable( ); 如果可能会在中断服务程序中读写U盘数据,那么必须关闭中断 */
	CH375WriteCmd( 0x0A );  /* 写命令口 */
	CH375WriteData( RegAddr );  /* 写数据口 */
	d = CH375ReadData( );  /* 读数据口 */
/*	enable( ); 恢复中断 */
	return( d );
}

/* 用途: 在DOS系统中检查CH375连接的U盘是否在线
   输入: PortBaseAddr 为CH375EDM的I/O基址, 如果是PCI总线, 那么参考CH365DOS.C获取I/O基址
   输出: 0 说明U盘已经断开, 1 说明U盘已经连接 */
unsigned char	IsDiskOnLine( void )
{
	unsigned char status;
	status = CH375ReadReg( 0x20 );
	return( ( status & 0x20 ) ? 1 : 0 );  /* 判断CH375内部寄存器的状态 */
}

/* 用途: 在DOS系统中获取CH375连接的U盘的实际盘符
   输入: PortBaseAddr 为CH375EDM的I/O基址, 如果是PCI总线, 那么参考CH365DOS.C获取I/O基址
   输出: U盘对应的盘符, 有效值为字母A到Z, 由DOS系统自动分配, 通过执行DOS命令SUBST还可以将其指定为A盘等 */
unsigned char	GetDiskName( void )
{
	unsigned char drive;
	drive = CH375ReadReg( 0x3E );
	if ( drive < 'A' || drive > 'Z' ) return( 0 );  /* 盘符无效,请检查CH375硬件是否正常工作 */
	return( drive );  /* 返回盘符 */
}

main( ) {
	int i, last;
	PortBaseAddr = MY_CH375_IO_ADDR;
	i = GetDiskName( );
	if ( i ) {
		printf( "Disk name is %c:\n", i );
		printf( "DiskOnline now ? \n" );
		last = 0;
		for ( i = 0; i < 10000; i ++ ) {
			if ( kbhit( ) ) break;
			if ( IsDiskOnLine( ) ) {  /* U盘连接 */
				if ( last == 0 ) printf( "Disk_In\n" );
				last = 1;
			}
			else {  /* U盘断开 */
				if ( last == 1 ) printf( "Disk_Out\n" );
				last = 0;
			}
			sleep( 1 );  /* 仅作演示, 等待1秒后再查询 */
		}
	}
	else printf( "Please check CH375 hardware\n" );
}
