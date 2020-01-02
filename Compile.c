//
// Created by tal on 29/12/2019.
//
#include <stdlib.h>
#include <stdio.h>
#include "switch.c"
#include <string.h>
char *assemblyOfThatString(char *);
void returnRowInAssembly(char **str);
#define MAXCHAR 1000
typedef struct {
  int valOfCase;
  char *actionsInCase[100];
  char *actionsInAssembly[100];
  int numOfActions;
  int isBreak;
} Case;
void createJumpTable(FILE *file, Case *cases, int arrSize) {
  int numOfCaseInTheJumpTable = 1;
  for (int i = 0; i < arrSize; i++) {
    if (cases[i].valOfCase != cases[arrSize - 1].valOfCase) {
      fprintf(file, ".L%d:\n", numOfCaseInTheJumpTable);
      numOfCaseInTheJumpTable++;
      for (int j = 0; j < cases[i].numOfActions; j++) {
        fprintf(file, "%s", cases[i].actionsInAssembly[j]);
      }
      if (cases[i].isBreak)
        fprintf(file, "jmp .L%d\n", arrSize);
    }
  }
  fprintf(file, ".L%d:\nret", arrSize);

}
void insertInformationToFile(FILE *file, Case *cases, int arrSize) {
  int numOfCaseInTheJumpTable = 1;
  for (int i = 0; i < arrSize; i++) {
    if (cases[i].valOfCase == cases[arrSize - 1].valOfCase) {
      fprintf(file, "cmpq $%d, %%rsi\n", (arrSize - 1));
      fprintf(file, "ja .L%d", (arrSize - 1));

    }
  }
}
char *removeCharFromString(char *str, char ch) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == ch)
      for (int j = i; str[j] != '\0'; j++)
        str[j] = str[j + 1];
  }
  return str;
}
void fillAllArray(Case **cases, int arrSize) {
  for (int i = 0; i < arrSize; i++) {
    if (cases[i]->valOfCase == 0) {
      //(cases[i]->actionsInCase) = (cases[arrSize]->actionsInCase);
      for (int j = 0; j < cases[arrSize]->numOfActions; j++) {
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
void returnRowInAssembly(char **str) {
  char *temp1 = (char *) malloc(50);
  char *temp2 = (char *) malloc(50);
  char *temp3 = (char *) malloc(50);
  removeCharFromString(*str, ' ');
  if (strstr(*str, "<<=") != NULL) {
    temp1 = strtok(*str, "<<=");
    temp2 = temp1 + strlen(temp1) + 3;
    strtok(temp2, ";");
    strcpy(temp3, "salq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, ">>=") != NULL) {
    temp1 = strtok(*str, ">>=");
    temp2 = temp1 + strlen(temp1) + 3;
    strtok(temp2, ";");
    strcpy(temp3, "sarq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "+=") != NULL) {
    strcpy(temp1, strtok(*str, "+="));
    temp2 = *str + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "addq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    strcat(*str, ", ");
    strcat(*str, assemblyOfThatString(temp1));
    strcat(*str, "\n");
  } else if (strstr(*str, "-=") != NULL) {
    temp1 = strtok(*str, "-=");
    temp2 = temp1 + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "subq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "*=") != NULL) {
    temp1 = strtok(*str, "*=");
    temp2 = *str + strlen(temp1) + 2;
    strtok(temp2, ";");
    strcpy(temp3, "imuq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  } else if (strstr(*str, "=") != NULL) {
    temp1 = strtok(*str, "=");
    strtok(*str, "=");
    temp2 = temp1 + strlen(temp1) + 1;
    strtok(temp2, ";");
    //strcpy(*str,strcat(strcat("movq", assemblyOfThatString(temp2)),assemblyOfThatString(temp1)));
    //*str = (char *) realloc(*str, strlen(*str) + 10);
    strcpy(temp3, "movq ");
    *str = strcat(temp3, assemblyOfThatString(temp2));
    *str = strcat(*str, ", ");
    *str = strcat(*str, assemblyOfThatString(temp1));
    *str = strcat(*str, "\n");
  }
}
void makeTableStartFrom0(Case **cases, int sizeOfArr) {
  //make the table start from 0.
  for (int i = 0; i < sizeOfArr; i++) {
    (*cases)[i].valOfCase -= (*cases)[0].valOfCase;
    int j;
  }
}
void updateAssemblyFields(Case **cases, int sizeOfArr) {
  char *temp = (char *) malloc(1000);
  for (int i = 0; i < sizeOfArr; i++) {
    for (int j = 0; j < (*cases)[i].numOfActions; j++) {
      strcpy(temp, (*cases)[i].actionsInCase[j]);
      if (temp[strlen(temp) - 1] == ';') {
        temp[strlen(temp) - 1] = '\0';
      }
      returnRowInAssembly(&temp);
      (*cases)[i].actionsInAssembly[j] = (char *) malloc(strlen(temp));
      strcpy((*cases)[i].actionsInAssembly[j], temp);
    }
  }
}
void readFromFile(FILE *fToR, FILE *fToW) {
  Case *cases = NULL;
  int numOfCases = 0;
  int flagForInit = 0;
  int numOfCase;
  char *temp;
  int max = -99999;
  int min = 0;
  int arrSize = 0;
  int valOfCurrentCase = 0;
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
  fclose(fToR);
  fToR = fopen("switch.c", "r");
  cases = (Case *) malloc(sizeof(Case) * (max - min + 2));
  for (int i = 0; i < max - min + 2; i++) {
    cases[i].numOfActions = 0;
    cases[i].valOfCase = max + 1;
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
        //cases = (Case *) realloc(cases, numOfCases * sizeof(Case) + 1);
        valOfCurrentCase = (atoi(temp)) - min;
        cases[valOfCurrentCase].valOfCase = atoi(temp);
        //cases[valOfCurrentCase].numOfActions = 0;
        cases[valOfCurrentCase].isBreak = 0;
        numOfCases++;
        //what to do when i see break!
      } else if (strstr(str, "break") != NULL) {
        cases[valOfCurrentCase].isBreak = 1;
        //case for default!
      } else if (strstr(str, "default") != NULL) {
        valOfCurrentCase = max - min + 1;
        cases[max - min + 1].valOfCase = max + 1;
        cases[max - min + 1].isBreak = 0;
        //cases[max - min + 1].numOfActions = 0;
      } else if (strstr(str, "}") != NULL) {
        break;
      } else {
        cases[valOfCurrentCase].actionsInCase[cases[valOfCurrentCase].numOfActions] = (char *) malloc(strlen(str) + 1);
        strcpy(cases[valOfCurrentCase].actionsInCase[cases[valOfCurrentCase].numOfActions], str);
        cases[valOfCurrentCase].numOfActions++;
      }
    }
  }
  updateAssemblyFields(&cases, arrSize);
  makeTableStartFrom0(&cases, arrSize);
  createJumpTable(fToW,cases,arrSize);
  fclose(fToR);
}
int main() {
  FILE *fToRead = NULL;
  FILE *fToWrite = NULL;
  readFromFile(fToRead, fToWrite);
  //fclose(fToRead);
  //fclose(fToWrite);
  return 0;
}