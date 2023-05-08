        lw      0   1   ten             load reg1 with x = 10
        lw      0   2   stack           load reg2 with stack = 100
        lw      0   3   negTwo          load reg3 with -2
        lw      0   4   posTwo          load reg4 with 2
        lw      0   5   mainAdd         load reg5 with address of main address
        jalr    5   6                   store address of PC in reg6, goto main address
        halt                            return function save result in reg 7
        noop
main    add     2   3   2               stack = stack -2
        sw      2   6   1               store return address into $sp + 1
        sw      2   1   0               store x into $sp
        beq     0   1   EXIT            if x == 0 go to EXIT
        add     1   3   1               x -= 2
        lw      0   5   mainAdd         load reg5 with main address
        jalr    5   6                   store address of PC in reg6, goto main address
        noop
        lw      2   6   1               load return address from $sp + 1
        lw      2   1   0               load x from $sp + 1
        add     2   4   2               restore stack
        add     1   7   7               add input to return value(reg7)
        jalr    6   5                   store address of PC in reg5, goto return address
        noop
EXIT    add     2   4   2               restore stack
        add     0   0   7               set reg7 to 0
        jalr    6   5                   store address of PC in reg5, goto return address
        noop
ten     .fill   10
negTwo  .fill   -2
posTwo  .fill   2
stack   .fill   100                     stack address
mainAdd .fill   main                    main address
