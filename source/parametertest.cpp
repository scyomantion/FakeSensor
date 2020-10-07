#include "pch.h"
#include "Process.h"

int main(int argc, char **argv)
{
    if(argc < 2) {
        Process p("parametertest.exe -test -parameter 1", "", false);
        p.start();
        p.waitForOutput();
        printf("output was: %s\n", p.output().c_str());

        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, sizeof(path));
        for (size_t i = strlen(path) - 1; i > 0; i--) {
            if (path[i] == '\\')
                break;
            path[i] = 0;
        }
        strcat_s(path, sizeof(path)-1, "autocal.ini");

        char command[MAX_PATH];
        GetPrivateProfileStringA("autocal", "command", NULL, command, sizeof(command) - 1, path);
        printf("command to execute from autocal.ini is: %s\n", command);
    } else {
        printf("parameter count: %d\n", argc);
        for(int i = 0; i < argc; i++) {
            printf("%d: %s\n", i, argv[i]);
        }
    }

    return 0;
}