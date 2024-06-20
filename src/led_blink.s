main:
    ldr     w5, lediosetall
    ldr     w6, iosaddr
    str     w5, [x6]

    movz    w3, #0

    b       seton

loop:
    add     w0, w0, #1
    ldr     w1, ncyclmin
    cmp     w0, w1
    b.ge    switch
    b       loop

switch:
    neg     w3, w3
    b.eq    setoff
    b       seton

seton:
    ldr     w5, ledcol1ctrl
    ldr     w6, setaddr
    str     w5, [x6]
    ldr     w6, clraddr
    str     wzr, [x6]
    b       finally

setoff:
    ldr     w5, ledcol1ctrl
    ldr     w6, clraddr
    str     w5, [x6]
    ldr     w6, setaddr
    str     wzr, [x6]
    b       finally

finally:
    movz    w0, #0
    b       loop


exit:
    
ncyclptr:
    .int    0x0000d000

ncyclmax:
    .int    0x10000000

ncyclmin:
    .int    0x00100000

iosaddr:
    .int    0x3f200000

lediosetall:
    .int    0x09249249

ledioset:
    .int    0x00000240

setaddr:
    .int    0x3f20001c

clraddr:
    .int    0x3f200028

ledctrlptr:
    .int    0x0000d008

ledcol1ctrl:
    .int    0xffffffff

ledcol2ctrl:
    .int    0x00000004

swiaddr:
    .int    0x3f200034

freqmask:
    .int    0x00310000

colmask:
    .int    0x0004c000
