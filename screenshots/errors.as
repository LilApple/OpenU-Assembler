; file ps.as

.entry LENGTH
.extern W
MAIN:     mov @r10 ,LENGTH
LOOP:     jmp L1
          prn -5
          bne W
	  lea
	  lea LOOP, 1
	  red LOOP, 1
	  stop
          sub @b1, r4
          bne L3
2L1:      inc K
.entry LOOP

         jmp Z
END:     stop a
STR:    .string "abcdef
LENGTH: ..data 6,-9,15
K:      .data 
.extern L3
