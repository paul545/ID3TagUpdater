#ifndef thread_h
#define thread_h


#define IDV1_TAG_SIZE 128
#define IDV1_TEXT_SIZE 30
#define IDV1_STR_SIZE 31
#define INVALID_TRACK_NO 255

typedef struct {
	char title[IDV1_STR_SIZE],
		artist[IDV1_STR_SIZE],
		album[IDV1_STR_SIZE],
		comment[IDV1_STR_SIZE];
	int year;
	unsigned char genre, track;
} Id3v1Tag;

int loadId3v1Tag(const char *filename, Id3v1Tag *);
void printId3v1Tag(Id3v1Tag *);
void testFile(const char *filename);
int runTagReader(char* trackName);


#endif // thread_h
