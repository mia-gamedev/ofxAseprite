#include "ofxAseprite.h"

// Creates a new ofxAseprite animation object.
// string jsonFile a path to an Aseprite JSON file
// string spriteSheetPath a path to an Aseprite image file 
ofxAseprite::ofxAseprite(const of::filesystem::path& jsonFilePath, const of::filesystem::path& spriteSheetPath)
    : m_isPaused(true)
    , m_tagName("none")
    , m_hasDifferentAnchor(false)
    , m_anchorpoint(0,0)
{
    ofFile json(jsonFilePath);
    if (!json.exists()) {
        ofLogError("ofxAseprite") << "JSON file does not exist: " << jsonFilePath;
        return;
    }
    m_asepriteData = ofLoadJson(json);
    m_image.loadImage(spriteSheetPath);

    _checkImageSizeAgainst_jsonData();
    m_image.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    _initializeFrames();
    _initializeTags();
}

// Internal: loads all of the frames
// Loads quads and frame duration data from the JSON.
// Called from constructor
void ofxAseprite::_initializeFrames()
{

    ofJson framesData = m_asepriteData["frames"];
    for(auto& frameData : framesData){
        Aseprite_Frame frame;
        if (frameData.contains("frame")) {
            frame.quad.x = frameData["frame"]["x"].get<int>();
            frame.quad.y = frameData["frame"]["y"].get<int>();
            frame.quad.w = frameData["frame"]["w"].get<int>();
            frame.quad.h = frameData["frame"]["h"].get<int>();
        }
        if (frameData.contains("spriteSourceSize")) {
            frame.off_x = frameData["spriteSourceSize"]["x"].get<int>();
            frame.off_y = frameData["spriteSourceSize"]["y"].get<int>();
        }
        else {
            frame.off_x = 0;
            frame.off_y = 0;
        }

        if (frameData.contains("duration")){
            frame.duration = frameData["duration"].get<int>();
        } 

        m_sourceWidth = frameData["sourceSize"]["w"].get<int>();
        m_sourceHeight = frameData["sourceSize"]["h"].get<int>();

        frame.isRotated = frameData["rotated"].get<bool>();
        frame.isTrimmed = frameData["trimmed"].get<bool>();
        m_frames.push_back(frame);
    }
}

// Internal: loads all of the animation tags
// called in constructor
void ofxAseprite::_initializeTags()
{
    ofJson tagsData = m_asepriteData["meta"]["frameTags"];
    for(auto& tagData : tagsData){
        Aseprite_FrameTag frameTag;

        if (tagData.contains("direction")) {
            frameTag.direction = tagData["direction"].get<std::string>();
        }

        if (tagData.contains("from"))
        {
            for (int i = tagData["from"].get<int>(); i <= tagData["to"].get<int>(); i++)
            {
                frameTag.frames.push_back(m_frames[i]);
            }
        }
        if (tagData.contains("repeat"))
        {
            frameTag.repeat = stoi(tagData["repeat"].get<std::string>());
        } 
        else 
        {
            frameTag.repeat = 0;
        }
        if (tagData.contains("name")) {
            m_frameTags[tagData["name"].get<std::string>()]=frameTag;
        }
    }
}

// Internal: checks that the loaded image size matches the metadata
// called in constructor
void ofxAseprite::_checkImageSizeAgainst_jsonData()
{
    int imageHeight = m_asepriteData["meta"]["size"]["h"].get<int>();
    int imageWidth = m_asepriteData["meta"]["size"]["w"].get<int>();
    assert((int)m_image.getHeight()==imageHeight,"Image height metadata doesn't match actual height of file");
    assert((int)m_image.getWidth()==imageWidth,"Image width metadata doesn't match actual width of file");
}

// Internal: handles the ping-pong animation type.
// Should only be called when we actually want to bounce.
// Swaps the direction.
void ofxAseprite::_pingpongBounce()
{
    //We need to increment/decrement frame index by 2 because
    //at this point we've already gone to the next frame
    if (m_direction == "forward")
    {
        m_direction = "reverse";
        m_frameIndex = m_frameIndex - 2;
    }
    else 
    {
        m_direction = "forward";
        m_frameIndex = m_frameIndex + 2;
    }
}

