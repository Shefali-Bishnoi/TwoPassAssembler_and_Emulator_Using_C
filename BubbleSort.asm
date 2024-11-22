;Name-Shefali Bishnoi
;Roll number-2301CS87
; Bubble Sort Algorithm
; Sorts an array in ascending order using bubble sort
; Assuming array starts at memory location with a label `array`

; Memory Layout:
; - array_size (a label holding the number of elements in the array)
; - array elements (labeled as `array`)

START:
    ldc 0              ; Initialize outer loop counter (i = 0)
    stl 0              ; Store i in a local variable

OUTER_LOOP:
    ldc 0              ; Initialize inner loop counter (j = 0)
    stl 1              ; Store j in a local variable

    ldl 0              ; Load outer loop counter i
    ldc array_size     ; Load array size
    sub                ; Calculate array_size - i - 1
    brz END_SORT       ; If i >= array_size - 1, sorting is complete

    ldc 0              ; Initialize swap flag to 0 (no swaps yet)
    stl 2              ; Store swap flag in a local variable

INNER_LOOP:
    ldl 1              ; Load inner loop counter j
    ldc array_size     ; Load array size
    ldc 1              ; Load 1
    sub                ; Calculate array_size - 1
    sub                ; Calculate array_size - 1 - i
    brz NEXT_PASS      ; If j >= array_size - 1 - i, go to next pass

    ; Load A[j] and A[j+1]
    ldl 1              ; Load j
    ldc array          ; Load starting address of the array
    add                ; Calculate address of A[j]
    ldnl 0             ; Load A[j]

    ldl 1              ; Load j
    ldc array          ; Load starting address of the array
    add
    ldc 1              ; Load 1
    add                ; Calculate address of A[j+1]
    ldnl 0             ; Load A[j+1]

    sub                ; Compare A[j] and A[j+1]
    brlz NO_SWAP       ; If A[j] <= A[j+1], no swap needed

    ; Swap A[j] and A[j+1]
    ldl 1              ; Load j
    ldc array          ; Load starting address of the array
    add                ; Calculate address of A[j]
    ldnl 0             ; Load A[j]
    
    ldl 1              ; Load j
    ldc array          ; Load starting address of the array
    add
    ldc 1              ; Load 1
    add                ; Calculate address of A[j+1]
    stnl 0             ; Store A[j] into A[j+1]

    ldl 1              ; Load j
    ldc array          ; Load starting address of the array
    add                ; Calculate address of A[j]
    stnl 0             ; Store A[j+1] into A[j]

    ldc 1              ; Set swap flag to 1
    stl 2              ; Store swap flag

NO_SWAP:
    ; Increment j for inner loop
    ldl 1              ; Load j
    ldc 1              ; Load 1
    add                ; j = j + 1
    stl 1              ; Store updated j
    br INNER_LOOP      ; Repeat inner loop

NEXT_PASS:
    ; Check if any swaps were made
    ldl 2              ; Load swap flag
    brz END_SORT       ; If swap flag is 0, sorting is complete

    ; Increment i for outer loop
    ldl 0              ; Load i
    ldc 1              ; Load 1
    add                ; i = i + 1
    stl 0              ; Store updated i
    br OUTER_LOOP      ; Repeat outer loop

END_SORT:
    HALT               ; End of program

; Data section for array initialization
array_size data 5      ; Define the size of the array
array data 7           ; Array elements to be sorted
      data 3
      data 9
      data 2
      data 5
