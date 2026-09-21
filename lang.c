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

enum tk // 열거형
{
    Semicol = -10,
    Korean = -9,
    English = -8,
    Num = -7,
    Blank = -6,
    unary = -5,
    binary = -4,
    equal = -3,
    unavailable = -2
};

const char *token_name(int n) // token구조체 안에있는 token_type을 다시 문자열로 변환해서 반환해주는 함수
{                             // 반환타입이 const char *인 이유는 ReadOnly메모리에 있는 문자열을 건드릴수 없게 경고장 부착
    switch (n)
    {
    case Korean:
        return "한글토큰\n";
        break;
    case English:
        return "영문토큰\n";
        break;
    case Num:
        return "숫자토큰\n";
        break;
    case Blank:
        return "공백토큰\n";
        break;
    case unary:
        return "단항연산토큰\n";
        break;
    case binary:
        return "이항연산토큰\n";
        break;
    case equal:
        return "대입연산토큰\n";
        break;
    case unavailable:
        return "미지원기능토큰\n";
        break;
    case Semicol:
        return "세미콜론토큰\n";
        break;

    default:
        break;
    }
}

typedef struct token // 토큰 하나하나가 될 구조체 선언
{
    char array[30];
    int token_type;
    int is_notempty;
} token;
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
void tokenize(char *idx, struct token *tp);

token tarray[30] = {0};
char *source = "연봉최고 연봉은 = 500 + 1 + n++;";

int main(void)
{
    char *idx = source;

    printf("소스코드 : %s\n\n\n\n", source);

    tokenize(idx, tarray);

    token *tp = tarray; // main함수에서 tarray++같은 포인터연산을 불가능하니 구조체 포인터 하나 선언해준다.

    while (tp->is_notempty == true) // 구조체배열들중 토큰이 들어있는것들만 반복.
    {
        printf("%s(%s)\n", token_name(tp->token_type), tp->array);
        tp++;
    }

    return 0;
}

void tokenize(char *idx, token *tp) // 소스코드 읽어서 Tokenize후 구조체배열에 넣어주기
{
    char *start;
    int len;

    while (*idx != '\0')
    {
        if (is_korean(*idx))
        {
            start = idx;

            while (is_korean(*idx))
            {
                idx += 3; // idx포인터가 지금 서있는곳이 "한글"이라면 3바이트를 건너뛰어야만 다음 char을 point할 수 있다.
            }

            len = idx - start;              // 만약 포인터가 int였고 포인터와 포인터를 뺄셈한다
                                            // 가정하자 주소값의 차이가 12바이트이면 cal에는 3이 들어간다 int 3개가 들어가는 자리
            strncpy(tp->array, start, len); // start가 가르키는 char부터 len만큼 tp->array에 저장하기.
            tp->array[len] = '\0';
            tp->token_type = Korean; // 구조체안 멤버들 토큰종류에 맞게 바꿔주기
            tp->is_notempty = 1;     // 구조체 하나를 채웠으면 in_notempty를 1로바꿔 토큰이 들어있다고 명시해준다.
            tp++;
        }

        else if (isalpha(*idx))
        {

            start = idx;

            while (isalpha(*idx))
            {
                idx++;
            }

            len = idx - start;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = English;
            tp->is_notempty = 1;
            tp++;
        }
        else if (isdigit(*idx))
        {
            start = idx;
            while (isdigit(*idx))
            {
                idx++;
            }
            len = idx - start;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = Num;
            tp->is_notempty = 1;
            tp++;
        }
        else if (isspace(*idx))
        {
            start = idx;
            while (isspace(*idx))
            {
                idx++;
            }
            len = idx - start;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = Blank;
            tp->is_notempty = 1;
            tp++;
        }
        else if (*idx == '+')
        {
            start = idx;

            if (*(idx + 1) == '+') // 단항연산자라면
            {
                idx += 2;

                len = idx - start;

                strncpy(tp->array, start, len);
                tp->array[len] = '\0';
                tp->token_type = unary;
                tp->is_notempty = 1;
                tp++;
            }
            else // 이항연산이라면
            {
                idx++;

                len = idx - start;

                strncpy(tp->array, start, len);
                tp->array[len] = '\0';
                tp->token_type = binary;
                tp->is_notempty = 1;
                tp++;
            }
        }

        else if (*idx == '-')
        {
            start = idx;

            if (*(idx + 1) == '-') // 단항연산자라면
            {
                idx += 2;

                len = idx - start;

                strncpy(tp->array, start, len);
                tp->array[len] = '\0';
                tp->token_type = unary;
                tp->is_notempty = 1;
                tp++;
            }
            else // 이항연산이라면
            {
                idx++;

                len = idx - start;

                strncpy(tp->array, start, len);
                tp->array[len] = '\0';
                tp->token_type = binary;
                tp->is_notempty = 1;
                tp++;
            }
        }

        else if (*idx == '=')
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = equal;
            tp->is_notempty = 1;
            tp++;
        }

        else if (*idx == ';')
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = Semicol;
            tp->is_notempty = 1;
            tp++;
        }

        else
        {
            start = idx;

            idx++;

            len = 1;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = unavailable;
            tp->is_notempty = 1;
            tp++;
        }
    }
}
