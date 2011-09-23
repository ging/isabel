/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////


========================================================

library: vCodecs
author : Vicente Sirvent Orts and Gabriel Huecas
em@il  : {sirvent, gabriel}@dit.upm.es

========================================================

0 - Compilation

  Getting source code:

  a) Download vCodecs from repository.

  b) Set Visual Studio .Net enviroment to include ./vCodecs/include
  
  c) Make sure nasm.exe windows version is intalled in $PATH 

  Supported platforms:
	
	  - Win32 
	  - Linux (NOTE: Root user is needed in order to install lib) 
	  - Symbian OS


1 - Usage (See /example/test.cxx for a real implementation)

	
	//----------------------------------
	// 0 - Initializing the library
	//----------------------------------
	
	//Initializes library, 
	//called only once in an application program.

	vCodecInit(void);

	//----------------------------------
	// 1 - Getting codecs
	//----------------------------------

	//Fill the param struct
	vCoderArgs_t params;

	params.BitRate = 1000; //output coder bitrate (in kbps)
	params.FR      = 30;   //expected fps, (used to calc Bitrate)
	params.Height  = 240;  
	params.Width   = 320;
	params.format  = BGR_FORMAT; //wanted input format, 
	                             //posible formats {I420_FORMAT,BGR_FORMAT}

	params.Q       = 0; //Quality depends of codec presets

	//To create a Coder from a PayloadType with selected params:

	vCoder_t * coder = vGetCoderByPT(PT)->newCoder(&params) //PT = MPEG4_PT, MPEG1_PT, H263_PT, H264_PT, MJPEG_PT

	//----------------------------------
	// 2 - Getting decoders
	//----------------------------------

	//Fill the param struct
	DecoderArgs params;
	params.Height  = 0;  //not needed, decoders get it from frames
	params.Width   = 0;  //not needed, decoders get it from frames
	params.format  = BGR_FORMAT; //wanted output format ,
	                             //posible formats {I420_FORMAT,BGR_FORMAT}
    

	vDecoder_t * decoder = vGetDecoderByPT(PT)->newDecoder(&params) //PT = PT = MPEG4_PT, MPEG1_PT, H263_PT, H264_PT, MJPEG_PT

	
	//----------------------------------
	// 3 - Encoding
	//----------------------------------

	int n = vEncode(coder,inBuff,inBuffLen,outBuff,outBuffLen);
	//Params:
	//  coder     :: "vCoder_t *" obtained in pass "1"
	//  inBuff    :: "unsigned char *" input buffer with frame to encode
	//  inBuffLen :: "int" input buffer len
	//  outBuff   :: "unsigned char *" output buffer to save encoded frame
	//  outBuffLen:: "int" output buffer len
	//
	//Returns:
	//  int n     :: Possible meanings:
	//    1)  if n<0 --> ERROR::Error encoding (see posible error messages)
	//    2)  if 0<n<outBuffLen --> OK::len (in bytes) of encoded image stored at "outBuff".
	//    3)  if n>outBuffLen --> ERROR::Not enough outBuffLen,
        //        alloc at least n bytes and call encode again.


	//----------------------------------
	// 4 - Decoding
	//----------------------------------

	int n = vDecode(decoder,inBuff,inBuffLen,outBuff,outBuffLen);
	//Params:
	//  decoder   :: "vDecoder_t *" obtained in pass "2"
	//  inBuff    :: "unsigned char *" input buffer with encoded frame
	//  inBuffLen :: "int" input buffer len
	//  outBuff   :: "unsigned char *" output buffer to save decoded frame
	//  outBuffLen:: "int" output buffer len
	//
	//Returns:
	//  int n     :: Possible meanings:
	//    1)  if n<0 --> ERROR::Error decoding (see posible error messages)
	//    2)  if n=0 --> Can't decode due first I-VOP have not arrived yet.
	//    3)  if 0<n<outBuffLen --> OK::len (in bytes) of decoded image stored at "outBuff".
	//    4)  if n>outBuffLen --> ERROR::Not enough outBuffLen,
        //        alloc at least n bytes and call encode again.

	//----------------------------------
	// 5 - Getting parameters
	//----------------------------------

	// Used to get info to alloc memory

	vCoderArgs_t coderParams;
	vGetParams(coder, &coderParams);

	vDecoderArgs_t decoderParams;
	vGetParams(decoder, &decoderParams);

        This functions MUST be called after vEncode/vDecode to get
        the params of the resulting image.
        Note: do not confuse with the initial structure used
              in the creation of the coder/decoder 

	//----------------------------------
	// 6 - Deleting coders/decoders
	//----------------------------------

	vDeleteCoder(coder)     // Params :: vCoder_t * obtained in "1"
	vDeleteDecoder(decoder) // Params :: vDecoder_t * obtained in "2"


	//----------------------------------
	// 7 - Shutting down the library
	//----------------------------------
	//called only once in an application program.

	vCodecRelease(void);

