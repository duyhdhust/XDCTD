#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 100
#define MAX_STOP_WORDS 100
#define MAX_INDEX_WORDS 10000
#define MAX_LINE_LIST_LEN 8192

const char* INPUT_FILE = "vanban.txt";
const char* STOPW_FILE = "stopw.txt";

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
    char lineNumbers[MAX_LINE_LIST_LEN];
    int lastLine;
} IndexEntry;

IndexEntry indexTable[MAX_INDEX_WORDS];
int indexWordCount = 0;

char stopWords[MAX_STOP_WORDS][MAX_WORD_LEN];
int stopWordCount = 0;

void loadStopWords(const char* filename);
int isStopWord(const char* word);
void processTextFile(const char* filename);
void processWord(char* word, int lineNumber, int isProperNounCandidate, int isAfterSentenceEnd);
void addWordToIndex(char* word, int lineNumber);
int compareEntries(const void* a, const void* b);
void sortIndex();
void printIndex();

int main() {
    loadStopWords(STOPW_FILE);
    processTextFile(INPUT_FILE);
    sortIndex();
    
    printIndex();

    return 0;
}

void loadStopWords(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Loi: Khong mo duoc tep stopw.txt");
        return;
    }

    while (stopWordCount < MAX_STOP_WORDS && 
           fscanf(file, "%s", stopWords[stopWordCount]) == 1) {
        stopWordCount++;
    }
    fclose(file);
}

int isStopWord(const char* word) {
    for (int i = 0; i < stopWordCount; i++) {
        if (strcmp(word, stopWords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void processTextFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Loi: Khong mo duoc tep vanban.txt");
        return;
    }

    char currentWord[MAX_WORD_LEN];
    int wordIndex = 0;
    int c;
    int lineNumber = 1;

    int isAfterSentenceEnd = 1;
    int isProperNounCandidate = 0;

    while ((c = fgetc(file)) != EOF) {
        if (isalpha(c)) {
            if (wordIndex < MAX_WORD_LEN - 1) {
                if (wordIndex == 0) {
                    if (isupper(c)) {
                        isProperNounCandidate = 1; 
                    } else {
                        isProperNounCandidate = 0;
                    }
                }
                currentWord[wordIndex++] = tolower(c);
            }
        } 
        else {
            if (wordIndex > 0) {
                currentWord[wordIndex] = '\0';
                processWord(currentWord, lineNumber, isProperNounCandidate, isAfterSentenceEnd);
                wordIndex = 0;
            }

            if (c == '.' || c == '?' || c == '!') {
                isAfterSentenceEnd = 1; 
            } else if (c == '\n') {
                lineNumber++;
                isAfterSentenceEnd = 1;
            } else {
                isAfterSentenceEnd = 0;
            }
        }
    }

    if (wordIndex > 0) {
        currentWord[wordIndex] = '\0';
        processWord(currentWord, lineNumber, isProperNounCandidate, isAfterSentenceEnd);
    }
    fclose(file); 
}

void processWord(char* word, int lineNumber, int isProperNounCandidate, int isAfterSentenceEnd) {
    
    if (strlen(word) <= 1) {
        return; 
    }

    if (isProperNounCandidate && !isAfterSentenceEnd) {
        return;
    }

    if (isStopWord(word)) {
        return;
    }

    addWordToIndex(word, lineNumber);
}

void addWordToIndex(char* word, int lineNumber) {
    int foundIndex = -1;

    for (int i = 0; i < indexWordCount; i++) {
        if (strcmp(indexTable[i].word, word) == 0) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1) {
        indexTable[foundIndex].count++;
        
        if (indexTable[foundIndex].lastLine != lineNumber) {
            char lineStr[20];
            sprintf(lineStr, ",%d", lineNumber);
            
            if (strlen(indexTable[foundIndex].lineNumbers) + strlen(lineStr) < MAX_LINE_LIST_LEN) {
                strcat(indexTable[foundIndex].lineNumbers, lineStr); 
            }
            indexTable[foundIndex].lastLine = lineNumber;
        }
    } 
    else if (indexWordCount < MAX_INDEX_WORDS) {
        strcpy(indexTable[indexWordCount].word, word);
        indexTable[indexWordCount].count = 1;
        indexTable[indexWordCount].lastLine = lineNumber;
        
        sprintf(indexTable[indexWordCount].lineNumbers, "%d", lineNumber);
        
        indexWordCount++;
    }
}

int compareEntries(const void* a, const void* b) {
    IndexEntry* entryA = (IndexEntry*)a;
    IndexEntry* entryB = (IndexEntry*)b;
    return strcmp(entryA->word, entryB->word);
}

void sortIndex() {
    qsort(indexTable, indexWordCount, sizeof(IndexEntry), compareEntries);
}

void printIndex() {
    for (int i = 0; i < indexWordCount; i++) {
        printf("%s %d,%s\n", 
               indexTable[i].word, 
               indexTable[i].count, 
               indexTable[i].lineNumbers);
    }
}