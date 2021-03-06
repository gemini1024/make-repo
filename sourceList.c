// sourceList.c
// create by Choi Ji-Hun. 2016.11.22
// It is a function for em's main.c
// print Makefile's source files When call -s or --source option
// "--all" is print all source files
// else is print source files that belongs string

#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void printFileInfo(char *ptok, char *pdot);
char* checkFileAccess(unsigned int st_mode);
char selectRWXbyIndex(int index);

// -s option's main function
// param : findFile - belongs to the files
void printSourcefile(char *findFile) {
	FILE* fp;
	char buf[BUFSIZ];
	char *ptok, *pdot; // ptok : pointing token, pdot : pointing dot(.)
	int fileCnt=0;

	if((fp=fopen("Makefile", "r")) == NULL) {
		perror("fopen : Makefile");
		exit(1);
	}

	// Print header
	fprintf(stdout, "access\t\t uid\t\t mtime\t\t\t name\n");
	fprintf(stdout, "---------------------------------------------------------------------\n");

	while (fgets(buf, BUFSIZ, fp) != NULL) {

		// Check when .o file is exist in buf
		// (find target - ex.o: ex.c)
		if(strstr(buf, ".o") != NULL) {

			// Starting tokenize (for find .c file)
			ptok = strtok(buf, " \t:");
			while(ptok != NULL) {

				// Find .c file
				if((pdot = strstr(ptok, ".c\0")) != NULL) {

					// findFile == "--all" is print all sources
					// else is print find sources
					if(strcmp("--all", findFile) == 0 || strstr(ptok, findFile) != NULL) {
						printFileInfo(ptok, pdot);
						fileCnt++;
					}
				}

				// Next token
				ptok = strtok(NULL, " \t:");
			}
		}
	}

	// Print result. (not found or number of found files)
	if(fileCnt == 0) {
		fprintf(stdout, "Not found source file or OBJS\n");
	} else {
		fprintf(stdout, "---------------------------------------------------------------------\n");
		fprintf(stdout, "%d file(s)\n", fileCnt);
	}

	fclose(fp);
}



// Print file's Infomation
// param : ptok - pointing token include fileName
// param : pdot - pointing file's dot(.)
void printFileInfo(char *ptok, char *pdot) {
	char fileName[256];
	struct stat fileStat;
	struct passwd *pwd;
	struct tm *mtime;

	// ptok -> fileName
	memset(fileName, 0, sizeof(fileName));
	strncpy(fileName, ptok, pdot-ptok+2);

	// Get File Info
	if(access(fileName, 0) !=0 ) {
		fprintf(stdout, "Not Access file : %s\n", fileName);
		return;
	}
	stat(fileName, &fileStat);
	pwd = getpwuid((int)fileStat.st_uid); // uid
	mtime = localtime(&fileStat.st_mtime); // modified time

	// Print File Info (seq : access uid mtime fileName)
	fprintf(stdout, "%s\t %s\t %d.%d.%d %d:%d:%d\t %s\n", 
			checkFileAccess(fileStat.st_mode), pwd->pw_name,
			mtime->tm_year+1900, mtime->tm_mon+1, mtime->tm_mday,
			mtime->tm_hour, mtime->tm_min, mtime->tm_sec, fileName);
}


// Check file's access permission
// and return string
// param : st_mode - struct stat's st_mode
char* checkFileAccess(unsigned int st_mode) {
	static char permission[10];
	int index;
	
	for(index=0; index<9; index++) {
		if((st_mode & (S_IREAD >> index)) == 0)
			permission[index] = '-';
		else
			permission[index] = selectRWXbyIndex(index);
	}
	permission[index] = '\0';
	return permission;
}


// For checkFileAccess()
// index -> select 'r' or 'w' or 'x'
// param : index - file permission's index
char selectRWXbyIndex(int index) {
	switch(index % 3) {
		case 0 : return 'r';
		case 1 : return 'w';
		default : return 'x';
	}
}

