main:
    ldr     w5, ledioset
    ldr     w6, iosaddr
    str     w5, [x6]

loop:
    ldr     w6, swiaddr
    ldr     w2, [x6]
    orr     w9, w9, w2
    add     w0, w0, #1
    ldr     w1, ncycl
    cmp     w0, w1
    b.ge    switch
    b       loop

switch:
    neg     w3, w3
    b.eq    setoff
    b       seton

seton:
    ldr     w5, ledctrl
    ldr     w6, setaddr
    str     w3, [x6]
    b       finally

setoff:
    ldr     w5, ledctrl
    ldr     w6, clraddr
    str     w5, [x6]
    b       finally

finally:
    movz    w0, #0
    ldr     w5, freqmask
    tst     w9, w5
    add     x23, pc, #4
    b.ne    chncycl
    ldr     w5, colmask
    tst     w9, w5
    add     x23, pc, #4
    b.ne    chncol
    b       loop

chncycl:
    ldr     w1, ncycl
    orr     w1, wzr, w1, lsl #2
    ldr     w5, ncyclmax
    cmp     w1, w5
    b.le    savcycl
    ldr     w1, ncyclmin

savcycl:
    str     w1, ncycl
    movz    w9, #0
    mov     pc, x23

chncol:
    ldr     w1, ledctrl
    ldr     w5, ledcol1ctrl
    cmp     w1, w5
    b.eq    chncol2
    str     w5, ledctrl
    mov     pc, x23

chncol2:
    ldr     w5, ledcol2ctrl
    str     w5, ledctrl
    mov     pc, x23

exit:
    
ncycl:
    .int    0x00010000

ncyclmax:
    .int    0x00010000

ncyclmin:
    .int    0x00001000

iosaddr:
    .int    0x3f200000

ledioset:
    .int    0x00000240

setaddr:
    .int    0x3f20001c

clraddr:
    .int    0x3f200028

ledctrl:
    .int    0x00000008

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
