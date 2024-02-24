  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line



	.globl delay, time2string, hexasc, enable_interrupt
	.data 
	.text 

enable_interrupt:
	ei
	jr $ra
	nop
	

 delay:
	li $t0, 0			# t0 = varibel i. 			
	li $t1, 6700			# t1 = antalet varv i loopen (4711)
	
	addi $a0, $a0, -1		# minskar a0 med -1 för varje varv i whileloopen
	bge $a0, $0, forloop		# ( ms > 0 )
	nop
	jr $ra
	nop
	
 forloop:
	addi $t0, $t0, 1	#Lägger till 1 för att iterera genom loopen
	blt $t0, $t1, forloop	#forloopen
	nop       		
	j delay			#Hoppar tillbaka till whileloopen
	nop


 time2string: 	
		#a1 = 0x00001653 våran tid vi vill printa ut. 
		#a0 = Adressen till minnet som vill spara våran tid.
		
		PUSH $ra		  	#PUSHAR $ra till stacken för att kunna hitta tillbaks därifrån vi körde time2string
		PUSH $s0
		PUSH $s1
		PUSH $s2
		PUSH $s3

		andi $s0, $a1, 0x0000f000 	#Nollar alla bitar utan de, 4 vill ha (NBCD-siffra 1)
		srl $s0, $s0, 12	   	#Förskjuter biten till till lsb
		
		andi $s1, $a1, 0x00000f00 	#Nollar alla bitar utan de, 4 vill ha (NBCD-siffra 2)
		srl $s1, $s1, 8	   		#Förskjuter biten till till lsb
		
		andi $s2, $a1, 0x000000f0 	#Nollar alla bitar utan de, 4 vill ha (NBCD-siffra 3)
		srl $s2, $s2, 4	   		#Förskjuter biten till till lsb
		
		andi $s3, $a1, 0x0000000f 	#Nollar alla bitar utan de, 4 vill ha 	(NBCD-siffra 4)
					   	#Behöver inte förskjutas det är redan lsb

		move $a1, $s0
		jal hexasc
		nop		
		sb  $v0, 0($a0)			#Sparar i minnet (NBCD-siffra 1)
		addi $a0, $a0, 1		#Lägger till 1 till a0 för att komma till nästa plats i minnet
		
		move $a1, $s1			
		jal hexasc
		nop 
		sb $v0, 0($a0)			#Sparar i minnet (NBCD-siffra 2)
		addi $a0, $a0, 1		#Lögger till 1 till a0 för att komma till nästa plats i minnet.
		
		li $t1, 0x3A			#Sparar symbolen ":"
		sb $t1, 0($a0)
		addi $a0, $a0, 1		#Lägger till 1 till a0 för att komma till nästa plats i minnet
		
		move $a1, $s2
		jal hexasc
		nop 
		sb $v0, 0($a0)			#Sparar i minnet (NBCD-siffra 4)
		addi $a0, $a0, 1		#Lägger till 1 till a0 för att komma till nästa plats i minnet.
		
		move $a1, $s3
		jal hexasc
		nop 
		sb $v0, 0($a0)			#Sparar i minnet (NBCD-siffra 3)
		addi $a0, $a0, 1		#Lägger till 1 till a0 för att komma till nästa plats i minnet
		
		li $t1, 0x00		#Sparar symbolen "NULL"
		sb $t1, 0($a0)
		
		POP $s3
		POP $s2
		POP $s1
		POP $s0
		POP $ra
		jr $ra
		nop 

				
 hexasc:
	andi $v0,$a1, 0x0000000f #Nollställer alla andra bits utan sista 4.
	li $t1,9		 #Laddar med 9 för att jämföra hur många steg vi ska hoppa.
	ble $v0,$t1, smaller
	nop 	 # Jämför om sista 4 bitsen är mindre eller lika med 9.
	add $v0,$v0,55		#Beroende på reslutat hoppar vi olika steg för att komma till
	jr $ra	
	nop		#rätt position i ASCI alfabetet
 smaller:
	add $v0,$v0, 0x30 
	jr $ra
	nop




	
		
