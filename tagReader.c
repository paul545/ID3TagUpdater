/* code originally from user baavgai on http://www.dreamincode.net/forums/topic/287707-read-mp3-tags-id3-v1-c/ 
 *code has been modified*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tagReader.h"
#include "GetWebpage1.h"




//genre codes: Metal = 09, Death Metal = 22

int runTagReader(char* trackName) {
	
	testFile(trackName);
	return 0;
}

void testFile(const char *filename) {
	Id3v1Tag tag;

	printf("Filename: %s\n", filename);
	if (loadId3v1Tag(filename, &tag)) {
		printId3v1Tag(&tag);
		getWebpage(&tag.artist);
	} else {
		printf("Load fail\n");
	}
	printf("---------\n");

}


void printId3v1Tag(Id3v1Tag *tag) {
	printf("Title: %s\n", tag->title);
	printf("Artist: %s\n", tag->artist);
	printf("Album: %s\n", tag->album);
	printf("Year: %d\n", tag->year);
	printf("Comment: %s\n", tag->comment);
	if (tag->track != INVALID_TRACK_NO) {
		printf("Track: %d\n", tag->track);
	}
	printf("Genre: %d\n", tag->genre);
}

static char *copyTagText(char *src, char *dst, int size) {
	int i, foundData = 0;
	dst[size] = 0;
	for(i=size-1; i>=0; i--) {
		char ch = src[i];
		if(!foundData) { foundData = (ch!=0 && !isspace(ch)); }
		dst[i] = foundData ? ch : 0;
	}
	return src + size;
}

static char *copyTagYear(char *src, int *num) {
	int i;
	char yearBuff[5];
	for(i=0; i<4; i++) { yearBuff[i] = *src++; }
	yearBuff[i] = 0;
	*num = atoi(yearBuff);
	return src;
}

int loadId3v1Tag(const char *filename, Id3v1Tag *tag) {
	int resultStatus = 0;
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		perror("File open failed");
	} else {
		if (fseek(fp, -1 * IDV1_TAG_SIZE, SEEK_END) == -1) {
			perror("fseek failed");
		} else {
			char buff[IDV1_TAG_SIZE];
			if (fread(buff, IDV1_TAG_SIZE, 1, fp) != 1) {
				printf("Failed reading tag\n");
			} else if (buff[0]!='T' || buff[1]!='A' || buff[2]!='G') {
				printf("Failed to find TAG\n");
			} else {
				char *p = buff + 3;
				p = copyTagText(p, tag->title, IDV1_TEXT_SIZE);
				p = copyTagText(p, tag->artist, IDV1_TEXT_SIZE);
				p = copyTagText(p, tag->album, IDV1_TEXT_SIZE);
				p = copyTagYear(p, &(tag->year));
				if (p[28]==0) {
					copyTagText(p, tag->album, 28);
					tag->track = p[29];
					p += IDV1_STR_SIZE;
				} else {
					p = copyTagText(p, tag->album, IDV1_TEXT_SIZE);
					tag->track = INVALID_TRACK_NO;
				}
				tag->genre = *p;
				resultStatus = 1;
			}
		}
		fclose(fp);
	}
	return resultStatus;
}


