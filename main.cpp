#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

/*
   this program automaticall check inet IP address of host PC
   and setup hci0 BLE dongle to broadcast it by "Complete Local Name"
   through Advertise packet.



   you need put following command into /etc/rc.local
   /home/xxx/hcicastIP/bin/Debug/hcicastIP &
*/

int main()
{
    FILE *fp;
    char strline[1035];
    char strtemp[64];
    char strIP[1035];
    char * p0;
    char * p1;
    int ipv4[4] = {0};

    for(int t=0; t<20; t++)
    {
        bool bFound = false;
        /* Open the command for reading. */
        fp = popen("ip -f inet addr", "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        }

        sprintf(strIP,"(");
        /* Read the output a line at a time - output it. */
        while (fgets(strline, sizeof(strline)-1, fp) != NULL) {
            //printf("%s", path);

            //search for inet
            p0 = strstr(strline, "inet ");
            if(p0 == NULL) continue;

            p1 = strstr(p0, "global");
            if(p1 == NULL) continue;

            //printf("%s", strline);

            if(sscanf(p0,"inet %d.%d.%d.%d/", &ipv4[0], &ipv4[1], &ipv4[2], &ipv4[3]) < 4) continue;

            sprintf(strtemp, "%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
            strcat(strIP, strtemp);
            bFound = true;
            break;
        }
        strcat(strIP, ")");
        pclose(fp); /* close */

        fp = popen("hostname", "r");
        if (fp == NULL) {
            printf("Failed to run command\n" );
            exit(1);
        }
        while (fgets(strline, sizeof(strline)-1, fp) != NULL) {
            strcat(strIP, strline);
            break;
        }
        pclose(fp); /* close */

        //printf("%s", strIP);

        system("hciconfig hci up");
        system("hciconfig hci0 noleadv");
        system("hciconfig hci0 leadv 3");
        system("hciconfig hci0 noscan");
        sprintf(strline, "hcitool -i hci0 cmd 0x08 0x0008 1E 02 01 06 1A 09");

        int len = strlen(strIP);
        for(int i=0;i<26;i++)
        {
            if(i < len)
                sprintf(strtemp, " %02x", strIP[i]);
            else
                sprintf(strtemp, " 00");

            strcat(strline, strtemp);
        }

        cout << strline << endl;


        system(strline);

        sleep(5);
    }
    return 0;
}
