; Dans cet exercice on se sert de r6 comme d'une variable temporaire
lsh r4 r0 r2 ; c
mov r6 64
sub r2 r6 r2
rsh r3 r0 r2 ; max - c
rsh r6 r1 r2
add r4 r4 r6
lsh r5 r1 12
mov r6 0 ; remise à 0 de r6