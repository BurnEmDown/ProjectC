#include "structs.h"

int checkOneOperand(char *, char *);
int checkTwoOperands(char *, char *, char *);
int skipSpaces(char *, char *);
int frstOpChk(char *);


/* the first loop that: opens the given filename, reads through it, adding the relevant information to symbols linked-list, code table, and data table */
void startLoop(char * fileName)
{

	FILE *fp;	
	int lineNum = 0;			/* holds the current line number */
	int j,i;				/* index variable */
	int length;
	bool label_flag;			/* holds info on whether a label exists in line or not */
	char buf[MAX_BUF];			/* a buffer to hold the current line read */
	char tempBuf[MAX_BUF];			/* a temp buffer to hold a part of buf temporarily */
	char tempBuf2[MAX_BUF];			/* another temp buffer to hold a part of buf temporarily */
	char tempLabel[MAX_BUF];		/* a buffer for holding potential label name */
	char label[MAX_LABEL_LENGTH];   	/* holds current label if it exists */
	char tempWord[MAX_LABEL_LENGTH];	/* a temp buffer to hold a temporary word from buf */
	int tempNum;				/* a temp int to hold numbers */
	char sourceFile[MAX_BUF];
	int code_length;
	int data_length;

	char letter;
	char * buf2;		
	int labelpos;			/* label position in line */ 
	int cmdNUM;			/* command type */
	int op1result;
	char * op1name;
	int op2result;
	char * op2name;
	int check;
	int L; 				/* the number of words required by the line */
	symp newSym;
	datp newDat;
	codp newCod;
	datp tempDat;

	strcpy(sourceFile,fileName);
	strcat(sourceFile,SOURCE_FILE);	/* add ".as" to the end of the filename, to address to the proper file */
	strcat(sourceFile,STRING_END);

	fp = fopen(sourceFile, "r");

	if(!fp)
	{
	   printf("cannot open file\n");
	   exit(0);
	}

	while(fgets(buf,MAX_BUF,fp) != NULL) 	/* reads a line from the text file */
	{
	   strcat(buf,STRING_END);
	   lineNum++;
	   /* check for empty or comment line */
	   j = 0; /* reset index j before check */

	   if((j = skipSpaces(buf,tempWord)) == -1)
	   {
		/* empty line */
		break;
	   }

	   /*--- MAKE SURE TO CHECK IF J IS NEEDED IN NEXT LINE ---*/
	   /*--- MAKE SURE TO CHECK IF J IS NEEDED IN NEXT LINE ---*/ 
	   /*--- MAKE SURE TO CHECK IF J IS NEEDED IN NEXT LINE ---*/

	   
	   letter = buf[j];		/* check first non-space letter of the line to see if it's a comment line */
	   if(letter == COMMENT_SIGN)
	   {
		break;
	   }
	   j = 0; /* reset index j after check */
	   
	   if((labelpos = strcspn(buf,(char *)LABEL_SIGN)) != (strlen(buf))) 		/* checks if the line contains a label */
	   {
		strncpy(tempLabel,buf,labelpos);
		strcat(tempLabel,STRING_END);
		/* checks for valid label */
		if(strchr(tempLabel,(char)STRING_SIGN) != NULL)				/* find if there is a '"' in the word */
		{
		   check = strcspn(tempLabel,(char *)LABEL_SIGN);			/* find the location of ':' */
		   strncpy(tempWord,tempLabel,length);					/* copy the part of the word before ':' into tempWord, to check */
		   strcat(tempWord,STRING_END);
		   if(strchr(tempWord,STRING_SIGN) == strrchr(tempWord,STRING_SIGN))	/* check that only one '"' exists before ':' */
		   {
			if(strchr(tempLabel+check,(char)LABEL_SIGN) != NULL)
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
		
		if(strlen(tempLabel)>MAX_LABEL_LENGTH)
		{
		   /* error code 102: label length is too big*/
		   errorFunction(fileName, lineNum, 102);
		}
		
		label_flag = true;
		strncpy(label,tempLabel,(strlen(tempLabel)-1));			/* copy label name to label string, without the ":" at the end */
		strcat(label,STRING_END);

	   }

	   else
	   {
	     	label_flag = false;
	   }

	   if(strstr(buf,DATA_WORD) != NULL || strstr(buf,STRING_WORD) != NULL)
	   {
		/* step 6+7 in algorithm */  
		j = skipSpaces(buf,tempWord);			/* find the first word in line */

		if(j == -1)
		{
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		}

		strcpy(tempBuf2,buf+j);				/* copy buf (without first spaces) to tempBuf2 (will be recopied if buf has a label) */
		strcat(tempBuf2,STRING_END);
	
		if(label_flag)					/* if label exists */
		{	
		   check = strcspn(buf+j,(char *)SPACE);		/* find the end of label name */
		   strncpy(tempBuf,buf+j,check-1);			/* copy the label name to tempbuf, without spaces and ":" */
		   strcat(tempBuf,STRING_END);
		   strcpy(tempBuf2,buf+check+j);			/* copy the rest of buf to tempBuf2 */
		   strcat(tempBuf2,STRING_END);
		   newSym = createSym(tempBuf, DC, false, instruction);	/* create a symbol with the label name */
		  
	           if(getLabel(tempBuf) == NULL)			/* check that a label with the same name doesn't exist already */
		   {
		   	addSymbol(newSym);				/* add the new external symbol to the symbol list */
		   }

		   else
		   {
			/* error code 104: label name already exists */
			errorFunction(fileName, lineNum, 104);
		   }		
		}

		j = skipSpaces(tempBuf2,tempWord);			/* find the next word in tempBuf2 (should be ".string" or ".data") */
		if(j == -1)
		{
			/* error code 105: line is empty (after label name if there was label) */
			/errorFunction(fileName, lineNum, 105);
		}

  		if(strncmp(tempWord,DATA_WORD,5) == 0)			/* if the word is ".data" */
		{
			j = skipSpaces(tempBuf2+j+5,tempBuf2);		/* find the next word in buf (skipping previous spaces and ".data") */ 
			if(j == -1)
			{
				/* error code 105: line is empty after ".data" */
				errorFunction(fileName, lineNum, 105);
			}
			
			if(sscanf(tempBuf2, "%d", &tempNum) != 0)
			{
				if(label_flag)				/* create a data node to hold first number, with the label if it exists */
				{	
					newDat = createDat(DC, tempNum, label , data);
					if(getLabel(label) == NULL)				/* check that a label with the same name doesn't exist already */ 
					{
						addData(newDat);	
						DC++;
					}
					else
					{
						/* error code 104: label name already exists */
						errorFunction(fileName, lineNum, 104);
					}
				}
				else
				{
					newDat = createDat(DC, tempNum, "" , data);
					addData(newDat); 	
					DC++;
				}

				while((check = strcspn(tempBuf2,(char *)COMMA)) != strlen(tempBuf2)) 	/* as long as there are no more commas (numbers) in line */
				{
					j = skipSpaces(tempBuf2+check,tempBuf2);		/* find the next number in buf (skipping previous numbers) */ 
					if(j == -1)
					{
						/* line is empty after previous number */
						
					}

					if(sscanf(tempBuf2, "%d", &tempNum) != 0)		/* add the number to data list */
					{
						newDat = createDat(DC, tempNum, "" , data);
						addData(newDat); 	
						DC++;
					}
				}

				/* no more commas, there should still be one more number */

				if(sscanf(tempBuf2, "%d", &tempNum) != 0)		/* add the number to data list */
				{
					newDat = createDat(DC, tempNum, "" , data);
					addData(newDat); 	
					DC++;
				}
				else
				{
					/* error code 106: no number after last comma */
					errorFunction(fileName, lineNum, 106);
				}
				
			}
			else 		/* no number after ".data" */
			{
				/* error code 105: line is empty after ".data" */
				errorFunction(fileName, lineNum, 105);
			}
			
			
		}
		else if(strncmp(tempWord,STRING_WORD,7) == 0)		/* if the word is ".string" */
		{
			j = skipSpaces(tempBuf2+j+7,tempBuf2);		/* find the next word in buf (skipping previous spaces and ".string") */
			if(j == -1)
			{
				/* error code 105: line is empty after ".string" */
				errorFunction(fileName, lineNum, 105);
			}

			if(tempBuf2[0] != STR_FLAG)
			{
				/* error code 107: next word is not a string */
				errorFunction(fileName, lineNum, 107);
			}
			else
			{
				buf2 = strchr(tempBuf2+1,(char)STR_FLAG);
				strcpy(tempBuf2,buf2);
				strcat(tempBuf2,STRING_END);
				if(tempBuf2 != NULL) 	/* find end of string word, copy it to tempbuf */
				{
					j = skipSpaces(tempBuf,tempWord);		/* find the next word in buf (skipping previous spaces and ".string") */
					if(j != -1)
					{
						/* error code 108: line isn't empty after the string */
						errorFunction(fileName, lineNum, 108);
					}
				}
				else
				{
					/* error code 109: no closing '"' for the string */
					errorFunction(fileName, lineNum, 109);
				}

				i = 0;
				if(label_flag)
				{	
					newDat = createDat(DC, tempBuf[1+i], label, string); 	/* create a data node to hold next letter, with the label if it exists */
					if(getLabel(label) == NULL)				/* check that a label with the same name doesn't exist already */ 
					{
						addData(newDat);
						DC++;
						i++;
					}
					else
					{
						/* error code 104: label name already exists */
						errorFunction(fileName, lineNum, 104);
					}

				}

				while(strncmp((char *)tempBuf+1+i,(char *)STR_FLAG,1) != 0)
				{
					newDat = createDat(DC, tempBuf[1+i], "" , string); 	/* create a data node to hold next letter */
					addData(newDat);
					DC++;
					i++;
				}

				newDat = createDat(DC, 0, "" , string); 	/* create a data node of only "0" to signal end of string */
				addData(newDat);
				DC++;
								
			}
		}
		else
		{
			/* error code 110: first word (after possible label) isnt ".data" or ".string" */
			errorFunction(fileName, lineNum, 110);
		}

		
	   }
		
	   if(strstr(buf,ENTRY_WORD) != NULL || strstr(buf,EXTERN_WORD) != NULL)
	   {
		strcpy(tempBuf,strchr(buf,(char)SPACE));	/* find the end of instruction name */
		strcat(tempBuf,STRING_END);

		/* step 9+10 in algorithm */
		if(strstr(buf,EXTERN_WORD) != NULL)		/* if it's external instruction */
		{
		   j = skipSpaces(tempBuf,tempBuf);		/* check that line isn't empty after ".extern" */
		   if(j == -1)
		   {
			/* error code 103: no more letters in line */
			errorFunction(fileName, lineNum, 103);
		   }
	
		   check = strcspn(tempBuf,(char *)SPACE);		/* find the end of external symbol name */
		   strncpy(tempBuf2,tempBuf,check-1);		/* copy the symbol name to tempBuf2, without spaces and ":" */
		   strcat(tempBuf2,STRING_END);
		   j = skipSpaces(tempBuf,tempWord);
		   if(j != -1)
		   {
			/* error code 111: line isn't empty after external symbol name */
			errorFunction(fileName, lineNum, 111);
		   }

		   newSym = createSym(tempBuf2, 0, true, instruction);	/* create a symbol with the external symbol name */
		   if(getLabel(tempBuf2) == NULL)			/* check that a label with the same name doesn't exist already */
		   {
		   	addSymbol(newSym);				/* add the new external symbol to the symbol list */
		   }
		   else
		   {
			/* error code 104: label name already exists */
			errorFunction(fileName, lineNum, 104);
		   }
		}
			 	
	   }
		
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
	   switch(cmdNUM)
	   {
		case -1: /* error: no command */
		case 0:  /* mov */ 
			j = skipSpaces(buf+j,tempWord);
			
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j,op1name,op2name);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorFunction(fileName, lineNum, 112);
			}

			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'mov' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op2result == 42) 	/* both operands are registers: they require only 1 word */
			{
				L = 2;
			}

			else
			{
				L = 3;
			}

			newCod = createCod(IC, cmdNUM, op1name, op2name, (op2result/10), (op2result % 10), lineNum);
			addCodeNode(newCod);
		  	if(label_flag)
			{
			   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
			   if(getLabel(label) == NULL)
			   {
				addSymbol(newSym);				/* add the new symbol to the symbol list */
			   }
			   else
			   {
				/* error code 104: label name already exists */
				errorFunction(fileName, lineNum, 104);				
			   }
			}
			IC += L;
			
			
		case 1:  /* cmp */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorFunction(fileName, lineNum, 112);
			}

			if(op2result == 42) 	/* both operands are registers: they require only 1 word */
			{
				L = 2;
			}
			else
			{
				L = 3;
			}

			newCod = createCod(IC, cmdNUM, op1name, op2name, (op2result/10), (op2result % 10), lineNum);
			addCodeNode(newCod);
		  	if(label_flag)
			{
			   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
			   if(getLabel(label) == NULL)
			   {
				addSymbol(newSym);				/* add the new symbol to the symbol list */
			   }
			   else
			   {
				/* error code 104: label name already exists */
				errorFunction(fileName, lineNum, 104);				
			   }
			}
			IC += L;

		case 2:  /* add */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorFunction(fileName, lineNum, 112);
			}
			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'add' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op2result == 42) 	/* both operands are registers: they require only 1 word */
			{
				L = 2;
			}
			else
			{
				L = 3;
			}

			newCod = createCod(IC, cmdNUM, op1name, op2name, (op2result/10), (op2result % 10), lineNum);
			addCodeNode(newCod);
		  	if(label_flag)
			{
			   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
			   if(getLabel(label) == NULL)
			   {
				addSymbol(newSym);				/* add the new symbol to the symbol list */
			   }
			   else
			   {
				/* error code 104: label name already exists */
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 3:  /* sub */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorFunction(fileName, lineNum, 112);
			}
			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'sub' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}
			
			else if(op2result == 42) 	/* both operands are registers: they require only 1 word */
			{
				L = 2;
			}
			else
			{
				L = 3;
			}

			newCod = createCod(IC, cmdNUM, op1name, op2name, (op2result/10), (op2result % 10), lineNum);
			addCodeNode(newCod);
		  	if(label_flag)
			{
			   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
			   if(getLabel(label) == NULL)
			   {
				addSymbol(newSym);				/* add the new symbol to the symbol list */
			   }
			   else
			   {
				/* error code 104: label name already exists */
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 4:  /* not */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'not' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}
			else
			{
				L = 2; /* 'not' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
			 	   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}
			
		case 5:  /* clr */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address for command 'clr' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}
			else
			{
				L = 2; /* 'clr' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
			  	   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}

		case 6:  /* lea */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name);
			if(op2result < 10)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}

			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'lea' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			else if(op2result < 30 ) 
			{
				/* error code 114: origin operand is using illegal address methods for command 'lea' (immediate or register) */
				errorFunction(fileName, lineNum, 114);
			}

			L = 3; /* 'lea' command can't use 2 registers, so it always requires 3 word lines */

			newCod = createCod(IC, cmdNUM, op1name, op2name, (op2result/10), (op2result % 10), lineNum);
			addCodeNode(newCod);
		  	if(label_flag)
			{
			   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
			   if(getLabel(label) == NULL)
			   {
				addSymbol(newSym);				/* add the new symbol to the symbol list */
			   }
			   else
			   {
				/* error code 104: label name already exists */
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 7:  /* inc */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'inc' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}
			else
			{
				L = 2; /* 'inc' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
			 	   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}
			
		case 8:  /* dec */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'dec' (immediate) */
				errorFunction(fileName, lineNum, 113);	
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}

			else
			{
				L = 2; /* 'dec' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}

			
		case 9:  /* jmp */ 
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address for command 'jmp' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}

			else
			{
				L = 2; /* 'jmp' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
			  	if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);				
				   }
				}
				IC += L;
			}
			
		case 10: /* bne */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'bne' (immediate) */
				errorFunction(fileName, lineNum, 113);	
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}

			else
			{
				L = 2; /* 'bne' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
			  	if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}
			
		case 11: /* red */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'red' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}

			else
			{
				L = 2; /* 'red' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
			  	if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}
			
		case 12: /* prn */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}
			else
			{
				L = 2; /* 'prn' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
			  	if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);					
				   }
				}
				IC += L;
			}

			
		case 13: /* jsr */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'jsr' (immediate) */
				errorFunction(fileName, lineNum, 113);
			}
			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorFunction(fileName, lineNum, 115);
			}
			else
			{
				L = 2; /* 'jsr' command always requires 2 words */
				newCod = createCod(IC, cmdNUM, "", op2name, 0, op1result, lineNum);
				addCodeNode(newCod);
			  	if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);				
				   }
				}
				IC += L;
			}
			
		case 14: /* rts */

				L = 1; /* 'rts' command always requires 1 word */
				newCod = createCod(IC, cmdNUM, "", "", 0, 0, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);				
				   }
				}
				IC += L;

	   	case 15: /* stop */
			
				L = 1; /* 'stop' command always requires 1 word */
				newCod = createCod(IC, cmdNUM, "", "", 0, 0, lineNum);
				addCodeNode(newCod);
		  		if(label_flag)
				{
				   newSym = createSym(label, IC, false, action);	/* create a symbol with the label name */
				   if(getLabel(label) == NULL)
				   {
				   	addSymbol(newSym);				/* add the new symbol to the symbol list */
				   }
				   else
				   {
					/* error code 104: label name already exists */
					errorFunction(fileName, lineNum, 104);			
				   }
				}
			 	IC += L;

		default: /* if not a command, or error */
			
			continue;
			/* Error not fount */
			errorFunction(fileName, lineNum, 000);	
	   }

	lineNum++;
	
	/* end of reading current line */
	
	 
	} /* end of while loop that reads each line */

	code_length = IC;
	data_length = DC;

	updateSymbolAdress(IC);

	i = 0;

	while(i<=DC)		/* translate data list to machine data list */
	{
		tempDat = getDataInfo(i);
		addMdata(createMdata(i, (tempDat->value)));
		i++;
	}
}

