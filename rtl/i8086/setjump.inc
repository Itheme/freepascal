{
    This file is part of the Free Pascal run time library.
    Copyright (c) 2013 by the Free Pascal development team

    SetJmp and LongJmp implementation for exception handling

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

Function fpc_SetJmp (Var S : Jmp_buf) : smallint;assembler;nostackframe;[Public, alias : 'FPC_SETJMP']; compilerproc;
asm
  mov ax, bp
  mov di, sp
  push bp
  mov bp, sp

  mov bx, ss:[bp + 4 + extra_param_offset]  // S
  mov word [bx + Jmp_buf.bp], ax
  mov word [bx + Jmp_buf.sp], di
  mov cx, word ss:[di]
  mov word [bx + Jmp_buf.ip], cx
{$ifdef FPC_X86_CODE_FAR}
  mov cx, word ss:[di + 2]
  mov word [bx + Jmp_buf.cs], cx
{$endif FPC_X86_CODE_FAR}

  xor ax, ax
  pop bp
end;


Procedure fpc_longJmp (Var S : Jmp_buf; value : smallint); assembler;nostackframe;[Public, alias : 'FPC_LONGJMP']; compilerproc;
asm
  push bp
  mov bp, sp

  mov bx, ss:[bp + 6 + extra_param_offset]  // S
  mov ax, ss:[bp + 4 + extra_param_offset]  // value
  test ax, ax
  jnz @@L1
  inc ax
@@L1:
  mov bp, word [bx + Jmp_buf.bp]
  mov sp, word [bx + Jmp_buf.sp]
  // we should also clear the fpu
  // fninit no must be done elsewhere PM
  // or we should reset the control word also
{$ifdef FPC_X86_CODE_NEAR}
  jmp word [bx + Jmp_buf.ip]
{$else FPC_X86_CODE_NEAR}
  // the inline asm doesn't support jmp far yet, so we use db for now
  // jmp far [bx + Jmp_buf.ip]
  db 0FFh, 06Fh, Jmp_buf.ip
{$endif FPC_X86_CODE_NEAR}
end;