// Update the animation
// Should be called from ofApp.update and given ofElapsedTimef()
void ofxAseprite::update(float elapsedTime)
{
    if (m_isPaused)
    {
        return;
    }

    m_frameTimer=m_frameTimer-elapsedTime;
    if (m_frameTimer<=0)
    {
        nextFrame();
    }
    //ideally
    //frameTimer(self.frame.duration, callback)
    //frameTimer:update(elapsedTime);
}

// Move to the next frame.
// Internal: unless you want to skip frames, this generally will not ever
// need to be called manually.
void ofxAseprite::nextFrame()
{
    bool isForward = m_direction == "forward";

    if (isForward){
        m_frameIndex = m_frameIndex + 1;
    }
    else {
        m_frameIndex = m_frameIndex - 1;
    }
    //Do Looping
    bool isLooping = m_amountTagisRepeated == 0;
    bool outOfRange = (isForward && m_frameIndex >= m_tag.frames.size()) || (!isForward && m_frameIndex < 0);

    if (outOfRange)
    {
        //looping animation or has a fixed repeat amount
        if (isLooping || m_amountTagisRepeated > 1) {
            if (m_tag.direction == "pingpong")
            {
                _pingpongBounce();
            }
            else {
                if (isForward)
                {
                    m_frameIndex = 0;
                }
                else {
                    m_frameIndex = m_tag.frames.size() - 1;
                }
            }
            //reduce amounts it should repeat
            if (m_amountTagisRepeated > 1) {
                m_amountTagisRepeated = m_amountTagisRepeated - 1;
            }
        }
        //dont reset the animation pause at the last frame. We check for 1 because we are handling outOfRange
        //so its the last repeat
        else if (m_amountTagisRepeated == 1)
        {
            if (isForward)
            {
                m_frameIndex = m_tag.frames.size() - 1;
            }
            else {
                m_frameIndex = 0;
            }
            pause();
        }
    }

    //Get next frame
    m_frame = m_tag.frames[m_frameIndex];

    m_frameTimer = m_frame.duration*0.001;
}

// Draw the animation's current frame in a specified location with optional scale;
// \param float: x the x position.
// \param float: y the y position.
// \param float scale: optional scaleFactor
void ofxAseprite::draw(float x,float y,float scale)
{
    ofVec3f position(x, y, 0);
    draw(position, scale);
}
// Draw the animation's current frame in a specified location with optional scale;
// \param ofVec3f: vector position.
// \param float scale: optional scaleFactor
void ofxAseprite::draw(ofVec3f position, float scale)
{
    if (m_tagName == "none")
    {
        return;
    }
    /// \brief Draw an \p image with this renderer.
    /// \param image The image to draw with this renderer.
    /// \param x The x coordinate to draw cropped image at.
    /// \param y The y coordinate to draw cropped image at.
    /// \param w width of subsection to draw.
    /// \param h height of subsection to draw.
    /// \param sx x position in image to begin cropping from.
    /// \param sy y position in image to begin cropping from.
    /// \param sw source width of cropped area
    /// \param sh source height of cropped area
    float width = m_frame.quad.w * scale;
    float height = m_frame.quad.h * scale;
    ofPushMatrix();
    if (m_frame.isTrimmed || m_hasDifferentAnchor)
    {
        ofTranslate(m_frame.off_x * scale - m_anchorpoint.x * scale, m_frame.off_y * scale - m_anchorpoint.y * scale);
    }
    if (m_frame.isRotated)
    {
        ofRotate(270);
    }
    m_image.drawSubsection(position.x,position.y,position.z, width,height, m_frame.quad.x, m_frame.quad.y, m_frame.quad.w, m_frame.quad.h);
    ofPopMatrix();
}

