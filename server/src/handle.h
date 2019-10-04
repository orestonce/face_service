#pragma once

#include <httplib.h>
#include <vector>
#include <sstream>
#include <seeta/Struct.h>
#include "cvimage.h"
#include "ajson.hpp"

struct KeyPoint
{
	int X, Y;
	KeyPoint()
	{
		X =0;
		Y =0;
	}
};

struct OneFaceRect
{
	int X, Y, W, H;
	std::vector<KeyPoint> KeyPointList;
	OneFaceRect()
	{
		X =0;
		Y =0;
		W =0;
		H =0;
	}
};
AJSON(KeyPoint, X, Y)
AJSON(OneFaceRect, X, Y, W, H, KeyPointList)

struct FaceSearchResult
{
	std::string Name;
	int X, Y;
	float Similarity;
	std::vector<KeyPoint> KeyPointList;
};
AJSON(FaceSearchResult, Name, X, Y, Similarity, KeyPointList)

void report_error(httplib::Response& resp, const char *str);
bool load_image(CvImage& image, const httplib::Request& req, httplib::Response& resp, const char *pname);
void handle_facedetectcnn(const httplib::Request& req, httplib::Response& resp);
void handle_facedetectsf(const httplib::Request& req, httplib::Response& resp);
void handle_facesearchsf(const httplib::Request& req, httplib::Response& resp);
void handle_facedetectdlib(const httplib::Request& req, httplib::Response& resp);
seeta::ImageData CvImage_to_seeta(const CvImage& image);

template<typename T>
void response_json(httplib::Response& resp,const T& result)
{
	std::ostringstream oss;
	ajson::save_to(oss, result);
	resp.set_content(oss.str().c_str(), "application/json");
}

