//**********************************************************************`
//* This is an include file generated by Message Compiler.             *`
//*                                                                    *`
//* Copyright (c) Microsoft Corporation. All Rights Reserved.          *`
//**********************************************************************`
#pragma once
#include <wmistr.h>
#include <evntrace.h>
#include "evntprov.h"
//
//  Initial Defs
//
#if !defined(ETW_INLINE)
#define ETW_INLINE DECLSPEC_NOINLINE __inline
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//
// Allow Diasabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION
#if  !defined(McGenDebug)
#define McGenDebug(a,b)
#endif 


#if !defined(MCGEN_TRACE_CONTEXT_DEF)
#define MCGEN_TRACE_CONTEXT_DEF
typedef struct _MCGEN_TRACE_CONTEXT
{
    TRACEHANDLE            RegistrationHandle;
    TRACEHANDLE            Logger;
    ULONGLONG              MatchAnyKeyword;
    ULONGLONG              MatchAllKeyword;
    ULONG                  Flags;
    ULONG                  IsEnabled;
    UCHAR                  Level; 
    UCHAR                  Reserve;
    USHORT                 EnableBitsCount;
    PULONG                 EnableBitMask;
    const ULONGLONG*       EnableKeyWords;
    const UCHAR*           EnableLevel;
} MCGEN_TRACE_CONTEXT, *PMCGEN_TRACE_CONTEXT;
#endif

#if !defined(MCGEN_LEVEL_KEYWORD_ENABLED_DEF)
#define MCGEN_LEVEL_KEYWORD_ENABLED_DEF
FORCEINLINE
BOOLEAN
McGenLevelKeywordEnabled(
    _In_ PMCGEN_TRACE_CONTEXT EnableInfo,
    _In_ UCHAR Level,
    _In_ ULONGLONG Keyword
    )
{
    //
    // Check if the event Level is lower than the level at which
    // the channel is enabled.
    // If the event Level is 0 or the channel is enabled at level 0,
    // all levels are enabled.
    //

    if ((Level <= EnableInfo->Level) || // This also covers the case of Level == 0.
        (EnableInfo->Level == 0)) {

        //
        // Check if Keyword is enabled
        //

        if ((Keyword == (ULONGLONG)0) ||
            ((Keyword & EnableInfo->MatchAnyKeyword) &&
             ((Keyword & EnableInfo->MatchAllKeyword) == EnableInfo->MatchAllKeyword))) {
            return TRUE;
        }
    }

    return FALSE;

}
#endif

#if !defined(MCGEN_EVENT_ENABLED_DEF)
#define MCGEN_EVENT_ENABLED_DEF
FORCEINLINE
BOOLEAN
McGenEventEnabled(
    _In_ PMCGEN_TRACE_CONTEXT EnableInfo,
    _In_ PCEVENT_DESCRIPTOR EventDescriptor
    )
{

    return McGenLevelKeywordEnabled(EnableInfo, EventDescriptor->Level, EventDescriptor->Keyword);

}
#endif


//
// EnableCheckMacro
//
#ifndef MCGEN_ENABLE_CHECK
#define MCGEN_ENABLE_CHECK(Context, Descriptor) (Context.IsEnabled &&  McGenEventEnabled(&Context, &Descriptor))
#endif

#if !defined(MCGEN_CONTROL_CALLBACK)
#define MCGEN_CONTROL_CALLBACK

DECLSPEC_NOINLINE __inline
VOID
__stdcall
McGenControlCallbackV2(
    _In_ LPCGUID SourceId,
    _In_ ULONG ControlCode,
    _In_ UCHAR Level,
    _In_ ULONGLONG MatchAnyKeyword,
    _In_ ULONGLONG MatchAllKeyword,
    _In_opt_ PEVENT_FILTER_DESCRIPTOR FilterData,
    _Inout_opt_ PVOID CallbackContext
    )
