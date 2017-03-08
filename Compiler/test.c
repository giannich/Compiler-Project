// Function Declaration
int main(int argc)
{
	// Declaration
	int a;
	int b;
	int c;
	int d;

	// Expression Instruction and Assignment with Division, Modulo Operation, and Constant Folding
	a = 12 * 10 - 5;
	b = 24 % 5;
	c = 2 + 5;
	d = 1;

	// Iterator Instruction - While
	while (a > 100)
	{
		c = c + 1;
		a = a - 5;
	}

	// Iterator Instruction - Do with Function Call
	do
	{
		c = c + powerFunction(d, 1 + 1);
		d = d + 1;
	} while(c < 1000)

	// Jump Instruction
	return a + 2 * b - c;
}

// Function call with argument handling
int powerFunction(int base, int exponent)
{
	int result;
	result = base;

	// Select Instruction - If/Else
	if (exponent > 0)
	{
		// Iterator Instruction - For
		for (int i = exponent; i > 0; i = i - 1)
		{
			result = result * base;
		}
	}

	else
	{
		result = 1;
	}

	return result;
}
