{
    This file is part of the Free Pascal run time library.
    Copyright (c) 1999-2000 by the Free Pascal development team

    Implement OS-dependent part of dynamic library loading.

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

{$ifdef readinterface}

{ ---------------------------------------------------------------------
    Interface declarations
  ---------------------------------------------------------------------}

Type
  { using PtrInt here is compliant with the other platforms }
  TLibHandle = PtrInt;

Const
  NilHandle = TLibHandle(0);
// these are for easier crossplatform construction of dll names in dynloading libs.
{$if defined(Darwin)}
  SharedSuffix = 'dylib';
{$elseif defined(aix)}
  SharedSuffix = 'a';
{$else}
  SharedSuffix = 'so';
{$endif}

{$else}

{ ---------------------------------------------------------------------
    Implementation section
  ---------------------------------------------------------------------}

uses dl;

Function DoLoadLibrary(const Name : RawByteString) : TLibHandle;
{$ifdef aix}
var
  MemberName: RawByteString;
{$endif}
begin
{$ifndef aix}
  Result:=TLibHandle(dlopen(PAnsiChar(Name),RTLD_LAZY));
{$else aix}
  { in aix, most shared libraries are static libraries (archives) that contain
    a single object: shr.o for 32 bit, shr_64.o for 64 bit. You have to specify
    this object file explicitly via the RTLD_MEMBER member flag }
{$ifdef cpu64}
  MemberName:='(shr_64.o)';
{$else cpu64}
  MemberName:='(shr.o)';
{$endif cpu64}
  SetCodePage(MemberName,DefaultFileSystemCodePage,false);
  MemberName:=Name+MemberName;
  Result:=TLibHandle(dlopen(PAnsiChar(MemberName),RTLD_LAZY or RTLD_MEMBER));
{$endif aix}
end;

Function GetProcedureAddress(Lib : TLibHandle; const ProcName : AnsiString) : Pointer;

begin
  Result:=dlsym(lib,pchar(ProcName));
end;

Function UnloadLibrary(Lib : TLibHandle) : Boolean;

begin
  Result:=dlClose(Lib)=0;
end;

Function GetLoadErrorStr: string;

begin
  Result:=dl.dlerror;
end;

{$endif}

