#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Add functions here

int TXTLineExtracter() {
	int n;
	printf("Number of lines to extract (should match the number of headings for each entry): ");
	scanf("%i", &n);
	char temp;
	scanf("%c", &temp);           // Eats the new line left over from above
	char headings[n][100];
	for (int i = 0; i < n; i++) {
		printf("Enter heading of the line %i of %i you want extracted: ", i+1, n);
		char *p = &headings[i][0];
		fgets(p, 100, stdin);
	}
	printf("File path to input file to be read: ");
	char inFilePath[256];
	scanf("%s", inFilePath);
	FILE *fdIn = fopen(inFilePath, "r");
	if (fdIn == NULL) {
		return -1;
	}
	printf("File path to output file to be overwritten or created: ");
	char outFilePath[256];
	scanf("%s", outFilePath);
	FILE *fdOut = fopen(outFilePath, "w+");
	if (fdOut == NULL) {
		return -1;
	}
	char mode;
	include_headers:
	printf("Include headers in output? Y/N ");
	scanf("%c", &temp);           // Eats the new line left over from above
	scanf("%c", &mode);
	char headerbuffer[103*n];
	int c = 0;
	if (mode == 'Y') {
		for (int i = 0; i < n; i++) {
			headerbuffer[c++] = '"';
			int j = 0;
			while(headings[i][j] != 0) {
				headerbuffer[c++] = headings[i][j++];
			}
			c--;
			headerbuffer[c++] = '"';
			headerbuffer[c++] = ',';
		}
		fwrite(headerbuffer, 1, c, fdOut);
		// printf("%s", headerbuffer);          // Debug
	} else if (mode == 'N') {
		// Do nothing
	} else {
		printf("Unable to read your header selection\n");
		goto include_headers;
	}
	char contentInBuffer[8192], contentOutBuffer[8192];
	int i = 0;
	while(!feof(fdIn)) {
		int inChar = 0, outChar = 0;
		fgets(contentInBuffer, 8192, fdIn);
		// printf("%s", contentInBuffer);          // Debug
		if (contentInBuffer[0] == '\n') {
			// printf("----- Blank Line Detected -----");          // Debug
			if (i == n) {
				fwrite("\"", 1, 1, fdOut);
				i = 0;
			}
			continue;
		}
		int j = 0;
		char headingMatch = 'T';
		while(headings[i][j] != '\n') {
			// printf("%c == %c\n", contentInBuffer[inChar], headings[i][j]);          // Debug
			if (contentInBuffer[inChar++] != headings[i][j++]) {
				inChar = 0;
				headingMatch = 'F';
				break;
			}
		}
		// printf("%c == %c\n", ':', contentInBuffer[inChar]);          // Debug
		if (headingMatch == 'T' && contentInBuffer[inChar++] != ':') {
			inChar = 0;
			headingMatch = 'F';
		}
		// printf("%c == %c\n", ' ', contentInBuffer[inChar]);          // Debug
		if (headingMatch == 'T' && contentInBuffer[inChar++] != ' ') {
			inChar = 0;
			headingMatch = 'F';
		}
		if (headingMatch == 'F') {
			// skips line in the text file
			// printf("----- No Match -----\n");          // Debug
			continue;
			// contentOutBuffer[outChar++] = '\n';          // Old
		} else {
			// write to new cell
			// printf("----- Match -----\n");          // Debug
			if (mode == 'Y') {
				mode = 'N';
			} else if (i == 0) {
				contentOutBuffer[outChar++] = '\n';
			}
			contentOutBuffer[outChar++] = '"';
			if (i > 0) {
				contentOutBuffer[outChar++] = ',';
				contentOutBuffer[outChar++] = '"';
			}
			i++;
		}
		while(inChar < 8192 && contentInBuffer[inChar] != '\n') {
			contentOutBuffer[outChar++] = contentInBuffer[inChar++];
		}
		// printf("%s", contentOutBuffer);          // Debug
		fwrite(contentOutBuffer, 1, outChar, fdOut);
	}
	fclose(fdIn);
	fclose(fdOut);
}

int main () {
	TXTLineExtracter();
}
