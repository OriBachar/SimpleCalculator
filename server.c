
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

int MathOp(int num1, int op, int num2);

int ReadLine(int fd, char* buffer);

void Result();

void SIGUSR1_Hand(int sig);

void SIGARLM_Hand(int sig);

int main(int argc, char* argv[])
{
	if (access("to_srv.txt", F_OK) ==0)
		if (remove("to_srv.txt") != 0)
			PrintError();

	signal(SIGALRM, SIGARLM_Hand);

	signal(SIGUSR1, SIGUSR1_Hand);

	while(1)
	{
		alarm(60);

		pause();
	}
}

void PrintError()
{
	printf("Error in server file\n");

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

    if(num >= 0)
    {
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
    }

    else
    {
        char tempstr[SIZE];

        temp *= -1;

        num *= -1;

        for (i = 0; temp != 0; i++)
        {
            temp /= 10;

            count++;
        }

        tempstr[count] = '\0';

        for (i = 0; count > 0; count--)
        {
            tempstr[count - 1] = num % 10 + '0';

            num /= 10;
        }

        appendS(string,"-");

        appendS(string,tempstr);
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

int MathOp(int num1, int op, int num2)
{
   switch (op)
   {
        case 1:
            return num1 + num2;

        case 2:
            return num1 - num2;

        case 3:
            return num1 * num2;

        case 4:
            return num1 / num2;

        default:
            PrintError();
    }

    return 0;
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

void Result()
{
	int srv_fd;

	int cl_fd;

	char cl_file[SIZE] = "";

	char cl_pid[SIZE];

	char num1[SIZE];

	char op[SIZE];

	char num2[SIZE];

	if ((srv_fd = open("to_srv.txt", O_RDWR | O_APPEND)) == -1)
		PrintError();

	ReadLine(srv_fd, cl_pid);

	ReadLine(srv_fd, num1);

	ReadLine(srv_fd, op);

	ReadLine(srv_fd, num2);

	if (srv_fd = remove("to_srv.txt") != 0)
		PrintError();

	char strres[SIZE] = "";

	if ((StrToInt(op) == 4) && StrToInt(num2) == 0)
		appendS(strres, "Cant dived by zero\n");

	else
	{
		int res = MathOp(StrToInt(num1), StrToInt(op), StrToInt(num2));

		IntToStr(res, strres);
	}

	appendS(cl_file, "to_client_");

	appendS(cl_file, cl_pid);

	appendS(cl_file, ".txt");

	if ((cl_fd = open(cl_file, O_RDWR | O_APPEND | O_CREAT, 0666)) == -1)
		PrintError();

	if((write(cl_fd, &strres, stringLen(strres))) < 0)
	{
		close(cl_fd);

		PrintError();
	}

	puts("\n******* End of Stage 2 ******* \n");

	kill(StrToInt(cl_pid), SIGUSR2);

	exit(-1);
}

void SIGUSR1_Hand(int sig)
{
	puts("\n******* End of Stage 1 ******* \n");

	signal(SIGUSR1, SIGUSR1_Hand);

	int status;

	pid_t pid;

	if ((pid = fork()) < 0)
		PrintError();

	else if (pid == 0)
		Result();

	else
		if (waitpid(pid, &status, 0) == -1)
			PrintError();
}

void SIGARLM_Hand(int sig)
{
    signal(SIGALRM, SIGARLM_Hand);

    printf("Server closed no request was received from the client for 60 seconds\n");

    exit(0);
}
