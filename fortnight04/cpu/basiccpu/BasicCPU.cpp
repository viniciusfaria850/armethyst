/* ----------------------------------------------------------------------------

    (EN) armethyst - A simple ARM Simulator written in C++ for Computer Architecture
    teaching purposes. Free software licensed under the MIT License (see license
    below).

    (PT) armethyst - Um simulador ARM simples escrito em C++ para o ensino de
    Arquitetura de Computadores. Software livre licenciado pela MIT License
    (veja a licença, em inglês, abaixo).

    (EN) MIT LICENSE:

    Copyright 2020 André Vital Saúde

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

   ----------------------------------------------------------------------------
*/

#include "BasicCPU.h"
#include "Util.h"

BasicCPU::BasicCPU(Memory *memory) {
	this->memory = memory;
}

/**
 * Métodos herdados de CPU
 */
int BasicCPU::run(uint64_t startAddress)
{

	// inicia PC com o valor de startAddress
	PC = startAddress;

	// ciclo da máquina
	while ((cpuError != CPUerrorCode::NONE) && !processFinished) {
		IF();
		ID();
		if (fpOp == FPOpFlag::FP_UNDEF) {
			EXI();
		} else {
			EXF();
		}
		MEM();
		WB();
	}
	
	if (cpuError) {
		return 1;
	}
	
	return 0;
};

/**
 * Busca da instrução.
 * 
 * Lê a memória de instruções no endereço PC e coloca no registrador IR.
 */
void BasicCPU::IF()
{
	IR = memory->readInstruction32(PC);
};

/**
 * Decodificação da instrução.
 * 
 * Decodifica o registrador IR, lê registradores do banco de registradores
 * e escreve em registradores auxiliares o que será usado por estágios
 * posteriores.
 *
 * Escreve A, B e ALUctrl para o estágio EXI
 * ATIVIDADE FUTURA: escreve registradores para os estágios EXF, MEM e WB.
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::ID()
{
	// TODO
	// Acrescente os cases no switch já iniciado, para detectar o grupo
	//
	// Deve-se detectar em IR o grupo da qual a instrução faz parte e
	//		chamar a função 'decodeGROUP()' para o grupo detectado,
	// 		onde GROUP é o sufixo do nome da função que decodifica as
	//		instruções daquele grupo.
	//
	// Exemplos:
	//		1. Para 'sub sp, sp, #32', chamar 'decodeDataProcImm()',
	//		2. Para 'add w1, w1, w0', chamar 'decodeDataProcReg()',

	// operação inteira como padrão
	fpOp = FPOpFlag::FP_UNDEF;

	int group = IR & 0x1E000000; // bits 28-25	
	switch (group)
	{
		//100x Data Processing -- Immediate
		case 0x10000000: // x = 0
		case 0x12000000: // x = 1
			return decodeDataProcImm();
			break;
		// x101 Data Processing -- Register on page C4-278
		case 0x0A000000: 
		case 0x1A000000:
			return decodeDataProcReg();
			break;
		
		// TODO
		// implementar o GRUPO A SEGUIR
		//
		// x111 Data Processing -- Scalar Floating-Point and Advanced SIMD on page C4-288
		case 0x1E000000:
		case 0x0E000000:
			return decodeDataProcFloat();
			break;	
		
		// ATIVIDADE FUTURA
		// implementar os DOIS GRUPOS A SEGUIR
		//
		// 101x Loads and Stores on page C4-237
		// 101x Branches, Exception Generating and System instructions on page C4-237
		 case 0x08000000:
         case 0x0C000000:
         case 0x18000000:
         case 0x1C000000:
		 return decodeLoadStore();
		 break; 
		
		 case 0x14000000:
		 case 0x16000000:
		 	return decodeBranches();
		 	break;
		 	
		 	
		 	
		default:
			return 1; // instrução não implementada
	}
};

/**
 * Decodifica instruções do grupo
 * 		100x Data Processing -- Immediate
 *
 * C4.1.2 Data Processing -- Immediate (p. 232)
 * This section describes the encoding of the Data Processing -- Immediate group.
 * The encodings in this section are decoded from A64 instruction set encoding.
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::decodeDataProcImm() {
	unsigned int n, d;
	int imm;
	
	/* Add/subtract (immediate) (pp. 233-234)
		This section describes the encoding of the Add/subtract (immediate)
		instruction class. The encodings in this section are decoded from
		Data Processing -- Immediate on page C4-232.
	*/
	switch (IR & 0xFF800000)
	{
		case 0x71000000:
			if (IR & 0x00400000) return 1; // sh = 1 não implementado
			
			// ler A e B
			n = (IR & 0x000003E0) >> 5;
			if (n == 31) {
				A = SP;
			} else {
				A = getX(n); // 64-bit variant
			}
			imm = (IR & 0x003FFC00) >> 10;
			B = imm;
			
			// registrador destino
			d = (IR & 0x0000001F);
			if (d == 31) {
				Rd = &SP;
			} else {
				Rd = &(R[d]);
			}
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::SUB;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::WB_NONE;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
		
		case 0xD1000000:
			//1 1 0 SUB (immediate) - 64-bit variant on page C6-1199
			
			if (IR & 0x00400000) return 1; // sh = 1 não implementado
			
			// ler A e B
			n = (IR & 0x000003E0) >> 5;
			if (n == 31) {
				A = SP;
			} else {
				A = getX(n); // 64-bit variant
			}
			imm = (IR & 0x003FFC00) >> 10;
			B = imm;
			
			// registrador destino
			d = (IR & 0x0000001F);
			if (d == 31) {
				Rd = &SP;
			} else {
				Rd = &(R[d]);
			}
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::SUB;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
		default:
			// instrução não implementada
			return 1;
	}
	
	
	

	// instrução não implementada
	return 1;
}

