#ifndef TEST_H
#define TEST_H

class test{
    public:
        test() = default;
        ~test() = default;
        void print();
        void set(int v);
    private:
        int a = 0;
};

#endif
