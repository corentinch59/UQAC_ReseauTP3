#pragma once

extern int DUOEntry(int argc, char* argv[], int(*mainFunc)(int argc, char* argv[]));

#define DUOMain(FuncName) int main(int argc, char* argv[]) \
{ \
	return DUOEntry(argc, argv, &FuncName);\
}