/*++

Routine Description:

    This is the notification callback for Vista.

Arguments:

    SourceId - The GUID that identifies the session that enabled the provider. 

    ControlCode - The parameter indicates whether the provider 
                  is being enabled or disabled.

    Level - The level at which the event is enabled.

    MatchAnyKeyword - The bitmask of keywords that the provider uses to 
                      determine the category of events that it writes.

    MatchAllKeyword - This bitmask additionally restricts the category 
                      of events that the provider writes. 

    FilterData - The provider-defined data.

    CallbackContext - The context of the callback that is defined when the provider 
                      called EtwRegister to register itself.

Remarks:

    ETW calls this function to notify provider of enable/disable

--*/
{
    PMCGEN_TRACE_CONTEXT Ctx = (PMCGEN_TRACE_CONTEXT)CallbackContext;
    ULONG Ix;
#ifndef MCGEN_PRIVATE_ENABLE_CALLBACK_V2
    UNREFERENCED_PARAMETER(SourceId);
    UNREFERENCED_PARAMETER(FilterData);
#endif

    if (Ctx == NULL) {
        return;
    }

    switch (ControlCode) {

        case EVENT_CONTROL_CODE_ENABLE_PROVIDER:
            Ctx->Level = Level;
            Ctx->MatchAnyKeyword = MatchAnyKeyword;
            Ctx->MatchAllKeyword = MatchAllKeyword;
            Ctx->IsEnabled = EVENT_CONTROL_CODE_ENABLE_PROVIDER;

            for (Ix = 0; Ix < Ctx->EnableBitsCount; Ix += 1) {
                if (McGenLevelKeywordEnabled(Ctx, Ctx->EnableLevel[Ix], Ctx->EnableKeyWords[Ix]) != FALSE) {
                    Ctx->EnableBitMask[Ix >> 5] |= (1 << (Ix % 32));
                } else {
                    Ctx->EnableBitMask[Ix >> 5] &= ~(1 << (Ix % 32));
                }
            }
            break;

        case EVENT_CONTROL_CODE_DISABLE_PROVIDER:
            Ctx->IsEnabled = EVENT_CONTROL_CODE_DISABLE_PROVIDER;
            Ctx->Level = 0;
            Ctx->MatchAnyKeyword = 0;
            Ctx->MatchAllKeyword = 0;
            if (Ctx->EnableBitsCount > 0) {
                RtlZeroMemory(Ctx->EnableBitMask, (((Ctx->EnableBitsCount - 1) / 32) + 1) * sizeof(ULONG));
            }
            break;
 
        default:
            break;
    }

#ifdef MCGEN_PRIVATE_ENABLE_CALLBACK_V2
    //
    // Call user defined callback
    //
    MCGEN_PRIVATE_ENABLE_CALLBACK_V2(
        SourceId,
        ControlCode,
        Level,
        MatchAnyKeyword,
        MatchAllKeyword,
        FilterData,
        CallbackContext
        );
#endif
   
    return;
}

#endif
#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION
//+
// Provider OVR-SDK-LibOVR Event Count 19
//+
EXTERN_C __declspec(selectany) const GUID LibOVRProvider = {0x553787fc, 0xd3d7, 0x4f5e, {0xac, 0xb2, 0x15, 0x97, 0xc7, 0x20, 0x9b, 0x3c}};

//
// Channel
//
#define DEBUG_CHANNEL 0x10
#define ANALYTIC_CHANNEL 0x11
#define ERROR_CHANNEL 0x12

//
// Opcodes
//
#define FN_CALL 0xa
#define FN_RETURN 0xb
#define FN_WAYPOINT 0xc
#define DIS_BEGIN 0xd
#define DIS_WAITGPU 0xe
#define DIS_PRESENT 0xf
#define DIS_END 0x10
#define HMD_DESC 0x11
#define CAM_RECEIVE 0x12
#define CAM_REQUEST 0x13

//
// Tasks
//
#define FN_TRACE 0x1
#define DIS_TRACE 0x2
#define HMD_TRACE 0x3
#define CAMERA_TRACE 0x4
#define LOG_TRACE 0x5

