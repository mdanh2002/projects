对于使用DOS系统的8086兼容的PC工控机或者单片机，通过CH375读写U盘或者移动硬盘，
可以使用CH375的DOS设备驱动程序，将U盘仿真为硬盘，在DOS中会增加一个逻辑盘（通常是D盘），
使用方法类似于普通硬盘和电子盘及RAM内存盘，该方法还支持U盘带电热插拔，更方便交换数据。

硬件要求：兼容8086的CPU，CH375与CPU之间的连接方式可以是以下任何一种：
并口方式可以做成U盘插卡，例如：PCI插卡、ISA插卡、PC104接口板等。
    优点：速度快（约每秒200K到500K字节），效率高，卡中自带5V电源，支持U盘带电热插拔。
    缺点：连接线多，安装比串口板复杂。
串口方式可以做成U盘接口板，例如：RS232串口板、TTL串口板等。
    优点：硬件连接方便，支持U盘带电热插拔，支持整个串口板硬件带电热插拔。
    缺点：速度慢（约每秒9K到10K字节），需要外供5V电源（串口连线未包含电源供电）。
沁恒电子可以提供上述各种板卡样品：PCI插卡、ISA插卡、PC104接口板、RS232串口板、TTL串口板。

方案用途及特点：
  1、用于在计算机与工控机/嵌入式系统之间非实时地交换数据。
  2、用于备份工控机/嵌入式系统中的重要数据。
  3、使用市面上通用的U盘，比闪存和电子盘容易采购，容量不限。
  4、支持DOS 3.X到7.X版本、支持WINDOWS 3.X、WINDOWS 95/98/ME等操作系统。
  5、支持操作系统所能支持的所有文件系统，例如用DOS 7.X可以支持FAT12/FAT16/FAT32。
  6、只需要一个驱动程序文件CH375DOS.SYS，长度小于5K，加载后只需要RAM内存约4K。
  7、驱动程序只在应用程序读写U盘时工作，空闲时不影响原DOS系统及工控机的运行。
  8、支持U盘带电热插拔，方便使用，U盘可以只在需要时插上。
  9、串口连接支持带电热插拔，方便使用，串口板本身及U盘都可以只在需要时连接到串口。
  10、并口方式如果增加扩展ROM，还可以支持从U盘引导操作系统，（兼容性不如DOS驱动）。
  11、极低成本，批量产品只要几十元人民币。
  12、如果不使用DOS操作系统，那么可以参考CH375的U盘文件级子程序库进行API调用。
  13、简化并口连接只需要一个地址片选译码和一个CH375芯片，驱动程序文件长度小于3K。

关于硬件电路的具体说明

1、并口连接，查询方式
   驱动程序使用查询方式处理CH375的中断，具体电路可以参考CH375EDM，硬件I/O地址分别为：
     I/O基址 + 0 为CH375的数据端口，可读可读，
     I/O基址 + 1 为CH375的命令端口，只写，
     I/O基址 + 2 为CH375的状态端口，只读，其中位0为CH375的INT#引脚状态，便于查询中断。
   如果CH375连接到ISA总线或者PC104总线，那么需要指定CH375的I/O基址（也就是I/O端口），
     参考PC104ISA.PDF中的原理图，可以应用于ISA或者PC104总线，图中的ROM芯片U4是可选的。
     图中I/O基址为260H，当然可以使用其它I/O地址。图中27C256可选，仅用于支持从U盘引导。
   如果CH375是通过PCI芯片CH365连接到PCI总线，那么驱动程序会自动获取I/O基址，不必指定。
     参考PCI.PDF中的原理图。

1.X 简化的并口连接，内部寄存器查询方式
   简化并口连接只支持CH375A和CH375B芯片(不支持CH375S芯片)，不需要状态端口（I/O基址+2），
   在电路设计上，只需要为CH375提供片选信号CS#、读RD#、写WR#、地址A0和数据线D7-D0，
   在这种简化的并口连接中，必须使用NO_INTER子目录下的CH375DOS.SYS驱动程序，文件更小，
   该驱动程序通过查询CH375A和CH375B的内部寄存器代替查询芯片INT#引脚上的低电平中断。

   对于CH375B芯片建议使用简化的并口连接，不需要状态端口（I/O基址+2），无需连接INT#引脚

2、并口连接，中断方式（建议：请优先使用查询方式）
   驱动程序使用硬件中断方式处理CH375的中断，CH375的INT#引脚连接到主机的中断请求端，
   注意，由于CH375的INT#是低电平有效，所以对于ISA和PC104都需要先反相后再连接中断。
   支持任意中断号，支持任意I/O地址，采用标准的并口连接方式，可以参考CH375芯片手册，
   CH375的CS#引脚由I/O地址译码电路产生低电平有效的片选信号，硬件I/O地址分别为：
     I/O基址 + 0 为CH375的数据端口，可读可读，
     I/O基址 + 1 为CH375的命令端口，只写。
     参考ISA_INT.PDF中的原理图。

3、串口连接，TTL电平
   主机串口必须兼容16C550并且可以禁止FIFO，串口号不限，输入输出为TTL电平。
   驱动程序使用查询方式处理CH375的中断，CH375工作于串口方式：
     CH375的RXD引脚接主机某串口的TXD，CH375的TXD引脚接主机某串口的RXD，
     CH375的RSTI引脚接主机某串口的RTS#，CH375的INT#引脚接主机某串口的CTS#。
   在复位CH375芯片后先以9600bps通讯一次，然后双方都被驱动程序切换到115200bps通讯。
   参考SERIAL.PDF中的原理图，去掉TTL/RS232电平转换芯片U2，其左右两侧信号直连。

