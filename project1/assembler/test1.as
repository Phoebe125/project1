        lw      0       1       wrongLabel    load reg1 with wrongLabel, Should make an error
        lw      1       2       3      load reg2 with -1 (numeric address)
start   add     1       2       1       decrement reg1
        beq     0       1       2       goto end of program when reg1==0
        beq     0       0       start   go back to the beginning of the loop
        noop
done    halt                            end of program
five    .fill   5
neg1    .fill   -1
stAddr  .fill   start                   will contain the address of start (2)

This assembler code include undefined label, So this assembler code would make error