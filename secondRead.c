#include "structs.h"

int skipSpaces(char *, char *);
void errorFunction(char *, int, int);


/* the second loop that: opens the given filename to read and 3 other output files to write to, translates the relevant code from the given file in assembly to machine code in the output files */
void endLoop(char * fileName)
{

	FILE * fp;
	FILE * fob;
	FILE * fent;
	FILE * fext;	
	int lineNum = 0;			/* holds the current line number */
	int j,i;				/* index variable */
	bool label_flag;			/* holds info on whether a label exists in line or not */
	char buf[MAX_BUF];			/* a buffer to hold the current line read */
	char tempBuf[MAX_BUF];			/* a temp buffer to hold a part of buf temporarily */
	char tempBuf2[MAX_BUF];			/* another temp buffer to hold a part of buf temporarily */
	char tempLabel[MAX_BUF];		/* a buffer for holding potential label name */
	char label[MAX_LABEL_LENGTH];   	/* holds current label if it exists */
	char tempWord[MAX_LABEL_LENGTH];	/* a temp buffer to hold a temporary word from buf */
	char * tempString;
	char sourceFile[MAX_LABEL_LENGTH];
	char objFile[MAX_LABEL_LENGTH];
	char entFile[MAX_LABEL_LENGTH];
	char extFile[MAX_LABEL_LENGTH];

	codp tempCod;				/* holds info on current line of code that we collected on first loop */
	symp tempSym;				/* holds info on a temporary symbol */
	int groupCode;
	int opCode;
	int op1;				/* holds info on 1st operand type */
	int op2;				/* holds info on 2nd operand type */
	int ARE;				/* holds current ERA value */
	int op1ARE;				/* holds ERA value of 1st operand in a 2 operand command line */
	int code_length;		

	char letter;				
	int labelpos;				/* label position in line */ 
	int cmdNUM;				/* command type */
	int op1result;				/* saves 1st operand's value (decimal) */
	int op2result;				/* saves 2nd operand's value (decimal) */
	int check;				/* temp int for some checks */
	int address;				/* hold address of current symbol\entry\extern word */

	strcpy(sourceFile, fileName);	
	strcpy(objFile, fileName);
	strcpy(entFile, fileName);
	strcpy(extFile, fileName);
	
	strcat(sourceFile, SOURCE_FILE);	/* add ".as" to the end of the filename, to address to the proper file */
	strcat(objFile, OBJ_FILE);		/* add ".ob" to the end of the filename, to address to the proper file */
	strcat(entFile, ENT_FILE);		/* add ".ent" to the end of the filename, to address to the proper file */
	strcat(extFile, EXT_FILE);		/* add ".ext" to the end of the filename, to address to the proper file */
	
	strcat(sourceFile,STRING_END);
	strcat(objFile,STRING_END);
	strcat(entFile,STRING_END);
	strcat(extFile,STRING_END);

	fp = fopen(sourceFile, "r");
	fob = fopen(objFile, "a");
	fent = fopen(entFile, "a");
	fext = fopen(extFile, "a");

	if(!fp)
	{
	   printf("cannot open file\n");
	   exit(0);
	}

	while(fgets(buf,MAX_BUF,fp) != NULL) 	/* reads a line from the text file */
	{
	   lineNum++;
	   /* check for empty or comment line */
	   j = 0; /* reset index j before check */
	   if((j = skipSpaces(buf,tempWord)) == -1)
	   {
		/* empty line */
		continue;
	   }

	   letter = buf[0]; 		/* check first letter of the line to see if it's a comment line */
	   if(letter == COMMENT_SIGN)
	   {
		continue;
	   }
	   j = 0; /* reset index j after check */
	   
	   tempString = strchr(buf,LABEL_SIGN);
	   if(tempString) 	 /* checks if the line contains a label */
	   {
		labelpos = strcspn(buf,(char *)LABEL_SIGN);
		if(strlen(tempWord)>MAX_LABEL_LENGTH)
		{
		   /* error code 102: label length is too big*/
		   errorFunction(fileName, lineNum, 102);
		}

		strncpy(tempLabel,buf,labelpos);
		strcat(tempLabel,STRING_END);

		/* checks for valid label */
		if(strchr(tempLabel, STRING_SIGN) != NULL)				/* find if there is a '"' in the word */
		{
		   
		   check = strcspn(tempLabel, (char *)LABEL_SIGN);			/* find the location of ':' */
		   strncpy(tempWord, tempLabel, check);					/* copy the part of the word before ':' into tempWord, to check */
		   strcat(tempWord,STRING_END);
		   if(strchr(tempWord, STRING_SIGN) == strrchr(tempWord, STRING_SIGN))	/* check that only one '"' exists before ':' */
		   {
			if(strchr(tempLabel+check, LABEL_SIGN) != NULL)
			{
			   break;		/* we have found that ':' is between two '"'s so it's not part of a label, but part of a string */
			}

		   }
		}
		else if(strchr(tempLabel,LABEL_SIGN) == NULL)
		{
		    /* error code 100: ':' is not part of label */
		    errorFunction(fileName, lineNum, 100);
		}

		if(isalpha(letter)==0)
		{
		    /* error code 101: first letter is not a real letter */
		    errorFunction(fileName, lineNum, 101);
		}
		
		label_flag = true;
		strncpy(label,tempLabel,(strlen(tempLabel)-1));		/* copy label name to label string, without the ":" at the end */
		strcat(label,STRING_END);
	   }

	   else
	   {
	     	label_flag = false;
	   }
	
	   if(strstr(buf,DATA_WORD) != NULL || strstr(buf,STRING_WORD) != NULL)
	   {
		/* skip this line */
	   }
		
	   if(strstr(buf,ENTRY_WORD) != NULL || strstr(buf,EXTERN_WORD) != NULL)
	   {
		strcpy(tempBuf,strchr(buf,SPACE));		/* find the end of instruction name */
	  	strcat(tempBuf,STRING_END);
		if(strstr(buf,EXTERN_WORD) != NULL)		/* if it's external instruction */
		{
		   j = skipSpaces(tempBuf,tempBuf);		/* check that line isn't empty after ".extern" */
		   if(j == -1)
		   {
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		   }	

		   check = strcspn(tempBuf,(char *)SPACE);		/* find the end of external symbol name */
		   strncpy(tempBuf2,tempBuf,check-1);		/* copy the symbol name to tempbuf2, without spaces and ":" */			
		   strcat(tempBuf2,STRING_END);
		   j = skipSpaces(tempBuf,tempWord);

		   if(j != -1)
		   {
			/* error code 111: line isn't empty after external symbol name */
			errorFunction(fileName, lineNum, 111);
		   }

		   if(getLabel(tempBuf2) != NULL)		/* make sure external word is in symbol list */
		   {
			address = getSymbolAddress(check);	/* get the address of label name*/

			/* convert address to hexadecimal */

			fprintf(fext, "%d \t %s", address, tempBuf2);	/* write the label name and its address (in hexa) to external labels file */

		   }

		   else
		   {
			/* error code 127: label name doesn't exist in label list */
			errorFunction(fileName, lineNum, 127);
		   }

		}
		else if(strstr(buf,ENTRY_WORD) != NULL)		/* if it's entry instruction */	
		{
		   j = skipSpaces(tempBuf,tempBuf);		/* check that line isn't empty after ".entry" */
		   if(j == -1)
		   {
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		   }
	
		   check = strcspn(tempBuf,(char *)SPACE);		/* find the end of entry symbol name */
		   strncpy(tempBuf2,tempBuf,check-1);		/* copy the symbol name to tempbuf2, without spaces and ":" */			
		   strcat(tempBuf2,STRING_END);
		   j = skipSpaces(tempBuf,tempWord);
		   if(j != -1)
		   {
			/* error code 128: line isn't empty after entry symbol name */
			errorFunction(fileName, lineNum, 128);
		   }

		   if(getLabel(tempBuf2) != NULL)			/* make sure entry word is in symbol list */
		   {
			address = getSymbolAddress(check);	/* get the address of label name*/

			/* convert address to hexadecimal */

			fprintf(fent, "%d \t %s", address, tempBuf2);	/* write the label name and its address (in hexa) to entry labels file */
		   }
		   else
		   {
			/* error code 127: label name doesn't exist in label list */
			errorFunction(fileName, lineNum, 127);
		   }
		}	 	
	   }
	
	i = 0;
	while(i<code_length)
	{
	   i++;
	}
	i = 0;
		
	   /* step 11 in algorithm */

	   /* skip spaces in line until next word is reached */
	   if(label_flag)
	   {
		j = skipSpaces(buf+labelpos,tempWord);
	   	if(j == -1)
		{
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		}
		j += labelpos;
	   }
	   else
	   {
		j = skipSpaces(buf+j,tempWord);
		if(j == -1)
		{
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		}
	   }
	   

	   cmdNUM = checkCMD(tempWord);
	   if((cmdNUM >= 0) && (cmdNUM <= 15)) 	/* command in line is a proper command, which should be in code linked list */
	   {
		opCode = tempCod->command;
		tempCod = LineGetCodeInfo(lineNum);
		if(tempCod->command >= 14) /* command is either: rts, stop */
		{
		   groupCode = 0;	/* the command is in the first group, with no operands */
		   op1 = 0;		/* set both operands to 0 */
		   op2 = 0;
		   addMachCode(createMach(tempCod->address, groupCode, opCode, op1, op2, 0));	/* add the command line to the machine code list */
		}

		else if(((tempCod->command >= 0) && (tempCod->command <= 3)) || (tempCod->command == 6)) /* command is either: mov, cmp, add, sub, lea */
		{
		   groupCode = 2;		/* the command is in the second group, with two operands */
		   switch(tempCod->op1addressMethod)	/* check origin operand addressing method */	
		   {
			case 1:	/* immediate number */
				op1 = 0;		/* fix the operand addressing method to be the correct number required by the assembler */


				op1result = atoi(tempCod->operand1) << 2;
				/* copy the immediate number with atoi, then shift it 2 times to get it to the right location */
				ARE = 0;		/* absolute */


			case 2: /* register */
				op1 = 3;		/* fix the operand addressing method to be the correct number required by the assembler */


				op1result = atoi(tempCod->operand1+1) << 8;
				/* get the register number from the operand by looking at the 2nd letter, then shift it 8 times to get it to the right location*/
				ARE = 0;		/* absolute */


			case 3: /* register index */
				op1 = 2;		/* fix the operand addressing method to be the correct number required by the assembler */


				op1result = (atoi(tempCod->operand1+4) << 2) + (atoi(tempCod->operand1+1) << 8);
				/* shift the bits in the first and second register given by 2 (second register) and 8 (first register) 
				 * to get them to the right location */
				ARE = 0;		/* absolute */


			case 4: /* variable */
				op1 = 1;		/* fix the operand addressing method to be the correct number required by the assembler */

				if((tempSym = getLabel(tempCod->operand1)) != NULL)	/* if 1st operand is in symbol list */
				{
					if(tempSym->isExtern == false)
					{
						op1result = (tempSym->address) << 2;	/* get the address of the symbol, shift it 2 times to get 
											 * it to the right location */
						ARE = 2;	/* relocatable */
					}
					else
					{
						op1result = 0 << 2;			/* if symbol is external, address should be 0 */
						addExt(createExt(tempSym->name, (tempCod->address)+1));	/* add the external symbol to the external list, 
													 * to be written later to the external file, use "address+1"
													 * because we need the address following 
													 * the original command */
						ARE = 1;	/* external */
					}
				}
				else
				{
					/* error code 129: symbol is not in symbol list */
					errorFunction(fileName, lineNum, 129);
				}

		   } /* end of first operand switch check */

		   op1ARE = ARE; /* save 1st operand ERA status before checking 2nd operand and its ERA */


		   switch(tempCod->op2addressMethod)	/* check destination operand addressing method */	
		   {
			case 1:	/* immediate number */
				op2 = 0;		/* fix the operand addressing method to be the correct number required by the assembler */


				op2result = atoi(tempCod->operand2) << 2;
				/* copy the immediate number with atoi, then shift it 2 times to get it to the right location */
				ARE = 0;		/* absolute */


			case 2: /* register */
				op2 = 3;		/* fix the operand addressing method to be the correct number required by the assembler */
				if(op1 == 3) /* if both operands are registers, so only 1 word is required */
				{
					op1result += atoi(tempCod->operand2+1) << 2;
				}
				else
				{
					op2result = atoi(tempCod->operand2+1) << 2;
				/* get the register number from the operand by looking at the 2nd letter, then shift it 2 times to get it to the right location*/
				}
				ARE = 0;		/* absolute */


			case 3: /* register index */
				op2 = 2;		/* fix the operand addressing method to be the correct number required by the assembler */

				op2result = (atoi(tempCod->operand2+4) << 2) + (atoi(tempCod->operand2+1) << 8);
				/* shift the bits in the first and second register given by 2 (second register) and 8 (first register) to get 
				 * them to the right location */
				ARE = 0;		/* absolute */


			case 4: /* variable */
				op2 = 1;		/* fix the operand addressing method to be the correct number required by the assembler */

				if((tempSym = getLabel(tempCod->operand2)) != NULL)	/* if 2nd operand is in symbol list */
				{
					if(tempSym->isExtern == false)
					{
						op2result = (tempSym->address) << 2;	/* get the address of the symbol, shift it 2 times to get it to 
											 * the right location */
						ARE = 2;	/* relocatable */
					}
					else
					{
						op2result = 0 << 2;			/* if symbol is external, address should be 0 */
						addExt(createExt(tempSym->name, (tempCod->address)+2));	/* add the external symbol to the external list, 
													 * to be written later to the external file, use "address+2"
													 * because we need the address following the first operand */
						ARE = 1;	/* external */
					}
				}
				else
				{
					/* error code 129: symbol is not in symbol list */
					errorFunction(fileName, lineNum, 129);
				}
		   } /* end of second operand switch check */
		   addMachCode(createMach(tempCod->address, groupCode, opCode, op1, op2, 0));	/* add the command line to the machine code list */
		   addMword(createMword((tempCod->address)+1, op1result+op1ARE));		/* add the first operand information to the machine word list
												 * (should be both operands if both are registers) */
		   if((op1 != 3) || op2 != 3)	/* if one of the operands isn't a register */
		   {
			addMword(createMword((tempCod->address)+2, op2result+op1ARE));		/* add the second operand information to the machine word list */
		   }

		}
		else	/* command is either: not, clr, inc, dec, jmp, bne, red, prn, jsr */
		{
		   groupCode = 1;		/* the command is in the third group, with only 1 operand */
		   op1 = 0;			/* since only 1 operand is needed, set the origin operand to 0 */
		   switch(tempCod->op2addressMethod)		
		   {
			case 1:	/* immediate number */
				op2 = 0;		/* fix the operand addressing method to be the correct number required by the assembler */

				op2result = atoi(tempCod->operand2) << 2;
				/* copy the immediate number with atoi, then shift it 2 times to get it to the right location */
				ARE = 0;		/* absolute */

			case 2: /* register */
				op2 = 3;		/* fix the operand addressing method to be the correct number required by the assembler */

				op2result = atoi(tempCod->operand2+1) << 8;
				/* get the register number from the operand by looking at the 2nd letter, then shift it 8 times to get it to the right location*/
				ARE = 0;		/* absolute */

			case 3: /* register index */
				op2 = 2;		/* fix the operand addressing method to be the correct number required by the assembler */

				op2result = (atoi(tempCod->operand2+4) << 2) + (atoi(tempCod->operand2+1) << 8);
				/* shift the bits in the first and second register given by 2 (second register) and 8 (first register) to get them to the right
				 * location */
				ARE = 0;		/* absolute */

			case 4: /* variable */
				op2 = 1;		/* fix the operand addressing method to be the correct number required by the assembler */

				if((tempSym = getLabel(tempCod->operand2)) != NULL)	/* if 2nd operand is in symbol list */
				{
					if(tempSym->isExtern == false)
					{
						op2result = (tempSym->address) << 2;	/* get the address of the symbol, shift it 2 times to get it to the right
											 * location */
						ARE = 2;	/* relocatable */
					}
					else
					{
						op2result = 0 << 2;				/* if symbol is external, address should be 0 */
						addExt(createExt(label, (tempCod->address)+1));	/* add the external symbol to the external list, 
												 * to be written later to the external file */
						ARE = 1;	/* external */
					}
				}
				else
				{
					/* error code 129: symbol is not in symbol list */
					errorFunction(fileName, lineNum, 129);					
				}
		   } /* end of operand switch check */

		addMachCode(createMach(tempCod->address, groupCode, opCode, op1, op2, 0));	/* add the command line to the machine code list */
		addMword(createMword((tempCod->address)+1, op2result+ARE));			/* add the operand information to the machine word list */
		}


		
	   } 

	   else
	   {
		/* error code 116: unknown command */
		errorFunction(fileName, lineNum, 116);	
	   }
	
	 
	}
	/* TO DO: */
	/* merge code table with data table */
	/* print linked lists to their relevant files (in hexadecimal), with relevant lengths of data\code */
	/* delete entry and\or external files, if they are not needed */
	/* free all lists */
	
}