int skipSpaces(char * str, char * toWord)		/* a function for "skipping" spaces in a given string */
{
	int i = 0;
	while(isspace(str+i)) 	 			/* as long as current letter pointed to in the string is space */
	{
	   i++;			 			/* skip a letter */
	   if(strncmp(str+i,(char *)NEWLINE,1) == 0) 	/* check for making sure we didn't reach end of string */
		return -1;				/* if we did, return -1 to signal */
	}
	toWord = strcpy(toWord,str+i);  		/* copy the next "word" to the desired string */
	strcat(toWord,STRING_END);
	
	return i;					/* return the index for the next non-space character in str */
}



int checkTwoOperands(char * str,char * op1name, char * op2name)			/* a function to check the operands of a command that needs 2 operands. assume these 2 operands exist in the string and the given string begins with the first operand and not a space */
{
	int i = 0;
	int j = 0;
	int length = 0;
	
	/* bool variables used as flags by the function */
	
	bool op1directNum = false;
	bool op1variable = false;
	bool op1regIndx = false;
	bool op1reg = false;
	

	int op1type = 0;	/* variables for address method types. 0 is not found or error, 1 is immediate number */
	int op2type = 0;	/* 2 is register, 3 is register-index, and 4 is variable */

	char * op1;		/* a string to hold first operand information */
	char * op2;		/* a string to hold second operand information */
	char * garbage; 	/* a char array to hold garbage  */
	

	if(str[i] == IMD_FLAG)
	{

	   if((garbage = strchr(str,COMMA)) == NULL)	/* check line for comma */
	   {
			/* error code 117: no comma separating operands */
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
		   errorFunction(fileName, lineNum, 118);	
		}
	   }

	   if(frstOpChk(op1) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
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
		errorFunction(fileName, lineNum, 120);	
	   }

	   else if(str[i+2] != SPACE && str[i+2] != COMMA) 	/* check if next letter in str is not a valid char */
	   {
		if(str[i+2] == LEFT_BRACKET && str[i+3] == REG_FLAG && isdigit(str[i+4])) 	/* check if using index address method by checking for expression like "r1[r2]" */
		{
		   if(str[i+1] % 2 == 1)	/* check to make sure first reg is odd number */
		   {
			if(str[i+5] == RIGHT_BRACKET)
			{
			   if(str[i+6] == SPACE || str[i+6] == COMMA)
			   {
			   	op1type = 3; 		/* signal that first operand is a register-index adress method */

			  	if((garbage = strchr(str,COMMA)) == NULL)
			   	{
					/* error code 117: no comma separating operands */
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
			      errorFunction(fileName, lineNum, 119);	
			   }

			}
			else
			{
			    /* error code 121: no closing right bracket */
			    errorFunction(fileName, lineNum, 121);	
			}

		   }
		   else
		   {
			/* error code 122: first register must be odd number */
			errorFunction(fileName, lineNum, 122);
	   	   }
		   
		}
		else
		{
		   /* error code 123: not a valid char */
		   errorFunction(fileName, lineNum, 123);
		}
	   }
	   else
	   {
	   	op1type = 2; 		/* signal that first operand is a register */
		if((garbage = strchr(str,(char)COMMA)) == NULL)
		{
			/* error code 117: no comma separating operands */
			errorFunction(fileName, lineNum, 117);	
		}

		length = strcspn(str,(char *)COMMA);		/* find the beginning of the next operand info */
		strcpy(op2,str+length+1); 		/* copy it to op2, skipping the comma */
		strcat(op2,STRING_END);
		strncpy(op1,str,length);		/* copy the first part to op1 (with the comma, for the check) */
		strcat(op1,STRING_END);
		
	   }

	   if(frstOpChk(op1) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
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
	   if((garbage = strchr(str,(char)COMMA)) == NULL)
	   {
		/* error code 117: no comma separating operands */
		errorFunction(fileName, lineNum, 117);	
	   }

	   length = strcspn(str,(char *)COMMA);	/* find the beginning of the next operand info */
	   strcpy(op2,str+length+1); 		/* copy it to op2, skipping the comma */
	   strcat(op2,STRING_END);
	   strncpy(op1,str,length);		/* copy the first part to op1 (with the comma, for the check) */
	   strcat(op1,STRING_END);
	   
	   

	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */
	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */
	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */

	   
	   if(frstOpChk(op1) == -1)
	   {
		/* error code 119: non-space exists between first operand and comma */
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
		errorFunction(fileName, lineNum, 124);
	}

	op2type = checkOneOperand(op2, op2name);		/* send the second operand to be checked by the single operand check function */
	if(op2type == 0)	/* operand type not found for second operand */
	{
	   /* error code 124: error with 2nd operand */
	   errorFunction(fileName, lineNum, 124);
	}
	
	if(op1type == 0)	/* operand type not found for first operand */
	{
		/* error code 125: no valid first operand found */		
		errorFunction(fileName, lineNum, 125);
		return;
	}

	else
	{
	   op1type *= 10; 		/* turn the result of op1type to "tens" */
	   op1type += op2type;  	/* then add op2type, and we get both types in one integer to work with*/
	   return op1type;		/* return the result. the origin operand is in the "tens" digit, and the destination operand is in the "ones" digit */
	}

}

int frstOpChk(char * op1) /* a function that recieves the first operand of a two-operand command, and checks that there are no values between it and the comma separating it from the second operand (should be the last letter of op1) */
{

	int j = 0;
	int i = 0;
	char * garbage;
	
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


int checkOneOperand(char * str, char * opName) /* a function to check the validity and type of a single operand, which can be a second operand of a two-operand command, or the single operand of a one-operand command. assumes the string given starts with the first letter of the operand. return 1 for immediate number, 2 for register, 3 for register-index, and 4 for variable. 0 is given if no type is detected */
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
		   errorFunction(fileName, lineNum, 125);
		}

	   }

	   j = skipSpaces(str+i, garbage);
	   if(j != -1)
	   {
		/* error code 126: non-space letter between operand and newline */
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
		errorFunction(fileName, lineNum, 120);
	   }

	   if(str[2] != SPACE) 	/* check if next letter in str is not a valid char */
	   {
		if(str[2] == LEFT_BRACKET && str[3] == REG_FLAG && isdigit(str[4])) 	/* check if using index address method by checking for expression like "r1[r2]" */
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
					errorFunction(fileName, lineNum, 126);
			   	}

				opType = 3; 		/* register index address method found with no errors */   
			   }
			   else
			   {
			      /* error code 126: non-space non-comma character after first operand */
			      errorFunction(fileName, lineNum, 126);
			   }
			}
			else
			{
			    /* error code 121: no closing right bracket */
			    errorFunction(fileName, lineNum, 121);
			}
		   }
		   else
		   {
			/* error code 120: first register must be odd number */
			errorFunction(fileName, lineNum, 120);
	   	   }
		   
		}
		else
		{
		   /* error code 123: not a valid char */
		   errorFunction(fileName, lineNum, 123);
		}

	   }

	   else 	/* there was a space after register number */
	   {
		j = skipSpaces(str, garbage);
	   	if(j != -1)
	   	{
			/* error code 126: non-space letter between operand and newline */
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
		/errorFunction(fileName, lineNum, 126);
	   }
	      

	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */
	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */
	   /* ADD HANDLING THE FIRST OPERAND AS A VARIABLE */


	   i = skipSpaces(str, garbage);
	   if(i != -1)
	   {
			/* error code 113: line isn't empty after first operand */
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
