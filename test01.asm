;Name-Shefali Bishnoi
;Roll number-2301CS87
;This is a program to add first 6 numbers stored in memory

;data in memory
data 5
data 10
data 15
data 20
data 25
data 30

;At end of this program register A will contain the final sum
ldc 0 ;initialise the sum to zero
ldl 6
add
ldl 5
add
ldl 4
add
ldl 3
add
ldl 2
add
ldl 1
add
