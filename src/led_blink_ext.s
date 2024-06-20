main:
    ldr     w5, ledioset
    ldr     w6, iosaddr
    str     w5, [x6]

    ldr     w1, ncyclmin
    ldr     w6, ncyclptr
    str     w1, [x6]

    ldr     w1, ledcol1ctrl
    ldr     w6, ledctrlptr
    str     w1, [x6]

    movz    w3, #0

loop:
    ldr     w6, swiaddr
    ldr     w2, [x6]
    orr     w9, w9, w2
    add     w0, w0, #1
    ldr     w6, ncyclptr
    ldr     w1, [x6]
    cmp     w0, w1
    b.ge    switch
    b       loop

switch:
    neg     w3, w3
    b.eq    setoff
    b       seton

seton:
    ldr     w6, ledctrlptr
    ldr     w5, [x6]
    ldr     w6, setaddr
    str     w5, [x6]
    b       finally

setoff:
    ldr     w6, ledctrlptr
    ldr     w5, [x6]
    ldr     w6, setaddr
    movz    w5, #0
    ldr     w6, ledctrlptr
    ldr     w5, [x6]
    ldr     w6, clraddr
    str     w5, [x6]
    b       finally

finally:
    movz    w0, #0
    ldr     w5, freqmask
    tst     w9, w5
    b.ne    chncycl

aftchncycl:
    ldr     w5, colmask
    tst     w9, w5
    b.ne    chncol

aftchncol:
    b       loop

chncycl:
    ldr     w6, ncyclptr
    ldr     w1, [x6]
    orr     w1, wzr, w1, lsl #2
    ldr     w5, ncyclmax
    cmp     w1, w5
    b.le    savcycl
    ldr     w1, ncyclmin

savcycl:
    ldr     w6, ncyclptr
    str     w1, [x6]
    movz    w9, #0
    b       aftchncycl

chncol:
    ldr     w6, ledctrlptr
    ldr     w1, [x6]
    ldr     w5, ledcol1ctrl
    cmp     w1, w5
    b.ne    savcol
    ldr     w5, ledcol2ctrl

savcol:
    ldr     w6, ledctrlptr
    str     w5, [x6]
    b       aftchncol

exit:
    
ncyclptr:
    .int    0x0000d000

ncyclmax:
    .int    0x10000000

ncyclmin:
    .int    0x00100000

iosaddr:
    .int    0x3f200000

ledioset:
    .int    0x00000240

setaddr:
    .int    0x3f20001c

clraddr:
    .int    0x3f200028

ledctrlptr:
    .int    0x0000d008

ledcol1ctrl:
    .int    0x00000008

ledcol2ctrl:
    .int    0x00000004

swiaddr:
    .int    0x3f200034

freqmask:
    .int    0x00310000

colmask:
    .int    0x0004c000
