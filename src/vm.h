#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
    PUSH,
    PUSHA,
    POP,
    DROP,
    SWAP,
    DUP,
    ADD,
    SUB,
    MUL,
    DIV,
    PUTC,
    READC,
    PUTNUM,
    READNUM,
    JE,
    JNE,
    JG,
    JL,
    JMP,
    HLT,
};

typedef struct
{
    unsigned char type;
    double argument; // optional mostly just 0 if its not used
} instruction;

typedef struct
{
    double accumulator;
    double *stack;
    int sp;
    int pc;
    int stack_size;
} vm;

void create_vm(vm *ptr, int stack_size)
{
    if (ptr != 0)
    {
        ptr->accumulator = 0;
        ptr->stack = malloc(stack_size * sizeof(double));
        ptr->sp = stack_size;
        ptr->pc = 0;
        ptr->stack_size = stack_size;
    }
    if (ptr == 0)
    {
        printf("VM not allocated and cannot be created.\n");
    }
}

void dump_stack(vm *ptr)
{
    for (int i = ptr->stack_size - 1; i > ptr->sp - 1; --i)
    {
        printf("STACK[%d] = %f\n", (ptr->stack_size - i) - 1, ptr->stack[i]);
    }
}

void fatal_error(char *msg)
{
    printf("FATAL ERROR: %s\n", msg);
    exit(-1);
}

void execute(instruction ins, vm *virtual_machine)
{
    switch (ins.type)
    {
    case PUSH:
        virtual_machine->sp--;
        if (virtual_machine->sp < 0)
        {
            fatal_error("stack overflow.");
        }
        virtual_machine->stack[virtual_machine->sp] = ins.argument;
        return;
    case PUSHA:
        execute((instruction){PUSH, virtual_machine->accumulator}, virtual_machine);
        return;
    case DROP:
        if (ins.argument < 1)
            ins.argument = 1;
        for (int i = 0; i < ins.argument; ++i)
        {
            virtual_machine->sp++;
            if (virtual_machine->sp > virtual_machine->stack_size)
            {
                fatal_error("stack underflow.");
            }
        }
        return;
    case POP:
        virtual_machine->accumulator = virtual_machine->stack[virtual_machine->sp];
        virtual_machine->sp++;
        return;
    case SWAP:
        double x = virtual_machine->stack[virtual_machine->sp];
        virtual_machine->stack[virtual_machine->sp] = virtual_machine->stack[virtual_machine->sp + 1];
        virtual_machine->stack[virtual_machine->sp + 1] = x;
        return;
    case DUP:
        double y = virtual_machine->stack[virtual_machine->sp];
        execute((instruction){PUSH, y}, virtual_machine);
        return;
    case ADD:
        double z = virtual_machine->stack[virtual_machine->sp];
        double a = virtual_machine->accumulator;
        double b = z + a;
        execute((instruction){DROP, 1}, virtual_machine);
        execute((instruction){PUSH, b}, virtual_machine);
        return;
    case SUB:
        z = virtual_machine->stack[virtual_machine->sp];
        a = virtual_machine->accumulator;
        b = z - a;
        execute((instruction){DROP, 1}, virtual_machine);
        execute((instruction){PUSH, b}, virtual_machine);
        return;
    case DIV:
        z = virtual_machine->stack[virtual_machine->sp];
        a = virtual_machine->accumulator;
        b = z / a;
        execute((instruction){DROP, 1}, virtual_machine);
        execute((instruction){PUSH, b}, virtual_machine);
        return;
    case MUL:
        z = virtual_machine->stack[virtual_machine->sp];
        a = virtual_machine->accumulator;
        b = z * a;
        execute((instruction){DROP, 1}, virtual_machine);
        execute((instruction){PUSH, b}, virtual_machine);
        return;
    case PUTC:
        a = virtual_machine->stack[virtual_machine->sp];
        printf("%c", (int)a);
        execute((instruction){DROP, 0}, virtual_machine);
        return;
    case PUTNUM:
        a = virtual_machine->stack[virtual_machine->sp];
        printf("%f", a);
        execute((instruction){DROP, 0}, virtual_machine);
        return;
    case JE:
        a = virtual_machine->stack[virtual_machine->sp];
        if (a == virtual_machine->accumulator)
        {
            virtual_machine->pc += ins.argument - 1;
            execute((instruction){DROP, 1}, virtual_machine);
        }
        return;
    case JNE:
        a = virtual_machine->stack[virtual_machine->sp];
        if (a != virtual_machine->accumulator)
        {
            virtual_machine->pc += ins.argument - 1;
            execute((instruction){DROP, 1}, virtual_machine);
        }
        return;
    case JG:
        a = virtual_machine->stack[virtual_machine->sp];
        if (a > virtual_machine->accumulator)
        {
            virtual_machine->pc += ins.argument - 1;
            execute((instruction){DROP, 1}, virtual_machine);
        }
        return;
    case JL:
        a = virtual_machine->stack[virtual_machine->sp];
        if (a < virtual_machine->accumulator)
        {
            virtual_machine->pc += ins.argument - 1;
            execute((instruction){DROP, 1}, virtual_machine);
        }
        return;
    case JMP:
        virtual_machine->pc += ins.argument - 1;
        return;
    }
}

void execute_all(instruction *instructions, vm *virtual_machine)
{
    printf("EXECUTING...\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    while (instructions[virtual_machine->pc].type != HLT)
    {
        execute(instructions[virtual_machine->pc], virtual_machine);
        ++virtual_machine->pc;
    }
}

#define INS(t, arg) \
    (instruction) { t, arg }
