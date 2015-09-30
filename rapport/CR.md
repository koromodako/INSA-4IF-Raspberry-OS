## Compte Rendu TP SEA ##

#### Q2-2)

(.text)
- debut du code    0x8000
- fin du code 	   0x90c4 (= kmain address)

(.bss)
- debut des data	0x90c8
- fin des data	    0x90d8

- debut kmain  	    0x90c4

#### Q2-3)

Les registres utilisés par la fonction div sont : R0,R1,R2,R3 

Registre untilisé pour la variable result : R0

#### Q2-4)

La pile croit dans le sens des adresses décroissantes.

La plus petite adresse que la pile atteint est : 0x95d4

SP pointe sur une case remplie.

#### Q2-5)

|      STACK      |      ADDRESS
|-----------------|-----------------
|     0x0153 (339)     | 0x95d4		
|     0x2b55c (177500)   | 0x95d8		
|     0x00cb (203)     | 0x95dc		
|     0x020b      | 0x95e0
|        0        | 0x95e4
|     0x0005      | 0x95e8
|        0        | 0x95ec
|     0x007d      | 0x95f0
|        0        | 0x95f4
|     0x9198      | 0x95f8
|        0        | 0x95fc
|     0x0005      | 0x9600
|        0        | 0x9604
|     0x8098      | 0x9608
|        0        | 0x960c


#### Q2-6)

L'instruction BL met à jour implicitement le registre lr. En cas d'appels imbriqués, lr est stocké dans la pile.

#### Q2-7)

L'instruction B n'a pas sauvegardé l'adresse de retour. Il faut remplacer B par BL.

#### Q2-8)
```c
// On met radius dans r2
__asm("mov r2, %0" : : "r"(radius));
// On met r3 dans radius
__asm("mov %0, r3" : "=r"(radius));
```
#### Q2-9)

```asm
000090c4 <dummy>:
90c4:	e1a00000 	nop			; (mov r0, r0)
90c8:	e12fff1e 	bx	lr

000090cc <dummyNaked>:
90cc:	e1a00000 	nop			; (mov r0, r0)
```
#### Q2-10)

On passe bien deux fois dans la fonction `bidule()`.

#### Q2-11)

On a vu que le script place 2 breakpoints, un au debut de `kmain()` et l'autre au debut de `bidule()` et print l'adresse de la fonction à chaque fois.

Le test compare ensuite que les trois premieres variables correspondent dans l'ordre au adresses de `kmain()` puis `bidule()` et encore `bidule()`.

Le script execute donc le même test.

#### Q2-12)
Exemple de commande qui fonctionne avec le script : 
	$ ./run-test.sh ./../test/kmain-bidule.c ../test/bidule-called-twice.gdb 


