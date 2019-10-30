#pragma once
#include <string>
#include <iostream>
#include <IL/devil_internal_exports.h>


using namespace std;

void InitializeImgLoader();
ILuint loadimage(string filename);
void ImagePtr(ILuint imagename, void* ptr);
ILubyte* ImagePtr(ILuint imagename);
int WidthOfImg(ILuint imagename);
int HeightOfImg(ILuint imagename);