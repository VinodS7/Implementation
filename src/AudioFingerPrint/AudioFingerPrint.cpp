#include <iostream>
#include <iomanip>
#include <math.h>

#include "Vector.h"
#include "Util.h"
#include "ErrorDef.h"

#include "AudioFingerPrint.h"
#include "Fft.h"

using namespace std;

CAudioFingerPrint::CAudioFingerPrint(void):
	_is_initialized(false),
	_block_length(0),
	_hop_size(0)
	{
		reset();
	}

CAudioFingerPrint::~CAudioFingerPrint(void) {};

Error_t CAudioFingerPrint::create(CAudioFingerPrint*& pCAudioFingerPrint)
{
	pCAudioFingerPrint = new CAudioFingerPrint();

	if(!pCAudioFingerPrint)
		return kUnknownError;

	return kNoError;
}

Error_t CAudioFingerPrint:: destroy(CAudioFingerPrint*& pCAudioFingerPrint)
{
	if(!pCAudioFingerPrint)
		return kUnknownError;

	pCAudioFingerPrint->reset();

	delete pCAudioFingerPrint;
	pCAudioFingerPrint = 0;

	return kNoError;
}

Error_t CAudioFingerPrint::init(int iBlockLength , int iHopSize , int iLowerFrequencyBand, int iUpperFrequencyBand , int iNumberBits)
{
	reset();

	if(CUtil::isPowOf2(iBlockLength)){
		_block_length = iBlockLength;	
	}
	else{
		_block_length = CUtil::nextPowOf2(iBlockLength);
	}
	
	if(CUtil::isPowOf2(iHopSize)){
		_hop_size = iHopSize;
	}
	else{
		_hop_size = CUtil::nextPowOf2(iHopSize);
	}
	
	if(CUtil::isPowOf2(iNumberBits)){
		_number_bits = iNumberBits;	
	}
	else{
		_number_bits = CUtil::nextPowOf2(iNumberBits);
	}

	_lower_freq_band_limit = iLowerFrequencyBand;
	_upper_freq_band_limit = iUpperFrequencyBand;


	_is_initialized = true;
	return kNoError;
}

Error_t CAudioFingerPrint::reset()
{
	_block_length = 0;
	_hop_size = 0;
	_is_initialized = false;
	_lower_freq_band_limit = 0;
	_upper_freq_band_limit = 0;
	_number_bits = 0;
	return kNoError;
}


Error_t CAudioFingerPrint::process(float *pfInputBuffer, float *pfOutputBuffer, float *pfOldBuffer, int iLengthOfBuffers)
{
	CFft *phFft = 0;
    float *pfInputBlock = 0;
    
    pfInputBlock = new float[_block_length];
    
	if(!_is_initialized)
    {
        return kNotInitializedError;
    }

    for(int j = 0; j<_block_length; j++)
    {
        if( j<iLengthOfBuffers)
        {
            pfInputBlock[j] = pfInputBuffer[j]*2*_block_length;
        }
        else
        {
            pfInputBlock[j] = 0;
        }
    }
	
    //Process the input blocks
    CFft::complex_t* pfSpectrum = new CFft::complex_t[_block_length];
    float* pfMagnitude = new float[_block_length];
    float fFingerPrint[_number_bits];
    float*pfMagCompressed = new float [_number_bits+1];
    float fLowerBound = 0;
    float fUpperBound = 0;
    float fLowerBoundExp = 0;
    float fUpperBoundExp = 0;
    float fFreq[_block_length];
    CFft::create(phFft);
    phFft->init(_block_length, 2, CFft::WindowFunction_t::kWindowHann, CFft::Windowing_t::kPreWindow);
    phFft->doFft(pfSpectrum,pfInputBlock);
    phFft->getMagnitude(pfMagnitude, pfSpectrum);

   //    pfOutputBuffer = pfInputBlock;

    for (int j = 0; j < _block_length;j++)
    {
        cout<<pfInputBlock[j]<< " "<<pfMagnitude[j]<<endl;
    }
    
    for(int j = 0; j<_number_bits+1; j++ )
    {
        fUpperBoundExp = fLowerBoundExp+log10(_upper_freq_band_limit/_lower_freq_band_limit)/(_number_bits+1);

        fLowerBound = _lower_freq_band_limit*pow(10,fLowerBoundExp);
        fUpperBound = _lower_freq_band_limit*pow(10,fUpperBoundExp);
        pfMagCompressed[j] = 0;

        for(int k = 0; k< _block_length; k++)
        {
            fFreq[k] = phFft->bin2freq(k, 44100);
            
            if(fLowerBound<=fFreq[k]&&fUpperBound>=fFreq[k])
            {
                pfMagCompressed[j] = pfMagCompressed[j]+pfMagnitude[k];
                
            }
        }
        fLowerBoundExp = fUpperBoundExp;
    }
    
    for(int i = 0; i<_number_bits;i++)
    {
        fFingerPrint[i] = pfMagCompressed[i]-pfMagCompressed[i+1]- pfOldBuffer[i] +pfOldBuffer[i+1];
        cout<<fFingerPrint[i]<<endl;
        if(fFingerPrint[i]>0)
        {
            pfOutputBuffer[i] = 1;
        }
        else
        {
            pfOutputBuffer[i] = 0;
        }
        
    }

    for(int i = 0; i<_number_bits+1;i++)
    {
        pfOldBuffer[i] = pfMagCompressed[i];
    }
    return kNoError;
}