//
// Event Descriptors
//
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR Call = {0x0, 0x0, 0x11, 0x4, 0xa, 0x1, 0x4000000000000000};
#define Call_value 0x0
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR Return = {0x1, 0x0, 0x11, 0x4, 0xb, 0x1, 0x4000000000000000};
#define Return_value 0x1
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR Waypoint = {0x2, 0x0, 0x10, 0x4, 0xc, 0x1, 0x8000000000000000};
#define Waypoint_value 0x2
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR DistortionBegin = {0x4, 0x0, 0x11, 0x4, 0xd, 0x2, 0x4000000000000000};
#define DistortionBegin_value 0x4
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR DistortionWaitGPU = {0x5, 0x0, 0x11, 0x4, 0xe, 0x2, 0x4000000000000000};
#define DistortionWaitGPU_value 0x5
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR DistortionPresent = {0x6, 0x0, 0x11, 0x4, 0xf, 0x2, 0x4000000000000000};
#define DistortionPresent_value 0x6
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR DistortionEnd = {0x7, 0x0, 0x11, 0x4, 0x10, 0x2, 0x4000000000000000};
#define DistortionEnd_value 0x7
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR HmdDesc = {0x8, 0x0, 0x11, 0x4, 0x11, 0x3, 0x4000000000000000};
#define HmdDesc_value 0x8
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraFrameReceived = {0x9, 0x0, 0x11, 0x4, 0x12, 0x4, 0x4000000000000000};
#define CameraFrameReceived_value 0x9
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraBeginProcessing = {0xa, 0x0, 0x11, 0x4, 0xd, 0x4, 0x4000000000000000};
#define CameraBeginProcessing_value 0xa
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraFrameRequest = {0xb, 0x0, 0x11, 0x4, 0x13, 0x4, 0x4000000000000000};
#define CameraFrameRequest_value 0xb
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraEndProcessing = {0xc, 0x0, 0x11, 0x4, 0x10, 0x4, 0x4000000000000000};
#define CameraEndProcessing_value 0xc
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraSkippedFrames = {0xd, 0x0, 0x11, 0x4, 0xc, 0x4, 0x4000000000000000};
#define CameraSkippedFrames_value 0xd
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR JSONChunk = {0xe, 0x0, 0x11, 0x4, 0xc, 0x3, 0x4000000000000000};
#define JSONChunk_value 0xe
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR LogDebugMessage = {0xf, 0x0, 0x10, 0x5, 0xc, 0x5, 0x8000000000000000};
#define LogDebugMessage_value 0xf
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR LogInfoMessage = {0x10, 0x0, 0x11, 0x4, 0xc, 0x5, 0x4000000000000000};
#define LogInfoMessage_value 0x10
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR LogErrorMessage = {0x11, 0x0, 0x12, 0x2, 0xc, 0x5, 0x2000000000000000};
#define LogErrorMessage_value 0x11
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR HmdTrackingState = {0x12, 0x0, 0x11, 0x4, 0xc, 0x3, 0x4000000000000000};
#define HmdTrackingState_value 0x12
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR CameraBlobs = {0x13, 0x0, 0x11, 0x4, 0xc, 0x4, 0x4000000000000000};
#define CameraBlobs_value 0x13

//
// Note on Generate Code from Manifest Windows Vista and above
//
//Structures :  are handled as a size and pointer pairs. The macro for the event will have an extra 
//parameter for the size in bytes of the structure. Make sure that your structures have no extra padding.
//
//Strings: There are several cases that can be described in the manifest. For array of variable length 
//strings, the generated code will take the count of characters for the whole array as an input parameter. 
//
//SID No support for array of SIDs, the macro will take a pointer to the SID and use appropriate 
//GetLengthSid function to get the length.
//

//
// Allow Diasabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION

//
// Globals 
//


//
// Event Enablement Bits
//

EXTERN_C __declspec(selectany) DECLSPEC_CACHEALIGN ULONG OVR_SDK_LibOVREnableBits[1];
EXTERN_C __declspec(selectany) const ULONGLONG OVR_SDK_LibOVRKeywords[4] = {0x4000000000000000, 0x8000000000000000, 0x8000000000000000, 0x2000000000000000};
EXTERN_C __declspec(selectany) const UCHAR OVR_SDK_LibOVRLevels[4] = {4, 4, 5, 2};
EXTERN_C __declspec(selectany) MCGEN_TRACE_CONTEXT LibOVRProvider_Context = {0, 0, 0, 0, 0, 0, 0, 0, 4, OVR_SDK_LibOVREnableBits, OVR_SDK_LibOVRKeywords, OVR_SDK_LibOVRLevels};

EXTERN_C __declspec(selectany) REGHANDLE OVR_SDK_LibOVRHandle = (REGHANDLE)0;

#if !defined(McGenEventRegisterUnregister)
#define McGenEventRegisterUnregister
DECLSPEC_NOINLINE __inline
ULONG __stdcall
McGenEventRegister(
    _In_ LPCGUID ProviderId,
    _In_opt_ PENABLECALLBACK EnableCallback,
    _In_opt_ PVOID CallbackContext,
    _Inout_ PREGHANDLE RegHandle
    )
/*++

Routine Description:

    This function register the provider with ETW USER mode.

Arguments:
    ProviderId - Provider Id to be register with ETW.

    EnableCallback - Callback to be used.

    CallbackContext - Context for this provider.

    RegHandle - Pointer to Registration handle.

Remarks:

    If the handle != NULL will return ERROR_SUCCESS

--*/
{
    ULONG Error;


    if (*RegHandle) {
        //
        // already registered
        //
        return ERROR_SUCCESS;
    }

    Error = EventRegister( ProviderId, EnableCallback, CallbackContext, RegHandle); 

    return Error;
}


