#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// field types for tiff IFD
#define SHORT 	 3	// 16 bit unsigned int
#define LONG 	 4	// 32 bit unsigned int
#define RATIONAL 5	// two LONGs first being numerator and second being denominator of fraction

struct screen {   // image data entity
	// raw image data
	uint8_t *dat;

	// width, length, length of data stored in dat
	int w, l, datmax;

	// current color.
	uint8_t r, g, b;

} typedef screen;

// set pixel (x,y) to current color defined in the struct pointed to by s
void setPixel(screen *s, int x, int y) {
	int b = y*(s->w)*3 + x*3;
	s->dat[b]   = s->r;
	s->dat[b+1] = s->g;
	s->dat[b+2] = s->b;
}

// set pixel (x,y) to color rgb(r,g,b)
void setPixelWithColor(screen *s, int x, int y, uint8_t r, uint8_t g, uint8_t b) {
	int d = y*(s->w)*3 + x*3;
	s->dat[d] = r;
	s->dat[d+1] = g;
	s->dat[d+2] = b;
}

// create image data entity. return pointer to struct.
screen *sopen(int wid, int len) {
	screen *out = malloc(sizeof(screen));
	out->datmax = wid*len*sizeof(uint8_t)*3;
	out->dat = malloc(out->datmax);
	memset(out->dat,0,out->datmax);
	out->w = wid;
	out->l = len;
	out->r = out->g = out->b = 255;
	return out;
}

// free used memory and leave.
void sclose(screen *s) {
	free(s->dat);
	free(s);
}


// write Image File Directory
void write_IFD(uint16_t tag, uint16_t field, uint32_t vals, uint32_t offset, FILE *fp) {
	fwrite(&tag,2,1,fp);
	fwrite(&field,2,1,fp);
	fwrite(&vals,4,1,fp);
	fwrite(&offset,4,1,fp);
}

// write a tiff file header for an image with width (int) w and length (int) l in pixels.
void writeHeader(FILE *fp, int w, int l) {
	uint16_t bytes_2 = 42;
	uint32_t bytes_4 = 8;
	// header
	fwrite("II",1,2,fp);      // 0-1
	fwrite(&bytes_2,2,1,fp);  // 2-3
	fwrite(&bytes_4,4,1,fp);  // 4-7 offset of first ifd (8)
	
	bytes_2 = 12;
	fwrite(&bytes_2,2,1,fp);               // 8-9
	write_IFD(256,SHORT,1,(w),fp);         // IMAGE WIDTH
	write_IFD(257,SHORT,1,(l),fp);         // IMAGE LENGTH
	write_IFD(258,SHORT,3,(174),fp);       // BITS PER SAMPLE
	write_IFD(259,SHORT,1,(1),fp);         // COMPRESSION (None)
	write_IFD(262,SHORT,1,(2),fp);         // PHOTOMETRIC INTERPRETATION (black is zero)
	write_IFD(273,SHORT,1,(180),fp);       // STRIP OFFSET (offset of each strip)
	write_IFD(277,SHORT,1,(3),fp);         // SAMPLES PER PIXEL
	write_IFD(278,SHORT,1,(l),fp);         // ROWS PER STRIP
	write_IFD(279,LONG,1,((w)*(l)),fp);	   // STRIP BYTE COUNT
	write_IFD(282,RATIONAL,1,158,fp);      // X RESOLUTION
	write_IFD(283,RATIONAL,1,166,fp);      // Y RESOLUTION
	write_IFD(296,SHORT,1,(1),fp);         // RESOLUTION UNIT
	
	bytes_4 = 0;
	fwrite(&bytes_4,4,1,fp);
	
	bytes_4 = 1;
	fwrite(&bytes_4,4,1,fp);
	fwrite(&bytes_4,4,1,fp);
	fwrite(&bytes_4,4,1,fp);
	fwrite(&bytes_4,4,1,fp); // X and Y resolution
	// 162
	bytes_2 = 8;
	fwrite(&bytes_2,2,1,fp);
	fwrite(&bytes_2,2,1,fp);
	fwrite(&bytes_2,2,1,fp);
}

// write image data to file.
void writeFile(char *name, screen *s) {
	FILE *fp = fopen(name,"w");
	writeHeader(fp,s->w,s->l);

	fwrite(s->dat,(s->w) * (s->l) * 3,1,fp);
	
	fclose(fp);
}

