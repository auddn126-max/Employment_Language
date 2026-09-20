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
    char *source = "연봉최고 연봉은 = 500 + 1 + n++;";
    char *idx = source;
    char *start;
    char buf[999] = {};
    int len;

    printf("소스코드 : %s\n\n\n\n", source);

    while (*idx != '\0')
    {
        if (is_korean(*idx))
        {
            start = idx;

            while (is_korean(*idx))
            {
                idx += 3; // idx포인터가 지금 서있는곳이 "한글"이라면 3바이트를 건너뛰어야만 다음 char을 point할 수 있다.
            }

            len = idx - start;        // 만약 포인터가 int였고 포인터와 포인터를 뺄셈한다
                                      // 가정하자 주소값의 차이가 12바이트이면 cal에는 3이 들어간다 int 3개가 들어가는 자리
            strncpy(buf, start, len); // start가 가르키는 char부터 len만큼 buf에 저장하기.
            buf[len] = '\0';

            printf("한글토큰 : %s\n", buf);

            continue;
        }

        else if (isalpha(*idx))
        {

            start = idx;

            while (isalpha(*idx))
            {
                idx++;
            }

            len = idx - start;

            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("알파벳토큰 : %s\n", buf);
        }
        else if (isdigit(*idx))
        {
            start = idx;
            while (isdigit(*idx))
            {
                idx++;
            }
            len = idx - start;
            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("숫자토큰 : %s\n", buf);
        }
        else if (isspace(*idx))
        {
            start = idx;
            while (isspace(*idx))
            {
                idx++;
            }
            len = idx - start;
            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("공백토큰 : %s\n", buf);
        }
        else if (*idx == '+')
        {
            start = idx;

            if (*(idx + 1) == '+') // 단항연산자라면
            {
                idx += 2;

                len = idx - start;
                strncpy(buf, start, len);
                buf[len] = '\0';

                printf("단항연산토큰 : %s\n", buf);
            }
            else // 이항연산이라면
            {
                idx++;

                len = idx - start;
                strncpy(buf, start, len);
                buf[len] = '\0';

                printf("이항연산토큰 : %s\n", buf);
            }
        }

        else if (*idx == '-')
        {
            start = idx;

            if (*(idx + 1) == '-') // 단항연산자라면
            {
                idx += 2;

                len = idx - start;
                strncpy(buf, start, len);
                buf[len] = '\0';

                printf("단항연산토큰 : %s\n", buf);
            }
            else // 이항연산이라면
            {
                idx++;

                len = idx - start;
                strncpy(buf, start, len);
                buf[len] = '\0';

                printf("이항연산토큰 : %s\n", buf);
            }
        }

        else if (*idx == '=')
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("대입연산토큰 : %s\n", buf);
        }

        else if (*idx == ';')
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("세미콜론토큰 : %s\n", buf);
        }

        else
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(buf, start, len);
            buf[len] = '\0';

            printf("미지원기능토큰 : %s\n", buf);
        }
    }

    return 0;
}
