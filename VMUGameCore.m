/*
 Copyright (c) 2009, OpenEmu Team
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the OpenEmu Team nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY OpenEmu Team ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL OpenEmu Team BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import "VMUGameCore.h"
#import <OpenEmuBase/OERingBuffer.h>
#import <OpenEmuBase/OEGameCore.h>
#import <OpenEmuBase/OEGeometry.h>
#import "OEVMUSystemResponderClient.h"
#import <OpenGL/gl.h>

#define _UINT32

#include "prototypes.h"

#define SAMPLERATE 44100
#define SAMPLEFRAME 735
#define SIZESOUNDBUFFER SAMPLEFRAME*4

@interface VMUGameCore () <OEVMUSystemResponderClient>
@end


// Global variables because the callbacks need to access them...
static OERingBuffer *ringBuffer;
VMUGameCore *current;
@implementation VMUGameCore

/*
 OpenEmu Core internal functions
 */
- (id)init
{
    if((self = [super init]))
    {
        soundLock = [[NSLock alloc] init];
		bufLock = [[NSLock alloc] init];
        
		tempBuffer = malloc(256 * 256 * 4);
		
		position = 0;
		sndBuf = malloc(SIZESOUNDBUFFER * sizeof(UInt16));
		memset(sndBuf, 0, SIZESOUNDBUFFER * sizeof(UInt16));
		ringBuffer = [self ringBufferAtIndex:0];
    }
	
	current = self;
    
	return self;
}

- (void) dealloc
{
	DLog(@"releasing/deallocating VMU memory");
	free(sndBuf);
	[soundLock release];
	[bufLock release];
    
	free(lcdbuffer);
	
	resetcpu();
	[super dealloc];
}

- (void)executeFrame
{
    [self executeFrameSkippingFrame:NO];
}

- (void)executeFrameSkippingFrame: (BOOL) skip
{
	//DLog(@"Executing");
	//Get a reference to the emulator
    
//    if(skip)[bufLock lock];
    run_cpu();   /// run cpu until it thinks it's time for an lcd update
//    if(skip)[bufLock unlock];
}

- (void)setupEmulation
{
    //loadflash([gamepath UTF8String]);
    //resetcpu();
    //do_vmsgame([gamepath UTF8String],NULL);
}

- (BOOL)loadFileAtPath:(NSString *)path error:(NSError **)error
{
    //gamepath = [[NSString alloc] initWithString:path];
    char *gamepath2 = (char *)[path UTF8String];
    NSLog(@"VMU Loaded file at path: %@",path);
    //do_vmsgame([path UTF8String],NULL);
    do_vmsgame(gamepath2, (char *)NULL);
    
    //TODO: Check that magic header is 0x211000D9 otherwise return NO
    
    //TODO: Handle loading with BIOS
    
    return YES;
}

- (void)resetEmulation
{
	resetcpu();
}

- (void)stopEmulation
{
//    resetcpu();
	[super stopEmulation];
}

#pragma mark Video
- (const void *)videoBuffer
{
	return lcdbuffer;
}

- (OEIntRect)screenRect
{
    return OEIntRectMake(0, 0, WIDTH, HEIGHT);
}

- (OEIntSize)bufferSize
{
    return OEIntSizeMake(WIDTH, HEIGHT);
}

- (OEIntSize)aspectSize
{
    return OEIntSizeMake(WIDTH, HEIGHT);
}

- (GLenum)pixelFormat
{
	return GL_RGBA;
}

- (GLenum)pixelType
{
	return GL_UNSIGNED_INT_8_8_8_8;
}

- (GLenum)internalPixelFormat
{
	return GL_RGBA;
}

#pragma mark Audio
- (double)audioSampleRate
{
    return SAMPLERATE;
}

- (NSTimeInterval)frameInterval
{
    return 60;
}

- (NSUInteger)channelCount
{
	return 1;
}

static const NSUInteger buttonDirections[2][5] = {
    { 0, VMU_PAD1_UP, VMU_PAD1_DOWN, VMU_PAD1_LEFT, VMU_PAD1_RIGHT }
};
static const NSUInteger buttonActions[2][2] = {
    { VMU_PAD1_A, VMU_PAD1_B }
};


#pragma mark Input
unsigned VMUButtonTable[] = {
    VMU_PAD1_UP, VMU_PAD1_DOWN, VMU_PAD1_LEFT, VMU_PAD1_RIGHT, VMU_PAD1_A, VMU_PAD1_B, VMU_PAD1_SLEEP, VMU_PAD1_MODE
};

NSString *VMUButtonNameTable[] = { @"VMU_PAD@_UP", @"VMU_PAD@_DOWN", @"VMU_PAD@_LEFT", @"VMU_PAD@_RIGHT", @"VMU_PAD@_A", @"VMU_PAD@_B", @"VMU_PAD@_SLEEP", @"VMU_PAD@_MODE" };

- (BOOL)shouldPauseForButton:(NSInteger)button
{
    if(button == VMU_PAD1_SLEEP)
    {
        [self pauseEmulation:self];
        return YES;
    }
    return NO;
}

- (oneway void)didPushVMUButton:(OEVMUButton)button forPlayer:(NSUInteger)player
{
    VeMU_ButtonPressed(VMUButtonTable[button]);
}

- (oneway void)didReleaseVMUButton:(OEVMUButton)button forPlayer:(NSUInteger)player
{
    VeMU_ButtonReleased(VMUButtonTable[button]);
}

@end
