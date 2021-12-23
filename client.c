/*
*
*	Nitzan Miranda - 316591759
*
*	Ori Bachar - 208254516
*
*	ניצן מרנדה - 316591759
*
*	אורי בכר - 208254516
*
*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/random.h>

#define SIZE 255

void PrintError();

void appendS(char* str1, char* str2);

int stringLen(char* str);

char* IntToStr(int num, char* string);

int StrToInt(char* string);

void WriteToFile(int fd, char* string);

int ReadLine(int fd, char* buffer);

void SIGUSR2_Hand(int sig);

void SIGARLM_Hand(int sig);

int main (int argc, char* argv[])
{
	if (argc != 5)
		PrintError();

	int rand;

	getrandom(&rand, sizeof(int), GRND_RANDOM);

	if (rand < 0)
		rand *= -1;

	sleep(rand % 5);

	char* srv_pid = argv[1];

	char* num1 = argv[2];

	char* op = argv[3];

	char* num2 =argv[4];

	int count = 0;

	int cl_pid = getpid();

	int fd_srv;

	signal(SIGUSR2, SIGUSR2_Hand);

	while (access("to_srv.txt", F_OK) == 0)
	{
		count++;

		if (count == 10)
			PrintError();

		rand = 0;

		 getrandom(&rand, sizeof(int), GRND_RANDOM);

        	if (rand < 0)
                	rand *= -1;

        	sleep(rand % 6);
	}

	if ((fd_srv = open("to_srv.txt", O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666)) < 0)
		PrintError();

	char str[SIZE] = "";

	WriteToFile(fd_srv, IntToStr(cl_pid, str));

	WriteToFile(fd_srv, argv[2]);

	WriteToFile(fd_srv, argv[3]);

	WriteToFile(fd_srv, argv[4]);

	close(fd_srv);

	kill(StrToInt(srv_pid), SIGUSR1);

	signal(SIGALRM, SIGARLM_Hand);

	alarm(30);

	pause();

	return 0;
}

void PrintError()
{
	printf("Error in client file\n");

	exit(-1);
}

void appendS(char* str1, char* str2)
{
	int i = 0, j = 0;

	while (str1[i] != '\0')
		 i++;

	while (str2[j] != '\0')
	{
		str1[i] = str2[j];

		i++;

		j++;
	}

	str1[i] = '\0';
}

int stringLen(char* str)
{
	int i = 0;

	while (str[i] != '\0')
		i++;

	return i;
}

char* IntToStr(int num, char* string)
{
    int temp = num;

    int count = 0;

    int i = 0;

    for (i = 0; temp != 0; i++)
    {
        temp /= 10;

        count++;
    }

    string[count] = '\0';

    for (i = 0; count > 0; count--)
    {
        string[count - 1] = num % 10 + '0';

        num /= 10;
    }

    return string;
}

int StrToInt(char* string)
{
    int i = 0;

    int sign = 1;

    int value = 0;

    while (string[i] == '-')
    {
        sign = -sign;

        i++;
    }

    while(string[i] >= '0' && string[i] <= '9')
    {
        int num = (int) (string[i] - '0');

        value = (value * 10) + num;

        i++;
    }

    return (value * sign);
}

void WriteToFile(int fd, char* string)
{
	if (write(fd, string, stringLen(string) + 1) < 0)
	{
		close(fd);

		PrintError();
	}

	if (write(fd, "\n", 1) < 0)
	{
                close(fd);

                PrintError();
    }

	return;
}

int ReadLine(int fd, char* buffer)
{
	int i = 0;

	int cur = 0;

	char ch;

	while (read(fd, &ch, 1) > 0)
	{
		if (ch != '\n')
			buffer[i++] = ch;

		else if (ch == '\n')
		{
			cur++;

			break;
		}

	}

	if (i == 0)
		return 0 ;

	buffer[i]  = '\0';

	return 1;
}

void SIGUSR2_Hand(int sig)
{
	signal(SIGUSR2, SIGUSR2_Hand);

	int cl_pid = getpid();

	int fd_cl;

	char cl_file[SIZE] = "";

	char str[SIZE];

	char res[SIZE];

	char* pid = IntToStr(cl_pid, str);

	appendS(cl_file, "to_client_");

	appendS(cl_file, pid);

	appendS(cl_file, ".txt");

	if ((fd_cl = open(cl_file, O_RDWR | O_APPEND | O_CREAT, 0666)) == -1)
		PrintError();

	if (ReadLine(fd_cl, res) == 0)
	{
		close(fd_cl);

		PrintError();
	}

	if (remove(cl_file) < 0)
		PrintError();

	puts("\n******* End of Stage 3 ******* \n");

	printf("*************************************\n");

	printf("Client pid: %s\n", pid);

	printf("The result is: %s\n", res);

 	printf("*************************************\n");

	return;
}

void SIGARLM_Hand(int sig)
{
    signal(SIGALRM, SIGARLM_Hand);

    printf("Client closed  no response was received from the server for 30 seconds");

    exit(0);
}