4、串口连接，RS232电平
   同上，区别仅在于主机串口的输入输出为RS232电平，
   所以CH375的两个输入RXD和RSTI、两个输出TXD和INT#也必须进行TTL与RS232电平的转换
   参考SERIAL.PDF中的原理图，图中由U2进行电平转换，注意图中U2电容尚未画出。

关于DOS驱动程序的具体说明
  只需要将 CH375DOS.SYS 文件复制到兼容PC工控机的硬盘或者其它电子启动盘中，
  然后修改DOS配置文件 CONFIG.SYS ，如果没有此文件可以用EDIT或者记事本新建。

  在DOS配置文件CONFIG.SYS中加入下面一行，以启用CH375的DOS驱动：
      DEVICE = CH375DOS.SYS   @nnnn  #rr  %mm
  如果CH375DOS.SYS不在根目录下，那么还应该指定包含目录的完全路径。参数说明：
  1、nnnn是十六进制表示的I/O基址，默认为0。如果nnnn为0，那么驱动程序会自动获取I/O基址，
     对于PCI应该是0，对于ISA和PC104以及串口，都应该指定I/O端点地址。
  2、rr是十进制表示的中断号，默认为0。如果rr为0，那么驱动程序使用查询方式，否则使用中断方式。
     如果rr为91或者92那么代表特殊含义，不是指中断号，而是指定使用串口连接方式，
     rr为91说明是静态串口连接，如果在DOS启动时串口连接失败，那么驱动程序将退出，
     rr为92说明是动态串口连接，即使DOS启动时串口连接失败，驱动程序也不退出，而是随时准备连接。
  3、mm为I/O速度，有效值是0到255，数值越小读写速度越快，默认为3，大多数低速工控机可以设置为2。

  例如ISA卡或PC104板，查询方式，I/O基址为2A0H，那么：    DEVICE = CH375DOS.SYS  @2A0
  例如由CH365实现的PCI卡，查询方式，那么：               DEVICE = CH375DOS.SYS
  例如8086低速工控机，查询方式，I/O基址为320H，那么：    DEVICE = CH375DOS.SYS  @320  %2
  例如ISA卡或PC104板，中断IRQ5，I/O基址为260H，那么：    DEVICE = CH375DOS.SYS  @260  #5
  例如通过串口1静态连接，也就是串口硬件始终连着，那么：  DEVICE = CH375DOS.SYS  @3F8  #91
  例如通过串口2动态连接，打算支持串口硬件热插拔，那么：  DEVICE = CH375DOS.SYS  @2F8  #92
  实例1，某 CONFIG.SYS 文件实例的内容，可以使用记事本打开查看或者新建
        DEVICE = C:\DOS\HIMEM.SYS
        DEVICE = C:\DOS\RAMDRIVE.SYS  1024
        DEVICE = C:\WCH\CH375DOS.SYS  @3F8  #92  %1
  实例2，某 CONFIG.SYS 文件实例的内容，可以使用记事本打开查看或者新建
        DEVICE = C:\CH375DOS.SYS  @260  %2
        DEVICE = C:\DOS\HIMEM.SYS

  当CH375的DOS驱动程序正常启动后，将显示版本信息以及硬件配置信息如下
      CH375: I/O address = 0260H, interrupt = 00, add disk D:
  上行中的0260H是用户在CONFIG.SYS文件中指定的I/O地址，中断号00说明是查询方式，
  上行中的D为盘符，如果计算机中已经存在D盘，那么U盘的盘符就可能是E，以此类推。
  对于U盘的使用完全类似于普通硬盘及电子盘，主要区别是U盘可以支持热插拔，在使用过程中，
  U盘可以拔出或者更换另一个U盘，但是不宜在写操作期间拔出，否则可能会丢失数据甚至损坏U盘。
  在U盘拔出后，对原盘符操作将始终返回错误，直到下一个U盘插入，其处理方法和效果类似于软盘。

  DOS应用程序可以参考CH375CHK.C通过I/O操作查询U盘是否连接，以及获知U盘盘符等。
  为了模拟软盘A盘或者B盘，可以使用DOS系统的SUBST命令，例如，如果U盘是D盘，那么执行DOS命令行
      SUBST   A:   D:\
  可以使得A盘实际指向D盘，从而保持应用程序对软盘的兼容性，操作A盘或D盘都实际对应于U盘。

  启动时的错误信息有：
      CH375: I/O data error
        说明I/O数据测试错误，可能是与CH375的物理连接有误，或者是地址译码有误
      CH375: interrupt timeout
        说明中断超时或者串口超时，有可能是与CH375的INT#中断引脚的物理连接有误
      CH375: disk not found
        说明DOS启动CH375驱动程序时U盘没有插入，只是提示当时U盘尚未插入，不影响使用
      CH375: error boot sector
        说明DOS启动CH375驱动程序时U盘无法读取，可能是U盘损坏或者不支持的其它USB设备
      CH375: wait serial
        说明指定了中断号92使用动态串口连接，并且此时驱动程序还没有检测到串口硬件
      CH375A only: parameter error
        说明使用了NO_INTER子目录中的驱动程序，该驱动只支持CH375A或CH375B芯片，只支持并口查询
