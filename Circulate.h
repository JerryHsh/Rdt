#pragma once

class Circulate
{
public:
    static int add(int size, int seqnum, int add_num);
    static int minus(int size, int seqnum, int minus_num);
    //return true if it over the windows size return false if it not
    static bool judge_over(int size, int window_size, int base, int nextSeqNum);
    //return true if a > b base on (base) in size circulate
    static bool judge_bigger(int size, int base, int a, int b);
    //return true if x in [base,nextSeqNum),telling that nextSeqNum is legal
    static bool judge_inner(int size, int base, int nextSeqNum, int x);
};
