//
// Created by tal on 29/12/2019.
//
#include <stdlib.h>
#include <stdio.h>
#include "switch.c"
#include <string.h>
char *assemblyOfThatString(char *);
#define MAXCHAR 1000
typedef struct {
  int valOfCase;
  char *actionsInCase[100];
  char *actionsInAssembly[100];
  int numOfActions;
  int numOfAssActions;
  int isBreak;
  int placeInJumpTable;
} Case;

void swap(Case *case1, Case *case2) {
  Case tempCase = *case1;
  *case1 = *case2;
  *case2 = tempCase;
}
Case *bubbleSort(Case *cases, int n) {
  int i, j;
  for (i = 0; i < n - 1; i++)

    // Last i elements are already in place
    for (j = 0; j < n - i - 1; j++) {
      if (cases[j].valOfCase > cases[j + 1].valOfCase)
        swap(&(cases[j]), &(cases[j + 1]));
    }
  return cases;

}
int createJumpTable(FILE *file, Case *cases, int arrSize) {
  int amountOfCases = 2;
  int i, j;
  for (i = 0; i < arrSize; i++) {
    if (cases[i].valOfCase != cases[arrSize - 1].valOfCase)
      amountOfCases++;
  }
  int numOfCaseInTheJumpTable = 1;
  for (i = 0; i < arrSize; i++) {
    cases[i].placeInJumpTable = amountOfCases;
    if (cases[i].valOfCase != cases[arrSize - 1].valOfCase) {
      fprintf(file, ".L%d:\n", numOfCaseInTheJumpTable);
      cases[i].placeInJumpTable = numOfCaseInTheJumpTable;
      numOfCaseInTheJumpTable++;
      for (j = 0; j < cases[i].numOfActions; j++) {
        fprintf(file, "%s", cases[i].actionsInAssembly[j]);
      }
      if (cases[i].isBreak)
        fprintf(file, "jmp .L%d\n", amountOfCases);
    }
  }
  fprintf(file, ".L%d:\n", numOfCaseInTheJumpTable);
  for (j = 0; j < cases[arrSize - 1].numOfActions; j++) {
    fprintf(file, "%s", cases[arrSize - 1].actionsInAssembly[j]);
  }
  fprintf(file, ".L%d:\nret\n", amountOfCases);
  return amountOfCases;
}
void insertInformationToFile(FILE *file, Case *cases, int arrSize) {
  int temp = -1;
  int numOfCaseInTheJumpTable = 1;
  int amountOfCases;
  int i, j;
/*  for (int i = 0; i < arrSize; i++) {
    if (cases[i].valOfCase == cases[arrSize - 1].valOfCase) {
      fprintf(file, "cmpq $%d, %%rsi\n", (arrSize - 1));
      fprintf(file, "ja .L%d", (arrSize - 1));
    }
  }*/
  amountOfCases = createJumpTable(file, cases, arrSize);
  fprintf(file, ".section .rodata\n"
                ".align 8\n");
  cases = bubbleSort(cases, arrSize);
  fprintf(file, ".L%d:\n", arrSize + 3);
  for (i = 0; i < arrSize; i++) {
    if (cases[i].valOfCase != cases[arrSize - 1].valOfCase) {
      if (temp != -1) {
        for (j = 1; j < cases[i].valOfCase - temp; ++j) {
          fprintf(file, ".quad .L%d\n", amountOfCases - 1);
        }
      }
      fprintf(file, ".quad .L%d\n", cases[i].placeInJumpTable);
/*      for (int j = 1; j < cases[i + 1].valOfCase - cases[i].valOfCase; j++)
        fprintf(file, ".quad .L%d\n", amountOfCases);*/
      numOfCaseInTheJumpTable++;
      temp = cases[i].valOfCase;
    }
    // else
    //fprintf(file,".quad .L%d\n", amountOfCases);
  }
  fprintf(file, ".quad .L%d\n", numOfCaseInTheJumpTable + 1);
}
char *removeCharFromString(char *str, char ch) {
  int i, j;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] == ch)
      for (j = i; str[j] != '\0'; j++)
        str[j] = str[j + 1];
  }
  return str;
}
void fillAllArray(Case **cases, int arrSize) {
  int i, j;
  for (i = 0; i < arrSize; i++) {
    if (cases[i]->valOfCase == 0) {
      //(cases[i]->actionsInCase) = (cases[arrSize]->actionsInCase);
      for (j = 0; j < cases[arrSize]->numOfActions; j++) {
        cases[i]->actionsInAssembly[j] = (char *) malloc(50);
        cases[i]->actionsInAssembly[j] = cases[arrSize]->actionsInAssembly[j];
      }
    }
  }
}
void checkIfItsMinOrMAx(int numOfCase, int *min, int *max) {
  if (numOfCase < *min)
    *min = numOfCase;
  if (numOfCase > *max)
    *max = numOfCase;
}
char *assemblyOfThatString(char *stringToCheck) {
  char *temp = (char *) malloc(50);
  if (strcmp(stringToCheck, "*p2") == 0) {
    return "%%rsi";
  }
  if (strcmp(stringToCheck, "*p1") == 0) {
    return "%%rdi";
  }
  if (strcmp(stringToCheck, "result") == 0) {
    return "%%rax";
  } else {
    strcpy(temp, "$");
    strcat(temp, stringToCheck);
    return temp;
  }
}
void returnRowInAssembly(char **str, Case *assemblyField) {
  char *temp1 = (char *) malloc(50);
  char *temp2;
  char *temp3 = (char *) malloc(50);
  removeCharFromString(*str, ' ');
  if (strstr(*str, "<<=") != NULL) {
    temp1 = strtok(*str, "<<=");
    temp2 = temp1 + strlen(temp1) + 3;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\nsalq %%edx");
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, ">>=") != NULL) {
    temp1 = strtok(*str, ">>=");
    temp2 = temp1 + strlen(temp1) + 3;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\nsarq %%edx");

    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "+=") != NULL) {
    strcpy(temp1, strtok(*str, "+="));
    temp2 = *str + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\naddq %%edx");
    strcat(*str, ", ");
    strcat(*str, assemblyOfThatString(temp1));
    strcat(*str, "\n");
  } else if (strstr(*str, "-=") != NULL) {
    temp1 = strtok(*str, "-=");
    temp2 = temp1 + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\nsubq %%edx");
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "*=") != NULL) {
    temp1 = strtok(*str, "*=");
    temp2 = *str + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\nimuq %%edx");
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "=") != NULL) {
    temp1 = strtok(*str, "=");
    strtok(*str, "=");
    temp2 = temp1 + strlen(temp1) + 1;
    strtok(temp2, ";");
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", %%edx\nmovq %%edx");
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  }
}
void makeTableStartFrom0(Case **cases, int sizeOfArr, int min) {
  //make the table start from 0.
  int i;
  for (i = 0; i <= sizeOfArr; i++) {
    (*cases)[i].valOfCase -= min;
  }
}
void updateAssemblyFields(Case **cases, int sizeOfArr) {
  int i, j;
  char *temp = (char *) malloc(1000);
  for (i = 0; i < sizeOfArr; i++) {
    for (j = 0; j < (*cases)[i].numOfActions; j++) {
      strcpy(temp, (*cases)[i].actionsInCase[j]);
      if (temp[strlen(temp) - 1] == ';') {
        temp[strlen(temp) - 1] = '\0';
      }
      returnRowInAssembly(&temp, &((*cases)[i]));
      (*cases)[i].actionsInAssembly[j] = (char *) malloc(strlen(temp));
      strcpy((*cases)[i].actionsInAssembly[j], temp);
    }
  }
}
void readFromFile(FILE *fToR, FILE *fToW) {
  Case *cases = NULL;
  int numOfCases = 0;
  int flagForInit = 0, defaultFlag = 0;
  int numOfCase;
  char *temp;
  int max = -99999;
  int min = 0;
  int arrSize = 0;
  int valOfCurrentCase = 0;
  int i = -1, j;
  int flag = 0;
  char *str = (char *) malloc(MAXCHAR);
  fToW = fopen("switch.s", "w");
  fprintf(fToW, ".section .text\n"
                ".globl switch2\n"
                "switch2:\n"
                "movl $0, %%rax\n");
  //reading from file
  fToR = fopen("switch.c", "r");
  if (fToR == NULL || fToW == NULL) {
    printf("Could not open file\n");
    exit(2);
  }
  while (fgets(str, MAXCHAR, fToR) != NULL) {
    //check for the first vision of "case"
    if (strstr(str, "case") != NULL) {
      temp = strtok(str, " ");
      temp = strtok(NULL, " ");
      temp = strtok(temp, ":");
      numOfCase = atoi(temp);
      if (!flagForInit) {
        flagForInit = 1;
        min = numOfCase;
      }
      checkIfItsMinOrMAx(numOfCase, &min, &max);
    }
  }
  fprintf(fToW, "subq %d, %%rsi\n", min);
  arrSize = max - min + 2;
  fprintf(fToW, "*.L%d(,%%rsi,8)\n", arrSize + 3);
  fclose(fToR);
  fToR = fopen("switch.c", "r");
  cases = (Case *) malloc(sizeof(Case) * (max - min + 2));
  for (j = 0; j < max - min + 2; j++) {
    cases[j].numOfActions = 0;
    cases[j].numOfAssActions = 0;
    cases[j].valOfCase = max + 1;
  }
  while (fgets(str, MAXCHAR, fToR) != NULL) {
    //check for the first vision of "case"
    if (strstr(str, "case") != NULL) {
      temp = strtok(str, " ");
      temp = strtok(NULL, " ");
      temp = strtok(temp, ":");
      flag = 1;
    }
    //temp = "";
    //what to do when i see "case"
    if (flag) {
      if (strstr(str, "case") != NULL) {
        i++;
        //cases = (Case *) realloc(cases, numOfCases * sizeof(Case) + 1);
        valOfCurrentCase = (atoi(temp)) - min;
        cases[i].valOfCase = atoi(temp);
        //cases[valOfCurrentCase].numOfActions = 0;
        cases[i].isBreak = 0;
        numOfCases++;
        //what to do when i see break!
      } else if (strstr(str, "break") != NULL) {
        cases[i].isBreak = 1;
        //case for default!
      } else if (strstr(str, "default") != NULL) {
        defaultFlag = 1;
        valOfCurrentCase = max - min + 1;
        cases[max - min + 1].valOfCase = max + 1;
        cases[max - min + 1].isBreak = 0;
        //cases[max - min + 1].numOfActions = 0;
      } else if (strstr(str, "}") != NULL) {
        break;
      } else {
        if (defaultFlag == 0) {
          cases[i].actionsInCase[cases[i].numOfActions] = (char *) malloc(strlen(str) + 1);
          strcpy(cases[i].actionsInCase[cases[i].numOfActions], str);
          cases[i].numOfActions++;
          cases[i].numOfAssActions++;
        } else {
          cases[valOfCurrentCase].actionsInCase[cases[valOfCurrentCase].numOfActions] =
              (char *) malloc(strlen(str) + 1);
          strcpy(cases[valOfCurrentCase].actionsInCase[cases[valOfCurrentCase].numOfActions], str);
          cases[valOfCurrentCase].numOfActions++;
          cases[valOfCurrentCase].numOfAssActions++;
        }
      }
    }
  }
  updateAssemblyFields(&cases, arrSize);
  makeTableStartFrom0(&cases, arrSize, min);
  insertInformationToFile(fToW, cases, arrSize);
  fclose(fToR);
  fclose(fToW);
}
int main() {
  FILE *fToRead = NULL;
  FILE *fToWrite = NULL;
  readFromFile(fToRead, fToWrite);
  //fclose(fToRead);
  //fclose(fToWrite);
  return 0;
}