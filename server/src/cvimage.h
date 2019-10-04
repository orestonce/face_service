#ifndef CVIMAGE_H
#define CVIMAGE_H

#include <memory>
#include <string>

class CvImage
{
public:
    CvImage();
    ~CvImage();
    CvImage(const CvImage&) = delete;
    CvImage& operator=(const CvImage&) = delete;
    
    unsigned char *data;
    int x, y, channel;
    bool isAutoFree;
};

void LoadImageFromMemory(const std::string& origin, CvImage& image);

#endif // CVIMAGE_H

