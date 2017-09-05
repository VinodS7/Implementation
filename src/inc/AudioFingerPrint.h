#if !defined(__AudioFingerPrint_hdr__)
#define __AudioFingerPrint_hdr__

#include <string>
#include <fstream>
#include <algorithm>

#include "ErrorDef.h"

class CAudioFingerPrint
{
public:

	virtual ~CAudioFingerPrint(void);
	//Default constructor and destructor

	static Error_t create(CAudioFingerPrint*& pCAudioFingerPrint);
	//Creates a new instance of an audio fingerprint
	static Error_t destroy(CAudioFingerPrint*& pCAudioFingerPrint);
	//Destroys an instance of an audio fingerprint

	Error_t init(int iBlockLength = 1025, int iHopSize = 512 , int iLowerFrequencyBand = 200 , int iUpperFrequencyBand = 2000 , int iNumberBits = 32);
	//Initialize the Block length and hop size

	Error_t reset();
	//Reset all internal class members

	Error_t process(float *pfInputBuffer, float *pfOutputBuffer, float *pfOldBuffer, int iLengthOfBuffers);
	// Processes the input file to generate the fingerprint
    

private:

	CAudioFingerPrint(void);

	bool _is_initialized;
	int _block_length;
	int _hop_size;
	int _lower_freq_band_limit;
	int _upper_freq_band_limit;
	int _number_bits;
};

#endif 
