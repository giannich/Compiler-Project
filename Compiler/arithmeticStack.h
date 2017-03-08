#include <stdlib.h>
#include <stdio.h>

typedef struct Arithmetic
{
	struct Arithmetic *next, *prev;
	char *id, *type;
} Arithmetic;

Arithmetic *initArithmeticStack();
Arithmetic *createArithmeticNode();

void pushArithmetic(Arithmetic *stackPtr, char *newId, char *newType);
Arithmetic popArithmetic(Arithmetic *stackPtr);
Arithmetic *peekArithmetic(Arithmetic *stackPtr);
int isArithmeticEmpty(Arithmetic *stackPtr);

Arithmetic *initArithmeticStack()
{
	Arithmetic *newStack;
	newStack = createArithmeticNode();
	newStack->id = NULL;
	newStack->type = NULL;
	newStack->next = NULL;
	newStack->prev = NULL;
	return newStack;
}

Arithmetic *createArithmeticNode()
{
	Arithmetic *node = (Arithmetic *) malloc(sizeof(Arithmetic));
	return node;
}

void pushArithmetic(Arithmetic *stackPtr, char *newId, char *newType)
{
	// If the stack is empty, create a new stack
	if (stackPtr->id == NULL)
	{
		stackPtr->id = newId;
		stackPtr->type = newType;
		return;
	}
	else
	{
		Arithmetic *oldNode = initArithmeticStack();

		oldNode->id = stackPtr->id;
		oldNode->type = stackPtr->type;
		oldNode->prev = stackPtr->prev;
		oldNode->next = stackPtr;

		stackPtr->id = newId;
		stackPtr->type = newType;
		stackPtr->prev = oldNode;
		return;
	}
}

Arithmetic popArithmetic(Arithmetic *stackPtr)
{
	// If the stack is not empty, pop the stackPtr
	if (stackPtr != NULL)
	{
		Arithmetic popArithmetic = *stackPtr;

		// If the stack is not going to be empty
		if (stackPtr->prev != NULL)
		{
			stackPtr->id = stackPtr->prev->id;
			stackPtr->type = stackPtr->prev->type;
			stackPtr->prev = stackPtr->prev->prev;
		}
		// If the stack is going to be empty
		else
		{
			stackPtr->id = NULL;
			stackPtr->type = NULL;
			stackPtr->prev = NULL;
		}
		return popArithmetic;
	}
	else
		return *stackPtr;
}

Arithmetic *peekArithmetic(Arithmetic *stackPtr)
{
	if (stackPtr != NULL)
		return stackPtr;
	else
		return NULL;
}

int isArithmeticEmpty(Arithmetic *stackPtr)
{
	if (stackPtr->id == NULL)
		return 1;
	else
		return 0;
}