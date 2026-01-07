.syntax unified

.section initgpio, "ax"
.global initForwardLED
.type initForwardLED, "function"

initForwardLED:
	//Enabling clock for Port B
	LDR   R0, =0x40048038
	LDR   R1, [R0]
	MOVS  R2, #1
	LSLS  R2, #10
	ORRS  R1, R1, R2
	STR   R1, [R0]

	LDR   R0, =0x4004A000
	LDR   R1, [R0]
	MOVS  R2, #7
	LSLS  R2, #8
	MVNS  R2, R2
	ANDS  R1, R1, R2

	MOVS  R2, #1
	LSLS  R2, #8
	ORRS  R1, R1, R2
	STR   R1, [R0]

	//Setting PTB0 as output
	LDR   R0, =0x400FF054
	LDR   R1, [R0]
	MOVS  R2, #1
	ORRS  R1, R1, R2
	STR   R1, [R0]

	BX LR

.end
