[BITS 16]
ORG    0x7C00
JMP    BOOT
; BIOS parameter blocks(FAT12)
BS_jmpBoot2    DB 0x90
BS_OEMName     DB "MyOS    "
BPB_BytsPerSec DW 0x0200         ;BytesPerSector
BPB_SecPerClus DB 0x01         ;SectorPerCluster
BPB_RsvdSecCnt DW 0x0001         ;ReservedSectors
BPB_NumFATs    DB 0x02         ;TotalFATs
BPB_RootEntCnt DW 0x00E0         ;MaxRootEntries
BPB_TotSec16   DW 0x0B40         ;TotalSectors
BPB_Media      DB 0xF0         ;MediaDescriptor
BPB_FATSz16    DW 0x0009         ;SectorsPerFAT
BPB_SecPerTrk  DW 0x0012         ;SectorsPerTrack
BPB_NumHeads   DW 0x0002         ;NumHeads
BPB_HiddSec    DD 0x00000000     ;HiddenSector
BPB_TotSec32   DD 0x00000000     ;TotalSectors
BS_DrvNum      DB 0x00         ;DriveNumber
BS_Reserved1   DB 0x00         ;Reserved
BS_BootSig     DB 0x29         ;BootSignature
BS_VolID       DD 0x20200203     ;VolumeSerialNumber 日付を入れました
BS_VolLab      DB "MyOS       " ;VolumeLabel
BS_FilSysType  DB "FAT12   "     ;FileSystemType

BOOT:
    cli
    mov si, info
    call print
    mov dx,8
    mov si, BS_FilSysType
    call print_s
    mov dx,8
    mov si,BS_VolLab
    call puts_s
    mov si,load
    call print
    mov si,bolo
    call puts

    ;Init Disk
    mov ah, 0x00
    mov dl, 0x00
    int 0x13

    jc if_main_fail_flpinit
    jnc if_main_success_flpinit
    if_main_fail_flpinit:
        hlt
    if_main_success_flpinit:

    ;Loading FAT
    mov bx,word[BX_FAT_ADDR]   ;bx =*BX_Fat_Addr
    add ax,word[BPB_RsvdSecCnt];ax+=*BPB_RsvdSecCnt
    xchg ax,cx                 ;ax<->cx
    mov ax,word[BPB_FATSz16]   ;ax = BPB_FATSz16
    mul word[BPB_NumFATs]      ;ax*= BPB_NumFATs
    xchg ax,cx                 ;ax<->cx
    main_loop_readfat:
        call ReadSector
        add bx, word [BPB_BytsPerSec]
        inc AX
        dec CX
        jcxz main_stat_fatloaded ;if(cx==0) goto FAT_LOADED
        jmp main_loop_readfat
    main_stat_fatloaded:
        hlt

;--------------------
;Functions
;--------------------
    ;--------------------
    ;LBA To CHS
    ;--------------------
    LBA2CHS:
    	xor dx,dx
    	div	word [BPB_SecPerTrk]
    	inc	dl				          ;dl++
    	mov	byte [physicalSector], dl ;*physicalSector = dl
    	xor	dx, dx                    ;dx = 0
    	div	word [BPB_NumHeads]       ;ax / BPB_NumHeads = al/ah
    	mov	byte [physicalHead],dl    ;*physicalHead  = dl
    	mov	byte [physicalTrack],al   ; physicalTrack = al
    	ret
    ;--------------------
    ;Put String to stdout
    ;arg: si=Message Address
    ;--------------------
    puts:
        push ax
        push bx
        call print
        mov si,EOL
        call print
        pop bx
        pop ax
        ret
    ;--------------------
    ;Put String to stdout
    ;arg: si=Message Address
    ;     dx=Message Length
    ;--------------------
    puts_s:
        push ax
        push bx
        call print_s
        mov si,EOL
        call print
        pop  bx
        pop ax
        ret
    ;--------------------
    ;Print String to stdout
    ;arg: si=Message Address
    ;--------------------
    print:
        push ax
        push bx
        print_l1s:
            mov al,[si]  ; al =*si
            inc si       ; si+=1
            cmp al,0     ;if(al)
            jz print_l1e ;if zero -> print_l1e
            mov ah,0x0E
            mov bh,0x00
            mov bl,0x07
            int 0x10
            jmp print_l1s
        print_l1e:
            pop bx
            pop ax
            ret
    ;--------------------
    ;Put String to stdout
    ;arg: si=Message Address
    ;     dx=Message Length
    ;--------------------
    print_s:
        push ax
        push bx
        print_s_loop_put:
            mov al,[si]
            inc si
            dec dx
            cmp dx,0
            jz print_s_stat_end
            mov ah,0x0E
            mov bh,0x00
            mov bl,0x07
            int 0x10
            jmp print_s_loop_put
        print_s_stat_end:
            pop bx
            pop ax
            ret
    ;--------------------
    ;Read a Sector
    ;--------------------
    ReadSector:
	    mov di, 0x0005
        ReadSector_loop_SECTOR:
            push ax ;Backup
            push bx
            push cx
            call LBA2CHS                ; AXのLBAを物理番号に変換
            mov ah, 0x02
            mov al, 0x01
            mov ch, byte [physicalTrack]
            mov cl, byte [physicalSector]
            mov dh, byte [physicalHead]
            mov dl, byte [BS_DrvNum]        ; ドライブ番号（Aドライブ）
            int 0x13                ;Read a Sector(1)
            jnc ReadSector_stat_end ;if success
            ;Error
            xor AX, AX                ;ax=0
            int 0x13                ; エラーが発生した時はヘッドを元に戻す
            dec DI                  ;di--
            pop CX                ;Restore
            pop BX
            pop AX
            jnz ReadSector_loop_SECTOR; DEC　DIの計算結果が0でなければ、セクタ読み込みをリトライ ;
            int 0x18                ; 昔のソースなので何がしたかったのか？？？Set Media Type
        ReadSector_stat_end:
            pop cx  ;Restore Register
            pop bx
            pop ax
            ret

;--------------------
;Constants
;--------------------
load: db "Loading ",0
bolo: db "Bootloader",0
fail: db "fail o",0
info: db "Info ",0
EOL:  db 0x0a,0x0d

;--------------------
;Var
;--------------------
BX_FAT_ADDR     DW 0x7E00
physicalSector  DB 0x00
physicalHead    DB 0x00
physicalTrack   DB 0x00

;end
TIMES 510 - ($ - $$) db 0
DW 0xAA55