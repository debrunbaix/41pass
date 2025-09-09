SECTION .text
				global _start

_start:
				; Obtenir argc, argv, envp de la pile
				mov		rdi, [rsp]
				lea		rsi, [rsp + 8]
				lea 	rdx, [rsi + rdi * 8 + 8]

				; Appler main(argc, argv, envp)
				extern main
				call	main

				; Appel systeme pour quitter le programme
				mov		rdi, rax
				mov		rax, 60
				syscall