/**
 * ATIVIDADE FUTURA: Decodifica instruções do grupo
 * 		101x Branches, Exception Generating and System instructions
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::decodeBranches() {
	//DONE
	//instru��o n�o implementada
	//declara��o do imm26 valor imm6 na p�gina C6-722
	int32_t imm26 = (IR & 0x03FFFFFF);
	int32_t Imm19 = (IR & 0x00FFFFE0)>>5;
	int32_t cond;
	unsigned int n;

	//switch para pegar o branch
	switch (IR & 0xFC000000) { //zera tudo que eu n�o quero deixando s� os que quero testar
		//000101 unconditional branch to a label on page C6-722 - verifica��o
		case 0x14000000: //aplico a mascara pra ver se o que eu peguei � o que eu esperava
			//exerc�cio
			// elimina��o dos zeros � esquerda, casting expl�cito para uint64_t e retorno dos 26 bits � posi��o original, mas com 2 bits 0 � direita
			B = ((int64_t)(imm26 << 6)) >> 4;
			//declara reg a
			A = PC; //salvo o endere�o da instru��o (PC) em A
			//declara reg d
			Rd = &PC; // salvo o endere�o da instru��o (PC) no registrador de destino
			
			// Atribui��o das Flags

			// atribuir ALUctrl
			//estagio de execu��o
			ALUctrl = ALUctrlFlag::ADD;//adi��o
			// atribuir MEMctrl
			//est�gio de acesso a memoria
			MEMctrl = MEMctrlFlag::MEM_NONE; //none pq nao acesso a memoria
			// atribuir WBctrl
			//estagio de write back
			WBctrl = WBctrlFlag::RegWrite; //onde eu vou escrever a informa��o, que � no registrador, por isso o "RegWrite"
			// atribuir MemtoReg
			//segunda pleg para o estagio WB
			MemtoReg=false;// como a info n�o vem da memoria � falso
			
			return 0;
		
	}
	
	switch(IR & 0xFF000010){ //Branch conditionally to a label at a PC-relative offset, with a hint that this is not a subroutine call or return
		
		case 0x54000000:
			cond=(IR & 0x0000000f);     //aqui selecionamos os bits referente ao condicional
		
			if(cond==13){  //condi��o Ble  -->   0d13 = 0x1101  
				if(not(Z_flag==0 && N_flag==V_flag)){    //ir� fazer o salto caso a condi��o do BLE do livro texto for verdadeira 
				
					A=PC;   //para este caso o registrador A guarda o PC 
				
					B = (((int64_t)(imm26 << 8)) >> 13)<<2; // B guarda o valor do Imediato em 64bits com 2 bits 0 a direita.
				
					Rd = &PC;  // e o registrador de destino ser� o proprio PC
				
						// Atribui��o das Flags

						// atribuir ALUctrl
						//estagio de execu��o
						ALUctrl = ALUctrlFlag::ADD;//adi��o
						// atribuir MEMctrl
						//est�gio de acesso a memoria
						MEMctrl = MEMctrlFlag::MEM_NONE; //none pq nao acesso a memoria
						// atribuir WBctrl
						//estagio de write back
						WBctrl = WBctrlFlag::RegWrite; //onde eu vou escrever a informa��o, que � no registrador, por isso o "RegWrite"
						// atribuir MemtoReg
						//segunda pleg para o estagio WB
						MemtoReg=false;// como a info n�o vem da memoria � falso
						
						
						return 0;
				}
				
		}
		
		
	}
	
	switch (IR & 0xFFFFFC1F){ 
	//Return from subroutine branches unconditionally to an address in a register, with a hint that this is a subroutine
	case 0xD65F0000:  // caso "RET"
	
	
	n = (((IR & 0x000003E0)<<21)>>26); //desloca-se 21 bits a esquerda para eliminar os zeros a esquerda em seguida desloca-se... 
										//...26 para a direita para retornar o valor de n correto.
	if(n==0){					//como especificado no documento grandao caso n valer 0 sera utilizado o registrador X[30]. 
	A = getX(30);	
	}
	else A=getX(n);  //caso contratio X[n].
	
	B=ZR;			//como PC ir� para o Valor de A , B foi utilizado como zero somente para realizar a soma.
	
	Rd= &PC;		// o registrador destino ser� o PC.
	// Atribui��o das Flags

		// atribuir ALUctrl
		//estagio de execu��o
			ALUctrl = ALUctrlFlag::ADD; //adi��o
			// atribuir MEMctrl
			//est�gio de acesso a memoria
			MEMctrl = MEMctrlFlag::MEM_NONE; //none pq nao acesso a memoria
			// atribuir WBctrl
			//estagio de write back
			WBctrl = WBctrlFlag::RegWrite; //onde eu vou escrever a informa��o, que � no registrador (Rd), por isso o "RegWrite"
		    // atribuir MemtoReg
			//segunda pleg para o estagio WB
			MemtoReg=false;// como a info n�o vem da memoria � falso	
			
			return 0;
	}
	
	return 1;
}

/**
 * ATIVIDADE FUTURA: Decodifica instruções do grupo
 * 		x1x0 Loads and Stores on page C4-246
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::decodeLoadStore() {
	// instrução não implementada
unsigned int n,d;

	// instru��o n�o implementada
	switch (IR & 0xFFC00000) { 
		case 0xB9800000://LDRSW C6.2.131 Immediate (Unsigned offset) 913
			// como � escrita em 64 bits, n�o h� problema em decodificar
			n = (IR & 0x000003e0) >> 5;
			if (n == 31) {
				A = SP;
			}
			else {
				A = R[n];
			}
			B = (IR & 0x003ffc00) >> 8; // immediate
			Rd = &R[IR & 0x0000001F];
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;//adi��o
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::READ64;
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			// atribuir MemtoReg
			MemtoReg=true;
			
			return 0;
		case 0xB9400000://LDR C6.2.119 Immediate (Unsigned offset) 886 
		//32 bits
			n = (IR & 0x000003E0) >> 5;
			if (n == 31) {
				A = SP;
			}
			else {
				A =R[n];
			}

			B = ((IR & 0x003FFC00) >> 10) << 2;

			d = IR & 0x0000001F;
			if (d == 31) {
				Rd = &SP;
			}
			else {
				Rd = &R[d];
			}
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;//adi��o
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::READ32;
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			// atribuir MemtoReg
			MemtoReg=true;
			
			return 0;
		case 0xB9000000://STR C6.2.257 Unsigned offset 1135
			//size = 10, 32 bit
			n = (IR & 0x000003E0) >> 5;
			if (n == 31) {
				A = SP;
			}
			else {
				A = R[n];
			}

			B = ((IR & 0x003FFC00) >> 10) << 2; //offset = imm12 << scale. scale == size

			d = IR & 0x0000001F;
			if (d == 31) {
				Rd = (uint64_t *) &ZR;
			}
			else {
				Rd = &R[d];
			}
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;//adi��o
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::WRITE32;
			// atribuir WBctrl
			WBctrl = WBctrlFlag::WB_NONE;
			// atribuir MemtoReg
			MemtoReg=false;
			
			return 0;
	}
	switch (IR & 0xFFE0FC00) {
		//1111 1111 1110 0000 1111 1100 0000 0000
		case 0xB8607800://LDR (Register) C6.2.121 891
			n = (IR & 0x000003E0) >> 5;
			if (n == 31) {
				A = SP;
			}
			else {
				A = R[n];
			}

			n = (IR & 0x001F0000) >> 16;
			if (n == 31) {
				B = SP << 2;
			}
			else {
				B = R[n] << 2;// como eu considero no and as "variaveis" como 1, so vai entrar nesse case se for: size 10, option 011 e s 1
			}

			d = IR & 0x0000001F;
		
			
			Rd = &R[d];

			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;//adicao
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::READ32;
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			// atribuir MemtoReg
			MemtoReg=true;
			
			return 0;

	}
	return 1;
}

/**
 * Decodifica instruções do grupo
 * 		x101 Data Processing -- Register on page C4-278
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::decodeDataProcReg() {
	// TODO
	// acrescentar switches e cases à medida em que forem sendo
	// adicionadas implementações de instruções de processamento
	// de dados por registrador.

	unsigned int n,m,shift,imm6;
	
	switch (IR & 0xFF200000)
	{
		
		// C6.2.5 ADD (shifted register) p. C6-688
		case 0x8B000000:
		case 0x0B000000:
			// sf == 1 not implemented (64 bits)
			if (IR & 0x80000000) return 1;
		
			n=(IR & 0x000003E0) >> 5;
			A=getW(n);
		
			m=(IR & 0x001F0000) >> 16;
			int BW=getW(m);
		
			shift=(IR & 0x00C00000) >> 22;
			imm6=(IR & 0x0000FC00) >> 10;
		
			switch(shift){
				case 0://LSL
					B= BW << imm6;
					break;
				case 1://LSR
					B=((unsigned long)BW) >> imm6;
					break;
				case 2://ASR
					B=((signed long)BW) >> imm6;
					break;
				default:
					return 1;
			}

			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;
			
			// ATIVIDADE FUTURA:
			// implementar informações para os estágios MEM e WB.
				MEMctrl = MEMctrlFlag::MEM_NONE;
				WBctrl = WBctrlFlag::RegWrite;
				MemtoReg=false;
				unsigned int d = (IR & 0x0000001F);
				if (d == 31)
				{
					Rd = &SP;
				}
				else
				{
					Rd = &(R[d]);
				}
			return 0;
	}
		
	// instrução não implementada
	return 1;
}

/**
 * Decodifica instruções do grupo
 * 		x111 Data Processing -- Scalar Floating-Point and Advanced SIMD
 * 				on page C4-288
 *
 * Retorna 0: se executou corretamente e
 *		   1: se a instrução não estiver implementada.
 */
