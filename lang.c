#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <ctype.h>

bool is_korean(unsigned char c) // 한글은 UTF-8로 표현되고 첫바이트의 값이 16진수로 0xE0 ~ 0xEF 사이이다
{                               // signed char은 표현 가능한 범위가 작아 음수로 넘어가게 된다
    if (c >= 0xE0 && c <= 0xEF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main(void)
{
    char *source = "연봉 = 500;";
    char *idx = source;
    printf("%s\n", idx);

    while (*idx != '\0')
    {
        if (is_korean(*idx))
        {
            printf("한글입니다.\n");
            for (int i = 0; i < 3; i++)
            {
                idx++;
            }
            continue;
        }

        else if (isalpha(*idx))
        {
            printf("알파벳입니다.\n");
        }
        else if (isdigit(*idx))
        {
            printf("숫자입니다.\n");
        }
        else if (isspace(*idx))
        {
            printf("공백입니다.\n");
        }
        idx++;
    }

    return 0;
}
