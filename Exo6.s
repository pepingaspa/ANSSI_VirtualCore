; on pose A est r1 et vaut 5 et B est r2 et vaut 12. la valeur de retour sera stockée dans r0. Pour changer les valeurs de A et B il suffit de modifier les 2 premières lignes.

mov r1 5 ;A=5
mov r2 12 ;B=12
cmp r1 r2 
BG 3 ;Si A<B on renvoie 0xfe
mov r0 175
B 2 ;Sinon on renvoie 0xaf
mov r0 254
mov r1 0 ;Moyen de jump ici et pouvoir finir le programme sans crash