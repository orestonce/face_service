#define STB_IMAGE_IMPLEMENTATION

#include "cvimage.h"
#include "stb_image.h"

unsigned char *load_image_cv_format(const std::string& origin, int* x, int* y, int* channel)
{
    unsigned char *ptr =stbi_load_from_memory((const stbi_uc*)origin.data(), origin.length(), x, y, channel, 0);
    if (ptr != NULL && (*channel) >= 3)
    {
        int max = (*x)*(*y)*(*channel);
        for (int idx =0; idx < max; idx += (*channel) )
        {
            int tmp= ptr[idx];
            ptr[idx] = ptr[idx+2];
            ptr[idx+2] = tmp;
        }

    }
    return ptr;
}

void LoadImageFromMemory(const std::string &origin, CvImage& image)
{
    int x, y, channel;
    unsigned char *ptr =stbi_load_from_memory((const stbi_uc*)origin.data(), origin.length(), &x, &y, &channel, 0);
    if (ptr != NULL && channel >= 3)
    {
        int max = x*y*channel;
        for (int idx =0; idx < max; idx += channel )
        {
            int tmp= ptr[idx];
            ptr[idx] = ptr[idx+2];
            ptr[idx+2] = tmp;
        }
    }
    image.data = ptr;
    image.x = x;
    image.y = y;
    image.channel = channel;
    image.isAutoFree = (ptr == NULL);
}


CvImage::CvImage()
{
    this->x =0;
    this->y = 0;
    this->data =NULL;
    this->channel =0;
    this->isAutoFree = false;
}

CvImage::~CvImage()
{
    if (this->isAutoFree)
    {
    	free(data);
    }
}

