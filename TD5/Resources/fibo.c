#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char BLANK = '_';


// buffer size for input read
#define BUF_SIZE 256

typedef struct
{
    char* buf;
    size_t size;
    ssize_t position;
} Tape;

Tape tapes[3];

char tape_read(size_t tape_num)
{
    const Tape* tape = &tapes[tape_num];

    if (tape->position < 0 || tape->position >= tape->size)
        return BLANK;

    return tape->buf[tape->position];
}

void tape_write(size_t tape_num, char symbol)
{
    Tape* tape = &tapes[tape_num];

    if (tape->position < 0)
    {
        size_t newsize = tape->size - tape->position;
        char* newbuf = malloc(newsize);
        memcpy(newbuf - tape->position, tape->buf, tape->size);
        memset(newbuf, BLANK, -tape->position);
        free(tape->buf);
        tape->buf = newbuf;
        tape->position = 0;
        tape->size = newsize;
    }
    else if (tape->position >= tape->size)
    {
        size_t newsize = tape->position + 1;
        tape->buf = realloc(tape->buf, newsize);
        memset(tape->buf + tape->size, BLANK, newsize - tape->size);
        tape->size = newsize;
    }

    tape->buf[tape->position] = symbol;
}

void tape_left(size_t tape_num)
{
    tapes[tape_num].position--;
}

void tape_right(size_t tape_num)
{
    tapes[tape_num].position++;
}

void tape_init(size_t tape_num)
{
    tapes[tape_num] = (Tape){ .buf = NULL, .size = 0, .position = 0 };
}

void tape_print(size_t tape_num)
{
    printf("%.*s\n", (int) tapes[tape_num].size, tapes[tape_num].buf);
}

void tape_read_stdin()
{
    size_t input_size = 0;
    size_t current_size = BUF_SIZE;
    char* input_buf = malloc(current_size);
    memset(input_buf, BLANK, current_size);
    size_t read;
    while ((read = fread(input_buf + input_size, 1, current_size - input_size, stdin)) > 0)
    {
        input_size += read;
        if (current_size - input_size < 16)
        {
            input_buf = realloc(input_buf, current_size += BUF_SIZE);
        }
    }

    input_buf = realloc(input_buf, input_size);

    tapes[0] = (Tape){ .buf = input_buf, .size = input_size, .position = 0 };
}


typedef int state_t;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void display(size_t step, state_t current)
{
    printf("Step %zu, current state : %d\n", step, current);
    for (size_t num = 0; num < 3; num++)
    {
        printf("Tape %2zu : ", num);
        tape_print(num);
    }
    fflush(stdout);
}

#define SHOW_STEP() fprintf(stderr, "Step count: %zu\n", step)

