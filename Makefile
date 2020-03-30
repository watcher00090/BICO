.SECONDEXPANSION:

DEBUG_OPT=-g

BUILD_DIR=build
TEST_DIR=test

C++_STD=11
CC=clang++

vpath %.o $(BUILD_DIR)
vpath % $(TEST_DIR)

.PHONY:
all: SkDataStream.o \
	SkDataStreamTest \
   	UCICovertypeDataStream.o \
	UCICovertypeDataStreamTest.o \
	KMeanspp.o \
	BICO_lite.o \
	BICO_liteTest.o \
	KMeansppTest.o \
	UCICovertypeDataStreamTest \
	KMeansppTest \
	BICO_liteTest

.PHONY:
clean:
	rm -rf $(BUILD_DIR)/* $(TEST_DIR)/*
	make all

UCICovertypeDataStream.o: UCICovertypeDataStream.h UCICovertypeDataStream.cpp
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c UCICovertypeDataStream.cpp -o $(BUILD_DIR)/UCICovertypeDataStream.o

UCICovertypeDataStreamTest.o: UCICovertypeDataStreamTest.cpp
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c UCICovertypeDataStreamTest.cpp -o $(BUILD_DIR)/UCICovertypeDataStreamTest.o

BICO_lite.o: BICO_lite.h BICO_lite.cpp Matrix.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c BICO_lite.cpp -o $(BUILD_DIR)/BICO_lite.o

KMeanspp.o: KMeanspp.cpp KMeanspp.h Matrix.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c KMeanspp.cpp -o $(BUILD_DIR)/KMeanspp.o

BICO_liteTest.o: BICO_liteTest.cpp BICO_lite.h UCICovertypeDataStream.h SkDataStream.h KMeanspp.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c BICO_liteTest.cpp -o $(BUILD_DIR)/BICO_liteTest.o

KMeansppTest.o: UCICovertypeDataStream.h KMeanspp.cpp KMeanspp.h Matrix.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c KMeansppTest.cpp -o $(BUILD_DIR)/KMeansppTest.o

SkDataStream.o: SkDataStream.cpp SkDataStream.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) -c SkDataStream.cpp -o $(BUILD_DIR)/SkDataStream.o

SkDataStreamTest: SkDataStreamTest.cpp SkDataStream.o SkDataStream.h
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) SkDataStreamTest.cpp $(BUILD_DIR)/SkDataStream.o -o $(TEST_DIR)/SkDataStreamTest

UCICovertypeDataStreamTest: UCICovertypeDataStreamTest.o UCICovertypeDataStream.o
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) $(BUILD_DIR)/UCICovertypeDataStream.o $(BUILD_DIR)/UCICovertypeDataStreamTest.o -o $(TEST_DIR)/UCICovertypeDataStreamTest

KMeansppTest: KMeanspp.o KMeansppTest.cpp KMeanspp.o Matrix.h SkDataStream.h UCICovertypeDataStream.h UCICovertypeDataStream.cpp UCICovertypeDataStream.o SkDataStream.o
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) KMeansppTest.cpp $(BUILD_DIR)/KMeanspp.o $(BUILD_DIR)/SkDataStream.o $(BUILD_DIR)/UCICovertypeDataStream.o -o $(TEST_DIR)/KMeansppTest

BICO_liteTest: BICO_lite.o BICO_liteTest.o KMeanspp.o UCICovertypeDataStream.o SkDataStream.o
	$(CC) $(DEBUG_OPT) -std=c++$(C++_STD) $(BUILD_DIR)/BICO_lite.o $(BUILD_DIR)/BICO_liteTest.o $(BUILD_DIR)/SkDataStream.o $(BUILD_DIR)/KMeanspp.o $(BUILD_DIR)/UCICovertypeDataStream.o -o $(TEST_DIR)/BICO_liteTest


