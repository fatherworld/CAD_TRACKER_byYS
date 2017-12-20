

int convertStretch(char * fileName, float * src, int width, int height, int channel, int showDim);

int DumpFloat(char * filename, char * buf, int mWidth, int mHeight);

int DrawLine(unsigned char * src, int width, int height, int x0, int y0, int x1, int y1, int shiness);

int convertStretch_dot(char * fileName, float * src, int width, int height, int channel, int showDim, void *ptr);


int drawAllPoint(void * ptr);