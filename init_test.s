; Boucle for : shift de 4 vers la gauche puis ajoute i à res 16 fois
lsh r0 r0 4
add r0 r0 r3
add r3 r3 1
cmp r3 16
BNE -4

mov r3 0
; Deuxieme boucle for, on left shift de 8 puis on ajoute 165 (a5) 8 fois
lsh r1 r1 8
add r1 r1 165
add r3 r3 1
cmp r3 8
BNE -4

; On ne peut pas faire de boucle for cette fois, on fera le tout à la main
add r2 r2 174 ; ae
lsh r2 r2 8
add r2 r2 244 ; f4
lsh r2 r2 8
add r2 r2 93 ; 5d
lsh r2 r2 8
add r2 r2 116 ; 74
lsh r2 r2 8
add r2 r2 90 ; 5a
lsh r2 r2 8
add r2 r2 255 ; ff
lsh r2 r2 8
add r2 r2 88 ; 58
lsh r2 r2 8
add r2 r2 79 ; 4f