

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Wed Aug 08 08:07:00 2018
 */
/* Compiler settings for Animator.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IOleImage,0x300E8ADA,0xAC70,0x4111,0xBD,0x19,0xBB,0x05,0x9C,0xCC,0x1C,0x56);


MIDL_DEFINE_GUID(IID, LIBID_AnimatorLib,0xBC1DBEB2,0x3514,0x4E05,0xA0,0x18,0xE5,0x2A,0x76,0x3F,0xDC,0xDF);


MIDL_DEFINE_GUID(IID, DIID__IOleImageEvents,0x62AAA53A,0x6F3F,0x46D3,0xBA,0xE4,0x12,0xBE,0xE7,0x08,0xA4,0xA0);


MIDL_DEFINE_GUID(CLSID, CLSID_OleImage,0x2F8CF783,0x16DC,0x4B24,0xA0,0x46,0x2B,0x0D,0x61,0xA6,0xAF,0xA9);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



