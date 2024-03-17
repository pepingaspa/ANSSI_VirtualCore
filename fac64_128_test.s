; Je vais écrire l'objectif en commentaires (fonction python qui renvoie factorielle(1e) sans multiplication):
; ran = 0x1e
; res = 1
; for i in range(ran):
;   tempi = 1
;   while tempi<i:
;       temp = res
;       res = res + temp
;       tempi = tempi+1
; return(res)

;voir statefile mais r0 contient la valeur dont on veut la factorielle, ici 1e

mov r2 1 ;res partie 2 et r1 vaut res partie 1

mov r3 0 ;i
add r3 r3 1 ; incrémentation de i
mov r4 1 ;tempi = 1
mov r5 r2 ;temp2 = res partie 2
mov r6 r1 ;temp1 = res partie 1
cmp r4 r3 ;while tempi != i
BEQ -5 ;retourne en haut du for si temp=i
add r2 r2 r5 ;calcul res
adc r1 r1 r6 ;calcul res
add r4 r4 1 ;incrémentation tempi
cmp r4 r3
BEQ 2
B -7 ; retourne au début du while si tempi != i
cmp r3 r0
BNE -2 ; retourne au début du for si i != 0x1e