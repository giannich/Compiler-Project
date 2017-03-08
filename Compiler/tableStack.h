#include <stdlib.h>
#include <stdio.h>

typedef struct Stack_node
{
	struct Stack_node *next, *prev;
	struct Symbol **table;
} Stack_node;

Stack_node *initStack();
Stack_node *createNode();

void pushTable(Stack_node *stackPtr, Symbol **newtable);
Symbol **popTable(Stack_node *stackPtr);
Symbol **peekTable(Stack_node *stackPtr);
int isEmpty(Stack_node *stackPtr);

Stack_node *initStack()
{
	Stack_node *newStack;
	newStack = createNode();
	newStack->table = NULL;
	newStack->next = NULL;
	newStack->prev = NULL;
	return newStack;
}

Stack_node *createNode()
{
	Stack_node *node = (Stack_node *) malloc(sizeof(Stack_node));
	return node;
}

void pushTable(Stack_node *stackPtr, Symbol **newTable)
{
	// If the stack is empty, create a new stack
	if (stackPtr->table == NULL)
	{
		stackPtr->table = newTable;
		return;
	}
	else
	{
		Stack_node *oldNode = initStack();

		oldNode->table = stackPtr->table;
		oldNode->prev = stackPtr->prev;
		oldNode->next = stackPtr;

		stackPtr->table = newTable;
		stackPtr->prev = oldNode;
		return;
	}
}

Symbol **popTable(Stack_node *stackPtr)
{
	if (stackPtr != NULL)
	{
		Symbol **popTable = stackPtr->table;

		if (stackPtr->prev != NULL)
		{
			stackPtr->table = stackPtr->prev->table;
			stackPtr->prev = stackPtr->prev->prev;
		}
		else
		{
			stackPtr->table = NULL;
			stackPtr->prev = NULL;
		}
		return popTable;
	}
	else
		return NULL;
}

Symbol **peekTable(Stack_node *stackPtr)
{
	if (stackPtr != NULL)
		return stackPtr->table;
	else
		return NULL;
}

int isEmpty(Stack_node *stackPtr)
{
	if (stackPtr->table == NULL)
		return 1;
	else
		return 0;
}