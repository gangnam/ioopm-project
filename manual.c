#include <ctype.h>
#include <stdio.h>
int typeReader(char* input) {
  int multiply = 0;
  int result = 0;
  while (input)
    {
      if (isdigit(*input))
	{
	  multiply *= 10;
	  multiply += *input;
	  input++;
	}
      if (multiply == 0) 
	{
	  multiply = 1;
	}
      if  (*input == '*')
	{
	  result += multiply * sizeof(void*);
	  multiply = 0;
	} 
      else if (*input == 'i')
	{
	  result += multiply * sizeof(int);
	  multiply = 0;
    }
      else if (*input == 'l')
	{
	  result += multiply * sizeof(long);
	  multiply = 0;
	}
      else if (*input == 'f')
	{
	  result += multiply * sizeof(float);
      multiply = 0;
    }
  else if (*input == 'd')
    {
      result += multiply * sizeof(double);
      multiply = 0;
    }
  else
    {
      result += multiply * sizeof(char);
      multiply = 0;
    }
  input++;
  multiply = 0;
}
  return result;
}

int main() {
  typeReader("*");
  return 0;
}
