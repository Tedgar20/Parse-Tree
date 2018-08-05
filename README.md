# Parse-Tree

Using c++ I made a parse tree that when given an appropriate language it will parse the language checking for any errors and if there are no errors then it will execute the program.
If errors exist in the code it will tell on what line the errors exist and why it is incorrect

SEMANTIC RULES FOR THIS LANGUAGE

1.  There are only two types: integer and string
2.  Variables are assigned a value using set
3.  An expression can be printed using either print or println
4.  There is one single scope for variables
5.  Variable names must be unique
6.  Adding two integers gets the sum
7.  Subtracting two integers gets the difference
8.  Multiplying two integers gets the product
9.  Dividing two integers gets the quotient
10.  Adding two strings will concatenate the strings
11. Multiplying an integer and a string results in a string repeated integer times ( 3 * "hi" = "hihihi" )
12. Dividing two strings results in a string where the first instance of the denominator that is found in the numerator is removed


EXAMPLE

int x;

int y;

string z;

set x 5;

set y 3;

set z "hi";

println x+y;

println z*x;

OUTPUT

hihihihihi

8
