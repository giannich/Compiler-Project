#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initRegisterStack();
int isRegister(char *regName);
char *nextRegister();
char *popArgumentRegister(int n);
void freeBottomRegister(int n);
void freeRegister(char *regName);
int isRegisterFree(char *regName);
void blockRegister(char *regName);
void flushRegisters();

typedef struct Register{
	char *name;
	int available;
} Register;

Register *regStack[6];

void initRegisterStack()
{
	char *eax = (char *) malloc(sizeof(char) * 4);
	char *ebx = (char *) malloc(sizeof(char) * 4);
	char *ecx = (char *) malloc(sizeof(char) * 4);
	char *edx = (char *) malloc(sizeof(char) * 4);
	char *esi = (char *) malloc(sizeof(char) * 4);
	char *edi = (char *) malloc(sizeof(char) * 4);

	eax = "%eax";
	ebx = "%ebx";
	ecx = "%ecx";
	edx = "%edx";
	esi = "%esi";
	edi = "%edi";

	regStack[0] = (Register *) malloc(sizeof(Register));
	regStack[0]->name = eax;
	regStack[0]->available = 1;

	regStack[1] = (Register *) malloc(sizeof(Register));
	regStack[1]->name = ebx;
	regStack[1]->available = 1;

	regStack[2] = (Register *) malloc(sizeof(Register));
	regStack[2]->name = ecx;
	regStack[2]->available = 1;

	regStack[3] = (Register *) malloc(sizeof(Register));
	regStack[3]->name = edx;
	regStack[3]->available = 1;

	regStack[4] = (Register *) malloc(sizeof(Register));
	regStack[4]->name = esi;
	regStack[4]->available = 1;

	regStack[5] = (Register *) malloc(sizeof(Register));
	regStack[5]->name = edi;
	regStack[5]->available = 1;
}

int isRegister(char *regName)
{
	if (!strcmp(regName, "%eax"))
		return 1;
	else if (!strcmp(regName, "%ebx"))
		return 2;
	else if (!strcmp(regName, "%ecx"))
		return 3;
	else if (!strcmp(regName, "%edx"))
		return 4;
	else if (!strcmp(regName, "%esi"))
		return 5;
	else if (!strcmp(regName, "%edi"))
		return 6;
	else
		return 0;
}

int isRegisterFree(char *regName)
{
	int i;
	for (i = 0; i < 6; i++)
		if (strcmp(regStack[i]->name, regName) == 0)
			return regStack[i]->available;

	return -1;
}

char *nextRegister()
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (regStack[i]->available == 1)
		{	
			regStack[i]->available = 0;
			return regStack[i]->name;
		}
	}

	return NULL;
}

char *popArgumentRegister(int n)
{
	return regStack[5-n]->name;
}

void freeBottomRegister(int n)
{
	regStack[5-n]->available = 1;
	return;
}

void freeRegister(char *regName)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (regStack[i]->name == regName)
		{
			regStack[i]->available = 1;
			return;
		}
	}
	printf("Nothing to free\n");
}

void blockRegister(char *regName)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		if (regStack[i]->name == regName)
		{
			regStack[i]->available = 0;
			return;
		}
	}
	printf("Nothing to block\n");
}

void flushRegisters()
{
	int i;
	for (i = 0; i < 6; i++)
		regStack[i]->available = 1;
}