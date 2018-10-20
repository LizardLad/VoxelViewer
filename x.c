#include <math.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
	double b = 65535.0L;
	printf("%lf\n", b);
	printf("%lf\n", b = pow(b, 3));
	uint32_t a = (uint32_t)b;
	printf("%llu\n", a);
	if(a < 0xFFFFFFFF ) printf("Yes its ok!\n");
	else printf("Nope don't do it\n");
	return 0;
}
