#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHORT 3
#define LONG 4
#define RATIONAL 5

struct screen {   // image data entity
	uint8_t *dat;
	int w, l, datmax;
	uint8_t r, g, b;
} typedef screen;

void setPixel(screen *s, int x, int y);
void setPixelWithColor(screen *s, int x, int y, uint8_t r, uint8_t g, uint8_t b);

screen *sopen(int wid, int len);
void sclose(screen *s);

void write_IFD(uint16_t tag, uint16_t field, uint32_t vals, uint32_t offset, FILE *fp);
void writeHeader(FILE *fp, int w, int l);
void writeFile(char *name, screen *s);