int BasicCPU::decodeDataProcFloat() {
	unsigned int n,m,d;

	// TODO
	// Acrescente os cases no switch já iniciado, para implementar a
	// decodificação das instruções a seguir:
	//		1. Em fpops.S
	//			1.1 'fadd s1, s1, s0'
	//				linha 58 de fpops.S, endereço 0xBC de txt_fpops.o.txt
	//				Seção C7.2.43 FADD (scalar), p. 1346 do manual.
	//
	// Verifique que ALUctrlFlag já tem declarados os tipos de
	// operação executadas pelas instruções acima.
	switch (IR & 0xFF20FC00)
	{
		case 0x1E203800:
			//C7.2.159 FSUB (scalar) on page C7-1615
			
			// implementado apenas ftype='00'
			if (IR & 0x00C00000) return 1;

			fpOp = FPOpFlag::FP_REG_32;
			
			// ler A e B
			n = (IR & 0x000003E0) >> 5;
			A = getSasInt(n); // 32-bit variant

			m = (IR & 0x001F0000) >> 16;
			B = getSasInt(m);

			// registrador destino
			d = (IR & 0x0000001F);
			Rd = &(V[d]);
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::SUB;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
		
		case 0x1E202800:
			
			// implementado apenas ftype='00'
			//if (IR & 0x00C00000) return 1;

			fpOp = FPOpFlag::FP_REG_32;
			
			// ler A e B
			n = (IR & 0x000003E0) >> 5;
			A = getSasInt(n); // 32-bit variant

			m = (IR & 0x001F0000) >> 16;
			B = getSasInt(m);

			// registrador destino
			d = (IR & 0x0000001F);
			Rd = &(V[d]);
			
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::ADD;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;	
			
	}
	
	switch(IR & 0xFF3FFC00){
		
		//Floating-point Negate (scalar). This instruction negates the value in the SIMD&FP source register and writes the
		//result to the SIMD&FP destination register
		
		
		case 0x1E214000:  
		
		if((IR & 0x00C00000)!=0) return 1;  // somente Ftype 00 implementado. 
		fpOp = FPOpFlag::FP_REG_32;
		
		n= (IR & 0x000003E0) >> 5;
		d = (IR & 0x0000001F);
		
		A=ZR;
		B= getSasInt(n);
		
		Rd = &(V[d]);
		
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::SUB;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
		
		
	}
	
	switch(IR & 0xFF20FC00){
		
        //Floating-point Divide (scalar). This instruction divides the floating-point value of the first source SIMD&FP
		//register by the floating-point value of the second source SIMD&FP register, and writes the result to the destination
		//SIMD&FP register.	
		case 0x1E201800:    
			
			n= (IR & 0x000003E0) >> 5;
		    d = (IR & 0x0000001F);
		    m= (IR & 0x001F0000)>>16;
		
			if((IR & 0x00C00000)!=0) return 1; // somente Ftype 00 implementado;	
			fpOp = FPOpFlag::FP_REG_32;
		
			A=getSasInt(n);
			B= getSasInt(m);	
			Rd = &(V[d]);
		
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::DIV;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
			
		case 0x1E200800:
			n= (IR & 0x000003E0) >> 5;
		    d = (IR & 0x0000001F);
		    m= (IR & 0x001F0000)>>16;
		
			if((IR & 0x00C00000)!=0) return 1; // somente Ftype 00 implementado;	
			fpOp = FPOpFlag::FP_REG_32;
		
			A=getSasInt(n);
			B= getSasInt(m);	
			Rd = &(V[d]);
		
			// atribuir ALUctrl
			ALUctrl = ALUctrlFlag::MUL;
			
			// atribuir MEMctrl
			MEMctrl = MEMctrlFlag::MEM_NONE;
			
			// atribuir WBctrl
			WBctrl = WBctrlFlag::RegWrite;
			
			// atribuir MemtoReg
			MemtoReg = false;
			
			return 0;
	}

	// instrução não implementada
	return 1;
}


