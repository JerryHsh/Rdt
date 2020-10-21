#include "Circulate.h"

int Circulate::add(int size, int seqnum, int add_num)
{
    return (seqnum + add_num) % size;
}

int Circulate::minus(int size, int seqnum, int minus_num)
{
    while ((seqnum - minus_num) < 0)
    {
        seqnum += size;
    }
    return (seqnum - minus_num) % size;
}

bool Circulate::judge_over(int size, int window_size, int base, int nextSeqNum)
{
    if (Circulate::minus(size, nextSeqNum, base) < window_size)
        return false;
    else
        return true;
}

bool Circulate::judge_bigger(int size, int base, int a, int b)
{
    if (minus(size, a, base) > minus(size, b, base))
        return true;
    else
        return false;
}

bool Circulate::judge_inner(int size, int base, int nextSeqNum, int x)
{
    for (; base != nextSeqNum; base = Circulate::add(size, base, 1))
    {
        if (base == x)
            return true;
    }
    return false;
}