int main()
{
    for (int i = 0; i < 3; i++)
    {
        tape_init(i);
    }

    

    state_t current = 1;

    for (size_t step = 0; ; step++)
    {
        

        char tape_values[] = {  tape_read(0),  tape_read(1),  tape_read(2),  };
        switch (current)
        {
        case 0:
        err_state:
            fprintf(stderr, "ERROR '%s' FROM STATE '%d': '%s'\n", "ERROR", current, "ERROR");
            display(step, current);
            return EXIT_FAILURE;

    
        case -1: // END
            fprintf(stderr, "END STATE '%s': %s\n", "END", "END");
            display(step, current);
            return EXIT_SUCCESS;
    
        case -2: // DEFAULT
            fprintf(stderr, "END STATE '%s': %s\n", "DEFAULT", "default option");
            display(step, current);
            return EXIT_SUCCESS;
    

    
        case 1: // INIT
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                  tape_write(0, '[');    tape_write(1, '[');    
                  tape_right(0);      
                current = 2;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 2: // 0
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 3;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 3: // 1
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 4;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 4: // 2
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 5;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 5: // 3
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 6;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 6: // 4
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 7;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 7: // 5
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 8;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 8: // 6
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 9;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 9: // 7
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 10;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 10: // 8
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 11;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 11: // 9
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 12;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 12: // 10
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 13;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 13: // 11
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 14;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 14: // 12
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 15;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 15: // 13
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 16;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 16: // 14
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 17;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 17: // 15
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 18;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 18: // 16
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 19;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 19: // 17
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 20;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 20: // 18
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 21;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 21: // 19
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 22;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 22: // 20
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 23;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 23: // 21
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 24;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 24: // 22
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 25;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 25: // 23
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 26;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 26: // 24
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 27;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 27: // 25
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 28;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 28: // 26
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 29;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 29: // 27
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 30;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 30: // 28
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 31;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 31: // 29
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 32;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 32: // 30
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 33;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 33: // 31
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 34;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 34: // 32
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '/');      
                  tape_right(0);      
                current = 35;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 35: // 33
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 36;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 36: // 34
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 37;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 37: // 35
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 38;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 38: // 36
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 39;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 39: // 37
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 40;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 40: // 38
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 41;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 41: // 39
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 42;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 42: // 40
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 43;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 43: // 41
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 44;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 44: // 42
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 45;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 45: // 43
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 46;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 46: // 44
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 47;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 47: // 45
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 48;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 48: // 46
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 49;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 49: // 47
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 50;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 50: // 48
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 51;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 51: // 49
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 52;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 52: // 50
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 53;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 53: // 51
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 54;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 54: // 52
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 55;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 55: // 53
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 56;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 56: // 54
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 57;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 57: // 55
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 58;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 58: // 56
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 59;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 59: // 57
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 60;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 60: // 58
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 61;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 61: // 59
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 62;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 62: // 60
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 63;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 63: // 61
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 64;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 64: // 62
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 65;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 65: // 63
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 66;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 66: // 64
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 67;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 67: // 65
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '/');      
                  tape_right(0);      
                current = 68;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 68: // 66
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 69;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 69: // 67
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 70;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 70: // 68
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 71;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 71: // 69
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 72;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 72: // 70
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 73;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 73: // 71
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 74;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 74: // 72
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 75;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 75: // 73
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 76;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 76: // 74
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 77;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 77: // 75
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 78;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 78: // 76
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 79;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 79: // 77
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 80;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 80: // 78
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 81;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 81: // 79
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 82;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 82: // 80
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 83;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 83: // 81
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 84;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 84: // 82
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 85;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 85: // 83
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 86;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 86: // 84
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 87;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 87: // 85
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 88;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 88: // 86
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 89;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 89: // 87
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 90;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 90: // 88
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 91;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 91: // 89
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 92;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 92: // 90
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 93;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 93: // 91
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 94;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 94: // 92
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 95;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 95: // 93
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 96;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 96: // 94
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 97;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 97: // 95
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 98;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 98: // 96
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 99;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 99: // 97
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 100;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 100: // 98
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '/');      
                  tape_right(0);      
                current = 101;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 101: // 99
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 102;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 102: // 100
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 103;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 103: // 101
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 104;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 104: // 102
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 105;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 105: // 103
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 106;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 106: // 104
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 107;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 107: // 105
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 108;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 108: // 106
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 109;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 109: // 107
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 110;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 110: // 108
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 111;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 111: // 109
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 112;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 112: // 110
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 113;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 113: // 111
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 114;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 114: // 112
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 115;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 115: // 113
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 116;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 116: // 114
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 117;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 117: // 115
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 118;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 118: // 116
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 119;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 119: // 117
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 120;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 120: // 118
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 121;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 121: // 119
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 122;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 122: // 120
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 123;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 123: // 121
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 124;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 124: // 122
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 125;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 125: // 123
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 126;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 126: // 124
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 127;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 127: // 125
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 128;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 128: // 126
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 129;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 129: // 127
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 130;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 130: // 128
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 131;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 131: // 129
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 132;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 132: // 130
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 133;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 133: // 131
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '/');      
                  tape_right(0);      
                current = 134;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 134: // 132
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 135;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 135: // 133
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 136;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 136: // 134
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 137;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 137: // 135
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 138;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 138: // 136
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 139;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 139: // 137
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 140;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 140: // 138
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 141;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 141: // 139
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 142;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 142: // 140
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 143;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 143: // 141
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 144;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 144: // 142
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 145;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 145: // 143
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 146;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 146: // 144
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 147;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 147: // 145
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 148;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 148: // 146
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 149;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 149: // 147
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 150;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 150: // 148
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 151;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 151: // 149
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 152;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 152: // 150
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 153;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 153: // 151
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 154;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 154: // 152
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 155;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 155: // 153
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 156;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 156: // 154
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 157;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 157: // 155
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 158;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 158: // 156
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 159;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 159: // 157
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 160;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 160: // 158
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 161;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 161: // 159
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 162;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 162: // 160
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 163;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 163: // 161
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 164;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 164: // 162
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 165;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 165: // 163
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');      
                  tape_right(0);      
                current = 166;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 166: // 164
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                  tape_write(0, '/');      
                  tape_right(0);      
                current = 167;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 167: // 165
        
            if (1  && tape_values[0] == '_'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 167;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 167;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 167;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 168;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 168: // 166
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 169;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 169;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 169;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 169;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 169: // 167
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 170;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 170;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 170: // 168
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 171;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 171;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 171: // 169
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 172;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 172;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 172: // 170
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 173;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 173;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 173: // 171
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 174;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 174;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 174: // 172
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 175;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 175;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 175: // 173
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 176;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 176;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 176: // 174
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 177;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 177;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 177: // 175
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 178;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 178;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 178: // 176
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 179;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 179;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 179: // 177
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 180;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 180;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 180: // 178
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 181;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 181;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 181: // 179
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 182;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 182;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 182: // 180
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 183;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 183;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 183: // 181
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 184;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 184;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 184: // 182
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 185;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 185;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 185: // 183
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 186;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 186;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 186: // 184
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 187;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 187;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 187: // 185
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 188;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 188;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 188: // 186
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 189;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 189;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 189: // 187
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 190;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 190;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 190: // 188
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 191;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 191;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 191: // 189
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 192;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 192;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 192: // 190
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 193;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 193;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 193: // 191
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 194;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 194;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 194: // 192
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 195;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 195;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 195: // 193
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 196;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 196;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 196: // 194
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 197;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 197;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 197: // 195
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 198;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 198;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 198: // 196
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 199;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 199;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 199: // 197
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 200;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 200;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 200: // 198
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 201;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 201;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 201: // 199
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 202;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 202;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 202: // 200
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 202;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 202;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 202;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 203;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 203: // 201
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 204: // 202
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 204;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 205: // 203
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 205;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 206;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 206: // 204
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 206;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 206;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 206;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 206;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 208;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 208;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 208;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 208;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 207;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 207: // 205
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 207;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 208;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 208: // 206
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 209;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 209;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 209;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 209;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 209: // 207
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 210;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 210;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 210: // 208
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 211;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 211;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 211: // 209
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 212;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 212;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 212: // 210
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 213;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 213;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 213: // 211
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 214;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 214;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 214: // 212
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 215;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 215;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 215: // 213
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 216;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 216;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 216: // 214
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 217;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 217;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 217: // 215
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 218;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 218;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 218: // 216
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 219;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 219;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 219: // 217
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 220;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 220;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 220: // 218
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 221;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 221;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 221: // 219
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 222;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 222;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 222: // 220
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 223;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 223;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 223: // 221
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 224;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 224;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 224: // 222
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 225;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 225;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 225: // 223
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 226;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 226;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 226: // 224
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 227;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 227;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 227: // 225
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 228;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 228;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 228: // 226
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 229;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 229;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 229: // 227
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 230;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 230;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 230: // 228
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 231;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 231;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 231: // 229
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 232;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 232;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 232: // 230
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 233;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 233;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 233: // 231
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 234;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 234;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 234: // 232
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 235;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 235;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 235: // 233
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 236;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 236;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 236: // 234
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 237;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 237;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 237: // 235
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 238;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 238;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 238: // 236
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 239;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 239;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 239: // 237
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 240;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 240;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 240: // 238
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 241;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 241;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 241: // 239
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 242;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 242;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 242: // 240
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 242;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 242;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 242;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 243;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 243: // 241
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 244: // 242
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 244;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 245;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 245;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 245: // 243
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 246: // 244
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 246;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 247: // 245
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 247;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 248;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 248: // 246
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 248;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 248;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 248;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 248;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 250;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 250;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 250;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 250;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 249;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 249: // 247
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 249;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 250;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 250: // 248
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 251;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 251;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 251;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 251;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 251: // 249
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 252;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 252;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 252: // 250
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 253;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 253;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 253: // 251
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 254;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 254;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 254: // 252
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 255;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 255;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 255: // 253
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 256;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 256;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 256: // 254
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 257;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 257;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 257: // 255
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 258;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 258;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 258: // 256
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 259;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 259;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 259: // 257
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 260;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 260;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 260: // 258
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 261;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 261;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 261: // 259
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 262;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 262;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 262: // 260
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 263;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 263;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 263: // 261
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 264;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 264;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 264: // 262
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 265;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 265;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 265: // 263
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 266;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 266;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 266: // 264
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 267;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 267;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 267: // 265
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 268;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 268;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 268: // 266
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 269;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 269;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 269: // 267
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 270;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 270;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 270: // 268
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 271;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 271;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 271: // 269
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 272;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 272;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 272: // 270
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 273;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 273;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 273: // 271
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 274;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 274;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 274: // 272
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 275;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 275;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 275: // 273
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 276;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 276;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 276: // 274
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 277;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 277;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 277: // 275
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 278;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 278;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 278: // 276
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 279;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 279;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 279: // 277
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 280;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 280;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 280: // 278
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 281;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 281;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 281: // 279
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 282;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 282;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 282: // 280
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 283;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 283;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 283: // 281
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 284;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 284;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 284: // 282
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 284;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 284;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 284;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 285;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 285: // 283
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 286: // 284
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 286;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 287;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 287;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 287: // 285
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 288: // 286
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 288;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 289;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 289;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 289: // 287
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 290: // 288
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 290;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 291: // 289
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 291;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 292;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 292: // 290
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 292;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 292;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 292;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 292;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 294;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 294;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 294;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 294;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 293;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 293: // 291
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 293;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 294;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 294: // 292
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 295;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 295;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 295: // 293
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 296: // 294
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 296;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 297;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 297;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 297: // 295
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 298: // 296
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 298;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 299;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 299;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 299: // 297
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 300: // 298
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 300;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 301;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 301: // 299
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 301;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 301;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 301;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 302;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 302: // 300
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 303;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 303;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 303;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 303;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 303: // 301
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 304;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 304;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 304: // 302
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 305;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 305;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 305: // 303
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 306;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 306;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 306: // 304
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 307;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 307;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 307: // 305
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 308;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 308;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 308: // 306
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 309;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 309;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 309: // 307
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 310;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 310;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 310: // 308
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 311;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 311;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 311: // 309
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 312;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 312;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 312: // 310
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 313;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 313;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 313: // 311
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 314;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 314;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 314: // 312
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 315;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 315;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 315: // 313
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 316;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 316;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 316: // 314
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 317;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 317;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 317: // 315
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 318;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 318;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 318: // 316
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 319;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 319;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 319: // 317
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 320;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 320;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 320: // 318
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 321;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 321;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 321: // 319
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 322;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 322;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 322: // 320
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 323;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 323;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 323: // 321
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 324;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 324;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 324: // 322
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 325;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 325;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 325: // 323
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 326;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 326;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 326: // 324
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 327;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 327;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 327: // 325
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 328;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 328;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 328: // 326
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 329;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 329;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 329: // 327
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 330;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 330;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 330: // 328
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 331;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 331;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 331: // 329
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 332;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 332;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 332: // 330
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 333;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 333;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 333: // 331
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 334;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 334;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 334: // 332
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 335;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 335;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 335: // 333
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 336;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 336;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 336: // 334
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 337;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 337: // 335
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '0');  
                    tape_left(1);    tape_left(2);  
                current = 337;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '1');  
                    tape_left(1);    tape_left(2);  
                current = 337;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 338;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 338: // 336
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 338;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 338;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 339: // 337
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 340;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 341;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 340: // 339
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 340;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 340;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 339;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 340;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 341;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 341: // 338
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 342;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 342: // 340
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 342;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 342;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 342;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 343;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 343: // 341
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 344: // 342
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 344;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 345;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 345;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 345: // 343
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 346: // 344
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 346;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 347;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 347;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 347: // 345
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 348: // 346
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 348;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 349: // 347
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 349;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 350;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 350: // 348
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 350;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 350;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 350;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 350;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 351;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 351: // 349
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 351;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 352: // 350
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 353;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 353;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 353: // 351
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 354;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 354;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 354: // 352
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 355: // 353
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 355;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 356;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 356;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 356: // 354
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 357: // 355
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 357;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 358;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 358;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 358: // 356
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 359: // 357
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 359;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 360;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 360: // 358
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 360;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 360;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 360;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 361;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 361: // 359
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 362;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 362: // 360
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 362;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 363;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 481;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 481;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 363: // 361
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 363;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 363;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 364;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 364;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 364: // 363
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 365;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 365;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 365: // 364
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 366: // 365
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 366;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 367;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 367: // 366
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 367;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 367;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 367;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 368;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 368: // 367
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 369;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 369;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 369: // 368
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 370: // 369
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 370;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 371;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 371;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 371: // 370
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 372: // 371
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 372;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 373;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 373: // 372
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 373;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 373;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 373;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 374;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 374: // 373
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 375;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 375: // 374
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '0');  
                    tape_left(1);    tape_left(2);  
                current = 375;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '1');  
                    tape_left(1);    tape_left(2);  
                current = 375;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 376;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 376: // 375
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 376;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 376;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 377: // 376
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 378;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 379;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 378: // 378
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 377;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 378;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 378;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 378;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 379;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 379: // 377
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 380;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 380: // 379
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 380;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 380;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 380;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 381;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 381: // 380
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 382: // 381
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 382;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 383;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 383;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 383: // 382
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 384: // 383
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 384;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 385;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 385;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 385: // 384
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 386: // 385
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 386;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 387;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 387;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 387: // 386
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 388: // 387
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 388;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 389: // 388
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 389;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 390;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 390: // 389
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 390;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 390;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 390;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 390;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 392;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 392;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 392;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 392;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 391;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 391: // 390
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 391;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 392;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 392: // 391
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 393;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 393;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 393: // 392
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 394: // 393
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 394;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 395;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 395;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 395: // 394
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 396: // 395
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 396;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 397;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 397: // 396
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 397;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 397;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 397;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 398;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 398: // 397
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 398;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 398;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 398;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 399;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 399: // 398
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 400: // 399
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 400;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 401: // 400
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 401;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 402;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 402: // 401
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 402;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 402;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 402;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 402;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 404;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 404;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 404;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 404;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 403;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 403: // 402
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 403;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 404;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 404: // 403
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 405;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 405;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 405: // 404
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 406: // 405
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 406;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 407;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 407;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 407: // 406
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 408: // 407
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 408;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 409;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 409;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 409: // 408
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 410: // 409
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 410;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 411;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 411;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 411: // 410
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 412: // 411
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 412;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 413;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 413: // 412
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 413;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 413;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 413;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 414;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 414: // 413
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 414;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 414;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 414;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 415;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 415: // 414
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 416: // 415
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 416;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 417;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 417;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 417: // 416
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 418: // 417
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 418;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 419: // 418
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 419;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 420;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 420: // 419
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 420;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 420;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 420;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 420;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 422;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 422;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 422;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 422;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 421;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 421: // 420
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 421;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 422;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 422: // 421
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 423;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 423;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 423: // 422
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 424: // 423
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 424;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 425;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 425;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 425: // 424
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 426: // 425
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 426;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 427;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 427;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 427: // 426
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 428: // 427
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 428;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 429;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 429: // 428
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 429;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 429;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 429;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 430;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 430: // 429
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 431;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 431;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 431;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    
                current = 431;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 431: // 430
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 432;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                    tape_right(1);    
                current = 432;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 432: // 431
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 433;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 433;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 433: // 432
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 434;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 434;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 434: // 433
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 435;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 435;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 435: // 434
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 436;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 436;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 436: // 435
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 437;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 437;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 437: // 436
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 438;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 438;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 438: // 437
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 439;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 439;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 439: // 438
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 440;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 440;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 440: // 439
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 441;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 441;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 441: // 440
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 442;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 442;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 442: // 441
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 443;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 443;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 443: // 442
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 444;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 444;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 444: // 443
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 445;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 445;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 445: // 444
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 446;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 446;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 446: // 445
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 447;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 447;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 447: // 446
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 448;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 448;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 448: // 447
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 449;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 449;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 449: // 448
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 450;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 450;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 450: // 449
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 451;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 451;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 451: // 450
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 452;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 452;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 452: // 451
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 453;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 453;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 453: // 452
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 454;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 454;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 454: // 453
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 455;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 455;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 455: // 454
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 456;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 456;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 456: // 455
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 457;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 457;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 457: // 456
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 458;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 458;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 458: // 457
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 459;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 459;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 459: // 458
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 460;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 460;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 460: // 459
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 461;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 461;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 461: // 460
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 462;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 462;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 462: // 461
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 463;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                    tape_right(1);    
                current = 463;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 463: // 462
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 464;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                      
                current = 464;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 464: // 463
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                    tape_left(1);    
                current = 465;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 465: // 464
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '0');  
                    tape_left(1);    tape_left(2);  
                current = 465;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    tape_write(2, '1');  
                    tape_left(1);    tape_left(2);  
                current = 465;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 466;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 466: // 465
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 466;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 466;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 467: // 466
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 468;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 469;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 468: // 468
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '0' )
            {
                    tape_write(1, '1');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 468;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 468;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '0' )
            {
                    tape_write(1, '0');    tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 467;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '1' )
            {
                      tape_write(2, '_');  
                    tape_right(1);    tape_right(2);  
                current = 468;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 469;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 469: // 467
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 470;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 470: // 469
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 470;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 470;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 470;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 471;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 471: // 470
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 472: // 471
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 472;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 473;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 473;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 473: // 472
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 474: // 473
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 474;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 475;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 475;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 475: // 474
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 476: // 475
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 476;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 477: // 476
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 477;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 478;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 478: // 477
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 478;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 478;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 478;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 478;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 480;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 480;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 480;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 480;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 479;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 479: // 478
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 479;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 480;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 480: // 479
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 352;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 481: // 362
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 482;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 482;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 482: // 480
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 483;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 483;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 483: // 481
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 484: // 482
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 484;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 485;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 485;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 485: // 483
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 486: // 484
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '0');    
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '1');    
                  tape_right(0);    tape_right(1);    
                current = 486;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                    tape_write(1, '/');    
                  tape_left(0);      
                current = 487;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 487: // 485
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 487;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 487;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 487;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 488;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 488: // 486
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 488;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 488;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 488;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 489;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 489: // 487
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 490: // 488
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 490;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 491;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 491;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 491: // 489
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 492: // 490
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 492;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 493;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 493;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 493: // 491
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 494: // 492
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 494;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 495;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 495;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 495: // 493
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 496: // 494
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);      
                current = 496;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 497;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 497;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 497: // 495
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 498: // 496
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                      
                    tape_left(1);    
                current = 498;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 499: // 497
        
            if (1  && tape_values[0] == '0'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '0'  && tape_values[2] == '_' )
            {
                  tape_write(0, '0');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                  tape_write(0, '1');    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '1'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_right(0);    tape_right(1);    
                current = 499;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                    tape_write(1, '_');    
                  tape_left(0);    tape_left(1);    
                current = 500;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 500: // 498
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 500;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 500;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 500;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '_'  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);    tape_left(1);    
                current = 500;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 502;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 502;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 502;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = 502;
            }
        
            else if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 501;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 501: // 499
        
            if (1  && tape_values[0] == '/'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '0'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '1'  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                  tape_left(0);      
                current = 501;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = 502;
            }
        
            else
            {
                goto err_state;
            }

            break;
    
        case 502: // 500
        
            if (1  && tape_values[0] == '['  && tape_values[1] == '/'  && tape_values[2] == '_' )
            {
                      
                      
                current = -1;
            }
        
            else if (1  && tape_values[0] == '['  && tape_values[1] == '['  && tape_values[2] == '_' )
            {
                      
                      
                current = -1;
            }
        
            else
            {
                goto err_state;
            }

            break;
    

        default:
            fprintf(stderr, "UNKNOWN STATE '%d'\n", current);
            display(step, current);
            return EXIT_FAILURE;
        }
    }
}
