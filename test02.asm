;Name-Shefali Bishnoi
;Roll number-2301CS87
;This is a program to find maximum of two numbers
;Let the two numbers are 7 and 12
;The result will be declared at memory location FFFFFD

data 7;first number
data 12; second number
data 0; place where we will store the result

ldl 3;load the first number into A  A=7
ldl 2;load the second number into A B=7,A=12
sub; A=B-A i.e. first number-second number
brlz secondismax; If the result is -ve it implies that second number is maximum

ldl 3;if we did not branch it means first number is maximum
stl 1; store the result
br end;

secondismax:
ldl 2;second number is maximum
stl 1;

end:
HALT