/**
 * Execução lógico aritmética inteira.
 * 
 * Executa a operação lógico aritmética inteira com base nos valores
 * dos registradores auxiliares A, B e ALUctrl, e coloca o resultado
 * no registrador auxiliar ALUout.
 *
 * Retorna 0: se executou corretamente e
 *		   1: se o controle presente em ALUctrl não estiver implementado.
 */
int BasicCPU::EXI()
{
	// TODO
	// Acrescente os cases no switch já iniciado, para implementar a
	// execução das instruções a seguir:
	//		1. Em isummation.S:
	//			'add w1, w1, w0' (linha 43 do .S endereço 0x68)
	//
	// Verifique que ALUctrlFlag já tem declarados os tipos de
	// operação executadas pelas instruções acima.
	int64_t temp;
	switch (ALUctrl)
	{
		case ALUctrlFlag::SUB:
			ALUout = A - B;
			 temp= ALUout;					
		    N_flag = (temp<0);   // caso o resultado for negativo a N_flag ser� true;
			Z_flag = (temp==0);  // caso o resultado seja 0 Z_flag ser� true.
			
			if(A>=0 && B<0 && temp<0){              //este condicional � para avaliar se houve overflow
				V_flag=true;
			}
			else if(A<0 && B>=0 && temp>=0){
				V_flag=true;
			}
						
			return 0;
			
		case ALUctrlFlag::ADD:
			ALUout = A + B;
			
			 N_flag = (temp<0);     // caso o resultado for negativo a N_flag ser� true;
			 Z_flag = (temp==0);    // caso o resultado seja 0 Z_flag ser� true.
			 
			 if(A>=0 && B>=0 && temp<0){       //este condicional � para avaliar se houve overflow
				V_flag=true;
			}
			else if(A<0 && B<0 && temp>=0){
				V_flag=true;
			}
			
			return 0;
			
		default:
			// Controle não implementado
			return 1;
	}
	
	// Controle não implementado
	return 1;
};

		
/**
 * Execução lógico aritmética em ponto flutuante.
 * 
 * Executa a operação lógico aritmética em ponto flutuante com base
 * nos valores dos registradores auxiliares A, B e ALUctrl, e coloca
 * o resultado no registrador auxiliar ALUout.
 *
 * Retorna 0: se executou corretamente e
 *		   1: se o controle presente em ALUctrl não estiver implementado.
 */
