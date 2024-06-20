main:
    ldr     w6, iosaddr
    ldr     w5, iosval
    str     w5, [x6]
    ldr     w6, swiiosaddr
    ldr     w5, swiiosval
    str     w5, [x6]
    str     wzr, [x6, #4]
    ldr     w5, ncycl
    mov     w20, w5
    ldr     w5, ctrlvalgreen
    mov     w16, w5

startloop1:
    movz    w1, #0
    movz    w17, #0
    ldr     w6, swiaddr
    sub     w2, wzr, #1
    str     w2, [x6]

loop1:
    cmp     w1, w20
    b.ge    startloop2
    add     w1, w1, #1
    ldr     w6, swiaddr
    ldr     w5, [x6]
    orr     w17, w17, w5

ledset:
    mov     w5, w16
    ldr     w6, setaddr
    str     w5, [x6]
    b       loop1

startloop2:
    movz    w1, #0

loop2:
    cmp     w1, w20
    b.ge    finally
    add     w1, w1, #1

ledclr:
    mov     w5, w16
    ldr     w6, clraddr
    str     w5, [x6]
    b       loop2

finally:

testcolour:
    ldr     w5, colmask
    tst     w17, w5
    b.eq    testfreq
    ldr     w5, ctrlvalgreen
    cmp     w5, w16
    b.eq    changered
    b       changegreen

changegreen:
    ldr     w5, ctrlvalgreen
    mov     w16, w5
    b       testfreq

changered:
    ldr     w5, ctrlvalred
    mov     w16, w5
    b       testfreq

testfreq:
    ldr     w5, freqmask
    tst     w17, w5
    b.eq    startloop1
    ldr     w5, ncyclmin
    cmp     w20, w5
    b.le    resetfreq
    orr     w20, wzr, w20, lsr #2
    b       startloop1

resetfreq:
    ldr     w5, ncycl
    mov     w20, w5
    b       startloop1


exit:
    
ncycl:
    .int    0x00400000

ncyclmin:
    .int    0x00004000

iosaddr:
    .int    0x3f200000

iosval:
    .int    0x00000240

setaddr:
    .int    0x3f20001c

clraddr:
    .int    0x3f200028

ctrlvalgreen:
    .int    0x00000008

ctrlvalred:
    .int    0x00000004

swiiosaddr:
    .int    0x3f200064

swiiosval:
    .int    0x00102000

swiaddr:
    .int    0x3f200040

freqmask:
    .int    0x00100000

colmask:
    .int    0x00002000
