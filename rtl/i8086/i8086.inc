{
    This file is part of the Free Pascal run time library.
    Copyright (c) 2013 by the Free Pascal development team.

    Processor dependent implementation for the system unit for
    intel i8086+

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

procedure fpc_cpuinit;
  begin
  end;

{$define FPC_SYSTEM_HAS_SPTR}
Function Sptr : Pointer;assembler;nostackframe;
asm
  mov ax, sp
end;

{$define FPC_SYSTEM_HAS_PTR}
function Ptr(sel,off: LongInt):farpointer;{$ifdef SYSTEMINLINE}inline;{$endif}assembler;nostackframe;
asm
  push bp
  mov bp, sp
  mov ax, ss:[bp + 4 + extra_param_offset]  // off
  mov dx, ss:[bp + 8 + extra_param_offset]  // sel
  pop bp
end;

{$define FPC_SYSTEM_HAS_CSEG}
function CSeg: Word;{$ifdef SYSTEMINLINE}inline;{$endif}assembler;nostackframe;
asm
  mov ax, cs
end;

{$define FPC_SYSTEM_HAS_DSEG}
function DSeg: Word;{$ifdef SYSTEMINLINE}inline;{$endif}assembler;nostackframe;
asm
  mov ax, ds
end;

{$define FPC_SYSTEM_HAS_SSEG}
function SSeg: Word;{$ifdef SYSTEMINLINE}inline;{$endif}assembler;nostackframe;
asm
  mov ax, ss
end;

{$define FPC_SYSTEM_HAS_GET_CALLER_ADDR}
function get_caller_addr(framebp:pointer;addr:pointer=nil):pointer;nostackframe;assembler;
asm
  push bp
  mov bp, sp
  mov ax, ss:[bp + 6 + extra_param_offset]  // framebp
  or ax, ax
  jz @@Lg_a_null
  xchg ax, bx
  mov bx, [bx+2]
  xchg ax, bx
@@Lg_a_null:
  pop bp
end;

{$define FPC_SYSTEM_HAS_GET_CALLER_FRAME}
function get_caller_frame(framebp:pointer;addr:pointer=nil):pointer;nostackframe;assembler;
asm
  push bp
  mov bp, sp
  mov ax, ss:[bp + 6 + extra_param_offset]  // framebp
  or ax, ax
  jz @@Lgnf_null
  xchg ax, bx
  mov bx, [bx]
  xchg ax, bx
@@Lgnf_null:
  pop bp
end;

{TODO: fix, use smallint?}
function InterLockedDecrement (var Target: longint) : longint; assembler;
asm
  push bp
  mov bp, sp
  mov bx, ss:[bp + 4 + extra_param_offset]  // Target
  sub word [bx], 1
  sbb word [bx+2], 0
  mov ax, [bx]
  mov dx, [bx+2]
  pop bp
end;

{TODO: fix, use smallint?}
function InterLockedIncrement (var Target: longint) : longint; assembler;
asm
  push bp
  mov bp, sp
  mov bx, ss:[bp + 4 + extra_param_offset]  // Target
  add word [bx], 1
  adc word [bx+2], 0
  mov ax, [bx]
  mov dx, [bx+2]
  pop bp
end;

{TODO: fix, use smallint?}
function InterLockedExchange (var Target: longint;Source : longint) : longint;
begin
  InterLockedExchange := Target;
  Target := Source;
end;

{TODO: implement}
function InterLockedExchangeAdd (var Target: longint;Source : longint) : longint;
begin
  runerror(304);
end;

{TODO: implement}
function InterlockedCompareExchange(var Target: longint; NewValue: longint; Comperand: longint): longint;
begin
  runerror(304);
end;

{****************************************************************************
                                  FPU
****************************************************************************}

const
  { Internal constants for use in system unit }
  FPU_Invalid = 1;
  FPU_Denormal = 2;
  FPU_DivisionByZero = 4;
  FPU_Overflow = 8;
  FPU_Underflow = $10;
  FPU_StackUnderflow = $20;
  FPU_StackOverflow = $40;
  FPU_ExceptionMask = $ff;

  { use Default8087CW instead
  fpucw : word = $1300 or FPU_StackUnderflow or FPU_Underflow or FPU_Denormal;
  }

{ returns true if FPU is present }
function DetectFPU: boolean;
  var
    localfpucw: word;
  begin
    asm
      fninit
      mov byte [localfpucw + 1], 0
      fnstcw localfpucw
    end;
    DetectFPU:=(localfpucw and $FF00)=$0300;
  end;

{$define FPC_SYSTEM_HAS_SYSINITFPU}
Procedure SysInitFPU;
  var
    { these locals are so we don't have to hack pic code in the assembler }
    localmxcsr: dword;
    localfpucw: word;
  begin
    localfpucw:=Default8087CW;
    asm
      fninit
      fldcw   localfpucw
      fwait
    end;
    softfloat_exception_mask:=float_flag_underflow or float_flag_inexact or float_flag_denormal;
  end;


{$define FPC_SYSTEM_HAS_SYSRESETFPU}
Procedure SysResetFPU;
  var
    { these locals are so we don't have to hack pic code in the assembler }
    localmxcsr: dword;
    localfpucw: word;
  begin
    localfpucw:=Default8087CW;
    asm
      fninit
      fwait
      fldcw   localfpucw
    end;
    softfloat_exception_flags:=0;
  end;

