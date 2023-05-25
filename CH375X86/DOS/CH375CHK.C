/* TC 2.0 */

#define		MY_CH375_IO_ADDR		0x0260		/* �ٶ�I/O��ַΪ260H,��ֵӦ����CONFIG.SYS�ļ���ָ����I/O��ַ��ͬ */
/*#define		MY_CARD_IS_SERIAL		1*/		/* ����Ǵ���������ָ��˶��� */

#define		DELAY_PORT_ADDR		0x61			/* ���ڶ�I/Oģ����ʱ��I/O�˿ڵ�ַ,������ڳ�ͻ������ѡ��ַ */
/*#define		DELAY_PORT_ADDR		( MY_CH375_IO_ADDR + 2 )*/	/* ����ǲ������Ӳ��������ṩ״̬�˿���ô��ָ��˶��� */

unsigned short PortBaseAddr;					/* ȫ�ֱ���,I/O�˿ڵ�ַ */
/* ���ڲ�������ΪCH375����Ӳ��I/O�˿ڵ�ַ,+0Ϊ���ݶ˿�,+1Ϊ����˿�; ���ڴ�������Ϊ���ڿ���оƬ16C550��I/O�˿ڵ�ַ */


/* I/O������ʱ */
void	DelayuS( unsigned short Delay )
{
	unsigned short c;
	c = Delay * 3;
	do {  /* ������ʱһ��I/O���� */
		inportb( DELAY_PORT_ADDR );  /* ͨ����I/O�˿�ģ����ʱ */
	} while ( c -- );
}


#ifdef	MY_CARD_IS_SERIAL

/* �ȴ����ڷ������ */
void	SerialWaitSend( void )
{
	unsigned short i;
	unsigned char  d;
	for ( i = 0; i < 10000; i ++ ) {  /* 10000uS��ʱ */
		DelayuS( 1 );
		d = inportb( PortBaseAddr + 0x05 );  /* SERIAL_LSR */
		if ( ( d & 0x60 ) == 0x60 ) break;  /* SERIAL_LSR_THRE | SERIAL_LSR_TSRE */
	}
	DelayuS( 0 );
}

/* ��������Ѿ����յ����� */
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

/* ͨ��������CH375���������� */
void	CH375WriteCmd( unsigned char cmd )
{
	SerialWaitSend( );  /* �ȴ�֮ǰ�ķ������ */
	SerialClearRecv( );  /* ���֮ǰ�Ľ��� */
	outportb( PortBaseAddr + 0x03, 0x2B );  /* SERIAL_LCR, (8λ����,MARK,1λֹͣλ) */
	DelayuS( 0 );
	outportb( PortBaseAddr + 0x00, cmd );  /* SERIAL_DIO */
	SerialWaitSend( );  /* �ȴ�������� */
	outportb( PortBaseAddr + 0x03, 0x3B );  /* SERIAL_LCR, (8λ����,SPACE,1λֹͣλ) */
	DelayuS( 0 );
}

/* ͨ��������CH375�������� */
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

/* ͨ�����ڴ�CH375�������� */
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
		DelayuS( 2000 );  /* ��ʱ2mS */
		SerialClearRecv( );  /* ������� */
		return( 0x80 );
	}
}

#else

/* ͨ��������CH375���������� */
void	CH375WriteCmd( unsigned char cmd )
{
	DelayuS( 2 );
	outportb( PortBaseAddr + 1, cmd );  /* д����� */
	DelayuS( 2 );
}

/* ͨ��������CH375�������� */
void	CH375WriteData( unsigned char dat )
{
	outportb( PortBaseAddr + 0, dat );  /* д���ݿ� */
	DelayuS( 1 );
}

/* ͨ�����ڴ�CH375�������� */
unsigned char	CH375ReadData( void )
{
	unsigned char d;
	DelayuS( 1 );
	d = inportb( PortBaseAddr + 0 );  /* �����ݿ� */
	return( d );
}

#endif


/* ��CH375�Ĵ��� */
unsigned char	CH375ReadReg( unsigned char RegAddr )
{
	unsigned char d;
/*	disable( ); ������ܻ����жϷ�������ж�дU������,��ô����ر��ж� */
	CH375WriteCmd( 0x0A );  /* д����� */
	CH375WriteData( RegAddr );  /* д���ݿ� */
	d = CH375ReadData( );  /* �����ݿ� */
/*	enable( ); �ָ��ж� */
	return( d );
}

/* ��;: ��DOSϵͳ�м��CH375���ӵ�U���Ƿ�����
   ����: PortBaseAddr ΪCH375EDM��I/O��ַ, �����PCI����, ��ô�ο�CH365DOS.C��ȡI/O��ַ
   ���: 0 ˵��U���Ѿ��Ͽ�, 1 ˵��U���Ѿ����� */
unsigned char	IsDiskOnLine( void )
{
	unsigned char status;
	status = CH375ReadReg( 0x20 );
	return( ( status & 0x20 ) ? 1 : 0 );  /* �ж�CH375�ڲ��Ĵ�����״̬ */
}

/* ��;: ��DOSϵͳ�л�ȡCH375���ӵ�U�̵�ʵ���̷�
   ����: PortBaseAddr ΪCH375EDM��I/O��ַ, �����PCI����, ��ô�ο�CH365DOS.C��ȡI/O��ַ
   ���: U�̶�Ӧ���̷�, ��ЧֵΪ��ĸA��Z, ��DOSϵͳ�Զ�����, ͨ��ִ��DOS����SUBST�����Խ���ָ��ΪA�̵� */
unsigned char	GetDiskName( void )
{
	unsigned char drive;
	drive = CH375ReadReg( 0x3E );
	if ( drive < 'A' || drive > 'Z' ) return( 0 );  /* �̷���Ч,����CH375Ӳ���Ƿ��������� */
	return( drive );  /* �����̷� */
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
			if ( IsDiskOnLine( ) ) {  /* U������ */
				if ( last == 0 ) printf( "Disk_In\n" );
				last = 1;
			}
			else {  /* U�̶Ͽ� */
				if ( last == 1 ) printf( "Disk_Out\n" );
				last = 0;
			}
			sleep( 1 );  /* ������ʾ, �ȴ�1����ٲ�ѯ */
		}
	}
	else printf( "Please check CH375 hardware\n" );
}