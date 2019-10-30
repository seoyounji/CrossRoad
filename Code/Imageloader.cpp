#include "Imageloader.h"

void InitializeImgLoader() {
	ilInit();
}

ILuint loadimage(string filename) {
	ILuint imagename;
	ilGenImages(1, &imagename);
	ilBindImage(imagename);
	ilEnable(IL_ORIGIN_SET);
	ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);
	ilLoadImage(filename.c_str());
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);

	cout << "filename : " << filename << endl;
	cout << "width : " << width << endl;
	cout << "height : " << height << endl;
	cout << "img_ptr : " << imagename << endl;
	return imagename;
}

void ImagePtr(ILuint imagename, void* ptr) {
	ilBindImage(imagename);
	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILubyte * imgptr = new ILubyte[width*height * 4];
	ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1
		, IL_RGBA, IL_UNSIGNED_BYTE, imgptr);
	ptr = imgptr;

}
ILubyte* ImagePtr(ILuint imagename) {
	ilBindImage(imagename);
	return ilGetData();
}

int WidthOfImg(ILuint imagename) {
	ilBindImage(imagename);
	return ilGetInteger(IL_IMAGE_WIDTH);
}
int HeightOfImg(ILuint imagename) {
	ilBindImage(imagename);
	return ilGetInteger(IL_IMAGE_HEIGHT);
}