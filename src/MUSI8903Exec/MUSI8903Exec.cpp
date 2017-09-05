
#include <iostream>
#include <ctime>

#include "MUSI8903Config.h"
#include "Fft.h"
#include "AudioFingerPrint.h"
#include "AudioFileIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
     std::string             sInputFilePath,                 //!< file paths
                            sOutputFilePath;

    static const int        kBlockSize          = 2048;
    
    static const int        kHopSize            = 1024;

    clock_t                 time                = 0;

    float                   **ppfAudioData      = 0;
    float                   *pfInputAudioData   = 0;
    float                   *pfOutputAudioData  = 0;
    float                   *pfPrevAudioData    = 0;
    float                   *pfOldAudioData     = 0;
    
    long long               iLengthInFrames     = 0;
    long long               iFrame              = 0;

    CAudioFileIf            *phAudioFile        = 0;
    std::fstream            hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    CAudioFingerPrint       *phAudioFingerPrint = 0;


    showClInfo ();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        return -1;
    }
    else
    {
        sInputFilePath  = argv[1];
        sOutputFilePath = sInputFilePath + ".txt";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    phAudioFile->getLength(iLengthInFrames);

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
    hOutputFile.open (sOutputFilePath.c_str(), std::ios::out);
    if (!hOutputFile.is_open())
    {
        cout << "Text file open error!";
        return -1;
    }
    
    //Create and Initialize Audio Fingerprint
    CAudioFingerPrint::create(phAudioFingerPrint);
    phAudioFingerPrint->init(kBlockSize,kHopSize,200,20000,32);

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData            = new float* [stFileSpec.iNumChannels];
    for(int c = 0; c<stFileSpec.iNumChannels;c++)
    {
        ppfAudioData[c] = new float [kBlockSize-kHopSize];
    }
    
    pfInputAudioData = new float [kBlockSize];
    pfOutputAudioData = new float [32];
    pfPrevAudioData = new float [kHopSize];
    pfOldAudioData = new float [33];

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output file
    while (!phAudioFile->isEof())
    {
        long long iNumFrames = kBlockSize-kHopSize;
        phAudioFile->readData(ppfAudioData, iNumFrames);
        phAudioFile->getPosition(iFrame);
     
        //Preprocess audio by normalizing and reducing to one channel
        if(iNumFrames !=kBlockSize-kHopSize)
        {
            float* pfTemp = new float[kBlockSize];
            
            for(int i = 0; i< kBlockSize-kHopSize; i++)
            {
                if(i<iNumFrames)
                {
                    pfTemp[i] = ppfAudioData[0][i];
                }
                else
                {
                    pfTemp[i] = 0;
                }
            }
            delete[] ppfAudioData[0];
            ppfAudioData[0] = pfTemp;
        }
        
        for(int i = 0; i<kHopSize; i++)
        {
            pfInputAudioData[i] = pfPrevAudioData[i];
            
        }
        
        for(int i = 0; i<kBlockSize-kHopSize; i++)
        {
            pfInputAudioData[i+kHopSize] = ppfAudioData[0][i];
        }

        
        for(int i = 0; i<kHopSize;i++)
        {
            pfPrevAudioData[i] = pfInputAudioData[kBlockSize-kHopSize+i];
        }


        //Processing Begins
        phAudioFingerPrint->process(pfInputAudioData,pfOutputAudioData,pfOldAudioData, kBlockSize);
//        for(int i = 0; i<33;i++)
//        {
//            cout<<pfOldAudioData[i]<<endl;
//        }
        for (int i = 0; i < 32; i++)
        {
            hOutputFile << pfOutputAudioData[i] << "\t";
        }
        hOutputFile<<endl;
    }

 
    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phAudioFile);
    CAudioFingerPrint::destroy(phAudioFingerPrint);
    hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete [] ppfAudioData[i];
    delete [] ppfAudioData;
    ppfAudioData = 0;
    
    delete [] pfInputAudioData;
    pfInputAudioData = 0;
    
    delete [] pfOutputAudioData;
    pfOutputAudioData = 0;
    
    delete [] pfOldAudioData;
    
    delete [] pfPrevAudioData;
    

    return 0;
    
}


void     showClInfo()
{
    cout << "Concert Stitching" << endl;
    cout << "(c) 2017 by Vinod Subramanian" << endl;
    cout  << endl;

    return;
}