DECLSPEC_NOINLINE __inline
ULONG __stdcall
McGenEventUnregister(_Inout_ PREGHANDLE RegHandle)
/*++

Routine Description:

    Unregister from ETW USER mode

Arguments:
            RegHandle this is the pointer to the provider context
Remarks:
            If Provider has not register RegHandle = NULL,
            return ERROR_SUCCESS
--*/
{
    ULONG Error;


    if(!(*RegHandle)) {
        //
        // Provider has not registerd
        //
        return ERROR_SUCCESS;
    }

    Error = EventUnregister(*RegHandle); 
    *RegHandle = (REGHANDLE)0;
    
    return Error;
}
#endif
//
// Register with ETW Vista +
//
#ifndef EventRegisterOVR_SDK_LibOVR
#define EventRegisterOVR_SDK_LibOVR() McGenEventRegister(&LibOVRProvider, McGenControlCallbackV2, &LibOVRProvider_Context, &OVR_SDK_LibOVRHandle) 
#endif

//
// UnRegister with ETW
//
#ifndef EventUnregisterOVR_SDK_LibOVR
#define EventUnregisterOVR_SDK_LibOVR() McGenEventUnregister(&OVR_SDK_LibOVRHandle) 
#endif

//
// Enablement check macro for Call
//

#define EventEnabledCall() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for Call
//
#define EventWriteCall(Name, Line, FrameID)\
        EventEnabledCall() ?\
        Template_zdq(OVR_SDK_LibOVRHandle, &Call, Name, Line, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for Return
//

#define EventEnabledReturn() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for Return
//
#define EventWriteReturn(Name, Line, FrameID)\
        EventEnabledReturn() ?\
        Template_zdq(OVR_SDK_LibOVRHandle, &Return, Name, Line, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for Waypoint
//

#define EventEnabledWaypoint() ((OVR_SDK_LibOVREnableBits[0] & 0x00000002) != 0)

//
// Event Macro for Waypoint
//
#define EventWriteWaypoint(Name, Line, FrameID)\
        EventEnabledWaypoint() ?\
        Template_zdq(OVR_SDK_LibOVRHandle, &Waypoint, Name, Line, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for DistortionBegin
//

#define EventEnabledDistortionBegin() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for DistortionBegin
//
#define EventWriteDistortionBegin(VidPnTargetId, FrameID)\
        EventEnabledDistortionBegin() ?\
        Template_qq(OVR_SDK_LibOVRHandle, &DistortionBegin, VidPnTargetId, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for DistortionWaitGPU
//

#define EventEnabledDistortionWaitGPU() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for DistortionWaitGPU
//
#define EventWriteDistortionWaitGPU(VidPnTargetId, FrameID)\
        EventEnabledDistortionWaitGPU() ?\
        Template_qq(OVR_SDK_LibOVRHandle, &DistortionWaitGPU, VidPnTargetId, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for DistortionPresent
//

#define EventEnabledDistortionPresent() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for DistortionPresent
//
#define EventWriteDistortionPresent(VidPnTargetId, FrameID)\
        EventEnabledDistortionPresent() ?\
        Template_qq(OVR_SDK_LibOVRHandle, &DistortionPresent, VidPnTargetId, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for DistortionEnd
//

#define EventEnabledDistortionEnd() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for DistortionEnd
//
#define EventWriteDistortionEnd(VidPnTargetId, FrameID)\
        EventEnabledDistortionEnd() ?\
        Template_qq(OVR_SDK_LibOVRHandle, &DistortionEnd, VidPnTargetId, FrameID)\
        : ERROR_SUCCESS\

//
// Enablement check macro for HmdDesc
//

#define EventEnabledHmdDesc() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for HmdDesc
//
#define EventWriteHmdDesc(Type, VendorId, ProductId, SerialNumber, FirmwareMajor, FirmwareMinor, HmdCaps, TrackingCaps, DistortionCaps, ResolutionWidth, ResolutionHeight)\
        EventEnabledHmdDesc() ?\
        Template_qlls24llqqqdd(OVR_SDK_LibOVRHandle, &HmdDesc, Type, VendorId, ProductId, SerialNumber, FirmwareMajor, FirmwareMinor, HmdCaps, TrackingCaps, DistortionCaps, ResolutionWidth, ResolutionHeight)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraFrameReceived
//

#define EventEnabledCameraFrameReceived() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraFrameReceived
//
#define EventWriteCameraFrameReceived(FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        EventEnabledCameraFrameReceived() ?\
        Template_fqggq(OVR_SDK_LibOVRHandle, &CameraFrameReceived, FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraBeginProcessing
//

#define EventEnabledCameraBeginProcessing() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraBeginProcessing
//
#define EventWriteCameraBeginProcessing(FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        EventEnabledCameraBeginProcessing() ?\
        Template_fqggq(OVR_SDK_LibOVRHandle, &CameraBeginProcessing, FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraFrameRequest
//

#define EventEnabledCameraFrameRequest() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraFrameRequest
//
#define EventWriteCameraFrameRequest(RequestNumber, FrameCounter, LastFrameNumber)\
        EventEnabledCameraFrameRequest() ?\
        Template_xxq(OVR_SDK_LibOVRHandle, &CameraFrameRequest, RequestNumber, FrameCounter, LastFrameNumber)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraEndProcessing
//

#define EventEnabledCameraEndProcessing() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraEndProcessing
//
#define EventWriteCameraEndProcessing(FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        EventEnabledCameraEndProcessing() ?\
        Template_fqggq(OVR_SDK_LibOVRHandle, &CameraEndProcessing, FrameRate, FrameNumber, ArrivalTimeSeconds, CaptureTime, LostFrames)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraSkippedFrames
//

#define EventEnabledCameraSkippedFrames() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraSkippedFrames
//
#define EventWriteCameraSkippedFrames(RequestNumber, FrameCounter, LastFrameNumber)\
        EventEnabledCameraSkippedFrames() ?\
        Template_xxq(OVR_SDK_LibOVRHandle, &CameraSkippedFrames, RequestNumber, FrameCounter, LastFrameNumber)\
        : ERROR_SUCCESS\

//
// Enablement check macro for JSONChunk
//

#define EventEnabledJSONChunk() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for JSONChunk
//
#define EventWriteJSONChunk(Name, TotalChunks, ChunkSequence, TotalSize, ChunkSize, ChunkOffset, Chunk)\
        EventEnabledJSONChunk() ?\
        Template_zqqqqqb(OVR_SDK_LibOVRHandle, &JSONChunk, Name, TotalChunks, ChunkSequence, TotalSize, ChunkSize, ChunkOffset, Chunk)\
        : ERROR_SUCCESS\

//
// Enablement check macro for LogDebugMessage
//

#define EventEnabledLogDebugMessage() ((OVR_SDK_LibOVREnableBits[0] & 0x00000004) != 0)

//
// Event Macro for LogDebugMessage
//
#define EventWriteLogDebugMessage(Message)\
        EventEnabledLogDebugMessage() ?\
        Template_s(OVR_SDK_LibOVRHandle, &LogDebugMessage, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for LogInfoMessage
//

#define EventEnabledLogInfoMessage() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for LogInfoMessage
//
#define EventWriteLogInfoMessage(Message)\
        EventEnabledLogInfoMessage() ?\
        Template_s(OVR_SDK_LibOVRHandle, &LogInfoMessage, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for LogErrorMessage
//

#define EventEnabledLogErrorMessage() ((OVR_SDK_LibOVREnableBits[0] & 0x00000008) != 0)

//
// Event Macro for LogErrorMessage
//
#define EventWriteLogErrorMessage(Message)\
        EventEnabledLogErrorMessage() ?\
        Template_s(OVR_SDK_LibOVRHandle, &LogErrorMessage, Message)\
        : ERROR_SUCCESS\

//
// Enablement check macro for HmdTrackingState
//

#define EventEnabledHmdTrackingState() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for HmdTrackingState
//
#define EventWriteHmdTrackingState(TimeInSeconds, HeadPoseQuat, HeadPoseTranslation, HeadAngularVelocity, HeadLinearVelocity, CameraPoseQuat, CameraPoseTranslation, SensorAccelerometer, SensorGyro, SensorMagnetometer, SensorTemperature, SensortTimeInSeconds, StatusFlags, LastCameraFrameCounter)\
        EventEnabledHmdTrackingState() ?\
        Template_gF4F3F3F3F4F3F3F3F3ffqq(OVR_SDK_LibOVRHandle, &HmdTrackingState, TimeInSeconds, HeadPoseQuat, HeadPoseTranslation, HeadAngularVelocity, HeadLinearVelocity, CameraPoseQuat, CameraPoseTranslation, SensorAccelerometer, SensorGyro, SensorMagnetometer, SensorTemperature, SensortTimeInSeconds, StatusFlags, LastCameraFrameCounter)\
        : ERROR_SUCCESS\

//
// Enablement check macro for CameraBlobs
//

#define EventEnabledCameraBlobs() ((OVR_SDK_LibOVREnableBits[0] & 0x00000001) != 0)

//
// Event Macro for CameraBlobs
//
#define EventWriteCameraBlobs(BlobCount, PositionX, PositionY, Size)\
        EventEnabledCameraBlobs() ?\
        Template_qGR0GR0DR0(OVR_SDK_LibOVRHandle, &CameraBlobs, BlobCount, PositionX, PositionY, Size)\
        : ERROR_SUCCESS\

#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION


//
// Allow Diasabling of code generation
//
#ifndef MCGEN_DISABLE_PROVIDER_CODE_GENERATION

//
// Template Functions 
//
//
//Template from manifest : FunctionWaypoint
//
#ifndef Template_zdq_def
#define Template_zdq_def
ETW_INLINE
ULONG
Template_zdq(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_opt_ PCWSTR  _Arg0,
    _In_ const signed int  _Arg1,
    _In_ const unsigned int  _Arg2
    )
{
#define ARGUMENT_COUNT_zdq 3

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_zdq];

    EventDataDescCreate(&EventData[0], 
                        (_Arg0 != NULL) ? _Arg0 : L"NULL",
                        (_Arg0 != NULL) ? (ULONG)((wcslen(_Arg0) + 1) * sizeof(WCHAR)) : (ULONG)sizeof(L"NULL"));

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(const signed int)  );

    EventDataDescCreate(&EventData[2], &_Arg2, sizeof(const unsigned int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_zdq, EventData);
}
#endif

//
//Template from manifest : Distortion
//
#ifndef Template_qq_def
#define Template_qq_def
ETW_INLINE
ULONG
Template_qq(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ const unsigned int  _Arg0,
    _In_ const unsigned int  _Arg1
    )
{
#define ARGUMENT_COUNT_qq 2

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_qq];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(const unsigned int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_qq, EventData);
}
#endif

//
//Template from manifest : HmdDesc
//
#ifndef Template_qlls24llqqqdd_def
#define Template_qlls24llqqqdd_def
ETW_INLINE
ULONG
Template_qlls24llqqqdd(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ const unsigned int  _Arg0,
    _In_ const signed short  _Arg1,
    _In_ const signed short  _Arg2,
    _In_reads_(24) LPCCH  _Arg3,
    _In_ const signed short  _Arg4,
    _In_ const signed short  _Arg5,
    _In_ const unsigned int  _Arg6,
    _In_ const unsigned int  _Arg7,
    _In_ const unsigned int  _Arg8,
    _In_ const signed int  _Arg9,
    _In_ const signed int  _Arg10
    )
{
#define ARGUMENT_COUNT_qlls24llqqqdd 11

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_qlls24llqqqdd];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(const signed short)  );

    EventDataDescCreate(&EventData[2], &_Arg2, sizeof(const signed short)  );

    EventDataDescCreate(&EventData[3], _Arg3, (ULONG)(sizeof(CHAR)*24));

    EventDataDescCreate(&EventData[4], &_Arg4, sizeof(const signed short)  );

    EventDataDescCreate(&EventData[5], &_Arg5, sizeof(const signed short)  );

    EventDataDescCreate(&EventData[6], &_Arg6, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[7], &_Arg7, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[8], &_Arg8, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[9], &_Arg9, sizeof(const signed int)  );

    EventDataDescCreate(&EventData[10], &_Arg10, sizeof(const signed int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_qlls24llqqqdd, EventData);
}
#endif

//
//Template from manifest : CameraFrameData
//
#ifndef Template_fqggq_def
#define Template_fqggq_def
ETW_INLINE
ULONG
Template_fqggq(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ const float  _Arg0,
    _In_ const unsigned int  _Arg1,
    _In_ const double  _Arg2,
    _In_ const double  _Arg3,
    _In_ const unsigned int  _Arg4
    )
{
#define ARGUMENT_COUNT_fqggq 5

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_fqggq];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(const float)  );

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[2], &_Arg2, sizeof(const double)  );

    EventDataDescCreate(&EventData[3], &_Arg3, sizeof(const double)  );

    EventDataDescCreate(&EventData[4], &_Arg4, sizeof(const unsigned int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_fqggq, EventData);
}
#endif

//
//Template from manifest : CameraFrameRequest
//
#ifndef Template_xxq_def
#define Template_xxq_def
ETW_INLINE
ULONG
Template_xxq(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ unsigned __int64  _Arg0,
    _In_ unsigned __int64  _Arg1,
    _In_ const unsigned int  _Arg2
    )
{
#define ARGUMENT_COUNT_xxq 3

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_xxq];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(unsigned __int64)  );

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(unsigned __int64)  );

    EventDataDescCreate(&EventData[2], &_Arg2, sizeof(const unsigned int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_xxq, EventData);
}
#endif

//
//Template from manifest : JSONChunk
//
#ifndef Template_zqqqqqb_def
#define Template_zqqqqqb_def
ETW_INLINE
ULONG
Template_zqqqqqb(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_opt_ PCWSTR  _Arg0,
    _In_ const unsigned int  _Arg1,
    _In_ const unsigned int  _Arg2,
    _In_ const unsigned int  _Arg3,
    _In_ const unsigned int  _Arg4,
    _In_ const unsigned int  _Arg5,
    _In_reads_(_Arg4) const BYTE*  _Arg6
    )
{
#define ARGUMENT_COUNT_zqqqqqb 7

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_zqqqqqb];

    EventDataDescCreate(&EventData[0], 
                        (_Arg0 != NULL) ? _Arg0 : L"NULL",
                        (_Arg0 != NULL) ? (ULONG)((wcslen(_Arg0) + 1) * sizeof(WCHAR)) : (ULONG)sizeof(L"NULL"));

    EventDataDescCreate(&EventData[1], &_Arg1, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[2], &_Arg2, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[3], &_Arg3, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[4], &_Arg4, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[5], &_Arg5, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[6], _Arg6, (ULONG)sizeof(char)*_Arg4);

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_zqqqqqb, EventData);
}
#endif

//
//Template from manifest : Log
//
#ifndef Template_s_def
#define Template_s_def
ETW_INLINE
ULONG
Template_s(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_opt_ LPCSTR  _Arg0
    )
{
#define ARGUMENT_COUNT_s 1

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_s];

    EventDataDescCreate(&EventData[0], 
                        (_Arg0 != NULL) ? _Arg0 : "NULL",
                        (_Arg0 != NULL) ? (ULONG)((strlen(_Arg0) + 1) * sizeof(CHAR)) : (ULONG)sizeof("NULL"));

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_s, EventData);
}
#endif

