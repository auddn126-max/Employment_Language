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
    Add = -20,
    Subtrack = -21,
    Times = -22,
    Divide = -23,
    Semicol = -10,
    Korean = -9,
    English = -8,
    Num = -7,
    Blank = -6,
    unary = -5,
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
    case Add:
        return "덧셈토큰\n";
        break;
    case Subtrack:
        return "뺄셈토큰\n";
        break;
    case Times:
        return "곱셈토큰\n";
        break;
    case Divide:
        return "나눗셈토큰\n";
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

bool statement_check(struct token *tp);

bool value_check(int n);

bool op_check(int n);

token *blank_cal(struct token *tp);

token tarray[30] = {0};
char *source = "연봉 최고 = 500 * 300 / 10 + 5;";

int main(void)
{
    char *idx = source;

    printf("소스코드 : %s\n\n\n\n", source);

    tokenize(idx, tarray);

    token *tp = tarray; // main함수에서 tarray++연산은 위험하니 포인터를 따로 선언해준다.

    while (tp->is_notempty == true) // 구조체배열들중 토큰이 들어있는것들만 반복.
    {
        printf("%s(%s)\n", token_name(tp->token_type), tp->array);
        tp++;
    }

    printf("***%d***\n", statement_check(tarray));

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

            idx++;

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
                tp->token_type = Add;
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
                tp->token_type = Subtrack;
                tp->is_notempty = 1;
                tp++;
            }
        }

        else if (*idx == '*')
        {
            start = idx;

            idx++;

            len = idx - start;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = Times;
            tp->is_notempty = 1;
            tp++;
        }

        else if (*idx == '/')
        {
            start = idx;

            idx++;

            len = idx - start;

            strncpy(tp->array, start, len);
            tp->array[len] = '\0';
            tp->token_type = Divide;
            tp->is_notempty = 1;
            tp++;
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

bool statement_check(token *tp) // 소스코드가 CFG에 적합한지 확인하는 함수.
{

    if (strcmp(tp->array, "연봉") != 0) // 현재 기능은 "연봉" 즉 int 자료형만 가능하다.
    {
        printf("변수타입이 맞지 않습니다.\n");
        return false;
    }

    tp++; // 자료형 토큰을 확인했으니 다음 토큰으로 이동시켜주기

    tp = blank_cal(tp); // 공백이 있다면 찾아서 건너 뛰는 함수.
    if (tp == NULL)
    {
        return false; // blank_cal 함수가 NULL을 반환했다는건 공백이 10개 이상이라는 의미
    }

    if (tp->token_type != Korean && tp->token_type != English)
    {
        printf("변수명으로 올바른 형식이 아닙니다\n");
        return false;
    }

    tp++;

    tp = blank_cal(tp);
    if (tp == NULL)
    {
        return false;
    }

    if (tp->token_type != equal)
    {
        printf("변수명 다음으론 대입연산자가 들어와야 합니다.\n");
        return false;
    }

    tp++;

    tp = blank_cal(tp);
    if (tp == NULL)
    {
        return false;
    }

    int check_num = tp - tarray;          // 현재 tp의 위치와 taaray의 시작주소를 빼주면 몇칸 이동했는지 알 수 있다.
    bool result = value_check(check_num); // value_check는 배열의 index번호를 보내줘야 한다.

    if (result == true)
    {
        printf("Correct\n");
        return true;
    }
    else
    {
        printf("수식이 맞지 않습니다\n");
        return false;
    }
}

token *blank_cal(token *tp) // 토큰의 위치를 받아 공백을 count하면서 뛰어 넘는 함수.
{
    int blank_num = 0;

    while (tp->token_type == Blank)
    {
        tp++;
        blank_num++;

        if (blank_num > 10)
        {
            printf("공백이 너무 많습니다.\n");
            return NULL;
        }
    }

    return tp;
}

bool value_check(int n) // 현재 토큰이 value일때 처리하는 함수
{
    if (n > 999) // 혹시라도 종료가 안되어서 토큰 999개가 넘어갔을때를 대비한 임시 종료장치
    {
        return false;
    }

    else if (tarray[n].token_type == unavailable || tarray[n].token_type == Blank)
    {
        return value_check(n + 1); // 현재 토큰이 미지원기능이거나 공백토큰이라면 다음 인덱스로 재귀호출헤주기
    }

    else if (tarray[n + 1].token_type == Semicol && (tarray[n].token_type == Korean || tarray[n].token_type == English || tarray[n].token_type == Num))
    {
        return true; // tarray 다음 토큰이 세미콜론이고 현재 tarray토큰은 한글 ,숫자 ,영어면 문법상 말이 된다.
    }

    else if (tarray[n].token_type == Korean || tarray[n].token_type == English || tarray[n].token_type == Num)
    {
        return op_check(n + 1); // 다음토큰이 세미콜론이 아니고 현재 토큰은 value일때 다음 자리에 연산자토큰이 와야함으로 op_check함수 호출
    }

    else
    {
        return false; // 문법상 맞지 않는 토큰임으로 false처리
    }
}

bool op_check(int n) // 현재 토큰이 오퍼레이터 일떄 처리하는 함수.
{
    if (n > 999)
    {
        return false;
    }

    else if (tarray[n].token_type == unavailable || tarray[n].token_type == Blank)
    {
        return op_check(n + 1); // 현재 토큰이 미지원기능이거나 공백토큰이라면 다음 인덱스로 재귀호출헤주기
    }

    else if (tarray[n].token_type == unary && tarray[n + 1].token_type == Semicol)
    {
        return true; // 단항연산 다음 세미콜론이 오는건 문법상 괜찮다.
    }

    else if (tarray[n + 1].token_type == Semicol)
    {
        return false; // 이항연산자 다음에 세미콜론이 오면 CFG가 성립이 안된다.
    }

    else if (tarray[n].token_type == Add || tarray[n].token_type == Subtrack || tarray[n].token_type == Times || tarray[n].token_type == Divide || tarray[n].token_type == equal)
    { // 덧셈 ,뺼셈 ,곱셈 ,나눗셈 ,대입연산자라면 문법상 맞는 토큰이므로 value_check를 위해 함수 호출
        return value_check(n + 1);
    }

    else
    {
        return false;
    }
}