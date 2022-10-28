#include <iostream>
#include <cstdlib>
#include "main.h"

using namespace std;

/* GLOBAL VARIABLES */
unsigned long long int noReads = 0; // number of reads from the disk
unsigned long long int noWrites = 0; // number of writes to the disk
unsigned long long int noPhases = 0; // number of phases of splitting the input tape into output tapes
unsigned long long int noMerges = 0; // number of merges of the input tapes into output tape
bool verbose = false;
char file[100] = "input.txt";

void initiateProgram() {
    remove(file);
    remove("tapeA.txt");
    remove("tapeB.txt");
}

void generateRecords(int n) {
    double x, y;

    auto *tape = new Tape(new File(file), verbose);
    Record *record = nullptr;

    srand(time(nullptr));

//    cout << "Number of records to generate: " << endl;
//    cin >> n;

    while (n--) tape->writeRecord(new Record(rand()%100, n));
    tape->writeRecord(nullptr);

//    Record *r;
//    cout << "---------------------------" <<endl;
//    while (r = tape->getRecord()) cout << r->x << " " << r->y << endl;
//    cout << "---------------------------" <<endl;
//    while (r = tape->getRecord()) cout << r->x << " " << r->y << endl;

    delete tape;
}

void getUserRecords() {
    double x, y;
    int n;

    auto *tape = new Tape(new File(file), verbose);
    Record *record = nullptr;

    cout << "Number of records: " << endl;
    cin >> n;

    while (n--) {
        cout << "Y : " << endl;
        cin >> x;
        cout << "X : " << endl;
        cin >> y;
        tape->writeRecord(new Record(x, y));
    }
    tape->writeRecord(nullptr);

    delete tape;
}

void changePath() {
    cout<< "File file: \n" << endl;
    cin >> file;
}

void printMenu() {

    int mode;
    char v;
    cout << "To enable verbose mode press v, press any other key to continue." << endl;
    cin >> v;
    if (v == 'v') verbose = true;

    cout << "Press: \n\t1 to generate random record \n\t2 to input records \n\t3 to import data from the file\n";
    cin >> mode;

    switch (mode) {
        case 1:
            generateRecords(5);
            break;
        case 2:
            getUserRecords();
            break;
        case 3:
            changePath();
            break;
        default:
            break;
    }
}

void getStats(int n) {
    int j = 16;
    remove("stats.txt");
    for (int i = 0; i < n; i++) {
        initiateProgram();
        generateRecords(j);

        do {
            splitTape();
        } while (mergeTapes());

        writeSorted();
        writeStats(j);
        j *= 2;
    }
}

void splitTape() {
    noPhases++;

    auto *inputTape = new Tape(new File(file));
    auto *tapeA = new Tape(new File("tapeA.txt"));
    auto *tapeB = new Tape(new File("tapeB.txt"));
    bool tape = true;

    Record *record = inputTape->getRecord();
    double tmp = record->x;
    while (record) {
        if(record->x < tmp) tape = not tape;

        if(tape) tapeA->writeRecord(record);
        else tapeB->writeRecord(record);

        tmp = record->x;
        record = inputTape->getRecord();
    }
    // finish writing to the tape
    tapeA->writeRecord(nullptr);
    tapeB->writeRecord(nullptr);

    delete inputTape;
    delete tapeA;
    delete tapeB;
    remove(file);
}

bool mergeTapes() {
    noMerges++;

    auto *outputTape = new Tape(new File(file), verbose);
    auto *tapeA = new Tape(new File("tapeA.txt"));
    auto *tapeB = new Tape(new File("tapeB.txt"));

    double tmp1, tmp2;
    Record *recordA = tapeA->getRecord(), *recordB = tapeB->getRecord();

    // file is sorted
    if(!recordB) return false;

    while (true) {
        // write remaining records (either tapeA or tapeB is empty)
        if (!recordA) {
            while (recordB) {
                outputTape->writeRecord(recordB);
                recordB = tapeB->getRecord();
            }
            break;
        } else if (!recordB) {
            while (recordA) {
                outputTape->writeRecord(recordA);
                recordA = tapeA->getRecord();
            }
            break;
        }
            // neither tape is empty
        else {
            if (recordA->x < tmp1) {
                while (recordB && recordB->x > tmp2) {
                    outputTape->writeRecord(recordB);
                    tmp2 = recordB->x;
                    recordB = tapeB->getRecord();
                }
                tmp1 = tmp2 = 0;
            } else if (recordB->x < tmp2) {
                while (recordA && recordA->x > tmp1) {
                    outputTape->writeRecord(recordA);
                    tmp1 = recordA->x;
                    recordA = tapeA->getRecord();
                }
                tmp1 = tmp2 = 0;
            } else {
                if (recordA->x < recordB->x) {
                    outputTape->writeRecord(recordA);
                    tmp1 = recordA->x;
                    recordA = tapeA->getRecord();
                } else {
                    outputTape->writeRecord(recordB);
                    tmp2 = recordB->x;
                    recordB = tapeB->getRecord();
                }
            }
        }
    }


    outputTape->writeRecord(nullptr);

    delete outputTape;
    delete tapeA;
    delete tapeB;

    remove("tapeA.txt");
    remove("tapeB.txt");

    return true;
}

void writeSorted() {
    auto *input = new Tape(new File("tapeA.txt"));
    auto *output = new Tape(new File(file));
    Record *record;

    cout << "Sorted file: " << endl;

    record = input->getRecord();
    while (record) {
        output->writeRecord(record);
        record = input->getRecord();
    }
    output->writeRecord(nullptr);

    delete input;
    delete output;

    remove("tapeA.txt");
    remove("tapeB.txt");
}

void printStats() {
    cout << "STATISTICS" << endl;
    cout << "Number of reads from the disk: \t" << noReads << endl;
    cout << "Number of writes to the disk: \t" << noWrites << endl;
    cout << "Number of phases of splitting the input tape into output tapes: \t" << noPhases << endl;
    cout << "Number of merges of the input tapes into output tape: \t" << noMerges << endl;
}

void writeStats(int i) {
    ofstream ofs("stats.txt", ios::out|ios::app);
    ofs << i << ";" << noReads << ";" << noWrites << ";" << noPhases << ";" << noMerges << "\n";
    ofs.close();
}



