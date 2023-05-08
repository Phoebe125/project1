        lw      0       1       four    load reg1 with 4 (symbolic address)
        lw      0       2       three   load reg2 with 3 (symbolic address)
        add     0       1       3       store reg1 to reg3
        add     0       2       1       swap reg1 to reg2
        add     0       3       2       swap reg2 to reg1 (using temp data)
        noop
done    halt                            end of program
four    .fill   4
three   .fill   3
