/** @file

MODULE:
 
TAG: 

FILE NAME: 

DESCRIPTION: 

COPYRIGHT: (c)CSIR 2007-2018 all rights reserved

LICENSE: Software License Agreement (BSD License)

RESTRICTIONS: 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may 
be used to endorse or promote products derived from this software without specific 
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
===========================================================================

*/
#pragma once
#include <cstdint>
#include <fstream>
#include <DirectShowExt/CodecControlInterface.h>
#include <DirectShowExt/CustomBaseFilter.h>
#include <DirectShowExt/DirectShowMediaFormats.h>
#include <DirectShowExt/FilterParameterStringConstants.h>
#include <DirectShowExt/NotifyCodes.h>
#include <DirectShowExt/ParameterConstants.h>
#include "VersionInfo.h"

// Forward
class ICodecv2;

static const GUID CLSID_RTVC_H264Encoder = 
{ 0x4d5a7dc8, 0x202a, 0x40f6, { 0x91, 0x3f, 0xf, 0xdf, 0x77, 0xda, 0x72, 0xf8 } };

// {06B47B23-E7C4-47C0-A054-74779A9DB113}
static const GUID CLSID_H264Properties = 
{ 0x6b47b23, 0xe7c4, 0x47c0, { 0xa0, 0x54, 0x74, 0x77, 0x9a, 0x9d, 0xb1, 0x13 } };

class H264EncoderFilter : public CCustomBaseFilter,
                          public ISpecifyPropertyPages,
                          public ICodecControlInterface
{
public:
  DECLARE_IUNKNOWN

	/// Constructor
	H264EncoderFilter();
	/// Destructor
	~H264EncoderFilter();
	/// Static object-creation method (for the class factory)
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr); 
	/**
	 * Overriding this so that we can set whether this is an RGB24 or an RGB32 Filter
	 */
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	/**
	 * @brief Used for Media Type Negotiation 
	 * Returns an HRESULT value. Possible values include those shown in the following table.
	 * <table border="0" cols="2"><tr valign="top"><td><b>Value</b></td><td><b>Description</b></td></TR><TR><TD>S_OK</TD><TD>Success</TD></TR><TR><TD>VFW_S_NO_MORE_ITEMS</TD><TD>Index out of range</TD></TR><TR><TD>E_INVALIDARG</TD><TD>Index less than zero</TD></TR></TABLE>
	 * The output pin's CTransformOutputPin::GetMediaType method calls this method. The derived class must implement this method. For more information, see CBasePin::GetMediaType.
	 * To use custom media types, the media type can be manipulated in this method.
	 */
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	/**
 	 * @brief Buffer Allocation
   * The output pin's CTransformOutputPin::DecideBufferSize method calls this method. The derived class must implement this method. For more information, see CBaseOutputPin::DecideBufferSize. 
 	 * @param pAlloc Pointer to the IMemAllocator interface on the output pin's allocator.
 	 * @param pProp Pointer to an ALLOCATOR_PROPERTIES structure that contains buffer requirements from the downstream input pin.
 	 * @return Value: Returns S_OK or another HRESULT value.
 	 */
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp);
	/**
	 * The CheckTransform method checks whether an input media type is compatible with an output media type.
	 * <table border="0" cols="2"> <tr valign="top"> <td  width="50%"><b>Value</b></td> <td width="50%"><b>Description</b></td> </tr> <tr valign="top"> <td width="50%">S_OK</td> <td width="50%">The media types are compatible.</td> </tr> <tr valign="top"> <td width="50%">VFW_E_TYPE_NOT_ACCEPTED</td> <td width="50%">The media types are not compatible.</td> </tr> </table>
	 */
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);

  virtual void doGetVersion(std::string& sVersion)
  {
    sVersion = VersionInfo::toString();
  }
	/// Interface methods
	///Overridden from CSettingsInterface
	virtual void initParameters()
	{
		addParameter(CODEC_PARAM_FRAME_BIT_LIMIT, &m_nFrameBitLimit, 0);
    addParameter(CODEC_PARAM_NOTIFYONIFRAME, &m_bNotifyOnIFrame, false);
    addParameter(CODEC_PARAM_IFRAME_PERIOD, &m_uiIFramePeriod, 0);
    addParameter(FILTER_PARAM_SPS, &m_sSeqParamSet, "", true);
    addParameter(FILTER_PARAM_PPS, &m_sPicParamSet, "", true);
    addParameter(FILTER_PARAM_H264_OUTPUT_TYPE, &m_nH264Type, H264_AVC1);
  }
	/// Overridden from SettingsInterface
	STDMETHODIMP GetParameter( const char* szParamName, int nBufferSize, char* szValue, int* pLength );
	STDMETHODIMP SetParameter( const char* type, const char* value);
	STDMETHODIMP GetParameterSettings( char* szResult, int nSize );
  /**
   * @brief Overridden from ICodecControlInterface. Getter for frame bit limit
   */
  STDMETHODIMP GetFramebitLimit(int& iFrameBitLimit);
  /**
   * @brief @ICodecControlInterface. Overridden from ICodecControlInterface
   */
  STDMETHODIMP SetFramebitLimit(int iFrameBitLimit);
  /**
   * @brief @ICodecControlInterface. Method to query group id
   */
  STDMETHODIMP GetGroupId(int& iGroupId);
  /**
   * @brief @ICodecControlInterface. Method to set group id
   */
  STDMETHODIMP SetGroupId(int iGroupId);
  /**
   * @brief @ICodecControlInterface. Overridden from ICodecControlInterface
   */
  STDMETHODIMP GenerateIdr();
  STDMETHODIMP GetBitrateKbps(int& uiBitrateKbps) { return E_FAIL; }
  STDMETHODIMP SetBitrateKbps(int uiBitrateKbps) { return E_FAIL; }
  
  STDMETHODIMP GetPages(CAUUID *pPages)
  {
    if (pPages == NULL) return E_POINTER;
    pPages->cElems = 1;
    pPages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID));
    if (pPages->pElems == NULL) 
    {
      return E_OUTOFMEMORY;
    }
    pPages->pElems[0] = CLSID_H264Properties;
    return S_OK;
  }

  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv)
  {
    if (riid == IID_ISpecifyPropertyPages)
    {
      return GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
    }
    else if (riid == IID_ICodecControlInterface)
    {
      return GetInterface(static_cast<ICodecControlInterface*>(this), ppv);
    }
    else
    {
      // Call the parent class.
      return CCustomBaseFilter::NonDelegatingQueryInterface(riid, ppv);
    }
  }

	/// Overridden from CCustomBaseFilter
	virtual void InitialiseInputTypes();

  HRESULT Transform(IMediaSample *pSource, IMediaSample *pDest);

