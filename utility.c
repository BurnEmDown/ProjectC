#include "common.h"

const char commands[16][4] = {"mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};

int errorCounter;

/*-------------------From Base 10 to Base 16-------------------*/

char HexChar [16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

static int * initHexArray (char * pDecStr, int * pnElements);
static void addDecValue (int * pMyArray, int nElements, int value);


/* start at the bottom of the array and work towards the top
 *
 * multiply the existing array value by 10, then add new value.
 * carry over remainder as you work back towards the top of the array
 */
static void addDecValue (int * pHexArray, int nElements, int value)
{
    int carryover = value;
    int tmp = 0;
    int i;

    for (i = (nElements-1); (i >= 0); i--)
    {
        tmp = (pHexArray[i] * 10) + carryover;
        pHexArray[i] = tmp % 16;
        carryover = tmp / 16;
    }
}

/* allocate an array of integer values to store intermediate results
 * only need as many as the input string as going from base 10 to
 * base 16 will never result in a larger number of digits, but for values
 * less than "16" will use the same number
 */
static int * initHexArray (char * pDecStr, int * pnElements)
{
    int * pArray = NULL;
    int lenDecStr = strlen (pDecStr);
    int i;

    pArray = (int *) calloc (lenDecStr,  sizeof (int));

    for (i = 0; i < lenDecStr; i++)
    {
        addDecValue (pArray, lenDecStr, pDecStr[i] - '0');
    }

    *pnElements = lenDecStr;

    return (pArray);
}



/*-------------------Error Table-------------------*/

/*
 * 000 - error not found
 * 100 - ':' is not part of lable
 * 101 - first letter invalid
 * 102 - lable length is too long
 * 103 - no more letters in the line
 * 104 - lable name already exists
 * 105 - missing expression after ':'
 * 106 - missing arguments
 * 107 - next word is not a string
 * 108 - line isn't empty after the string
 * 109 - missing '"' for the string
 * 110 - first word unknown
 * 111 - line isn't empty after external symbol name
 * 112 - error in operand information
 * 113 - destination operand is using illegal address method for the command
 * 114 - origin operand is using illegal address method for the command
 * 115 - no operand information recieved
 * 116 - unknown command
 * 117 - missing comma
 * 118 - first operand invalid
 * 119 - missing space between first operand and comma
 * 120 - illegal register number
 * 121 - missing bracket
 * 122 - first register must be an odd number
 * 123 - invalid char
 * 124 - invalid second operand
 * 125 - invalid firs operand
 * 126 - misplaced expression
 * 127 - label name doesn't exist in label list
 * 128 - line isn't empty after entry symbol name
 * 129 - symbol is not in symbol list
 */


/* the function gets file name, line number and code error,
 * and print to stderr details that are rellavent to this specific error */
void errorFunction(char * sourceFile, int lineNum, int errorCode)
{
    switch(errorCode)
    {
        case 100:
                printf("error in line %d - ':' is not part of lable\n", lineNum);
                break;

        case 101:
                printf("error in line %d - first letter invalid\n", lineNum);
                break;

        case 102:
                printf("error in line %d - lable length is too long\n", lineNum);
                break;

        case 103:
                printf("error in line %d - no more letters in the line\n", lineNum);
                break;

        case 104:
                printf("error in line %d - lable name already exists\n", lineNum);
                break;

        case 105:
                printf("error in line %d - missing expression after ':'\n", lineNum);
                break;

        case 106:
                printf("error in line %d - missing arguments\n", lineNum);
                break;

        case 107:
                printf("error in line %d - next word is not a string\n", lineNum);
                break;

        case 108:
                printf("error in line %d - line isn't empty after the string\n", lineNum);
                break;

        case 109:
                printf("error in line %d - missing \" for the string\n", lineNum);
                break;

        case 110:
                printf("error in line %d - first word unknown\n", lineNum);
                break;

        case 111:
                printf("error in line %d - line isn't empty after external symbol name\n", lineNum);
                break;

        case 112:
                printf("error in line %d - error in operand information\n", lineNum);
                break;

        case 113:
                printf("error in line %d - destination operand is using illegal address method for the command\n", lineNum);
                break;

        case 114:
                printf("error in line %d - origin operand is using illegal address method for the command\n", lineNum);
                break;

        case 115:
                printf("error in line %d - no operand information recieved\n", lineNum);
                break;

        case 116:
                printf("error in line %d - unknown command\n", lineNum);
                break;

        case 117:
                printf("error in line %d - missing comma\n", lineNum);
                break;

        case 118:
                printf("error in line %d - first operand invalid\n", lineNum);
                break;

        case 119:
                printf("error in line %d - missing space between first operand and comma\n", lineNum);
                break;

        case 120:
                printf("error in line %d - illegal register number\n", lineNum);
                break;

        case 121:
                printf("error in line %d - missing bracket\n", lineNum);
                break;

        case 122:
                printf("error in line %d - first register must be an odd number\n", lineNum);
                break;

        case 123:
                printf("error in line %d - invalid char\n", lineNum);
                break;

        case 124:
                printf("error in line %d - invalid second operand\n", lineNum);
                break;

        case 125:
                printf("error in line %d - invalid first operand\n", lineNum);
                break;

        case 126:
                printf("error in line %d - misplaced expression\n", lineNum);
                break;

	case 127:
                printf("error in line %d - label name doesn't exist in label list\n", lineNum);
                break;

	case 128:
                printf("error in line %d - line isn't empty after entry symbol name\n", lineNum);
                break;

	case 129:
                printf("error in line %d - symbol is not in symbol list\n", lineNum);
                break;

	case 000:
		printf("error not found\n");
                break;

        default:
                printf("error not found\n");
                break;
    }
}


/*------------------- Input Validation Functions -------------------*/


/* check if a given string is a command */
/* return its number if it is, or -1 if it's not */
int checkCMD(char * toCheck)
{
   int i;
   for(i = 0; i<COMMANDS_NUMBER; i++)
   {
	if(i == 15) 	/* for command "stop" we need to check 4 letters instead of 3 */
	{
		if(strncmp(commands[i],toCheck,4) == 0)
			return i;
	}
	else
	{
		if(strncmp(commands[i],toCheck,3) == 0)
			return i;
	}
   }

   return -1;
}

/* a function for "skipping" spaces in a given string */
int skipSpaces(char * str, char * toWord)
{
	
	int i = 0;
	while(str[i] == SPACE) 	 			/* as long as current letter pointed to in the string is space */
	{

	   i++;			 			/* skip a letter */
	   if(strncmp(str+i,(char *)NEWLINE,1) == 0) 	/* check for making sure we didn't reach end of string */
	   {
		return -1;				/* if we did, return -1 to signal */
	   }
	}
	
	toWord = strcpy(toWord,str+i);  		/* copy the next "word" to the desired string */
	strcat(toWord,STRING_END);
	
	return i;					/* return the index for the next non-space character in str */
}


/* a function that recieves the first operand of a two-operand command, and checks that there are no values between it and the comma separating it 
 * from the second operand (should be the last letter of op1) */
int frstOpChk(char * op1, char * fileName, int lineNum)
{

	int j = 0;
	int i = 0;
	char garbage[MAX_LABEL_LENGTH] = "";
	
	while(isalnum(op1+j) && op1[j] != COMMA) 	/* check the part between first operand and comma, until reaching space or comma */
	{
	   j++;
	}

	if(op1[j] != COMMA)				/* if space is reached */
	{
	   i = skipSpaces(op1+j, garbage); 	/* skip to next non-space letter */
	   if(i == -1)
	   {
		/* error code 106: line is empty after first operand */
		errorCounter++;	
		errorFunction(fileName, lineNum, 106);
	   }

	   if(op1[i] != COMMA)			/* check that it's a comma, if it's not, then there is an error in the line */
	   {
		/* error: non-space after first operand and before comma */
		return -1; 	/* return -1 to signal an error */
	   }

	}
	
	return 1; /* return 1 to signal everything is fine */	
}



/* a function to check the validity and type of a single operand, which can be a second operand of a two-operand command, 
 * or the single operand of a one-operand command. assumes the string given starts with the first letter of the operand. 
 * return 1 for immediate number, 2 for register, 3 for register-index, and 4 for variable. 0 is given if no type is detected */
int checkOneOperand(char * str, char * opName,char * fileName, int lineNum)
{

	int i = 0;
	int j = 0;
	int length = 0;
	int opType = 0;
	
	char * garbage; /* a char array to hold garbage  */

	if(str[i] == IMD_FLAG)
	{
	
	   if((garbage = strchr(str,COMMA)) != NULL)	/* check line for comma */
	   {
			/* error code 126: there is a comma but shouldn't be */
			errorCounter++;	
			errorFunction(fileName, lineNum, 126);
	   }

	   while(i<length-1)
	   {	   
		if(isdigit(str+1+i)) /* check to make sure that the operand is really a number, we add +1 to skip the "#" sign*/
		{
		   i++;
		}

		else if(i == 0 && (strncmp(str+1,(char *)PLUS,1) == 0 || strncmp(str+1,(char *)MINUS,1) == 0))
		{
		   i++; 	/* plus or minus signs are allowed before the number */
		}

		else if(strncmp(str+1+i,(char *)SPACE,1)) /* found end of number */
		{
			break;
		}

		else
		{
		   /* error code 125: no valid first operand found */	
		   errorCounter++;		
		   errorFunction(fileName, lineNum, 125);
		}

	   }

	   j = skipSpaces(str+i, garbage);
	   if(j != -1)
	   {
		/* error code 126: non-space letter between operand and newline */
		errorCounter++;	
		errorFunction(fileName, lineNum, 126);
	   }
	   
	   strncpy(opName,str+1,i-1);		/* copy the first operand to op1name without any spaces (but with sign) */
	   strcat(opName,STRING_END);
	   opType = 1;				/* immediate number found with no errors */
	}

	if(str[0] == REG_FLAG && isdigit(str[1]) && opType == 0) /* operand found to be a register */
	{
	
	   if(str[1] == 8 || str[1] == 9) /* check for illegal register numbers */
	   {
		/* error code 120: illegal number of register */
		errorCounter++;	
		errorFunction(fileName, lineNum, 120);
	   }

	   if(str[2] != SPACE) 	/* check if next letter in str is not a valid char */
	   {
		if(str[2] == LEFT_BRACKET && str[3] == REG_FLAG && isdigit(str[4])) 	/* check if using index address method
											 * by checking for expression like "r[r2]"*/
		{
		   if(str[1] % 2 == 1)	/* check to make sure first reg is odd number */
		   {
			if(str[5] == RIGHT_BRACKET)
			{
			   if(str[6] == SPACE)
			   {
			  	if((garbage = strchr(str,COMMA)) != NULL)
			   	{
					/* error code 126: there is a comma but shouldn't be */
					errorCounter++;	
					errorFunction(fileName, lineNum, 126);
			   	}

				opType = 3; 		/* register index address method found with no errors */   
			   }
			   else
			   {
			      /* error code 126: non-space non-comma character after first operand */
			      errorCounter++;	
			      errorFunction(fileName, lineNum, 126);
			   }
			}
			else
			{
			    /* error code 121: no closing right bracket */
			    errorCounter++;	
			    errorFunction(fileName, lineNum, 121);
			}
		   }
		   else
		   {
			/* error code 120: first register must be odd number */
			errorCounter++;	
			errorFunction(fileName, lineNum, 120);
	   	   }
		   
		}
		else
		{
		   /* error code 123: not a valid char */
		   errorCounter++;	
		   errorFunction(fileName, lineNum, 123);
		}

	   }

	   else 	/* there was a space after register number */
	   {
		j = skipSpaces(str, garbage);
	   	if(j != -1)
	   	{
			/* error code 126: non-space letter between operand and newline */
			errorCounter++;	
			errorFunction(fileName, lineNum, 126);
	  	}
	
		opType = 2; 			/* register found with no errors */
	   }


	i = 0;
	while(isgraph(str+i))
	{
	   i++;
	}
	strncpy(opName,str,i);		/* copy the first operand to opName without any spaces */
	strcat(opName,STRING_END);
	
	}

	if(opType == 0)	/* check that we didn't find operand type yet, so should be a variable */ 
	{
	   if((garbage = strchr(str,COMMA)) != NULL)
	   {
		/* error code 126: there is a comma but shouldn't be */
		errorCounter++;	
		errorFunction(fileName, lineNum, 126);
	   }	      

	   i = skipSpaces(str, garbage);
	   if(i != -1)
	   {
			/* error code 113: line isn't empty after first operand */
			errorCounter++;	
			errorFunction(fileName, lineNum, 113);
	   }

	   i = 0;
	   while(isgraph(str+i))
	   {
	       i++;
	   }

	   strncpy(opName, str, i);	/* copy the first operand to opName without any spaces */
	   strcat(opName, STRING_END);
	   opType = 4; 			/* variable found with no errors */
	}

	return opType;	/* return 1 for immediate number, 2 for register, 3 for registerindex, 4 for variable, and 0 for no type found */
}



/* a function to check the operands of a command that needs 2 operands. 
 * assume these 2 operands exist in the string and the given string begins with the first operand and not a space */
int checkTwoOperands(char * str,char * op1name, char * op2name, char * fileName, int lineNum)	
{
	int i = 0;
	int length = 0;

	int op1type = 0;	/* variables for address method types. 0 is not found or error, 1 is immediate number */
	int op2type = 0;	/* 2 is register, 3 is register-index, and 4 is variable */

	char * op1;		/* a string to hold first operand information */
	char * op2;		/* a string to hold second operand information */
	

	if(str[i] == IMD_FLAG)
	{

	   if(strchr(str,COMMA) == NULL)	/* check line for comma */
	   {
			/* error code 117: no comma separating operands */
			errorCounter++;	
			errorFunction(fileName, lineNum, 117);	
	   }

	   length = strcspn(str,(char *)COMMA); 	/* check for the end of first operand by finding comma */
	   strncpy(op1,str,length);			/* copy where first operand should be to op1 (with the comma, for the check) */
	   strcat(op1,STRING_END);
	   strcpy(op2,str+length); 			/* copy where next operand should be to op2 (without the comma) */
	   strcat(op2,STRING_END);
	   

	   i = 0;
	   while(i<length-1)
	   {	   
		if(isdigit(op1+1+i)) /* check to make sure that the first operand is really a number, we add +1 to skip the "#" sign*/
		{
		   i++;
		}

		else if((i == 0) && ((strncmp(op1+1,(char *)PLUS,1) == 0) || (strncmp(op1+1,(char *)MINUS,1) == 0)))
		{
		   i++; 	/* plus or minus signs are allowed before the number */
		}

		else if(strncmp(op1+1+i, (char *)SPACE,1) == 0) /* found end of number */
		{
			break;
		}

		else
		{
		   /* error code 118: first operand contains non-digit*/
		   errorCounter++;	
		   errorFunction(fileName, lineNum, 118);	
		}
	   }

	   if(frstOpChk(op1, fileName, lineNum) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
		 errorCounter++;	
		 errorFunction(fileName, lineNum, 119);	
	   }

	   strncpy(op1name,op1+1,i-1);		/* copy the first operand to op1name without any spaces (but with sign) */
	   strcat(op1name,STRING_END);
	   op1type = 1;				/* signal that first operand is an immediate number */
	}


	if(str[i] == REG_FLAG && isdigit(str[i+1]) && op1type == 0) 	/* first operand found to be a register */
	{
	   if(str[i+1] > MAX_REGISTER_DIGIT) /* check for illegal register numbers */
	   {
		/* error code 120: illegal number of register */
		errorCounter++;	
		errorFunction(fileName, lineNum, 120);	
	   }

	   else if(str[i+2] != SPACE && str[i+2] != COMMA) 	/* check if next letter in str is not a valid char */
	   {
		if(str[i+2] == LEFT_BRACKET && str[i+3] == REG_FLAG && isdigit(str[i+4])) 	/* check if using index address method
												 * by checking for expression like "r1[r2]" */
		{
		   if(str[i+1] % 2 == 1)	/* check to make sure first reg is odd number */
		   {
			if(str[i+5] == RIGHT_BRACKET)
			{
			   if(str[i+6] == SPACE || str[i+6] == COMMA)
			   {
			   	op1type = 3; 		/* signal that first operand is a register-index adress method */

			  	if(strchr(str,COMMA) == NULL)
			   	{
					/* error code 117: no comma separating operands */
					errorCounter++;	
					errorFunction(fileName, lineNum, 117);	
			   	}

			   	length = strcspn(str,(char *)COMMA);	/* find the beginning of the next operand info */
			   	strcpy(op2,str+length+1); 		/* copy it to op2, skipping the comma */
				strcat(op2,STRING_END);
			   	strncpy(op1,str,length);		/* copy the first part to op1 (with the comma, for the check) */	
				strcat(op1,STRING_END);
	 
			   }
			   else
			   {
			      /* error code 119: non-space non-comma character after first operand */
			      errorCounter++;	
			      errorFunction(fileName, lineNum, 119);	
			   }

			}
			else
			{
			    /* error code 121: no closing right bracket */
			    errorCounter++;	
			    errorFunction(fileName, lineNum, 121);	
			}

		   }
		   else
		   {
			/* error code 122: first register must be odd number */
			errorCounter++;	
			errorFunction(fileName, lineNum, 122);
	   	   }
		   
		}
		else
		{
		   /* error code 123: not a valid char */
		   errorCounter++;	
		   errorFunction(fileName, lineNum, 123);
		}
	   }
	   else
	   {
	   	op1type = 2; 		/* signal that first operand is a register */
		if(strchr(str,(char)COMMA) == NULL)
		{
			/* error code 117: no comma separating operands */
			errorCounter++;	
			errorFunction(fileName, lineNum, 117);	
		}

		length = strcspn(str,(char *)COMMA);	/* find the beginning of the next operand info */
		strcpy(op2,str+length+1); 		/* copy it to op2, skipping the comma */
		strcat(op2,STRING_END);
		strncpy(op1,str,length);		/* copy the first part to op1 (with the comma, for the check) */
		strcat(op1,STRING_END);
		
	   }

	   if(frstOpChk(op1, fileName, lineNum) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
		errorCounter++;	
		errorFunction(fileName, lineNum, 119);
	   }

	   i = 0;
	   while(isgraph(op1+i) && strncmp(op1+i,(char *)COMMA,1) != 0)
	   {
		i++;
	   }
	   strncpy(op1name,op1,i);		/* copy the first operand to op1name without any spaces */
	   strcat(op1name,STRING_END);   
	}
	
	if(op1type == 0)	/* check that we didn't find first operand type yet, so should be a variable */ 
	{
	   op1type = 4; 		/* signal that first operand is a variable */
	   if(strchr(str,(char)COMMA) == NULL)
	   {
		/* error code 117: no comma separating operands */
		errorCounter++;	
		errorFunction(fileName, lineNum, 117);	
	   }

	   length = strcspn(str,(char *)COMMA);	/* find the beginning of the next operand info */
	   strcpy(op2,str+length+1); 		/* copy it to op2, skipping the comma */
	   strcat(op2,STRING_END);
	   strncpy(op1,str,length);		/* copy the first part to op1 (with the comma, for the check) */
	   strcat(op1,STRING_END);
	   	   
	   if(frstOpChk(op1, fileName, lineNum) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
		errorCounter++;	
		errorFunction(fileName, lineNum, 119);
	   }

	   i = 0;
	   while((isgraph(op1+i)) && (strncmp(op1+i,(char *)COMMA,1) != 0))
	   {
		i++;
	   }
	   strncpy(op1name,op1,i);		/* copy the first operand to op1name without any spaces */
	   strcat(op1name,STRING_END);	   
	}



	/* ------- first operand check is finished ------- */
	/* reset index numbers before op2 check */
	i=0;
	
	i = skipSpaces(op2, op2);
	if(i == -1)
	{
		/* error code 124: line is empty after comma */
		errorCounter++;	
		errorFunction(fileName, lineNum, 124);
	}

	op2type = checkOneOperand(op2, op2name, fileName, lineNum);		/* send the second operand to be checked by the single operand check function */
	if(op2type == 0)	/* operand type not found for second operand */
	{
	   /* error code 124: error with 2nd operand */
	   errorCounter++;	
	   errorFunction(fileName, lineNum, 124);
	}
	
	if(op1type == 0)	/* operand type not found for first operand */
	{
		/* error code 125: no valid first operand found */
		errorCounter++;			
		errorFunction(fileName, lineNum, 125);
	}

	else
	{
	   op1type *= 10; 		/* turn the result of op1type to "tens" */
	   op1type += op2type;  	/* then add op2type, and we get both types in one integer to work with*/
	   return op1type;		/* return the result. the origin operand is in the "tens" digit, and the destination operand is in the "ones" digit */
	}
}

