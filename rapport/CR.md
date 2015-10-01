## Compte Rendu TP SEA ##

### Chapitre 2

#### Q2-2)

(.text) Code
- debut du code		0x8000
- fin du code		0x90c4 (= kmain address)

(.bss) Variables Globales
- debut des data	0x90c8
- fin des data		0x90d8

- debut kmain		0x90c4

#### Q2-3)

Les registres utilisés par la fonction div sont : R0,R1,R2,R3 

Registre untilisé pour la variable result : R0

Le registre R0 est toujours utilisé pour le retour des fonctions.

Exemple de valeur des registres lors d'une exécution : R0 -> 523 / R1 -> 339 / R2 -> 177500 (Dividend) / R3 -> 523 (Divisor)

#### Q2-4)

La pile croit dans le sens des adresses décroissantes et SP (Stack Pointer) pointe sur la dernière case pleine.

La plus petite adresse que la pile atteint est : 0x95d4

#### Q2-5)

|	STACK		|	ADDRESS		|
| ---------------------:|:--------------------- |
| 0x0153 (339)		| 0x95d4		|
| 0x2b55c (177500)	| 0x95d8		|
| 0x00cb (203)		| 0x95dc		|
| 0x020b		| 0x95e0		|
| 0			| 0x95e4		|
| 0x0005		| 0x95e8		|
| 0			| 0x95ec		|
| 0x007d		| 0x95f0		|
| 0			| 0x95f4		|
| 0x9198		| 0x95f8		|
| 0			| 0x95fc		|
| 0x0005		| 0x9600		|
| 0			| 0x9604		|
| 0x8098		| 0x9608		|
| 0			| 0x960c		|


#### Q2-6)

L'instruction BL met à jour implicitement le registre BL. En cas d'appels imbriqués, LR est stocké dans la pile de manière explicite (Avant l'appel : PUSH -> Sauvegarde LR dans la pile / Après l'appel : POP -> Remplace PC avec la valeur de LR dans la pile).

#### Q2-7)

Le retour dans kmain n'est pas fonctionnel car l'instruction B n'a pas sauvegardé l'adresse de retour. Il faut remplacer B par BL.

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

### Chapitre 3

#### Q3-1)

On remarque que dans CPSR les 5 derniers bits correspondent au mode Système.

#### Q3-2)

L'instruction `cps #19` permet de passer en mode SVC. On a alors CPSR qui change ainsi que SP car les pointeurs de pile sont différents.

#### Q3-3)

LR passe de `0x8098` à `0x0` qui pointe donc vers l'entrée de la table d'interruption.

#### Q3-4)

Pas de changement car opn a pas les privilèges dans le mode USER pour repasser en SVC.

#### Q3-5)

Le registre SPSR est vide (void) car il n'est pas disponible dans le mode Système.

##### Chapitre 4

#### Q4-7)

LR est utilisé et modifié dans `swi_handler`, il faut donc le protéger pour plus avoir de problème lors du retour de la fonction.

#### Q4-9)

PC doit être restauré pour que cela fonctionne.

#### Q4-10)

Le registre de statut est implicitement sauvegardé dans SPSR lors d'une exception (ex: interruption).

#### Q4-12)

Il faut ajouter l'attribut `naked` sur `swi_handler` pour retirer les PUSH et POP destinés à sauvegarder LR et le restaurer car on le fait explicitement.

#### Q4-15)

Les registres sont sauvegardés dans la pile SVC.

#### Q4-16)

Les variables locales ont été ecrasées.
