#include "structs.h"

int checkOneOperand(char *, char *, char *, int);
int checkTwoOperands(char *, char *, char *, char *, int);
int skipSpaces(char *, char *);
int frstOpChk(char *, char *, int);
void errorFunction(char *, int, int);


/* the first loop that: opens the given filename, reads through it, adding the relevant information to symbols linked-list, code table, and data table */
void startLoop(char * fileName, int errorCounter)
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

	char *p;
	char * tempString;
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
		continue;
	   }
	
	   
	   letter = buf[0];		/* check first letter of the line to see if it's a comment line */
	   if(letter == COMMENT_SIGN)
	   {
		/* comment line */
		continue;
	   }
	   
	   j = 0; /* reset index j after check */

	   tempString = strchr(buf,LABEL_SIGN);
	   if(tempString)		/* checks if the line contains a label */
	   {
		labelpos = strcspn(buf,(char *)LABEL_SIGN);				/* find position of ':' */
		strncpy(tempLabel,buf,labelpos);					/* copy part of buf before ':' to tempLabel */
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
			   continue;		/* we have found that ':' is between two '"'s so it's not part of a label, but part of a string */
			}

		   }
		}
		else if(strchr(tempLabel,LABEL_SIGN) == NULL)
		{
		    /* error code 100: ':' is not part of label */
		    errorCounter++;
		    errorFunction(fileName, lineNum, 100);
		}

		if(isalpha(letter)==0)
		{
		    /* error code 101: first letter is not a real letter */
		    errorCounter++;
		    errorFunction(fileName, lineNum, 101);
		}
		
		if(strlen(tempLabel)>MAX_LABEL_LENGTH)
		{
		   /* error code 102: label length is too big*/
		    errorCounter++;
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
			errorCounter++;
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
		        errorCounter++;
			errorFunction(fileName, lineNum, 104);
		   }		
		}

		j = skipSpaces(tempBuf2,tempWord);			/* find the next word in tempBuf2 (should be ".string" or ".data") */
		if(j == -1)
		{
			/* error code 105: line is empty (after label name if there was label) */
		        errorCounter++;
			errorFunction(fileName, lineNum, 105);
		}

  		if(strncmp(tempWord,DATA_WORD,5) == 0)			/* if the word is ".data" */
		{
			j = skipSpaces(tempBuf2+j+5,tempBuf2);		/* find the next word in buf (skipping previous spaces and ".data") */ 
			if(j == -1)
			{
				/* error code 105: line is empty after ".data" */
	   		        errorCounter++;
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
			 		        errorCounter++;
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
				        errorCounter++;
					errorFunction(fileName, lineNum, 106);
				}
				
			}
			else 		/* no number after ".data" */
			{
				/* error code 105: line is empty after ".data" */
			        errorCounter++;
				errorFunction(fileName, lineNum, 105);
			}
			
			
		}
		else if(strncmp(tempWord,STRING_WORD,7) == 0)		/* if the word is ".string" */
		{
			j = skipSpaces(tempBuf2+j+7,tempBuf2);		/* find the next word in buf (skipping previous spaces and ".string") */
			if(j == -1)
			{
				/* error code 105: line is empty after ".string" */
			        errorCounter++;
				errorFunction(fileName, lineNum, 105);
			}

			if(tempBuf2[0] != STR_FLAG)
			{
				/* error code 107: next word is not a string */
			        errorCounter++;
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
					        errorCounter++;
						errorFunction(fileName, lineNum, 108);
					}
				}
				else
				{
					/* error code 109: no closing '"' for the string */
				        errorCounter++;
					errorFunction(fileName, lineNum, 109);
				}

				i = 0;
				if(label_flag)
				{	
					newDat = createDat(DC, tempBuf[1+i], label, string); 	/* create a data node to hold next letter, 
												 * with the label if it exists */
					if(getLabel(label) == NULL)				/* check that a label with the same name doesn't exist already */ 
					{
						addData(newDat);
						DC++;
						i++;
					}
					else
					{
						/* error code 104: label name already exists */
					        errorCounter++;
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
		        errorCounter++;
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
		        errorCounter++;
			errorFunction(fileName, lineNum, 103);
		   }
	
		   check = strcspn(tempBuf,(char *)SPACE);		/* find the end of external symbol name */
		   strncpy(tempBuf2,tempBuf,check-1);		/* copy the symbol name to tempBuf2, without spaces and ":" */
		   strcat(tempBuf2,STRING_END);
		   j = skipSpaces(tempBuf,tempWord);
		   if(j != -1)
		   {
			/* error code 111: line isn't empty after external symbol name */
		        errorCounter++;
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
		        errorCounter++;
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
		        errorCounter++;
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
		        errorCounter++;
			errorFunction(fileName, lineNum, 103);
		}
	   }
	   

	   cmdNUM = checkCMD(tempWord);
	   switch(cmdNUM)
	   {
		case -1: /* error: no command */
			errorCounter++;	
			errorFunction(fileName, lineNum, 116);

		case 0:  /* mov */ 
			j = skipSpaces(buf+j,tempWord);
			
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j,op1name,op2name, fileName, lineNum);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorCounter++;	
				errorFunction(fileName, lineNum, 112);
			}

			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'mov' (immediate) */
				errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 104);				
			   }
			}
			IC += L;
			
			
		case 1:  /* cmp */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name, fileName, lineNum);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 104);				
			   }
			}
			IC += L;

		case 2:  /* add */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name, fileName, lineNum);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorCounter++;	
				errorFunction(fileName, lineNum, 112);
			}
			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'add' (immediate) */
				errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 3:  /* sub */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name, fileName, lineNum);
			if(op2result < 10)
			{
				/* error code 112: error in operand info */
				errorCounter++;	
				errorFunction(fileName, lineNum, 112);
			}
			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'sub' (immediate) */
				errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 4:  /* not */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'not' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address for command 'clr' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op2result = checkTwoOperands(buf+j, op1name, op2name, fileName, lineNum);
			if(op2result < 10)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
				errorFunction(fileName, lineNum, 115);
			}

			else if(op2result % 10 == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'lea' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			else if(op2result < 30 ) 
			{
				/* error code 114: origin operand is using illegal address methods for command 'lea' (immediate or register) */
				errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 104);					
			   }
			}
			IC += L;

		case 7:  /* inc */
			j = skipSpaces(buf+j,tempWord);
			if(j == -1)
			{
				/* error code 103: no more letters in line */
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'inc' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'dec' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);	
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address for command 'jmp' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'bne' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);	
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'red' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}

			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
				errorCounter++;	
				errorFunction(fileName, lineNum, 103);
			}

			op1result = checkOneOperand(buf+j, op2name, fileName, lineNum);
			if(op1result == 1)
			{
				/* error code 113: destination operand is using illegal address method for command 'jsr' (immediate) */
				errorCounter++;	
				errorFunction(fileName, lineNum, 113);
			}
			if(op1result == 0)
			{
				/* error code 115: no operand information recieved */
				errorCounter++;	
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
					errorCounter++;	
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
					errorCounter++;	
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
					errorCounter++;	
					errorFunction(fileName, lineNum, 104);			
				   }
				}
			 	IC += L;

		default: /* if not a command, or error */
			
			continue;
			/* Error not found */
			errorCounter++;	
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
