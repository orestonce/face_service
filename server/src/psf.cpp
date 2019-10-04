#include "handle.h"
#include "FaceEngine.h"
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <map>
#include <iostream>

const char* getPtsFilename(const httplib::Request& req)
{
	const char* ptsFilename = "/data/model/SeetaFace2/pd_2_00_pts81.dat";
	if (req.get_param_value("use_pts") == "5")
	{
		ptsFilename = "/data/model/SeetaFace2/pd_2_00_pts5.dat";
	}
	return ptsFilename;
}

void handle_facedetectsf(const httplib::Request& req, httplib::Response& resp)
{
	int id = 0;
	seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
	seeta::ModelSetting FD_model( "/data/model/SeetaFace2/fd_2_00.dat", device, id );
	seeta::ModelSetting FL_model( getPtsFilename(req), device, id );
	seeta::FaceDetector FD(FD_model);
	seeta::FaceLandmarker FL(FL_model);
	FD.set(seeta::FaceDetector::PROPERTY_VIDEO_STABLE, 1);	
	
	CvImage frame;
	if (!load_image(frame, req, resp, "image_content"))
	{
		return ;
	}

	seeta::ImageData simage = CvImage_to_seeta(frame);
	auto faces = FD.detect(simage);
	std::vector<OneFaceRect> resultList;

	for (int i = 0; i < faces.size; ++i)
	{
		auto &faceIn = faces.data[i];
		OneFaceRect face;
		face.X = faceIn.pos.x;
		face.Y = faceIn.pos.y;
		face.W = faceIn.pos.width;
		face.H = faceIn.pos.height;
		//face.Confidence = confidence;
		auto points = FL.mark(simage, faceIn.pos);
		
		// cv::rectangle(frame, cv::Rect(face.pos.x, face.pos.y, face.pos.width, face.pos.height), CV_RGB(128, 128, 255), 3);
		for (auto &point : points)
		{
			//cv::circle(frame, cv::Point(point.x, point.y), 3, CV_RGB(128, 255, 128), -1);
			KeyPoint kp;
			kp.X = point.x;
			kp.Y = point.y;
			face.KeyPointList.push_back(kp);
		}
		resultList.push_back(face);
	}
	response_json(resp, resultList);
}

bool load_image_idname_map(const httplib::Request& req, httplib::Response& resp, seeta::FaceEngine& engine, std::map<int64_t, std::string>& idname_map);

void handle_facesearchsf(const httplib::Request& req, httplib::Response& resp)
{
	seeta::ModelSetting::Device device = seeta::ModelSetting::CPU;
	int id = 0;
	seeta::ModelSetting FD_model( "/data/model/SeetaFace2/fd_2_00.dat", device, id );
	seeta::ModelSetting PD_model( getPtsFilename(req), device, id );
	seeta::ModelSetting FR_model( "/data/model/SeetaFace2/fr_2_10.dat", device, id );
	seeta::FaceEngine engine( FD_model, PD_model, FR_model, 2, 16 );

	// recognization threshold
	float threshold = 0.7f;

	//set face detector's min face size
	engine.FD.set( seeta::FaceDetector::PROPERTY_MIN_FACE_SIZE, 80 );
	
	std::map<int64_t, std::string> GalleryIndexMap;
	if (!load_image_idname_map(req, resp, engine, GalleryIndexMap))
	{
		return ;
	}
	
	CvImage frame;
	if (!load_image(frame, req, resp, "image_content_origin")) 
	{
		return ;
	}
	seeta::ImageData image = CvImage_to_seeta(frame);

	// Detect all faces
	std::vector<SeetaFaceInfo> faces = engine.DetectFaces( image );
	
	std::vector<FaceSearchResult> resultList;

	for( SeetaFaceInfo &face : faces )
	{
		FaceSearchResult result;
		// Query top 1
		int64_t index = -1; 
		float similarity = 0;

		auto points = engine.DetectPoints(image, face);
	
		auto queried = engine.QueryTop( image, points.data(), 1, &index, &similarity );

		// cv::rectangle( frame, cv::Rect( face.pos.x, face.pos.y, face.pos.width, face.pos.height ), CV_RGB( 128, 128, 255 ), 3 );
		for (int i = 0; i < 5; ++i)
		{   
			auto &point = points[i];
			// cv::circle(frame, cv::Point(int(point.x), int(point.y)), 2, CV_RGB(128, 255, 128), -1);
			KeyPoint kp;
			kp.X = point.x;
			kp.Y = point.y;
			result.KeyPointList.push_back(kp);
		}
		result.Similarity = similarity;

		// no face queried from database
		if (queried < 1) continue;
			
		// similarity greater than threshold, means recognized
		if( similarity > threshold )
		{
			// cv::putText( frame, GalleryIndexMap[index], cv::Point( face.pos.x, face.pos.y - 5 ), 3, 1, CV_RGB( 255, 128, 128 ) );
			// printf("found face : %s (%d, %d)\n", GalleryIndexMap[index].c_str(), face.pos.x, face.pos.y );
			result.Name = GalleryIndexMap[index];
			result.X = face.pos.x;
			result.Y = face.pos.y;
			resultList.push_back(result);
		}
	}
	response_json(resp, resultList);
}

bool load_image_idname_map(const httplib::Request& req, httplib::Response& resp, seeta::FaceEngine& engine, std::map<int64_t, std::string>& idname_map)
{
	int imageNum = atoi(req.get_param_value("face_count").c_str());
	std::vector<std::string> GalleryImageFilename; // = { "1.jpg" };
	for (int i =0; i < imageNum; i++)
	{
		char name[100] = "";
		sprintf(name, "face_name_%d", i);
		GalleryImageFilename.push_back(req.get_param_value(name));
	}
	std::vector<int64_t> GalleryIndex( GalleryImageFilename.size() );
	for( size_t i = 0; i < GalleryImageFilename.size(); ++i )
	{
		//register face into facedatabase
		std::string &filename = GalleryImageFilename[i];
		int64_t &index = GalleryIndex[i];
		CvImage inImage;
		char name[100] = "";
		sprintf(name, "face_content_%d", (int)i);
		if (!load_image(inImage, req, resp, name))
		{
			return false;
		}
		seeta::ImageData image = CvImage_to_seeta(inImage);
		auto id = engine.Register( image );
		std::cerr << "Registering... " << filename << " " << id << std::endl;
		if (id < 0) 
		{
			report_error(resp, "Can't register image");
			return false;
		}
		index = id;
	}
	for( size_t i = 0; i < GalleryIndex.size(); ++i )
	{
		// save index and name pair
		idname_map.insert( std::make_pair( GalleryIndex[i], GalleryImageFilename[i] ) );
	}
	return true;
}

bool load_image(CvImage& image,const httplib::Request& req, httplib::Response& resp, const char *pname)
{
	std::string file_content = req.get_param_value(pname);
	
	LoadImageFromMemory(file_content, image);
	if (image.data == NULL) {
		report_error(resp, "load image error");
		return false;
	}
	return true;
}

void report_error(httplib::Response& resp, const char *str)
{
	resp.status = 500;
	resp.set_content(str, "text/plain");
}

seeta::ImageData CvImage_to_seeta(const CvImage& image)
{
	seeta::ImageData after;
	after.width = image.x;
	after.height = image.y;
	after.channels = image.channel;
	after.data = image.data;
	return after;
}