// Draw the animation's current frame in a specified location with specified width and height;
// \param float x: the x position.
// \param float y: the y position.
// \param float width: the width of the frame to draw
// \param float height: the height of the frame to draw
void ofxAseprite::draw(float x, float y, float width, float height)
{
    if (m_tagName == "none")
    {
        return;
    }
    /// \brief Draw an \p image with this renderer.
    /// \param image The image to draw with this renderer.
    /// \param x The x coordinate to draw cropped image at.
    /// \param y The y coordinate to draw cropped image at.
    /// \param w width of subsection to draw.
    /// \param h height of subsection to draw.
    /// \param sx x position in image to begin cropping from.
    /// \param sy y position in image to begin cropping from.
    /// \param sw source width of cropped area
    /// \param sh source height of cropped area
    float scaleX = width / m_sourceWidth;
    float scaleY = height / m_sourceHeight;
    ofPushMatrix();
    if (m_frame.isTrimmed || m_hasDifferentAnchor)
    {
        ofTranslate(m_frame.off_x * scaleX - m_anchorpoint.x * scaleX, m_frame.off_y * scaleY - m_anchorpoint.y * scaleY);
    }
    if (m_frame.isRotated)
    {
        ofRotate(270);
    }
    m_image.drawSubsection(x, y, m_frame.quad.w* scaleX, m_frame.quad.h* scaleY, m_frame.quad.x, m_frame.quad.y, m_frame.quad.w, m_frame.quad.h);
    ofPopMatrix();
}

// Changes the drawing position specified from the normal top-left corner
// of the image to a position specified by xPct and yPct in
// relation to the dimensions of the image.
// Note: range of xPct and yPct is 0.0 to 1.0. For xPct, 1.0
// represents the width of the image. For yPct, 1.0 represents the
// height of the image. These values are not capped.
//
// \param xPct X position of the new anchor, specified as a percent of the width of the image.
// \param yPct Y position of the new anchor, specified as a percent of the height of the image.
void ofxAseprite::setAnchorPercent(float xPrc, float yPrc)
{
    m_hasDifferentAnchor = true;
    m_anchorpoint.x = m_sourceWidth * xPrc;
    m_anchorpoint.y = m_sourceHeight * yPrc;
}

// Changes the drawing position specified back to the normal top-left corner
void ofxAseprite::resetAnchorpoint()
{
    m_hasDifferentAnchor = false;
    m_anchorpoint.x = 0;
    m_anchorpoint.y = 0;
}
// Unpauses the animation
void ofxAseprite::play()
{
    m_isPaused=false;
}
// Pauses the animation
void ofxAseprite::pause()
{
    m_isPaused=true;
}
// Toggle between playing/paused
void ofxAseprite::togglePlay()
{
    if (m_isPaused){
        play();
    }
    else{
        pause();
    }
}
// Get the current frame of the current animation
int ofxAseprite::getFrame()
{
    return m_frameIndex;
}

// Switch to a different animation tag
// In the case that we're attempting to switch to the animation currently playing,
// nothing will happen.
// \param string tag: the animation tag name to switch to.
void ofxAseprite::setTag(std::string tag)
{
    if (!m_frameTags.count(tag)) {
        ofLogError("ofxAseprite") << "tag: "+ tag + " not found in frametags!";
        return;
    }
    if (m_tagName == tag)
    {
        return;
    }
    m_tagName = tag;
    m_tag = m_frameTags[m_tagName];
    m_direction = m_tag.direction;

    if (m_direction == "pingpong")
    {
        m_direction = "forward";
    }

    //set frame index initialsation for next frame
    if (m_direction=="forward")
    {
        m_frameIndex = -1;
    }
    else 
    {
        m_frameIndex = m_frameTags[m_tagName].frames.size();
    }
    m_amountTagisRepeated = m_frameTags[m_tagName].repeat;
    nextFrame();
}
// Jump to a particular frame index in the current animation.
// Errors if the frame is outside the tag's frame range.
// \param int frame: the frame index to jump to.
void ofxAseprite::setFrame(int frame)
{
    if (frame<0 || frame>=m_tag.frames.size())
    {
        ofLogError("ofxAseprite") << "specified frame is out of range for tag " + m_tagName;
    }
    
    m_frameIndex = frame;
    m_frame = m_tag.frames[m_frameIndex];
    m_frameTimer = m_frame.duration*0.001;
}

//overloaded constructor originally for ressource manager
ofxAseprite::ofxAseprite(ofJson data, ofImage image)
{
    m_asepriteData = data;
    m_image = image;

    _checkImageSizeAgainst_jsonData();
    m_image.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    _initializeFrames();
    _initializeTags();

    m_isPaused = true;
    m_tagName = "none";
}