private:
  /**
   * This method copies the h.264 sequence and picture parameter sets into the passed in buffer
   * and returns the total length including start codes
   */
  unsigned copySequenceAndPictureParameterSetsIntoBuffer(BYTE* pBuffer);
  unsigned getParameterSetLength() const;  
	/**
 	 * This method converts the input buffer from RGB24 | 32 to YUV420P
	 * @param pSource The source buffer
	 * @param pDest The destination buffer
	 */
	virtual HRESULT ApplyTransform(BYTE* pBufferIn, long lInBufferSize, long lActualDataLength, BYTE* pBufferOut, long lOutBufferSize, long& lOutActualDataLength);

  int m_nH264Type;
	ICodecv2* m_pCodec;
  /// Receive Lock
  CCritSec m_csCodec;
	int m_nFrameBitLimit;
	bool m_bNotifyOnIFrame;
  unsigned char* m_pSeqParamSet;
  unsigned m_uiSeqParamSetLen;
  unsigned char* m_pPicParamSet;
  unsigned m_uiPicParamSetLen;
  std::string m_sSeqParamSet;
  std::string m_sPicParamSet;

  // For auto i-frame generation
  unsigned m_uiIFramePeriod;
  // frame counter for i-frame generation
  unsigned m_uiCurrentFrame;

  REFERENCE_TIME m_rtFrameLength;
  REFERENCE_TIME m_tStart;
  REFERENCE_TIME m_tStop;
};
