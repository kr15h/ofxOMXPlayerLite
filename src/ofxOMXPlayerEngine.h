#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ofxOMXPlayerSettings.h"
#include "ofxOMXPlayerListener.h"

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "OMXThread.h"
#include "OMXClock.h"
#include "VideoPlayerTextured.h"
#include "VideoPlayerDirect.h"
#include "OMXAudioPlayer.h"


class ofxOMXPlayerEngine: public OMXThread
{
public:
    ofxOMXPlayerEngine();
    ~ofxOMXPlayerEngine();
    
    bool setup(ofxOMXPlayerSettings& settings);
    
    void loadMovie();
    
    void play();
    void stop();
    
    
    float getDurationInSeconds();
    
    
    void setVolume(float volume); // 0..1
    float getVolume();
    
    bool setPaused(bool doPause);
    
    int getCurrentFrame();
    int getTotalNumFrames();
    
    
    int getHeight();
    int getWidth();
    
    bool isPaused();
    bool isPlaying();
    
    double getMediaTime();
    bool useHDMIForAudio;
    
    bool isTextureEnabled;
    
    void stepFrameForward();
    void stepFrame(int);
    void scrubForward(int);
    void increaseVolume();
    void decreaseVolume();
    
    void addListener(ofxOMXPlayerListener* listener_);
    void removeListener();
    

     
    StreamInfo videoStreamInfo;
    StreamInfo audioStreamInfo;
    
    
    int increaseSpeed();
    void rewind();
    void setNormalSpeed();
    int normalPlaySpeed;
    int speedMultiplier;
    
    bool doSeek;
    
    bool openPlayer(int startTimeInSeconds=0);
    
    EGLImageKHR eglImage;
    ofxOMXPlayerSettings omxPlayerSettings;
    void setDisplayRect(float x, float y, float w, float h);
    
    float getFPS();
    
        
    //OMXThread inheritance
    void process();
    
    
    VideoPlayerDirect* directPlayer;
    VideoPlayerTextured* texturedPlayer;
    BaseVideoPlayer* videoPlayer;
    
    void enableLooping();
    void disableLooping();
    bool isLoopingEnabled()
    {
        return doLooping;
    }
    bool doRestart;
private:
    
    
    OMXClock* clock;
    
    OMXPacket* packet;
    
    
    
    OMXAudioPlayer* audioPlayer;
    ofxOMXPlayerListener* listener;
    OMXReader omxReader;
    
    
    
    bool hasVideo;
    bool hasAudio;
    
    
    bool didVideoOpen;
    bool didAudioOpen;
    bool doLooping;
    int videoWidth;
    int videoHeight;
    
    string moviePath;
    int nFrames;
    bool bPlaying;
    
    double loop_offset;
    double startpts;
    int loopCounter;
    
    float duration;
    
    void onVideoEnd();
    void onVideoLoop();
    double previousLoopOffset;
    
    ofRectangle displayRect;
    
    void setDisplayRect(ofRectangle& rectangle);
    bool didReadFile(bool doSkipAvProbe);
    int startFrame;
    
};

