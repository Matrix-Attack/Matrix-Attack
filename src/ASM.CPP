///////////////////////
//Code Taken from
//Real-Time Strategy Game Programming
//Mickey Kawick
//c. 1999
///////////////////////


typedef unsigned short			U16,		*P_U16;

//**************************************************
void __fastcall Memset16Bit ( P_U16 dst, U16 fill, int num )
{
    if (num <= 0) return;
    if (num&1) *dst++ = fill; // one too many for stosd
    if (num <2) return;

 _asm
 {
    mov    ax, fill   // store value
    mov   ecx, num    // how many copies
    shl   eax, 16     // shift it into high order
    mov   edi, dst    // to where
    add    ax, fill   // do two pixels at a time
    shr   ecx, 1      // divide by 2

    rep   stosd
 }
}
