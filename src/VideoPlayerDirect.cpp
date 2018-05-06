/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */



#include "VideoPlayerDirect.h"


VideoPlayerDirect::VideoPlayerDirect()
{
	directDecoder = NULL;
}

VideoPlayerDirect::~VideoPlayerDirect()
{
    close();
}

bool VideoPlayerDirect::open(StreamInfo& hints, OMXClock* av_clock, ofxOMXPlayerSettings& settings_)
{

	if (!av_clock)
	{
		return false;
	}

	if(ThreadHandle())
	{
		close();
	}
    
    
    settings = settings_;

	omxStreamInfo   = hints;
	omxClock        = av_clock;
	fps             = 25.0f;
	frameTime       = 0;
	currentPTS      = DVD_NOPTS_VALUE;
	doAbort         = false;
	doFlush         = false;
	cachedSize      = 0;
	speed           = DVD_PLAYSPEED_NORMAL;
	

	timeStampAdjustment = omxClock->getAbsoluteClock();

	if(!openDecoder())
	{
		close();
		return false;
	}

	Create();

	isOpen        = true;

	return true;
}


bool VideoPlayerDirect::openDecoder()
{

	if (omxStreamInfo.fpsrate && omxStreamInfo.fpsscale)
	{
		fps = DVD_TIME_BASE / OMXReader::normalizeFrameduration((double)DVD_TIME_BASE * omxStreamInfo.fpsscale / omxStreamInfo.fpsrate);
	}
	else
	{
		fps = 25;
	}

	if( fps > 100 || fps < 5 )
	{
		ofLog(OF_LOG_VERBOSE, "Invalid framerate %d, using forced 25fps and just trust timestamps\n", (int)fps); 
		fps = 25;
	}

	frameTime = (double)DVD_TIME_BASE / fps;

	directDecoder = new VideoDecoderDirect();
	
	decoder = (BaseVideoDecoder*)directDecoder;
	if(!directDecoder->open(omxStreamInfo, omxClock, settings))
	{

        delete directDecoder;
        directDecoder = NULL;
        decoder = NULL;
		return false;
	}
	
	stringstream info;
	info << "Video width: "	<<	omxStreamInfo.width         << "\n";
	info << "Video height: "	<<	omxStreamInfo.height    << "\n";
	info << "Video profile: "	<<	omxStreamInfo.profile   << "\n";
	info << "Video fps: "		<<	fps                     << "\n";
	ofLogVerbose(__func__) << "\n" << info.str();

	return true;
}

void VideoPlayerDirect::close()
{
	doAbort = true;
	doFlush = true;

	flush();

	if(ThreadHandle())
	{
		lock();
		pthread_cond_broadcast(&m_packet_cond);
		unlock();

		StopThread("VideoPlayerDirect");
	}
	
	if (directDecoder)
	{
		delete directDecoder;
		directDecoder = NULL;
	}
    decoder     = NULL;
    isOpen      = false;
	currentPTS  = DVD_NOPTS_VALUE;
	speed       = DVD_PLAYSPEED_NORMAL;
}

