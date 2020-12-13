#include <cmath>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

#pragma pack(push,1)
struct TGA{
	uint8_t id_len;
	uint8_t pal_type;
	uint8_t img_type;
	uint8_t pal_desc[5];
	uint16_t x_pos;
	uint16_t y_pos;
	uint16_t width;
	uint16_t height;
	uint8_t depth;
	uint8_t img_desc;
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1920;
constexpr uint16_t IMG_HEIGHT = 1080;
constexpr uint32_t COL_BACKGROUND = 0xff003f3f;
constexpr uint32_t COL_FOREGROUND = 0xffffffff;

double sinc(double x){
	if(x == 0) return 1.;
	return sin(x)/x;
}

double my_evil_function(double x,double y){
	return sinc(hypot(x, y));
}

const double PI = acos(-1.0);
constexpr double RX = 20., RY = 20., RZ = 450.;
constexpr double XMAX = 30., XMIN = -30.;
constexpr double YMAX = 30., YMIN = -30.;
constexpr double STEP1 = 0.5, STEP2 = 0.01;

void HOR(std::vector<uint32_t>& pixels, int x, int y, uint32_t c, std::vector<int>& h){
	if (x < 0 || x >= IMG_WIDTH) return;
	if ( y > h[x]) return;

	h[x] = y;
	if(y < 0) return;

	if ((int)(y*IMG_WIDTH + x) <= (int)pixels.size()){
		pixels[y*IMG_WIDTH + x] = c;
	}
}

int main(){
	std::vector <uint32_t> picture(IMG_WIDTH*IMG_HEIGHT);

	int SX = 0,SY = 0;
	double SZ;

	std::vector<int> hor;
	hor.resize(IMG_WIDTH);

	for(auto && a:hor){
		a = IMG_HEIGHT;
	}

	for(auto && c:picture){
		c = COL_BACKGROUND;
	}

	TGA hdr {};
	hdr.img_type = 2;
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_desc = 0x28;

	for(double i = XMAX; i > XMIN; i -= STEP1){
		for(double j = YMAX; j > YMIN; j -= STEP2){
			SZ = my_evil_function(i, j);
			SX = int(IMG_WIDTH/2 - RX*i*cos(M_PI/6) + RY*j*cos(M_PI/6));
			SY = int(IMG_HEIGHT/2 + RX*i*sin(M_PI/6) + RY*j*sin(M_PI/6) - RZ*SZ);
			HOR(picture, SX, SY, COL_FOREGROUND, hor);
		}
	}

	for(auto && a:hor){
		a = IMG_HEIGHT;
	}

	for(double i = XMAX; i > XMIN; i -= STEP2){
		for(double j = YMAX; j > YMIN; j -= STEP1){
			SZ = my_evil_function(i, j);
			SX = int(IMG_WIDTH/2 - RX*i*cos(PI/6) + RY*j*cos(PI/6));
			SY = int(IMG_HEIGHT/2 + RX*i*sin(PI/6) + RY*j*sin(PI/6) - RZ*SZ);
			HOR(picture, SX, SY, COL_FOREGROUND, hor);
		}
	}

	std::ofstream tga_file { "output.tga", std::ios::binary };
	tga_file.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	tga_file.write(reinterpret_cast<char*>(&picture[0]),
			picture.size() * sizeof(uint32_t));
	tga_file.close();

	return 0;
}
