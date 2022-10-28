#ifndef MAIN_H
#define MAIN_H

#include<iostream>
#include <fstream>

using namespace std;

/* DIRECTIVES */
#define BUFFER_SIZE 512 // size of the file buffer

extern unsigned long long int noReads;
extern unsigned long long int noWrites;
extern unsigned long long int noPhases;
extern unsigned long long int noMerges;

/* STRUCTURE DESCRIBING A SINGLE RECORD */
struct Record {
    double x;
    double y;

    // default constructor
    Record() = default;

    // overloaded constructor
    Record(double x, double y) {
        this->x = x;
        this->y = y;
    }
};

/* STRUCTURE DESCRIBING THE FILE */
struct File {
    char *fileName;
    int fileReference;  // last read record

    // constructor
    File(char *name) {
        this->fileName = name;
        this->fileReference = 0;
    }

    void removeFile() const {
        remove(fileName);
    }
};

/* STRUCTURE DESCRIBING TAPE */
struct Tape {
    Record *buffer;     // pointer to the array of the buffer
    int next;           // index of the next record to be read/write
    bool print;         // whether to print progress
    File *file;         // corresponding file

    // constructor
    explicit Tape(File *file, bool print = false) {
        this->buffer = static_cast<Record *>(malloc(sizeof(Record) * BUFFER_SIZE));
        this->next = 0;
        this->print = print;
        this->file = file;
    }

    ~Tape() {
        free(buffer);
    }

    void clearBuffer() {
        free(buffer);
        buffer = static_cast<Record *>(malloc(sizeof(Record) * BUFFER_SIZE));
        next = 0;
    }

    Record *getRecord() {
        if(next == 0 || next == BUFFER_SIZE) {
            noReads++;
            // temporary variables
            double tmp1, tmp2;

            // try to open file
            ifstream ifs(file->fileName, ios::in);
            if(!ifs.good()) {
                cerr<<"Could not open the file: "<< file->fileName << endl;
            }

            // start reading after last read record
            ifs.seekg(file->fileReference, ios::beg);

            for(int i = 0; i < BUFFER_SIZE; i++) {
                if(ifs >> tmp1 >> tmp2) {
                    // get next record and add to the buffer
                    buffer[i] = *new Record(tmp1, tmp2);
                    if(print) cout << tmp1 << " - " << tmp2 << endl;
                } else {
                    buffer[i] = *new Record(-1, -1);
                }
            }
            next = 0;
            // mark last read record
            file->fileReference = ifs.tellg();
            ifs.close();
        }
        if(buffer[next].x == -1) {
            clearBuffer();
            file->fileReference = 0;
            return nullptr;
        }

        else return &buffer[(next++)];
    }

    void writeRecord (Record * record) {

        // write to buffer
        if (record) buffer[next++] = *record;

        // buffer is full, write records to file
        if(next == BUFFER_SIZE || record == nullptr) {
            noWrites++;

            // open file in write/append mode
            ofstream ofs(file->fileName, ios::out|ios::app);

            // in verbose mode, print records to be written
            if(print) cout << "Records to be written: " <<endl;

            for(int i = 0; i < next; i++) {
                ofs << buffer[i].x << " " << buffer[i].y << endl;
                if(print) cout << buffer[i].x << " " << buffer[i].y << endl;
            }

            clearBuffer();
            ofs.close();
        }
    }
};

void initiateProgram();
void generateRecords(int i);
void getUserRecords();
void changePath();
void printMenu();
void getStats(int i);
void splitTape();
bool mergeTapes();
void writeSorted();
void printStats();
void writeStats(int i);

#endif /* MAIN_H */
