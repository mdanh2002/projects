00000000  9C                pushfw
00000001  1E                push ds
00000002  50                push ax
00000003  53                push bx
00000004  51                push cx
00000005  52                push dx
00000006  56                push si
00000007  57                push di
00000008  8BD8              mov bx,ax
0000000A  2E891E1800        mov [cs:0x18],bx
0000000F  33C0              xor ax,ax
00000011  8ED8              mov ds,ax
00000013  BEE801            mov si,0x1e8
00000016  E81401            call word 0x12d
00000019  F606170404        test byte [0x417],0x4
0000001E  7503              jnz 0x23
00000020  E9BC00            jmp word 0xdf
00000023  A11304            mov ax,[0x413]
00000026  8BD0              mov dx,ax
00000028  83EA03            sub dx,byte +0x3
0000002B  89161304          mov [0x413],dx
0000002F  D1E0              shl ax,1
00000031  D1E0              shl ax,1
00000033  D1E0              shl ax,1
00000035  D1E0              shl ax,1
00000037  D1E0              shl ax,1
00000039  D1E0              shl ax,1
0000003B  2DC000            sub ax,0xc0
0000003E  06                push es
0000003F  8EC0              mov es,ax
00000041  0E                push cs
00000042  1F                pop ds
00000043  33F6              xor si,si
00000045  33FF              xor di,di
00000047  B90006            mov cx,0x600
0000004A  FC                cld
0000004B  06                push es
0000004C  50                push ax
0000004D  55                push bp
0000004E  8BEC              mov bp,sp
00000050  C746025A01        mov word [bp+0x2],0x15a
00000055  5D                pop bp
00000056  F3A5              rep movsw
00000058  90                nop
00000059  CB                retf
0000005A  07                pop es
0000005B  33D2              xor dx,dx
0000005D  B83902            mov ax,0x239
00000060  8EDA              mov ds,dx
00000062  BF6400            mov di,0x64
00000065  8945FC            mov [di-0x4],ax
00000068  8C4DFE            mov [di-0x2],cs
0000006B  C606750401        mov byte [0x475],0x1
00000070  8705              xchg ax,[di]
00000072  8B4D02            mov cx,[di+0x2]
00000075  8C4D02            mov [di+0x2],cs
00000078  A3F001            mov [0x1f0],ax
0000007B  890EF201          mov [0x1f2],cx
0000007F  0E                push cs
00000080  1F                pop ds
00000081  2E8B1E1800        mov bx,[cs:0x18]
00000086  B809B1            mov ax,0xb109
00000089  BF0600            mov di,0x6
0000008C  CD1A              int 0x1a
0000008E  81F90004          cmp cx,0x400
00000092  753F              jnz 0xd3
00000094  B809B1            mov ax,0xb109
00000097  BF0200            mov di,0x2
0000009A  CD1A              int 0x1a
0000009C  81F94950          cmp cx,0x5049
000000A0  7531              jnz 0xd3
000000A2  B809B1            mov ax,0xb109
000000A5  BF1000            mov di,0x10
000000A8  CD1A              int 0x1a
000000AA  80E1F0            and cl,0xf0
000000AD  33DB              xor bx,bx
000000AF  890E3600          mov [0x36],cx
000000B3  8A4F02            mov cl,[bx+0x2]
000000B6  33C0              xor ax,ax
000000B8  D1E1              shl cx,1
000000BA  D1E1              shl cx,1
000000BC  D1E1              shl cx,1
000000BE  D1E1              shl cx,1
000000C0  D1E1              shl cx,1
000000C2  D1E1              shl cx,1
000000C4  D1E1              shl cx,1
000000C6  D1E1              shl cx,1
000000C8  D1E1              shl cx,1
000000CA  0207              add al,[bx]
000000CC  43                inc bx
000000CD  E2FB              loop 0xca
000000CF  28060F00          sub [0xf],al
000000D3  3B163600          cmp dx,[0x36]
000000D7  7448              jz 0x121
000000D9  E84D01            call word 0x229
000000DC  E8FD01            call word 0x2dc
000000DF  5F                pop di
000000E0  5E                pop si
000000E1  5A                pop dx
000000E2  59                pop cx
000000E3  5B                pop bx
000000E4  58                pop ax
000000E5  1F                pop ds
000000E6  9D                popfw
000000E7  CB                retf
000000E8  0D0A50            or ax,0x500a
000000EB  7265              jc 0x152
000000ED  7373              jnc 0x162
000000EF  204354            and [bp+di+0x54],al
000000F2  52                push dx
000000F3  4C                dec sp
000000F4  20746F            and [si+0x6f],dh
000000F7  207374            and [bp+di+0x74],dh
000000FA  61                popaw
000000FB  7274              jc 0x171
000000FD  20452D            and [di+0x2d],al
00000100  44                inc sp
00000101  49                dec cx
00000102  53                push bx
00000103  4B                dec bx
00000104  200D              and [di],cl
00000106  0A00              or al,[bx+si]
00000108  0D0A4D            or ax,0x4d0a
0000010B  697373696E        imul si,[bp+di+0x73],word 0x6e69
00000110  6720492F          and [ecx+0x2f],cl
00000114  4F                dec di
00000115  206164            and [bx+di+0x64],ah
00000118  647265            fs jc 0x180
0000011B  7373              jnc 0x190
0000011D  07                pop es
0000011E  0D0A00            or ax,0xa
00000121  BE0802            mov si,0x208
00000124  E80600            call word 0x12d
00000127  EBFE              jmp short 0x127
00000129  B40E              mov ah,0xe
0000012B  CD10              int 0x10
0000012D  BB0700            mov bx,0x7
00000130  2E8A04            mov al,[cs:si]
00000133  46                inc si
00000134  0AC0              or al,al
00000136  75F1              jnz 0x129
00000138  C3                ret
00000139  50                push ax
0000013A  55                push bp
0000013B  8BEC              mov bp,sp
0000013D  C746020000        mov word [bp+0x2],0x0
00000142  5D                pop bp
00000143  1F                pop ds
00000144  FB                sti
00000145  8B0EF201          mov cx,[0x1f2]
00000149  A1F001            mov ax,[0x1f0]
0000014C  51                push cx
0000014D  50                push ax
0000014E  50                push ax
0000014F  53                push bx
00000150  51                push cx
00000151  52                push dx
00000152  56                push si
00000153  57                push di
00000154  87066400          xchg ax,[0x64]
00000158  3D3902            cmp ax,0x239
0000015B  870E6600          xchg cx,[0x66]
0000015F  EB2D              jmp short 0x18e
00000161  2E8B163600        mov dx,[cs:0x36]
00000166  42                inc dx
00000167  42                inc dx
00000168  EC                in al,dx
00000169  2440              and al,0x40
0000016B  EB21              jmp short 0x18e
0000016D  B00A              mov al,0xa
0000016F  E87B01            call word 0x2ed
00000172  B020              mov al,0x20
00000174  E8B501            call word 0x32c
00000177  E8C101            call word 0x33b
0000017A  2420              and al,0x20
0000017C  7510              jnz 0x18e
0000017E  2EC606380081      mov byte [cs:0x38],0x81
00000184  E83000            call word 0x1b7
00000187  5F                pop di
00000188  5E                pop si
00000189  5A                pop dx
0000018A  59                pop cx
0000018B  5B                pop bx
0000018C  58                pop ax
0000018D  CB                retf
0000018E  5F                pop di
0000018F  5E                pop si
00000190  5A                pop dx
00000191  59                pop cx
00000192  5B                pop bx
00000193  58                pop ax
00000194  E82000            call word 0x1b7
00000197  E84201            call word 0x2dc
0000019A  33C0              xor ax,ax
0000019C  8ED8              mov ds,ax
0000019E  8ED0              mov ss,ax
000001A0  BC007C            mov sp,0x7c00
000001A3  FB                sti
000001A4  2E8A163800        mov dl,[cs:0x38]
000001A9  E8F700            call word 0x2a3
000001AC  BE2703            mov si,0x327
000001AF  53                push bx
000001B0  E87AFF            call word 0x12d
000001B3  5B                pop bx
000001B4  06                push es
000001B5  53                push bx
000001B6  CB                retf
000001B7  BE4C00            mov si,0x4c
000001BA  8CC8              mov ax,cs
000001BC  3B4402            cmp ax,[si+0x2]
000001BF  7438              jz 0x1f9
000001C1  A07504            mov al,[0x475]
000001C4  2EA23C00          mov [cs:0x3c],al
000001C8  2E8B163600        mov dx,[cs:0x36]
000001CD  42                inc dx
000001CE  42                inc dx
000001CF  EC                in al,dx
000001D0  2440              and al,0x40
000001D2  7514              jnz 0x1e8
000001D4  B280              mov dl,0x80
000001D6  06                push es
000001D7  E8C900            call word 0x2a3
000001DA  07                pop es
000001DB  750B              jnz 0x1e8
000001DD  2EC606380000      mov byte [cs:0x38],0x0
000001E3  B8240E            mov ax,0xe24
000001E6  CD10              int 0x10
000001E8  B81905            mov ax,0x519
000001EB  8CC9              mov cx,cs
000001ED  8704              xchg ax,[si]
000001EF  874C02            xchg cx,[si+0x2]
000001F2  A38C01            mov [0x18c],ax
000001F5  890E8E01          mov [0x18e],cx
000001F9  BE0B03            mov si,0x30b
000001FC  E82EFF            call word 0x12d
000001FF  E82700            call word 0x229
00000202  B022              mov al,0x22
00000204  E8E600            call word 0x2ed
00000207  E83101            call word 0x33b
0000020A  C3                ret
0000020B  0D0A53            or ax,0x530a
0000020E  7461              jz 0x271
00000210  7274              jc 0x286
00000212  204348            and [bp+di+0x48],al
00000215  3337              xor si,[bx]
00000217  352056            xor ax,0x5620
0000021A  312E352E          xor [0x2e35],bp
0000021E  2E2E2E2E2E07      cs pop es
00000224  0D0A00            or ax,0xa
00000227  2A00              sub al,[bx+si]
00000229  E8A500            call word 0x2d1
0000022C  E8D3FF            call word 0x202
0000022F  B015              mov al,0x15
00000231  E8B900            call word 0x2ed
00000234  B006              mov al,0x6
00000236  E8F300            call word 0x32c
00000239  E8A600            call word 0x2e2
0000023C  E8FC00            call word 0x33b
0000023F  E88F00            call word 0x2d1
00000242  B001              mov al,0x1
00000244  E8A600            call word 0x2ed
00000247  2EC6063D0001      mov byte [cs:0x3d],0x1
0000024D  E8EB00            call word 0x33b
00000250  3C80              cmp al,0x80
00000252  720C              jc 0x260
00000254  3CC0              cmp al,0xc0
00000256  7308              jnc 0x260
00000258  3CB5              cmp al,0xb5
0000025A  730E              jnc 0x26a
0000025C  3CA2              cmp al,0xa2
0000025E  7306              jnc 0x266
00000260  2EC6063D0000      mov byte [cs:0x3d],0x0
00000266  E87900            call word 0x2e2
00000269  C3                ret
0000026A  2EC6063D0002      mov byte [cs:0x3d],0x2
00000270  EBF4              jmp short 0x266
00000272  2EA03A00          mov al,[cs:0x3a]
00000276  F6E6              mul dh
00000278  8AD5              mov dl,ch
0000027A  8AF1              mov dh,cl
0000027C  83E13F            and cx,byte +0x3f
0000027F  49                dec cx
00000280  03C8              add cx,ax
00000282  D0EE              shr dh,1
00000284  D0EE              shr dh,1
00000286  D0EE              shr dh,1
00000288  D0EE              shr dh,1
0000028A  D0EE              shr dh,1
0000028C  D0EE              shr dh,1
0000028E  2EA03A00          mov al,[cs:0x3a]
00000292  B400              mov ah,0x0
00000294  F7E2              mul dx
00000296  2E8A163B00        mov dl,[cs:0x3b]
0000029B  F7E2              mul dx
0000029D  03C1              add ax,cx
0000029F  83D200            adc dx,byte +0x0
000002A2  C3                ret
000002A3  33C9              xor cx,cx
000002A5  B80102            mov ax,0x201
000002A8  8EC1              mov es,cx
000002AA  BB007C            mov bx,0x7c00
000002AD  B600              mov dh,0x0
000002AF  41                inc cx
000002B0  26898FFE01        mov [es:bx+0x1fe],cx
000002B5  CD13              int 0x13
000002B7  2681BFFE0155AA    cmp word [es:bx+0x1fe],0xaa55
000002BE  C3                ret
000002BF  50                push ax
000002C0  2E8A263900        mov ah,[cs:0x39]
000002C5  0AE4              or ah,ah
000002C7  7406              jz 0x2cf
000002C9  E461              in al,0x61
000002CB  FECC              dec ah
000002CD  75FA              jnz 0x2c9
000002CF  58                pop ax
000002D0  C3                ret
000002D1  51                push cx
000002D2  B90A00            mov cx,0xa
000002D5  E8E7FF            call word 0x2bf
000002D8  E2FB              loop 0x2d5
000002DA  59                pop cx
000002DB  C3                ret
000002DC  51                push cx
000002DD  B96400            mov cx,0x64
000002E0  EBF3              jmp short 0x2d5
000002E2  51                push cx
000002E3  B90A00            mov cx,0xa
000002E6  E8F3FF            call word 0x2dc
000002E9  E2FB              loop 0x2e6
000002EB  59                pop cx
000002EC  C3                ret
000002ED  52                push dx
000002EE  2E8A263D00        mov ah,[cs:0x3d]
000002F3  2E8B163600        mov dx,[cs:0x36]
000002F8  42                inc dx
000002F9  0AE4              or ah,ah
000002FB  50                push ax
000002FC  7510              jnz 0x30e
000002FE  E8BEFF            call word 0x2bf
00000301  E8BBFF            call word 0x2bf
00000304  58                pop ax
00000305  EE                out dx,al
00000306  E8B6FF            call word 0x2bf
00000309  E8B3FF            call word 0x2bf
0000030C  5A                pop dx
0000030D  C3                ret
0000030E  80FC02            cmp ah,0x2
00000311  7309              jnc 0x31c
00000313  E8A9FF            call word 0x2bf
00000316  B001              mov al,0x1
00000318  EE                out dx,al
00000319  E8A3FF            call word 0x2bf
0000031C  E8A0FF            call word 0x2bf
0000031F  E89DFF            call word 0x2bf
00000322  58                pop ax
00000323  EE                out dx,al
00000324  E898FF            call word 0x2bf
00000327  E895FF            call word 0x2bf
0000032A  5A                pop dx
0000032B  C3                ret
0000032C  52                push dx
0000032D  2E8B163600        mov dx,[cs:0x36]
00000332  EE                out dx,al
00000333  E889FF            call word 0x2bf
00000336  E886FF            call word 0x2bf
00000339  5A                pop dx
0000033A  C3                ret
0000033B  52                push dx
0000033C  2E8B163600        mov dx,[cs:0x36]
00000341  E87BFF            call word 0x2bf
00000344  E878FF            call word 0x2bf
00000347  EC                in al,dx
00000348  5A                pop dx
00000349  C3                ret
0000034A  52                push dx
0000034B  2E803E3D0002      cmp byte [cs:0x3d],0x2
00000351  2E8B163600        mov dx,[cs:0x36]
00000356  7201              jc 0x359
00000358  42                inc dx
00000359  EC                in al,dx
0000035A  2480              and al,0x80
0000035C  5A                pop dx
0000035D  C3                ret
0000035E  E85EFF            call word 0x2bf
00000361  E85BFF            call word 0x2bf
00000364  51                push cx
00000365  B90010            mov cx,0x1000
00000368  B464              mov ah,0x64
0000036A  E8DDFF            call word 0x34a
0000036D  7413              jz 0x382
0000036F  FECC              dec ah
00000371  75F7              jnz 0x36a
00000373  E2F3              loop 0x368
00000375  B017              mov al,0x17
00000377  E873FF            call word 0x2ed
0000037A  B9D007            mov cx,0x7d0
0000037D  E851FF            call word 0x2d1
00000380  E2FB              loop 0x37d
00000382  B022              mov al,0x22
00000384  E866FF            call word 0x2ed
00000387  E8B1FF            call word 0x33b
0000038A  0BC9              or cx,cx
0000038C  59                pop cx
0000038D  C3                ret
0000038E  B00A              mov al,0xa
00000390  E85AFF            call word 0x2ed
00000393  B01B              mov al,0x1b
00000395  E894FF            call word 0x32c
00000398  E8A0FF            call word 0x33b
0000039B  247F              and al,0x7f
0000039D  B014              mov al,0x14
0000039F  7566              jnz 0x407
000003A1  E838FF            call word 0x2dc
000003A4  E85BFE            call word 0x202
000003A7  E832FF            call word 0x2dc
000003AA  B051              mov al,0x51
000003AC  E83EFF            call word 0x2ed
000003AF  E8ACFF            call word 0x35e
000003B2  7453              jz 0x407
000003B4  3C14              cmp al,0x14
000003B6  754F              jnz 0x407
000003B8  E821FF            call word 0x2dc
000003BB  B053              mov al,0x53
000003BD  E82DFF            call word 0x2ed
000003C0  E89BFF            call word 0x35e
000003C3  7442              jz 0x407
000003C5  3C14              cmp al,0x14
000003C7  7411              jz 0x3da
000003C9  B81027            mov ax,0x2710
000003CC  E802FF            call word 0x2d1
000003CF  48                dec ax
000003D0  75FA              jnz 0x3cc
000003D2  B053              mov al,0x53
000003D4  E816FF            call word 0x2ed
000003D7  E884FF            call word 0x35e
000003DA  E8F4FE            call word 0x2d1
000003DD  B001              mov al,0x1
000003DF  E80BFF            call word 0x2ed
000003E2  E856FF            call word 0x33b
000003E5  A880              test al,0x80
000003E7  7419              jz 0x402
000003E9  B059              mov al,0x59
000003EB  E8FFFE            call word 0x2ed
000003EE  E86DFF            call word 0x35e
000003F1  7404              jz 0x3f7
000003F3  3C14              cmp al,0x14
000003F5  740B              jz 0x402
000003F7  B05A              mov al,0x5a
000003F9  E8F1FE            call word 0x2ed
000003FC  E85FFF            call word 0x35e
000003FF  E8DAFE            call word 0x2dc
00000402  B81400            mov ax,0x14
00000405  3AE0              cmp ah,al
00000407  C3                ret
00000408  E8D1FE            call word 0x2dc
0000040B  B017              mov al,0x17
0000040D  E8DDFE            call word 0x2ed
00000410  E8C9FE            call word 0x2dc
00000413  E8ECFD            call word 0x202
00000416  E9B8FE            jmp word 0x2d1
00000419  2E3A163800        cmp dl,[cs:0x38]
0000041E  755B              jnz 0x47b
00000420  80FC02            cmp ah,0x2
00000423  7441              jz 0x466
00000425  80FC03            cmp ah,0x3
00000428  743F              jz 0x469
0000042A  80FC04            cmp ah,0x4
0000042D  7447              jz 0x476
0000042F  80FC08            cmp ah,0x8
00000432  7478              jz 0x4ac
00000434  80FC01            cmp ah,0x1
00000437  763D              jna 0x476
00000439  80FC40            cmp ah,0x40
0000043C  725A              jc 0x498
0000043E  80FC41            cmp ah,0x41
00000441  7419              jz 0x45c
00000443  80FC42            cmp ah,0x42
00000446  7424              jz 0x46c
00000448  80FC43            cmp ah,0x43
0000044B  7422              jz 0x46f
0000044D  80FC44            cmp ah,0x44
00000450  7424              jz 0x476
00000452  3D2A57            cmp ax,0x572a
00000455  741B              jz 0x472
00000457  B401              mov ah,0x1
00000459  F9                stc
0000045A  EB1C              jmp short 0x478
0000045C  BB55AA            mov bx,0xaa55
0000045F  B421              mov ah,0x21
00000461  B90100            mov cx,0x1
00000464  EB12              jmp short 0x478
00000466  E90101            jmp word 0x56a
00000469  E9FF01            jmp word 0x66b
0000046C  E91001            jmp word 0x57f
0000046F  E90E02            jmp word 0x680
00000472  8CC9              mov cx,cs
00000474  EB00              jmp short 0x476
00000476  32E4              xor ah,ah
00000478  CA0200            retf 0x2
0000047B  80FA81            cmp dl,0x81
0000047E  7218              jc 0x498
00000480  2E803E380080      cmp byte [cs:0x38],0x80
00000486  7510              jnz 0x498
00000488  FECA              dec dl
0000048A  CD63              int 0x63
0000048C  9C                pushfw
0000048D  80FA80            cmp dl,0x80
00000490  7202              jc 0x494
00000492  FEC2              inc dl
00000494  9D                popfw
00000495  CA0200            retf 0x2
00000498  CD63              int 0x63
0000049A  CA0200            retf 0x2
0000049D  B600              mov dh,0x0
0000049F  B90000            mov cx,0x0
000004A2  B201              mov dl,0x1
000004A4  B401              mov ah,0x1
000004A6  5E                pop si
000004A7  5B                pop bx
000004A8  1F                pop ds
000004A9  F9                stc
000004AA  EBCC              jmp short 0x478
000004AC  1E                push ds
000004AD  53                push bx
000004AE  56                push si
000004AF  0E                push cs
000004B0  1F                pop ds
000004B1  E8DAFE            call word 0x38e
000004B4  74E7              jz 0x49d
000004B6  3C14              cmp al,0x14
000004B8  75E3              jnz 0x49d
000004BA  BE1C00            mov si,0x1c
000004BD  BB4000            mov bx,0x40
000004C0  C744020100        mov word [si+0x2],0x1
000004C5  895C04            mov [si+0x4],bx
000004C8  8C5C06            mov [si+0x6],ds
000004CB  33C0              xor ax,ax
000004CD  894408            mov [si+0x8],ax
000004D0  89440A            mov [si+0xa],ax
000004D3  B442              mov ah,0x42
000004D5  8A163800          mov dl,[0x38]
000004D9  CD13              int 0x13
000004DB  7232              jc 0x50f
000004DD  8A07              mov al,[bx]
000004DF  3CEB              cmp al,0xeb
000004E1  7432              jz 0x515
000004E3  3CE9              cmp al,0xe9
000004E5  742E              jz 0x515
000004E7  C6440201          mov byte [si+0x2],0x1
000004EB  895C04            mov [si+0x4],bx
000004EE  8C5C06            mov [si+0x6],ds
000004F1  8B87C601          mov ax,[bx+0x1c6]
000004F5  894408            mov [si+0x8],ax
000004F8  8B87C801          mov ax,[bx+0x1c8]
000004FC  89440A            mov [si+0xa],ax
000004FF  B442              mov ah,0x42
00000501  CD13              int 0x13
00000503  720A              jc 0x50f
00000505  8A07              mov al,[bx]
00000507  3CEB              cmp al,0xeb
00000509  740A              jz 0x515
0000050B  3CE9              cmp al,0xe9
0000050D  7406              jz 0x515
0000050F  B8FF00            mov ax,0xff
00000512  EB20              jmp short 0x534
00000514  90                nop
00000515  8A4F18            mov cl,[bx+0x18]
00000518  8A471A            mov al,[bx+0x1a]
0000051B  880E3A00          mov [0x3a],cl
0000051F  A23B00            mov [0x3b],al
00000522  F6E1              mul cl
00000524  8BC8              mov cx,ax
00000526  0BC0              or ax,ax
00000528  B008              mov al,0x8
0000052A  7408              jz 0x534
0000052C  8B4720            mov ax,[bx+0x20]
0000052F  8B5722            mov dx,[bx+0x22]
00000532  F7F1              div cx
00000534  8AE8              mov ch,al
00000536  8ACC              mov cl,ah
00000538  8A163C00          mov dl,[0x3c]
0000053C  D0E1              shl cl,1
0000053E  D0E1              shl cl,1
00000540  D0E1              shl cl,1
00000542  D0E1              shl cl,1
00000544  D0E1              shl cl,1
00000546  D0E1              shl cl,1
00000548  FEC2              inc dl
0000054A  803E380000        cmp byte [0x38],0x0
0000054F  7509              jnz 0x55a
00000551  8D7F0B            lea di,[bx+0xb]
00000554  1E                push ds
00000555  B310              mov bl,0x10
00000557  07                pop es
00000558  B201              mov dl,0x1
0000055A  0A0E3A00          or cl,[0x3a]
0000055E  8A363B00          mov dh,[0x3b]
00000562  FECE              dec dh
00000564  5E                pop si
00000565  5B                pop bx
00000566  1F                pop ds
00000567  E90CFF            jmp word 0x476
0000056A  06                push es
0000056B  50                push ax
0000056C  53                push bx
0000056D  51                push cx
0000056E  52                push dx
0000056F  56                push si
00000570  57                push di
00000571  55                push bp
00000572  8BEC              mov bp,sp
00000574  83EC08            sub sp,byte +0x8
00000577  8946FA            mov [bp-0x6],ax
0000057A  E8F5FC            call word 0x272
0000057D  EB1C              jmp short 0x59b
0000057F  06                push es
00000580  50                push ax
00000581  53                push bx
00000582  51                push cx
00000583  52                push dx
00000584  56                push si
00000585  57                push di
00000586  55                push bp
00000587  8BEC              mov bp,sp
00000589  83EC08            sub sp,byte +0x8
0000058C  8B4402            mov ax,[si+0x2]
0000058F  8946FA            mov [bp-0x6],ax
00000592  C45C04            les bx,[si+0x4]
00000595  8B4408            mov ax,[si+0x8]
00000598  8B540A            mov dx,[si+0xa]
0000059B  8946FC            mov [bp-0x4],ax
0000059E  8956FE            mov [bp-0x2],dx
000005A1  895EF8            mov [bp-0x8],bx
000005A4  BF0300            mov di,0x3
000005A7  E8E4FD            call word 0x38e
000005AA  7468              jz 0x614
000005AC  3C14              cmp al,0x14
000005AE  7564              jnz 0x614
000005B0  83CF40            or di,byte +0x40
000005B3  8B5EF8            mov bx,[bp-0x8]
000005B6  B054              mov al,0x54
000005B8  E832FD            call word 0x2ed
000005BB  8A46FC            mov al,[bp-0x4]
000005BE  E86BFD            call word 0x32c
000005C1  8A46FD            mov al,[bp-0x3]
000005C4  E865FD            call word 0x32c
000005C7  8A46FE            mov al,[bp-0x2]
000005CA  E85FFD            call word 0x32c
000005CD  8A46FF            mov al,[bp-0x1]
000005D0  E859FD            call word 0x32c
000005D3  8A46FA            mov al,[bp-0x6]
000005D6  B500              mov ch,0x0
000005D8  8AC8              mov cl,al
000005DA  E84FFD            call word 0x32c
000005DD  D1E1              shl cx,1
000005DF  D1E1              shl cx,1
000005E1  D1E1              shl cx,1
000005E3  E878FD            call word 0x35e
000005E6  742C              jz 0x614
000005E8  3C1D              cmp al,0x1d
000005EA  7528              jnz 0x614
000005EC  B028              mov al,0x28
000005EE  E8FCFC            call word 0x2ed
000005F1  E847FD            call word 0x33b
000005F4  B240              mov dl,0x40
000005F6  E842FD            call word 0x33b
000005F9  268807            mov [es:bx],al
000005FC  43                inc bx
000005FD  FECA              dec dl
000005FF  75F5              jnz 0x5f6
00000601  B055              mov al,0x55
00000603  E8E7FC            call word 0x2ed
00000606  E2DB              loop 0x5e3
00000608  E853FD            call word 0x35e
0000060B  7407              jz 0x614
0000060D  3C14              cmp al,0x14
0000060F  F8                clc
00000610  7502              jnz 0x614
00000612  EB41              jmp short 0x655
00000614  E8F1FD            call word 0x408
00000617  B001              mov al,0x1
00000619  E8D1FC            call word 0x2ed
0000061C  E81CFD            call word 0x33b
0000061F  A880              test al,0x80
00000621  741B              jz 0x63e
00000623  B05A              mov al,0x5a
00000625  E8C5FC            call word 0x2ed
00000628  E833FD            call word 0x35e
0000062B  7404              jz 0x631
0000062D  3C14              cmp al,0x14
0000062F  740D              jz 0x63e
00000631  E8D4FD            call word 0x408
00000634  B015              mov al,0x15
00000636  E8B4FC            call word 0x2ed
00000639  B006              mov al,0x6
0000063B  E8EEFC            call word 0x32c
0000063E  E89BFC            call word 0x2dc
00000641  E8BEFB            call word 0x202
00000644  8BC7              mov ax,di
00000646  4F                dec di
00000647  83E70F            and di,byte +0xf
0000064A  F9                stc
0000064B  7408              jz 0x655
0000064D  A840              test al,0x40
0000064F  7517              jnz 0x668
00000651  A820              test al,0x20
00000653  7553              jnz 0x6a8
00000655  8BE5              mov sp,bp
00000657  5D                pop bp
00000658  5F                pop di
00000659  5E                pop si
0000065A  5A                pop dx
0000065B  59                pop cx
0000065C  5B                pop bx
0000065D  58                pop ax
0000065E  B400              mov ah,0x0
00000660  7302              jnc 0x664
00000662  FEC4              inc ah
00000664  07                pop es
00000665  E910FE            jmp word 0x478
00000668  E93CFF            jmp word 0x5a7
0000066B  06                push es
0000066C  50                push ax
0000066D  53                push bx
0000066E  51                push cx
0000066F  52                push dx
00000670  56                push si
00000671  57                push di
00000672  55                push bp
00000673  8BEC              mov bp,sp
00000675  83EC08            sub sp,byte +0x8
00000678  8946FA            mov [bp-0x6],ax
0000067B  E8F4FB            call word 0x272
0000067E  EB1C              jmp short 0x69c
00000680  06                push es
00000681  50                push ax
00000682  53                push bx
00000683  51                push cx
00000684  52                push dx
00000685  56                push si
00000686  57                push di
00000687  55                push bp
00000688  8BEC              mov bp,sp
0000068A  83EC08            sub sp,byte +0x8
0000068D  8B4402            mov ax,[si+0x2]
00000690  8946FA            mov [bp-0x6],ax
00000693  C45C04            les bx,[si+0x4]
00000696  8B4408            mov ax,[si+0x8]
00000699  8B540A            mov dx,[si+0xa]
0000069C  8946FC            mov [bp-0x4],ax
0000069F  8956FE            mov [bp-0x2],dx
000006A2  895EF8            mov [bp-0x8],bx
000006A5  BF0300            mov di,0x3
000006A8  E8E3FC            call word 0x38e
000006AB  7461              jz 0x70e
000006AD  3C14              cmp al,0x14
000006AF  755D              jnz 0x70e
000006B1  83CF20            or di,byte +0x20
000006B4  8B5EF8            mov bx,[bp-0x8]
000006B7  B056              mov al,0x56
000006B9  E831FC            call word 0x2ed
000006BC  8A46FC            mov al,[bp-0x4]
000006BF  E86AFC            call word 0x32c
000006C2  8A46FD            mov al,[bp-0x3]
000006C5  E864FC            call word 0x32c
000006C8  8A46FE            mov al,[bp-0x2]
000006CB  E85EFC            call word 0x32c
000006CE  8A46FF            mov al,[bp-0x1]
000006D1  E858FC            call word 0x32c
000006D4  8A46FA            mov al,[bp-0x6]
000006D7  B500              mov ch,0x0
000006D9  8AC8              mov cl,al
000006DB  E84EFC            call word 0x32c
000006DE  D1E1              shl cx,1
000006E0  D1E1              shl cx,1
000006E2  D1E1              shl cx,1
000006E4  E877FC            call word 0x35e
000006E7  7425              jz 0x70e
000006E9  3C1E              cmp al,0x1e
000006EB  7521              jnz 0x70e
000006ED  B02B              mov al,0x2b
000006EF  E8FBFB            call word 0x2ed
000006F2  B040              mov al,0x40
000006F4  E835FC            call word 0x32c
000006F7  B240              mov dl,0x40
000006F9  268A07            mov al,[es:bx]
000006FC  43                inc bx
000006FD  E82CFC            call word 0x32c
00000700  FECA              dec dl
00000702  75F5              jnz 0x6f9
00000704  B057              mov al,0x57
00000706  E8E4FB            call word 0x2ed
00000709  E2D9              loop 0x6e4
0000070B  E9FAFE            jmp word 0x608
0000070E  E903FF            jmp word 0x614
00000711  0000              add [bx+si],al
00000713  0000              add [bx+si],al
00000715  0000              add [bx+si],al
00000717  0000              add [bx+si],al
00000719  0000              add [bx+si],al
0000071B  0000              add [bx+si],al
0000071D  0000              add [bx+si],al
0000071F  0000              add [bx+si],al
00000721  0000              add [bx+si],al
00000723  0000              add [bx+si],al
00000725  0000              add [bx+si],al
00000727  0000              add [bx+si],al
00000729  0000              add [bx+si],al
0000072B  0000              add [bx+si],al
0000072D  0000              add [bx+si],al
0000072F  0000              add [bx+si],al
00000731  0000              add [bx+si],al
00000733  0000              add [bx+si],al
00000735  0000              add [bx+si],al
00000737  0000              add [bx+si],al
00000739  0000              add [bx+si],al
0000073B  0000              add [bx+si],al
0000073D  0000              add [bx+si],al
0000073F  0000              add [bx+si],al
00000741  0000              add [bx+si],al
00000743  0000              add [bx+si],al
00000745  0000              add [bx+si],al
00000747  0000              add [bx+si],al
00000749  0000              add [bx+si],al
0000074B  0000              add [bx+si],al
0000074D  0000              add [bx+si],al
0000074F  0000              add [bx+si],al
00000751  0000              add [bx+si],al
00000753  0000              add [bx+si],al
00000755  0000              add [bx+si],al
00000757  0000              add [bx+si],al
00000759  0000              add [bx+si],al
0000075B  0000              add [bx+si],al
0000075D  0000              add [bx+si],al
0000075F  0000              add [bx+si],al
00000761  0000              add [bx+si],al
00000763  0000              add [bx+si],al
00000765  0000              add [bx+si],al
00000767  0000              add [bx+si],al
00000769  0000              add [bx+si],al
0000076B  0000              add [bx+si],al
0000076D  0000              add [bx+si],al
0000076F  0000              add [bx+si],al
00000771  0000              add [bx+si],al
00000773  0000              add [bx+si],al
00000775  0000              add [bx+si],al
00000777  0000              add [bx+si],al
00000779  0000              add [bx+si],al
0000077B  0000              add [bx+si],al
0000077D  0000              add [bx+si],al
0000077F  0000              add [bx+si],al
00000781  0000              add [bx+si],al
00000783  0000              add [bx+si],al
00000785  0000              add [bx+si],al
00000787  0000              add [bx+si],al
00000789  0000              add [bx+si],al
0000078B  0000              add [bx+si],al
0000078D  0000              add [bx+si],al
0000078F  0000              add [bx+si],al
00000791  0000              add [bx+si],al
00000793  0000              add [bx+si],al
00000795  0000              add [bx+si],al
00000797  0000              add [bx+si],al
00000799  0000              add [bx+si],al
0000079B  0000              add [bx+si],al
0000079D  0000              add [bx+si],al
0000079F  0000              add [bx+si],al
000007A1  0000              add [bx+si],al
000007A3  0000              add [bx+si],al
000007A5  0000              add [bx+si],al
000007A7  0000              add [bx+si],al
000007A9  0000              add [bx+si],al
000007AB  0000              add [bx+si],al
000007AD  0000              add [bx+si],al
000007AF  0000              add [bx+si],al
000007B1  0000              add [bx+si],al
000007B3  0000              add [bx+si],al
000007B5  0000              add [bx+si],al
000007B7  0000              add [bx+si],al
000007B9  0000              add [bx+si],al
000007BB  0000              add [bx+si],al
000007BD  0000              add [bx+si],al
000007BF  0000              add [bx+si],al
000007C1  0000              add [bx+si],al
000007C3  0000              add [bx+si],al
000007C5  0000              add [bx+si],al
000007C7  0000              add [bx+si],al
000007C9  0000              add [bx+si],al
000007CB  0000              add [bx+si],al
000007CD  0000              add [bx+si],al
000007CF  0000              add [bx+si],al
000007D1  0000              add [bx+si],al
000007D3  0000              add [bx+si],al
000007D5  0000              add [bx+si],al
000007D7  0000              add [bx+si],al
000007D9  0000              add [bx+si],al
000007DB  0000              add [bx+si],al
000007DD  0000              add [bx+si],al
000007DF  0000              add [bx+si],al
000007E1  0000              add [bx+si],al
000007E3  0000              add [bx+si],al
000007E5  0000              add [bx+si],al
000007E7  0000              add [bx+si],al
000007E9  0000              add [bx+si],al
000007EB  0000              add [bx+si],al
000007ED  0000              add [bx+si],al
000007EF  0000              add [bx+si],al
000007F1  0000              add [bx+si],al
000007F3  0000              add [bx+si],al
000007F5  0000              add [bx+si],al
000007F7  0000              add [bx+si],al
000007F9  0000              add [bx+si],al
000007FB  0000              add [bx+si],al
000007FD  0000              add [bx+si],al
000007FF  0000              add [bx+si],al
00000801  0000              add [bx+si],al
00000803  0000              add [bx+si],al
00000805  0000              add [bx+si],al
00000807  0000              add [bx+si],al
00000809  0000              add [bx+si],al
0000080B  0000              add [bx+si],al
0000080D  0000              add [bx+si],al
0000080F  0000              add [bx+si],al
00000811  0000              add [bx+si],al
00000813  0000              add [bx+si],al
00000815  0000              add [bx+si],al
00000817  0000              add [bx+si],al
00000819  0000              add [bx+si],al
0000081B  0000              add [bx+si],al
0000081D  0000              add [bx+si],al
0000081F  0000              add [bx+si],al
00000821  0000              add [bx+si],al
00000823  0000              add [bx+si],al
00000825  0000              add [bx+si],al
00000827  0000              add [bx+si],al
00000829  0000              add [bx+si],al
0000082B  0000              add [bx+si],al
0000082D  0000              add [bx+si],al
0000082F  0000              add [bx+si],al
00000831  0000              add [bx+si],al
00000833  0000              add [bx+si],al
00000835  0000              add [bx+si],al
00000837  0000              add [bx+si],al
00000839  0000              add [bx+si],al
0000083B  0000              add [bx+si],al
0000083D  0000              add [bx+si],al
0000083F  0000              add [bx+si],al
00000841  0000              add [bx+si],al
00000843  0000              add [bx+si],al
00000845  0000              add [bx+si],al
00000847  0000              add [bx+si],al
00000849  0000              add [bx+si],al
0000084B  0000              add [bx+si],al
0000084D  0000              add [bx+si],al
0000084F  0000              add [bx+si],al
00000851  0000              add [bx+si],al
00000853  0000              add [bx+si],al
00000855  0000              add [bx+si],al
00000857  0000              add [bx+si],al
00000859  0000              add [bx+si],al
0000085B  0000              add [bx+si],al
0000085D  0000              add [bx+si],al
0000085F  0000              add [bx+si],al
00000861  0000              add [bx+si],al
00000863  0000              add [bx+si],al
00000865  0000              add [bx+si],al
00000867  0000              add [bx+si],al
00000869  0000              add [bx+si],al
0000086B  0000              add [bx+si],al
0000086D  0000              add [bx+si],al
0000086F  0000              add [bx+si],al
00000871  0000              add [bx+si],al
00000873  0000              add [bx+si],al
00000875  0000              add [bx+si],al
00000877  0000              add [bx+si],al
00000879  0000              add [bx+si],al
0000087B  0000              add [bx+si],al
0000087D  0000              add [bx+si],al
0000087F  0000              add [bx+si],al
00000881  0000              add [bx+si],al
00000883  0000              add [bx+si],al
00000885  0000              add [bx+si],al
00000887  0000              add [bx+si],al
00000889  0000              add [bx+si],al
0000088B  0000              add [bx+si],al
0000088D  0000              add [bx+si],al
0000088F  0000              add [bx+si],al
00000891  0000              add [bx+si],al
00000893  0000              add [bx+si],al
00000895  0000              add [bx+si],al
00000897  0000              add [bx+si],al
00000899  0000              add [bx+si],al
0000089B  0000              add [bx+si],al
0000089D  0000              add [bx+si],al
0000089F  0000              add [bx+si],al
000008A1  0000              add [bx+si],al
000008A3  0000              add [bx+si],al
000008A5  0000              add [bx+si],al
000008A7  0000              add [bx+si],al
000008A9  0000              add [bx+si],al
000008AB  0000              add [bx+si],al
000008AD  0000              add [bx+si],al
000008AF  0000              add [bx+si],al
000008B1  0000              add [bx+si],al
000008B3  0000              add [bx+si],al
000008B5  0000              add [bx+si],al
000008B7  0000              add [bx+si],al
000008B9  0000              add [bx+si],al
000008BB  0000              add [bx+si],al
000008BD  0000              add [bx+si],al
000008BF  0000              add [bx+si],al
000008C1  0000              add [bx+si],al
000008C3  0000              add [bx+si],al
000008C5  0000              add [bx+si],al
000008C7  0000              add [bx+si],al
000008C9  0000              add [bx+si],al
000008CB  0000              add [bx+si],al
000008CD  0000              add [bx+si],al
000008CF  0000              add [bx+si],al
000008D1  0000              add [bx+si],al
000008D3  0000              add [bx+si],al
000008D5  0000              add [bx+si],al
000008D7  0000              add [bx+si],al
000008D9  0000              add [bx+si],al
000008DB  0000              add [bx+si],al
000008DD  0000              add [bx+si],al
000008DF  0000              add [bx+si],al
000008E1  0000              add [bx+si],al
000008E3  0000              add [bx+si],al
000008E5  0000              add [bx+si],al
000008E7  0000              add [bx+si],al
000008E9  0000              add [bx+si],al
000008EB  0000              add [bx+si],al
000008ED  0000              add [bx+si],al
000008EF  0000              add [bx+si],al
000008F1  0000              add [bx+si],al
000008F3  0000              add [bx+si],al
000008F5  0000              add [bx+si],al
000008F7  0000              add [bx+si],al
000008F9  0000              add [bx+si],al
000008FB  0000              add [bx+si],al
000008FD  0000              add [bx+si],al
000008FF  0000              add [bx+si],al
00000901  0000              add [bx+si],al
00000903  0000              add [bx+si],al
00000905  0000              add [bx+si],al
00000907  0000              add [bx+si],al
00000909  0000              add [bx+si],al
0000090B  0000              add [bx+si],al
0000090D  0000              add [bx+si],al
0000090F  0000              add [bx+si],al
00000911  0000              add [bx+si],al
00000913  0000              add [bx+si],al
00000915  0000              add [bx+si],al
00000917  0000              add [bx+si],al
00000919  0000              add [bx+si],al
0000091B  0000              add [bx+si],al
0000091D  0000              add [bx+si],al
0000091F  0000              add [bx+si],al
00000921  0000              add [bx+si],al
00000923  0000              add [bx+si],al
00000925  0000              add [bx+si],al
00000927  0000              add [bx+si],al
00000929  0000              add [bx+si],al
0000092B  0000              add [bx+si],al
0000092D  0000              add [bx+si],al
0000092F  0000              add [bx+si],al
00000931  0000              add [bx+si],al
00000933  0000              add [bx+si],al
00000935  0000              add [bx+si],al
00000937  0000              add [bx+si],al
00000939  0000              add [bx+si],al
0000093B  0000              add [bx+si],al
0000093D  0000              add [bx+si],al
0000093F  0000              add [bx+si],al
00000941  0000              add [bx+si],al
00000943  0000              add [bx+si],al
00000945  0000              add [bx+si],al
00000947  0000              add [bx+si],al
00000949  0000              add [bx+si],al
0000094B  0000              add [bx+si],al
0000094D  0000              add [bx+si],al
0000094F  0000              add [bx+si],al
00000951  0000              add [bx+si],al
00000953  0000              add [bx+si],al
00000955  0000              add [bx+si],al
00000957  0000              add [bx+si],al
00000959  0000              add [bx+si],al
0000095B  0000              add [bx+si],al
0000095D  0000              add [bx+si],al
0000095F  0000              add [bx+si],al
00000961  0000              add [bx+si],al
00000963  0000              add [bx+si],al
00000965  0000              add [bx+si],al
00000967  0000              add [bx+si],al
00000969  0000              add [bx+si],al
0000096B  0000              add [bx+si],al
0000096D  0000              add [bx+si],al
0000096F  0000              add [bx+si],al
00000971  0000              add [bx+si],al
00000973  0000              add [bx+si],al
00000975  0000              add [bx+si],al
00000977  0000              add [bx+si],al
00000979  0000              add [bx+si],al
0000097B  0000              add [bx+si],al
0000097D  0000              add [bx+si],al
0000097F  0000              add [bx+si],al
00000981  0000              add [bx+si],al
00000983  0000              add [bx+si],al
00000985  0000              add [bx+si],al
00000987  0000              add [bx+si],al
00000989  0000              add [bx+si],al
0000098B  0000              add [bx+si],al
0000098D  0000              add [bx+si],al
0000098F  0000              add [bx+si],al
00000991  0000              add [bx+si],al
00000993  0000              add [bx+si],al
00000995  0000              add [bx+si],al
00000997  0000              add [bx+si],al
00000999  0000              add [bx+si],al
0000099B  0000              add [bx+si],al
0000099D  0000              add [bx+si],al
0000099F  0000              add [bx+si],al
000009A1  0000              add [bx+si],al
000009A3  0000              add [bx+si],al
000009A5  0000              add [bx+si],al
000009A7  0000              add [bx+si],al
000009A9  0000              add [bx+si],al
000009AB  0000              add [bx+si],al
000009AD  0000              add [bx+si],al
000009AF  0000              add [bx+si],al
000009B1  0000              add [bx+si],al
000009B3  0000              add [bx+si],al
000009B5  0000              add [bx+si],al
000009B7  0000              add [bx+si],al
000009B9  0000              add [bx+si],al
000009BB  0000              add [bx+si],al
000009BD  0000              add [bx+si],al
000009BF  0000              add [bx+si],al
000009C1  0000              add [bx+si],al
000009C3  0000              add [bx+si],al
000009C5  0000              add [bx+si],al
000009C7  0000              add [bx+si],al
000009C9  0000              add [bx+si],al
000009CB  0000              add [bx+si],al
000009CD  0000              add [bx+si],al
000009CF  0000              add [bx+si],al
000009D1  0000              add [bx+si],al
000009D3  0000              add [bx+si],al
000009D5  0000              add [bx+si],al
000009D7  0000              add [bx+si],al
000009D9  0000              add [bx+si],al
000009DB  0000              add [bx+si],al
000009DD  0000              add [bx+si],al
000009DF  0000              add [bx+si],al
000009E1  0000              add [bx+si],al
000009E3  0000              add [bx+si],al
000009E5  0000              add [bx+si],al
000009E7  0000              add [bx+si],al
000009E9  0000              add [bx+si],al
000009EB  0000              add [bx+si],al
000009ED  0000              add [bx+si],al
000009EF  0000              add [bx+si],al
000009F1  0000              add [bx+si],al
000009F3  0000              add [bx+si],al
000009F5  0000              add [bx+si],al
000009F7  0000              add [bx+si],al
000009F9  0000              add [bx+si],al
000009FB  0000              add [bx+si],al
000009FD  0000              add [bx+si],al
000009FF  0000              add [bx+si],al
00000A01  0000              add [bx+si],al
00000A03  0000              add [bx+si],al
00000A05  0000              add [bx+si],al
00000A07  0000              add [bx+si],al
00000A09  0000              add [bx+si],al
00000A0B  0000              add [bx+si],al
00000A0D  0000              add [bx+si],al
00000A0F  0000              add [bx+si],al
00000A11  0000              add [bx+si],al
00000A13  0000              add [bx+si],al
00000A15  0000              add [bx+si],al
00000A17  0000              add [bx+si],al
00000A19  0000              add [bx+si],al
00000A1B  0000              add [bx+si],al
00000A1D  0000              add [bx+si],al
00000A1F  0000              add [bx+si],al
00000A21  0000              add [bx+si],al
00000A23  0000              add [bx+si],al
00000A25  0000              add [bx+si],al
00000A27  0000              add [bx+si],al
00000A29  0000              add [bx+si],al
00000A2B  0000              add [bx+si],al
00000A2D  0000              add [bx+si],al
00000A2F  0000              add [bx+si],al
00000A31  0000              add [bx+si],al
00000A33  0000              add [bx+si],al
00000A35  0000              add [bx+si],al
00000A37  0000              add [bx+si],al
00000A39  0000              add [bx+si],al
00000A3B  0000              add [bx+si],al
00000A3D  0000              add [bx+si],al
00000A3F  0000              add [bx+si],al
00000A41  0000              add [bx+si],al
00000A43  0000              add [bx+si],al
00000A45  0000              add [bx+si],al
00000A47  0000              add [bx+si],al
00000A49  0000              add [bx+si],al
00000A4B  0000              add [bx+si],al
00000A4D  0000              add [bx+si],al
00000A4F  0000              add [bx+si],al
00000A51  0000              add [bx+si],al
00000A53  0000              add [bx+si],al
00000A55  0000              add [bx+si],al
00000A57  0000              add [bx+si],al
00000A59  0000              add [bx+si],al
00000A5B  0000              add [bx+si],al
00000A5D  0000              add [bx+si],al
00000A5F  0000              add [bx+si],al
00000A61  0000              add [bx+si],al
00000A63  0000              add [bx+si],al
00000A65  0000              add [bx+si],al
00000A67  0000              add [bx+si],al
00000A69  0000              add [bx+si],al
00000A6B  0000              add [bx+si],al
00000A6D  0000              add [bx+si],al
00000A6F  0000              add [bx+si],al
00000A71  0000              add [bx+si],al
00000A73  0000              add [bx+si],al
00000A75  0000              add [bx+si],al
00000A77  0000              add [bx+si],al
00000A79  0000              add [bx+si],al
00000A7B  0000              add [bx+si],al
00000A7D  0000              add [bx+si],al
00000A7F  0000              add [bx+si],al
00000A81  0000              add [bx+si],al
00000A83  0000              add [bx+si],al
00000A85  0000              add [bx+si],al
00000A87  0000              add [bx+si],al
00000A89  0000              add [bx+si],al
00000A8B  0000              add [bx+si],al
00000A8D  0000              add [bx+si],al
00000A8F  0000              add [bx+si],al
00000A91  0000              add [bx+si],al
00000A93  0000              add [bx+si],al
00000A95  0000              add [bx+si],al
00000A97  0000              add [bx+si],al
00000A99  0000              add [bx+si],al
00000A9B  0000              add [bx+si],al
00000A9D  0000              add [bx+si],al
00000A9F  0000              add [bx+si],al
00000AA1  0000              add [bx+si],al
00000AA3  0000              add [bx+si],al
00000AA5  0000              add [bx+si],al
00000AA7  0000              add [bx+si],al
00000AA9  0000              add [bx+si],al
00000AAB  0000              add [bx+si],al
00000AAD  0000              add [bx+si],al
00000AAF  0000              add [bx+si],al
00000AB1  0000              add [bx+si],al
00000AB3  0000              add [bx+si],al
00000AB5  0000              add [bx+si],al
00000AB7  0000              add [bx+si],al
00000AB9  0000              add [bx+si],al
00000ABB  0000              add [bx+si],al
00000ABD  0000              add [bx+si],al
00000ABF  0000              add [bx+si],al
00000AC1  0000              add [bx+si],al
00000AC3  0000              add [bx+si],al
00000AC5  0000              add [bx+si],al
00000AC7  0000              add [bx+si],al
00000AC9  0000              add [bx+si],al
00000ACB  0000              add [bx+si],al
00000ACD  0000              add [bx+si],al
00000ACF  0000              add [bx+si],al
00000AD1  0000              add [bx+si],al
00000AD3  0000              add [bx+si],al
00000AD5  0000              add [bx+si],al
00000AD7  0000              add [bx+si],al
00000AD9  0000              add [bx+si],al
00000ADB  0000              add [bx+si],al
00000ADD  0000              add [bx+si],al
00000ADF  0000              add [bx+si],al
00000AE1  0000              add [bx+si],al
00000AE3  0000              add [bx+si],al
00000AE5  0000              add [bx+si],al
00000AE7  0000              add [bx+si],al
00000AE9  0000              add [bx+si],al
00000AEB  0000              add [bx+si],al
00000AED  0000              add [bx+si],al
00000AEF  0000              add [bx+si],al
00000AF1  0000              add [bx+si],al
00000AF3  0000              add [bx+si],al
00000AF5  0000              add [bx+si],al
00000AF7  0000              add [bx+si],al
00000AF9  0000              add [bx+si],al
00000AFB  0000              add [bx+si],al
00000AFD  0000              add [bx+si],al
00000AFF  0000              add [bx+si],al
00000B01  0000              add [bx+si],al
00000B03  0000              add [bx+si],al
00000B05  0000              add [bx+si],al
00000B07  0000              add [bx+si],al
00000B09  0000              add [bx+si],al
00000B0B  0000              add [bx+si],al
00000B0D  0000              add [bx+si],al
00000B0F  0000              add [bx+si],al
00000B11  0000              add [bx+si],al
00000B13  0000              add [bx+si],al
00000B15  0000              add [bx+si],al
00000B17  0000              add [bx+si],al
00000B19  0000              add [bx+si],al
00000B1B  0000              add [bx+si],al
00000B1D  0000              add [bx+si],al
00000B1F  0000              add [bx+si],al
00000B21  0000              add [bx+si],al
00000B23  0000              add [bx+si],al
00000B25  0000              add [bx+si],al
00000B27  0000              add [bx+si],al
00000B29  0000              add [bx+si],al
00000B2B  0000              add [bx+si],al
00000B2D  0000              add [bx+si],al
00000B2F  0000              add [bx+si],al
00000B31  0000              add [bx+si],al
00000B33  0000              add [bx+si],al
00000B35  0000              add [bx+si],al
00000B37  0000              add [bx+si],al
00000B39  0000              add [bx+si],al
00000B3B  0000              add [bx+si],al
00000B3D  0000              add [bx+si],al
00000B3F  0000              add [bx+si],al
00000B41  0000              add [bx+si],al
00000B43  0000              add [bx+si],al
00000B45  0000              add [bx+si],al
00000B47  0000              add [bx+si],al
00000B49  0000              add [bx+si],al
00000B4B  0000              add [bx+si],al
00000B4D  0000              add [bx+si],al
00000B4F  0000              add [bx+si],al
00000B51  0000              add [bx+si],al
00000B53  0000              add [bx+si],al
00000B55  0000              add [bx+si],al
00000B57  0000              add [bx+si],al
00000B59  0000              add [bx+si],al
00000B5B  0000              add [bx+si],al
00000B5D  0000              add [bx+si],al
00000B5F  0000              add [bx+si],al
00000B61  0000              add [bx+si],al
00000B63  0000              add [bx+si],al
00000B65  0000              add [bx+si],al
00000B67  0000              add [bx+si],al
00000B69  0000              add [bx+si],al
00000B6B  0000              add [bx+si],al
00000B6D  0000              add [bx+si],al
00000B6F  0000              add [bx+si],al
00000B71  0000              add [bx+si],al
00000B73  0000              add [bx+si],al
00000B75  0000              add [bx+si],al
00000B77  0000              add [bx+si],al
00000B79  0000              add [bx+si],al
00000B7B  0000              add [bx+si],al
00000B7D  0000              add [bx+si],al
00000B7F  0000              add [bx+si],al
00000B81  0000              add [bx+si],al
00000B83  0000              add [bx+si],al
00000B85  0000              add [bx+si],al
00000B87  0000              add [bx+si],al
00000B89  0000              add [bx+si],al
00000B8B  0000              add [bx+si],al
00000B8D  0000              add [bx+si],al
00000B8F  0000              add [bx+si],al
00000B91  0000              add [bx+si],al
00000B93  0000              add [bx+si],al
00000B95  0000              add [bx+si],al
00000B97  0000              add [bx+si],al
00000B99  0000              add [bx+si],al
00000B9B  0000              add [bx+si],al
00000B9D  0000              add [bx+si],al
00000B9F  0000              add [bx+si],al
00000BA1  0000              add [bx+si],al
00000BA3  0000              add [bx+si],al
00000BA5  0000              add [bx+si],al
00000BA7  0000              add [bx+si],al
00000BA9  0000              add [bx+si],al
00000BAB  0000              add [bx+si],al
00000BAD  0000              add [bx+si],al
00000BAF  0000              add [bx+si],al
00000BB1  0000              add [bx+si],al
00000BB3  0000              add [bx+si],al
00000BB5  0000              add [bx+si],al
00000BB7  0000              add [bx+si],al
00000BB9  0000              add [bx+si],al
00000BBB  0000              add [bx+si],al
00000BBD  0000              add [bx+si],al
00000BBF  0000              add [bx+si],al
00000BC1  0000              add [bx+si],al
00000BC3  0000              add [bx+si],al
00000BC5  0000              add [bx+si],al
00000BC7  0000              add [bx+si],al
00000BC9  0000              add [bx+si],al
00000BCB  0000              add [bx+si],al
00000BCD  0000              add [bx+si],al
00000BCF  0000              add [bx+si],al
00000BD1  0000              add [bx+si],al
00000BD3  0000              add [bx+si],al
00000BD5  0000              add [bx+si],al
00000BD7  0000              add [bx+si],al
00000BD9  0000              add [bx+si],al
00000BDB  0000              add [bx+si],al
00000BDD  0000              add [bx+si],al
00000BDF  0000              add [bx+si],al
00000BE1  0000              add [bx+si],al
00000BE3  0000              add [bx+si],al
00000BE5  0000              add [bx+si],al
00000BE7  0000              add [bx+si],al
00000BE9  0000              add [bx+si],al
00000BEB  0000              add [bx+si],al
00000BED  0000              add [bx+si],al
00000BEF  0000              add [bx+si],al
00000BF1  0000              add [bx+si],al
00000BF3  0000              add [bx+si],al
00000BF5  0000              add [bx+si],al
00000BF7  0000              add [bx+si],al
00000BF9  0000              add [bx+si],al
00000BFB  0000              add [bx+si],al
00000BFD  0000              add [bx+si],al
00000BFF  0000              add [bx+si],al
00000C01  0000              add [bx+si],al
00000C03  0000              add [bx+si],al
00000C05  0000              add [bx+si],al
00000C07  0000              add [bx+si],al
00000C09  0000              add [bx+si],al
00000C0B  0000              add [bx+si],al
00000C0D  0000              add [bx+si],al
00000C0F  0000              add [bx+si],al
00000C11  0000              add [bx+si],al
00000C13  0000              add [bx+si],al
00000C15  0000              add [bx+si],al
00000C17  0000              add [bx+si],al
00000C19  0000              add [bx+si],al
00000C1B  0000              add [bx+si],al
00000C1D  0000              add [bx+si],al
00000C1F  0000              add [bx+si],al
00000C21  0000              add [bx+si],al
00000C23  0000              add [bx+si],al
00000C25  0000              add [bx+si],al
00000C27  0000              add [bx+si],al
00000C29  0000              add [bx+si],al
00000C2B  0000              add [bx+si],al
00000C2D  0000              add [bx+si],al
00000C2F  0000              add [bx+si],al
00000C31  0000              add [bx+si],al
00000C33  0000              add [bx+si],al
00000C35  0000              add [bx+si],al
00000C37  0000              add [bx+si],al
00000C39  0000              add [bx+si],al
00000C3B  0000              add [bx+si],al
00000C3D  0000              add [bx+si],al
00000C3F  0000              add [bx+si],al
00000C41  0000              add [bx+si],al
00000C43  0000              add [bx+si],al
00000C45  0000              add [bx+si],al
00000C47  0000              add [bx+si],al
00000C49  0000              add [bx+si],al
00000C4B  0000              add [bx+si],al
00000C4D  0000              add [bx+si],al
00000C4F  0000              add [bx+si],al
00000C51  0000              add [bx+si],al
00000C53  0000              add [bx+si],al
00000C55  0000              add [bx+si],al
00000C57  0000              add [bx+si],al
00000C59  0000              add [bx+si],al
00000C5B  0000              add [bx+si],al
00000C5D  0000              add [bx+si],al
00000C5F  0000              add [bx+si],al
00000C61  0000              add [bx+si],al
00000C63  0000              add [bx+si],al
00000C65  0000              add [bx+si],al
00000C67  0000              add [bx+si],al
00000C69  0000              add [bx+si],al
00000C6B  0000              add [bx+si],al
00000C6D  0000              add [bx+si],al
00000C6F  0000              add [bx+si],al
00000C71  0000              add [bx+si],al
00000C73  0000              add [bx+si],al
00000C75  0000              add [bx+si],al
00000C77  0000              add [bx+si],al
00000C79  0000              add [bx+si],al
00000C7B  0000              add [bx+si],al
00000C7D  0000              add [bx+si],al
00000C7F  0000              add [bx+si],al
00000C81  0000              add [bx+si],al
00000C83  0000              add [bx+si],al
00000C85  0000              add [bx+si],al
00000C87  0000              add [bx+si],al
00000C89  0000              add [bx+si],al
00000C8B  0000              add [bx+si],al
00000C8D  0000              add [bx+si],al
00000C8F  0000              add [bx+si],al
00000C91  0000              add [bx+si],al
00000C93  0000              add [bx+si],al
00000C95  0000              add [bx+si],al
00000C97  0000              add [bx+si],al
00000C99  0000              add [bx+si],al
00000C9B  0000              add [bx+si],al
00000C9D  0000              add [bx+si],al
00000C9F  0000              add [bx+si],al
00000CA1  0000              add [bx+si],al
00000CA3  0000              add [bx+si],al
00000CA5  0000              add [bx+si],al
00000CA7  0000              add [bx+si],al
00000CA9  0000              add [bx+si],al
00000CAB  0000              add [bx+si],al
00000CAD  0000              add [bx+si],al
00000CAF  0000              add [bx+si],al
00000CB1  0000              add [bx+si],al
00000CB3  0000              add [bx+si],al
00000CB5  0000              add [bx+si],al
00000CB7  0000              add [bx+si],al
00000CB9  0000              add [bx+si],al
00000CBB  0000              add [bx+si],al
00000CBD  0000              add [bx+si],al
00000CBF  0000              add [bx+si],al
00000CC1  0000              add [bx+si],al
00000CC3  0000              add [bx+si],al
00000CC5  0000              add [bx+si],al
00000CC7  0000              add [bx+si],al
00000CC9  0000              add [bx+si],al
00000CCB  0000              add [bx+si],al
00000CCD  0000              add [bx+si],al
00000CCF  0000              add [bx+si],al
00000CD1  0000              add [bx+si],al
00000CD3  0000              add [bx+si],al
00000CD5  0000              add [bx+si],al
00000CD7  0000              add [bx+si],al
00000CD9  0000              add [bx+si],al
00000CDB  0000              add [bx+si],al
00000CDD  0000              add [bx+si],al
00000CDF  0000              add [bx+si],al
00000CE1  0000              add [bx+si],al
00000CE3  0000              add [bx+si],al
00000CE5  0000              add [bx+si],al
00000CE7  0000              add [bx+si],al
00000CE9  0000              add [bx+si],al
00000CEB  0000              add [bx+si],al
00000CED  0000              add [bx+si],al
00000CEF  0000              add [bx+si],al
00000CF1  0000              add [bx+si],al
00000CF3  0000              add [bx+si],al
00000CF5  0000              add [bx+si],al
00000CF7  0000              add [bx+si],al
00000CF9  0000              add [bx+si],al
00000CFB  0000              add [bx+si],al
00000CFD  0000              add [bx+si],al
00000CFF  0000              add [bx+si],al
00000D01  0000              add [bx+si],al
00000D03  0000              add [bx+si],al
00000D05  0000              add [bx+si],al
00000D07  0000              add [bx+si],al
00000D09  0000              add [bx+si],al
00000D0B  0000              add [bx+si],al
00000D0D  0000              add [bx+si],al
00000D0F  0000              add [bx+si],al
00000D11  0000              add [bx+si],al
00000D13  0000              add [bx+si],al
00000D15  0000              add [bx+si],al
00000D17  0000              add [bx+si],al
00000D19  0000              add [bx+si],al
00000D1B  0000              add [bx+si],al
00000D1D  0000              add [bx+si],al
00000D1F  0000              add [bx+si],al
00000D21  0000              add [bx+si],al
00000D23  0000              add [bx+si],al
00000D25  0000              add [bx+si],al
00000D27  0000              add [bx+si],al
00000D29  0000              add [bx+si],al
00000D2B  0000              add [bx+si],al
00000D2D  0000              add [bx+si],al
00000D2F  0000              add [bx+si],al
00000D31  0000              add [bx+si],al
00000D33  0000              add [bx+si],al
00000D35  0000              add [bx+si],al
00000D37  0000              add [bx+si],al
00000D39  0000              add [bx+si],al
00000D3B  0000              add [bx+si],al
00000D3D  0000              add [bx+si],al
00000D3F  0000              add [bx+si],al
00000D41  0000              add [bx+si],al
00000D43  0000              add [bx+si],al
00000D45  0000              add [bx+si],al
00000D47  0000              add [bx+si],al
00000D49  0000              add [bx+si],al
00000D4B  0000              add [bx+si],al
00000D4D  0000              add [bx+si],al
00000D4F  0000              add [bx+si],al
00000D51  0000              add [bx+si],al
00000D53  0000              add [bx+si],al
00000D55  0000              add [bx+si],al
00000D57  0000              add [bx+si],al
00000D59  0000              add [bx+si],al
00000D5B  0000              add [bx+si],al
00000D5D  0000              add [bx+si],al
00000D5F  0000              add [bx+si],al
00000D61  0000              add [bx+si],al
00000D63  0000              add [bx+si],al
00000D65  0000              add [bx+si],al
00000D67  0000              add [bx+si],al
00000D69  0000              add [bx+si],al
00000D6B  0000              add [bx+si],al
00000D6D  0000              add [bx+si],al
00000D6F  0000              add [bx+si],al
00000D71  0000              add [bx+si],al
00000D73  0000              add [bx+si],al
00000D75  0000              add [bx+si],al
00000D77  0000              add [bx+si],al
00000D79  0000              add [bx+si],al
00000D7B  0000              add [bx+si],al
00000D7D  0000              add [bx+si],al
00000D7F  0000              add [bx+si],al
00000D81  0000              add [bx+si],al
00000D83  0000              add [bx+si],al
00000D85  0000              add [bx+si],al
00000D87  0000              add [bx+si],al
00000D89  0000              add [bx+si],al
00000D8B  0000              add [bx+si],al
00000D8D  0000              add [bx+si],al
00000D8F  0000              add [bx+si],al
00000D91  0000              add [bx+si],al
00000D93  0000              add [bx+si],al
00000D95  0000              add [bx+si],al
00000D97  0000              add [bx+si],al
00000D99  0000              add [bx+si],al
00000D9B  0000              add [bx+si],al
00000D9D  0000              add [bx+si],al
00000D9F  0000              add [bx+si],al
00000DA1  0000              add [bx+si],al
00000DA3  0000              add [bx+si],al
00000DA5  0000              add [bx+si],al
00000DA7  0000              add [bx+si],al
00000DA9  0000              add [bx+si],al
00000DAB  0000              add [bx+si],al
00000DAD  0000              add [bx+si],al
00000DAF  0000              add [bx+si],al
00000DB1  0000              add [bx+si],al
00000DB3  0000              add [bx+si],al
00000DB5  0000              add [bx+si],al
00000DB7  0000              add [bx+si],al
00000DB9  0000              add [bx+si],al
00000DBB  0000              add [bx+si],al
00000DBD  0000              add [bx+si],al
00000DBF  0000              add [bx+si],al
00000DC1  0000              add [bx+si],al
00000DC3  0000              add [bx+si],al
00000DC5  0000              add [bx+si],al
00000DC7  0000              add [bx+si],al
00000DC9  0000              add [bx+si],al
00000DCB  0000              add [bx+si],al
00000DCD  0000              add [bx+si],al
00000DCF  0000              add [bx+si],al
00000DD1  0000              add [bx+si],al
00000DD3  0000              add [bx+si],al
00000DD5  0000              add [bx+si],al
00000DD7  0000              add [bx+si],al
00000DD9  0000              add [bx+si],al
00000DDB  0000              add [bx+si],al
00000DDD  0000              add [bx+si],al
00000DDF  0000              add [bx+si],al
00000DE1  0000              add [bx+si],al
00000DE3  0000              add [bx+si],al
00000DE5  0000              add [bx+si],al
00000DE7  0000              add [bx+si],al
00000DE9  0000              add [bx+si],al
00000DEB  0000              add [bx+si],al
00000DED  0000              add [bx+si],al
00000DEF  0000              add [bx+si],al
00000DF1  0000              add [bx+si],al
00000DF3  0000              add [bx+si],al
00000DF5  0000              add [bx+si],al
00000DF7  0000              add [bx+si],al
00000DF9  0000              add [bx+si],al
00000DFB  0000              add [bx+si],al
00000DFD  0000              add [bx+si],al
00000DFF  0000              add [bx+si],al
00000E01  0000              add [bx+si],al
00000E03  0000              add [bx+si],al
00000E05  0000              add [bx+si],al
00000E07  0000              add [bx+si],al
00000E09  0000              add [bx+si],al
00000E0B  0000              add [bx+si],al
00000E0D  0000              add [bx+si],al
00000E0F  0000              add [bx+si],al
00000E11  0000              add [bx+si],al
00000E13  0000              add [bx+si],al
00000E15  0000              add [bx+si],al
00000E17  0000              add [bx+si],al
00000E19  0000              add [bx+si],al
00000E1B  0000              add [bx+si],al
00000E1D  0000              add [bx+si],al
00000E1F  0000              add [bx+si],al
00000E21  0000              add [bx+si],al
00000E23  0000              add [bx+si],al
00000E25  0000              add [bx+si],al
00000E27  0000              add [bx+si],al
00000E29  0000              add [bx+si],al
00000E2B  0000              add [bx+si],al
00000E2D  0000              add [bx+si],al
00000E2F  0000              add [bx+si],al
00000E31  0000              add [bx+si],al
00000E33  0000              add [bx+si],al
00000E35  0000              add [bx+si],al
00000E37  0000              add [bx+si],al
00000E39  0000              add [bx+si],al
00000E3B  0000              add [bx+si],al
00000E3D  0000              add [bx+si],al
00000E3F  0000              add [bx+si],al
00000E41  0000              add [bx+si],al
00000E43  0000              add [bx+si],al
00000E45  0000              add [bx+si],al
00000E47  0000              add [bx+si],al
00000E49  0000              add [bx+si],al
00000E4B  0000              add [bx+si],al
00000E4D  0000              add [bx+si],al
00000E4F  0000              add [bx+si],al
00000E51  0000              add [bx+si],al
00000E53  0000              add [bx+si],al
00000E55  0000              add [bx+si],al
00000E57  0000              add [bx+si],al
00000E59  0000              add [bx+si],al
00000E5B  0000              add [bx+si],al
00000E5D  0000              add [bx+si],al
00000E5F  0000              add [bx+si],al
00000E61  0000              add [bx+si],al
00000E63  0000              add [bx+si],al
00000E65  0000              add [bx+si],al
00000E67  0000              add [bx+si],al
00000E69  0000              add [bx+si],al
00000E6B  0000              add [bx+si],al
00000E6D  0000              add [bx+si],al
00000E6F  0000              add [bx+si],al
00000E71  0000              add [bx+si],al
00000E73  0000              add [bx+si],al
00000E75  0000              add [bx+si],al
00000E77  0000              add [bx+si],al
00000E79  0000              add [bx+si],al
00000E7B  0000              add [bx+si],al
00000E7D  0000              add [bx+si],al
00000E7F  0000              add [bx+si],al
00000E81  0000              add [bx+si],al
00000E83  0000              add [bx+si],al
00000E85  0000              add [bx+si],al
00000E87  0000              add [bx+si],al
00000E89  0000              add [bx+si],al
00000E8B  0000              add [bx+si],al
00000E8D  0000              add [bx+si],al
00000E8F  0000              add [bx+si],al
00000E91  0000              add [bx+si],al
00000E93  0000              add [bx+si],al
00000E95  0000              add [bx+si],al
00000E97  0000              add [bx+si],al
00000E99  0000              add [bx+si],al
00000E9B  0000              add [bx+si],al
00000E9D  0000              add [bx+si],al
00000E9F  0000              add [bx+si],al
00000EA1  0000              add [bx+si],al
00000EA3  0000              add [bx+si],al
00000EA5  0000              add [bx+si],al
00000EA7  0000              add [bx+si],al
00000EA9  0000              add [bx+si],al
00000EAB  0000              add [bx+si],al
00000EAD  0000              add [bx+si],al
00000EAF  0000              add [bx+si],al
00000EB1  0000              add [bx+si],al
00000EB3  0000              add [bx+si],al
00000EB5  0000              add [bx+si],al
00000EB7  0000              add [bx+si],al
00000EB9  0000              add [bx+si],al
00000EBB  0000              add [bx+si],al
00000EBD  0000              add [bx+si],al
00000EBF  0000              add [bx+si],al
00000EC1  0000              add [bx+si],al
00000EC3  0000              add [bx+si],al
00000EC5  0000              add [bx+si],al
00000EC7  0000              add [bx+si],al
00000EC9  0000              add [bx+si],al
00000ECB  0000              add [bx+si],al
00000ECD  0000              add [bx+si],al
00000ECF  0000              add [bx+si],al
00000ED1  0000              add [bx+si],al
00000ED3  0000              add [bx+si],al
00000ED5  0000              add [bx+si],al
00000ED7  0000              add [bx+si],al
00000ED9  0000              add [bx+si],al
00000EDB  0000              add [bx+si],al
00000EDD  0000              add [bx+si],al
00000EDF  0000              add [bx+si],al
00000EE1  0000              add [bx+si],al
00000EE3  0000              add [bx+si],al
00000EE5  0000              add [bx+si],al
00000EE7  0000              add [bx+si],al
00000EE9  0000              add [bx+si],al
00000EEB  0000              add [bx+si],al
00000EED  0000              add [bx+si],al
00000EEF  0000              add [bx+si],al
00000EF1  0000              add [bx+si],al
00000EF3  0000              add [bx+si],al
00000EF5  0000              add [bx+si],al
00000EF7  0000              add [bx+si],al
00000EF9  0000              add [bx+si],al
00000EFB  0000              add [bx+si],al
00000EFD  0000              add [bx+si],al
00000EFF  FE                db 0xfe
