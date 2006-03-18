 #include <stdio.h>
 #include <stdlib.h>
 int main()
 {
  char *tmp;
  int i=0;
  /* on y va bourin */
  tmp=(char *)malloc(1024*sizeof(char));
  while (tmp[i]="Hello Wolrd"[i++]);
  /* Ooopps y'a une infusion ! */
  i=(int)tmp[8];
  tmp[8]=tmp[9];
  tmp[9]=(char)i;
  printf("%s\n",tmp);


  return 0; //return 0 else make's unhappy
 }