//
//Template from manifest : HmdTrackingState
//
#ifndef Template_gF4F3F3F3F4F3F3F3F3ffqq_def
#define Template_gF4F3F3F3F4F3F3F3F3ffqq_def
ETW_INLINE
ULONG
Template_gF4F3F3F3F4F3F3F3F3ffqq(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ const double  _Arg0,
    _In_reads_(4) const float *_Arg1,
    _In_reads_(3) const float *_Arg2,
    _In_reads_(3) const float *_Arg3,
    _In_reads_(3) const float *_Arg4,
    _In_reads_(4) const float *_Arg5,
    _In_reads_(3) const float *_Arg6,
    _In_reads_(3) const float *_Arg7,
    _In_reads_(3) const float *_Arg8,
    _In_reads_(3) const float *_Arg9,
    _In_ const float  _Arg10,
    _In_ const float  _Arg11,
    _In_ const unsigned int  _Arg12,
    _In_ const unsigned int  _Arg13
    )
{
#define ARGUMENT_COUNT_gF4F3F3F3F4F3F3F3F3ffqq 14

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_gF4F3F3F3F4F3F3F3F3ffqq];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(const double)  );

    EventDataDescCreate(&EventData[1],  _Arg1, sizeof(const float)*4);

    EventDataDescCreate(&EventData[2],  _Arg2, sizeof(const float)*3);

    EventDataDescCreate(&EventData[3],  _Arg3, sizeof(const float)*3);

    EventDataDescCreate(&EventData[4],  _Arg4, sizeof(const float)*3);

    EventDataDescCreate(&EventData[5],  _Arg5, sizeof(const float)*4);

    EventDataDescCreate(&EventData[6],  _Arg6, sizeof(const float)*3);

    EventDataDescCreate(&EventData[7],  _Arg7, sizeof(const float)*3);

    EventDataDescCreate(&EventData[8],  _Arg8, sizeof(const float)*3);

    EventDataDescCreate(&EventData[9],  _Arg9, sizeof(const float)*3);

    EventDataDescCreate(&EventData[10], &_Arg10, sizeof(const float)  );

    EventDataDescCreate(&EventData[11], &_Arg11, sizeof(const float)  );

    EventDataDescCreate(&EventData[12], &_Arg12, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[13], &_Arg13, sizeof(const unsigned int)  );

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_gF4F3F3F3F4F3F3F3F3ffqq, EventData);
}
#endif

