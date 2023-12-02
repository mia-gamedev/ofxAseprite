#pragma once

#include "ofMain.h"


#include <map>
#include <string>


typedef struct Aseprite_Quad
{
    int x;  //x Offset  
    int y;  //y Offset 
    int w;  //subsection width
    int h;  //subsection height
} Aseprite_Quad;

typedef struct Aseprite_Frame
{
    Aseprite_Quad quad;
    int duration;
    int off_x;
    int off_y;
    bool isTrimmed;
    bool isRotated;
} Aseprite_Frame;

typedef struct Aseprite_FrameTag
{
    vector<Aseprite_Frame> frames;
    std::string direction;    
    int repeat;
} Aseprite_FrameTag;

class ofxAseprite
{
private:
    ofJson m_asepriteData;
    ofImage m_image;

    std::map<std::string, Aseprite_FrameTag> m_frameTags;
    vector<Aseprite_Frame> m_frames;
    
    bool m_isPaused;
    std::string m_tagName;
    Aseprite_FrameTag m_tag;
    Aseprite_Frame m_frame;
    int m_frameIndex;
    std::string m_direction;
    float m_frameTimer;
    int m_amountTagisRepeated;
    int m_sourceWidth;
    int m_sourceHeight;
    bool m_hasDifferentAnchor;
    ofVec2f m_anchorpoint;
    //Internal: loads all of the frames
    //Loads quads and frame duration data from the JSON.
    //Called from constructor
    void _initializeFrames();

    //Internal: loads all of the animation tags
    //Called from constructor
    void _initializeTags();

    //Internal: checks that the loaded image size matches the metadata
    void _checkImageSizeAgainst_jsonData();

    void _pingpongBounce();
public:

    void update(float elapsedTime);
    void nextFrame();

    void draw(float x, float y, float scale=1);
    void draw(ofVec3f position, float scale=1);
    void draw(float x, float y, float width, float height);

    void setAnchorPercent(float xPrc,float yPrc);
    void resetAnchorpoint();

    void play();
    void pause();
    void togglePlay();

    int getFrame();
    //Switch to a different animation tag.
    void setTag(std::string tag);
    //Jump to a particular frame index (1-based indexes) in the current animation.
    void setFrame(int frame);

    //setCallback_onLoop(func, ...)
    //function callback_onLoop
    ofxAseprite(){};
    ofxAseprite(ofJson data, ofImage image);
    ofxAseprite(const of::filesystem::path& jsonFilePath, const of::filesystem::path& spriteSheetPath);
};