#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>

int main(int argc, char *argv[]) {
  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMModuleRef module = LLVMModuleCreateWithNameInContext("meu_modulo", context);
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

  // Cria um valor zero para colocar no retorno.
  LLVMValueRef Zero64 = LLVMConstInt(LLVMInt64Type(), 0, false);

  // Declara o tipo do retorno da função main.
  LLVMTypeRef mainFnReturnType = LLVMInt64TypeInContext(context);
  // Cria a função main.
  LLVMValueRef mainFn = LLVMAddFunction(module, "main", LLVMFunctionType(mainFnReturnType, NULL, 0, 0));

  // Declara o bloco de entrada.
  LLVMBasicBlockRef entryBlock = LLVMAppendBasicBlockInContext(context, mainFn, "entry");
  // Declara o bloco de saída.
  LLVMBasicBlockRef endBasicBlock = LLVMAppendBasicBlock(mainFn, "end");

  // Adiciona o bloco de entrada.
  LLVMPositionBuilderAtEnd(builder, entryBlock);

  // Cria o valor de retorno e inicializa com zero.
  LLVMValueRef returnVal = LLVMBuildAlloca(builder, LLVMInt64Type(), "retorno");
	LLVMBuildStore(builder, Zero64, returnVal);

  // Declara uma variável n.
  LLVMValueRef n = LLVMBuildAlloca(builder, LLVMInt64Type(), "n");
  
  // Inicializa n.
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt64Type(), 10, false), n);

  // Declara os blocos básicos para o if.
	LLVMBasicBlockRef iftrue = LLVMAppendBasicBlock(mainFn, "iftrue");
  LLVMBasicBlockRef iffalse = LLVMAppendBasicBlock(mainFn, "iffalse");
  LLVMBasicBlockRef ifend = LLVMAppendBasicBlock(mainFn, "ifend");
  
  // Carrega o valor que será comparado: n < 1024.
  LLVMValueRef n_cmp = LLVMBuildLoad(builder, n, "n");

  LLVMValueRef If = LLVMBuildICmp(builder, LLVMIntSLT, n_cmp, LLVMConstInt(LLVMInt64Type(), 1024, false), "if_test");
  LLVMBuildCondBr(builder, If, iftrue, iffalse);

  LLVMPositionBuilderAtEnd(builder, iftrue);
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt64Type(), 1, 0), returnVal);
  LLVMBuildBr(builder, ifend);

  LLVMPositionBuilderAtEnd(builder, iffalse);
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt64Type(), 2, 0), returnVal);
  LLVMBuildBr(builder, ifend);

  LLVMPositionBuilderAtEnd(builder, ifend);

  // Cria um salto para o bloco de saída.
	LLVMBuildBr(builder, endBasicBlock);
	
	// Adiciona o bloco de saída.
	LLVMPositionBuilderAtEnd(builder, endBasicBlock);
  
  // Cria o return.
	LLVMBuildRet(builder, LLVMBuildLoad(builder, returnVal, ""));

	// Imprime o código do módulo.
  LLVMDumpModule(module);

  // Escreve para um arquivo no formato bitcode.
  if (LLVMWriteBitcodeToFile(module, "meu_modulo.bc") != 0) {
    fprintf(stderr, "error writing bitcode to file, skipping\n");
  }
}
