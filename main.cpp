#include "mini_dump.h"


void SomeFunction()
{
    int *pBadPtr = 0;

    *(pBadPtr + 1) = 1;
}

int main()
{
    minidump::MiniDumpBegin("app","1.0.0.0");
    
    SomeFunction();

    return 0;
}