//
//Template from manifest : CameraBlobs
//
#ifndef Template_qGR0GR0DR0_def
#define Template_qGR0GR0DR0_def
ETW_INLINE
ULONG
Template_qGR0GR0DR0(
    _In_ REGHANDLE RegHandle,
    _In_ PCEVENT_DESCRIPTOR Descriptor,
    _In_ const unsigned int  _Arg0,
    _In_reads_(_Arg0) const double *_Arg1,
    _In_reads_(_Arg0) const double *_Arg2,
    _In_reads_(_Arg0) const signed int *_Arg3
    )
{
#define ARGUMENT_COUNT_qGR0GR0DR0 4

    EVENT_DATA_DESCRIPTOR EventData[ARGUMENT_COUNT_qGR0GR0DR0];

    EventDataDescCreate(&EventData[0], &_Arg0, sizeof(const unsigned int)  );

    EventDataDescCreate(&EventData[1],  _Arg1, sizeof(const double)*_Arg0);

    EventDataDescCreate(&EventData[2],  _Arg2, sizeof(const double)*_Arg0);

    EventDataDescCreate(&EventData[3],  _Arg3, sizeof(const signed int)*_Arg0);

    return EventWrite(RegHandle, Descriptor, ARGUMENT_COUNT_qGR0GR0DR0, EventData);
}
#endif

