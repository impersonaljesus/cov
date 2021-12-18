#include <time.h>
#include <stdio.h>

int main()
{
    char fnbuffer[100];
    struct tm *timenow;

    time_t now = time(NULL);
    timenow = gmtime(&now);

    strftime(fnbuffer, sizeof(fnbuffer), "output_%Y-%m-%d_%H:%M:%S.dat", timenow);

    fopen(fnbuffer,"w");
    printf("%s", fnbuffer);

    return 0;
}