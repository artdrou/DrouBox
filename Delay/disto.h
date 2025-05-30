// disto.h
#ifndef DISTO_H
#define DISTO_H

// Enum for distortion types
enum DistortionType
{
    HARD_CLIP,      // |....|
    TANH_CLIP,      // |...'|
    ARCTAN_CLIP,    // |..'.|
    LOG_CLIP,       // |..''|
    EXP_CLIP,       // |.'..|
    SIN_CLIP,       // |.'.'|
    HALF_WAVE,      // |.''.|
    FULL_WAVE,      // |.'''|
    BIT_CRUSH,      // |'...|
    TUBE_AMP,       // |'..'|
    NUM_DISTORTIONS // Always keep at end!
};

extern int currentTypeInt;
extern DistortionType currentTypeEnum;
extern DistortionType lastTypeEnum;

float computeMakeupGain(float drive, float k);

float hardClip(float inputSample, float drive, float level);
float tanhClip(float inputSample, float drive, float level);
float arctanClip(float inputSample, float drive, float level);
float logClip(float inputSample, float drive, float level);
float expClip(float inputSample, float drive, float level);
float sinClip(float inputSample, float drive, float level);
float halfWaveClip(float inputSample, float drive, float level);
float fullWaveReflection(float inputSample, float drive, float level);
float bitCrush(float inputSample, float drive, float level);
float tubeAmp(float inputSample, float drive, float level);

// Selector function
float applyDistortion(int type, float inputSample, float drive, float level);

float processDistortionFromDipsValue(int typeInt, float inputSample, float drive, float level);

// Distortion names
extern const char* distortionNames[NUM_DISTORTIONS];

DistortionType intToDistortionType(int input);

#endif // DISTO_H