#pragma once

#include "ofConstants.h"
#ifndef TARGET_ANDROID
#include "ofConstants.h"
#include "ofBaseTypes.h"
#include "ofPixels.h"
#include "ofTypes.h"
#include "ofEvents.h"

#define GST_DISABLE_DEPRECATED
#include <gst/gstpad.h>

class ofGstAppSink;
typedef struct _GstElement GstElement;
typedef struct _GstBuffer GstBuffer;
typedef struct _GstMessage GstMessage;

//-------------------------------------------------
//----------------------------------------- ofGstUtils
//-------------------------------------------------

class ofGstUtils{
public:
	ofGstUtils();
	virtual ~ofGstUtils();

	bool 	setPipelineWithSink(string pipeline, string sinkname="sink", bool isStream=false);
	bool 	setPipelineWithSink(GstElement * pipeline, GstElement * sink, bool isStream=false);

	void 	play();
	void 	stop();
	void 	setPaused(bool bPause);
	bool 	isPaused(){return bPaused;}
	bool 	isLoaded(){return bLoaded;}
	bool 	isPlaying(){return bPlaying;}

	float	getPosition();
	float 	getSpeed();
	float 	getDuration();
	int64_t  getDurationNanos();
	bool  	getIsMovieDone();

	void 	setPosition(float pct);
	void 	setVolume(float volume);
	void 	setLoopState(ofLoopType state);
	ofLoopType	getLoopState(){return loopMode;}
	void 	setSpeed(float speed);

	void 	setFrameByFrame(bool bFrameByFrame);
	bool	isFrameByFrame();

	GstElement 	* getPipeline();
	GstElement 	* getSink();
	unsigned long getMinLatencyNanos();
	unsigned long getMaxLatencyNanos();

	virtual void close();

	void setSinkListener(ofGstAppSink * appsink);

	// callbacks to get called from gstreamer
	virtual GstFlowReturn preroll_cb(GstBuffer * buffer);
	virtual GstFlowReturn buffer_cb(GstBuffer * buffer);
	virtual void 		  eos_cb();

	static void startGstMainLoop();
protected:
	ofGstAppSink * 		appsink;
	bool				isStream;

private:
	void 				gstHandleMessage();
	void				update(ofEventArgs & args);
	bool				startPipeline();

	bool 				bPlaying;
	bool 				bPaused;
	bool				bIsMovieDone;
	bool 				bLoaded;
	bool 				bFrameByFrame;
	ofLoopType			loopMode;

	GstElement  *		gstSink;
	GstElement 	*		gstPipeline;

	float				speed;
	int64_t				durationNanos;
	bool				isAppSink;
};





//-------------------------------------------------
//----------------------------------------- videoUtils
//-------------------------------------------------

class ofGstVideoUtils: public ofBaseVideo, public ofGstUtils{
public:

	ofGstVideoUtils();
	virtual ~ofGstVideoUtils();

	bool 			setPipeline(string pipeline, int bpp=24, bool isStream=false, int w=-1, int h=-1);

	bool 			allocate(int w, int h, int bpp);

	bool 			isFrameNew();
	unsigned char * getPixels();
	ofPixelsRef		getPixelsRef();
	void 			update();

	float 			getHeight();
	float 			getWidth();

	void 			close();

	// this events happen in a different thread
	// do not use them for opengl stuff
	ofEvent<ofPixels> prerollEvent;
	ofEvent<ofPixels> bufferEvent;
	ofEvent<ofEventArgs> eosEvent;

protected:
	GstFlowReturn 	preroll_cb(GstBuffer * buffer);
	GstFlowReturn 	buffer_cb(GstBuffer * buffer);
	void			eos_cb();


	ofPixels		pixels;				// 24 bit: rgb
	ofPixels		backPixels;
	ofPixels		eventPixels;
private:
	bool			bIsFrameNew;			// if we are new
	bool			bHavePixelsChanged;
	bool			bBackPixelsChanged;
	ofMutex			mutex;
	GstBuffer * 	buffer, *prevBuffer;
};


//-------------------------------------------------
//----------------------------------------- appsink listener
//-------------------------------------------------

class ofGstAppSink{
public:
	virtual GstFlowReturn	on_preroll(GstBuffer * buffer){
		return GST_FLOW_OK;
	}
	virtual GstFlowReturn	on_buffer(GstBuffer * buffer){
		return GST_FLOW_OK;
	}
	virtual void			on_eos(){}

	// return true to set the message as attended so upstream doesn't try to process it
	virtual bool on_message(GstMessage* msg){return false;};

	// pings when enough data has arrived to be able to get sink properties
	virtual void on_stream_prepared(){};
};

#endif

