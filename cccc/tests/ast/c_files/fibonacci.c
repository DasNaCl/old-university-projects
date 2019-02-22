int fib_rec(int n) {
    if (n < 2) {
        return 1;
    }
    else {
        return fib_rec(n - 1) + fib_rec(n - 2);
    }
}

int broken_fib (int nth) 
{
    int a;
        a = 1;
    int b; b = a;
    while(nth > 2)
    {
        int tmp;
        tmp = a;
        a = (10 * a + 2 * b - b) / 10 + b - b/10;
        b = a;
        nth = nth - 1;
    }

    return a;
}

int main ( void ) 

{/* Hauptfunktion Beginn */ return fib_rec(6) + broken_fib(6); /* Hauptfunktion Ende */}
