#include "handle.h"
#include "autofree.h"
#include "facedetectcnn.h"
#include <iostream>

void handle_facedetectcnn(const httplib::Request& req, httplib::Response& resp)
{
	//load an image and convert it to gray (single-channel)
	CvImage image;
	if(!load_image(image, req, resp, "image_content"))
	{
		return;
	}
	//define the buffer size. Do not change the size!
	const int DETECT_BUFFER_SIZE = 0x20000;

	int * pResults = NULL; 
	//pBuffer is used in the detection functions.
	//If you call functions in multiple threads, please create one buffer for each thread!
	AutoFreeVoidPtr pBuffer;
	pBuffer.Create(DETECT_BUFFER_SIZE);

	///////////////////////////////////////////
	// CNN face detection 
	// Best detection rate
	//////////////////////////////////////////
	//!!! The input image must be a BGR one (three-channel) instead of RGB
	//!!! DO NOT RELEASE pResults !!!
	pResults = facedetect_cnn((unsigned char *)pBuffer.data, (unsigned char*)(image.data), image.x, image.y, (int)image.x * image.channel);
	
	std::vector<OneFaceRect> resultList;
	//print the detection results
	for(int i = 0; i < (pResults ? *pResults : 0); i++)
	{
		short * p = ((short*)(pResults+1))+142*i;
		int x = p[0];
		int y = p[1];
		int w = p[2];
		int h = p[3];
		int confidence = p[4];
		if (confidence <80)
		{
			continue;
		}
		//int angle = p[5];
		OneFaceRect face;
		face.X = x;
		face.Y = y;
		face.W = w;
		face.H = h;
		resultList.push_back(face);
	}
	response_json(resp, resultList);
}

