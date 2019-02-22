int (foo());

int (main(void))
{
	int ((*x)(int (*)));
	int ((*y)(int (*)));
	(x = foo);
	(y = (&foo));
	int i;
	(i = 7);
	return (((i > 3) ? x : y)(0));
}
