PUSH 1
PUSH 2
PAIR        ; p = (1,2)

DUP
LEFT
PAIR        ; cycle: left → itself

HALT