int BasicCPU::EXF()
{
	// TODO
	// Acrescente os cases no switch já iniciado, para implementar a
	// execução das instruções a seguir:
	//		1. Em fpops.S:
	//			'fadd	s0, s0, s0' (linha 42 do .S endereço 0x80)
	//
	// Verifique que ALUctrlFlag já tem declarados os tipos de
	// operação executadas pelas instruções acima.

	if (fpOp == FPOpFlag::FP_REG_32) {
		// 32-bit implementation
		float fA = Util::uint64LowAsFloat(A);
		float fB = Util::uint64LowAsFloat(B);
		switch (ALUctrl)
		{
			case ALUctrlFlag::SUB:
				ALUout = Util::floatAsUint64Low(fA - fB);
				return 0;
			
			case ALUctrlFlag::ADD:
				ALUout = Util::floatAsUint64Low(fA + fB);
				return 0;
				
			case ALUctrlFlag::DIV:
				 ALUout = Util::floatAsUint64Low(fA / fB);
				 return 0;
				 
			case ALUctrlFlag::MUL:
				 ALUout = Util::floatAsUint64Low(fA * fB);
				 return 0;	
			default:
				// Controle não implementado
				return 1;
		}
	}
	// não implementado
	return 1;
}

/**
 * Acesso a dados na memória.
 * 
 * Retorna 0: se executou corretamente e
 *		   1: se o controle presente nos registradores auxiliares não
 * 				estiver implementado.
 */