#endif // MCGEN_DISABLE_PROVIDER_CODE_GENERATION

#if defined(__cplusplus)
};
#endif

#define MSG_OVR_SDK_LibOVR_opcode_FN_CALL_message 0x3000000AL
#define MSG_OVR_SDK_LibOVR_opcode_FN_RETURN_message 0x3000000BL
#define MSG_OVR_SDK_LibOVR_opcode_FN_WAYPOINT_message 0x3000000CL
#define MSG_OVR_SDK_LibOVR_opcode_DIS_BEGIN_message 0x3000000DL
#define MSG_OVR_SDK_LibOVR_opcode_DIS_WAITGPU_message 0x3000000EL
#define MSG_OVR_SDK_LibOVR_opcode_DIS_PRESENT_message 0x3000000FL
#define MSG_OVR_SDK_LibOVR_opcode_DIS_END_message 0x30000010L
#define MSG_OVR_SDK_LibOVR_opcode_HMD_DESC_message 0x30000011L
#define MSG_OVR_SDK_LibOVR_opcode_CAM_RECEIVE_message 0x30000012L
#define MSG_OVR_SDK_LibOVR_opcode_CAM_REQUEST_message 0x30000013L
#define MSG_level_Error                      0x50000002L
#define MSG_level_Informational              0x50000004L
#define MSG_level_Verbose                    0x50000005L
#define MSG_OVR_SDK_LibOVR_task_FN_TRACE_message 0x70000001L
#define MSG_OVR_SDK_LibOVR_task_DIS_TRACE_message 0x70000002L
#define MSG_OVR_SDK_LibOVR_task_HMD_TRACE_message 0x70000003L
#define MSG_OVR_SDK_LibOVR_task_CAMERA_TRACE_message 0x70000004L
#define MSG_OVR_SDK_LibOVR_task_LOG_TRACE_message 0x70000005L
#define MSG_OVR_SDK_LibOVR_event_0_message   0xB0000000L
#define MSG_OVR_SDK_LibOVR_event_1_message   0xB0000001L
#define MSG_OVR_SDK_LibOVR_event_2_message   0xB0000002L
#define MSG_OVR_SDK_LibOVR_event_4_message   0xB0000004L
#define MSG_OVR_SDK_LibOVR_event_5_message   0xB0000005L
#define MSG_OVR_SDK_LibOVR_event_6_message   0xB0000006L
#define MSG_OVR_SDK_LibOVR_event_7_message   0xB0000007L
#define MSG_OVR_SDK_LibOVR_event_8_message   0xB0000008L
#define MSG_OVR_SDK_LibOVR_event_9_message   0xB0000009L
#define MSG_OVR_SDK_LibOVR_event_10_message  0xB000000AL
#define MSG_OVR_SDK_LibOVR_event_11_message  0xB000000BL
#define MSG_OVR_SDK_LibOVR_event_12_message  0xB000000CL
#define MSG_OVR_SDK_LibOVR_event_13_message  0xB000000DL
#define MSG_OVR_SDK_LibOVR_event_14_message  0xB000000EL
#define MSG_OVR_SDK_LibOVR_event_15_message  0xB000000FL
#define MSG_OVR_SDK_LibOVR_event_16_message  0xB0000010L
#define MSG_OVR_SDK_LibOVR_event_17_message  0xB0000011L
#define MSG_OVR_SDK_LibOVR_event_18_message  0xB0000012L
#define MSG_OVR_SDK_LibOVR_event_19_message  0xB0000013L
