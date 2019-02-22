int rec_fac(int n)
{
    if (n < 2) return 1;
    else return n * rec_fac(n - 1);
}

int endrecursive_helper(int nth, int akk)
{
    if(nth < 2)
    return akk;
    else
    return endrecursive_helper(nth - 1, akk * nth);
}

int endrecursive_fac(int m)
{
    return endrecursive_helper(m, 1);
}

int fac(int k)              {
    int b;
    b = 1;
    while (k > 1)           {
        b = b * k;
        k = k - 1;          }
    return b;               }

int main(void)
{
    if (1) {
        // Do fancy mancy stuff       \
           But I do find this comment \
           structure pretty weird
        1;
    }
}