int BasicCPU::MEM()
{
	// TODO
	// Implementar o switch (MEMctrl) case MEMctrlFlag::XXX com as
	// chamadas aos métodos corretos que implementam cada caso de
	// acesso à memória de dados.
	// não implementado
	switch (MEMctrl) {
	case MEMctrlFlag::READ32:
		MDR = memory->readData32(ALUout);
		return 0;
	case MEMctrlFlag::WRITE32:
		memory->writeData32(ALUout,*Rd);
		return 0;
	case MEMctrlFlag::READ64:
		MDR = memory->readData64(ALUout);
		return 0;
	case MEMctrlFlag::WRITE64:
		memory->writeData64(ALUout,*Rd);
		return 0;
	default:
		return 0;
	}

	return 1;
}


/**
 * Write-back. Escreve resultado da operação no registrador destino.
 * 
 * Retorna 0: se executou corretamente e
 *		   1: se o controle presente nos registradores auxiliares não
 * 				estiver implementado.
 */
int BasicCPU::WB()
{
	// TODO
	// Implementar o switch (WBctrl) case WBctrlFlag::XXX com as
	// atribuições corretas do registrador destino, quando houver, ou
	// return 0 no caso WBctrlFlag::WB_NONE.
	
	// não implementado
	 switch (WBctrl) {
        case WBctrlFlag::WB_NONE:
            return 0;
        case WBctrlFlag::RegWrite:
            if (MemtoReg) {
                *Rd = MDR;
            } else {
                *Rd = ALUout;
            }
            return 0;
        default:
            // não implementado
            return 1;
    }
	return 1;
}


/**
 * Métodos de acesso ao banco de registradores
 */

/**
 * Lê registrador inteiro de 32 bits.
 */
uint32_t BasicCPU::getW(int n) {
	return (uint32_t)(0x00000000FFFFFFFF & R[n]);
}

/**
 * Escreve registrador inteiro de 32 bits.
 */
void BasicCPU::setW(int n, uint32_t value) {
	R[n] = (uint64_t)value;
}

/**
 * Lê registrador inteiro de 64 bits.
 */
uint64_t BasicCPU::getX(int n) {
	return R[n];
}

/**
 * Escreve registrador inteiro de 64 bits.
 */
void BasicCPU::setX(int n, uint64_t value) {
	R[n] = value;
}


/**
 * Lê registrador ponto flutuante de 32 bits.
 */
float BasicCPU::getS(int n) {
	return Util::uint64LowAsFloat(V[n]);
}

/**
 * Lê registrador ponto flutuante de 32 bits, sem conversão.
 */
uint32_t BasicCPU::getSasInt(int n)
{
	return (uint32_t)(0x00000000FFFFFFFF & V[n]);
}

/**
 * Escreve registrador ponto flutuante de 32 bits.
 */
void BasicCPU::setS(int n, float value) {
	V[n] = Util::floatAsUint64Low(value);
}

/**
 * Lê registrador ponto flutuante de 64 bits.
 */
double BasicCPU::getD(int n) {
	return Util::uint64AsDouble(V[n]);
}

/**
 * Escreve registrador ponto flutuante de 64 bits.
 */
void BasicCPU::setD(int n, double value) {
	V[n] = Util::doubleAsUint64(value);
}
