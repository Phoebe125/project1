        lw  0   2   mplier      load mplier (12328)
        lw  0   3   mcand       load mcand (32766)
        lw  0   4   maxbit      load max bit
        lw  0   5   compare     load compare bit
        noop
loop    nor 2   5   6           extract last bit (reg 6), if 1 -> 0, 0 -> 1
        beq 0   6   Add         if last bit == 1 goto Add
        beq 0   0   calc        else, goto calc
        noop
Add     add 1   3   1           if last bit == 1 add mcand with output
calc    lw  0   7   negOne      load -1
        add 4   7   4           max index -= 1
        beq 4   0   done        if (max index == 0) halt 
        add 3   3   3           shift left mcand by using add
        add 5   5   5           shift left compare by using add
        lw  0   7   one
        add 5   7   5           add 1 to compare bit (ex 1110 -> 1101)
        beq 0   0   loop        goto loop
        noop
done    halt
mcand   .fill   32766
mplier  .fill   12328
maxbit  .fill   15          max bit  = 15
compare .fill   -2          compare with 1111 1110 (inital)
one     .fill   1
negOne  .fill   -1
