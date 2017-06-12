# Compilador a ser utilizado
CC = gcc
# Comando para remover um diretório recursivamente e ignorar caso não exista
RMDIR = rm -rf
# Comando para remover um arquivo e ignorar caso não exista
RM = rm -f

MKDIR = mkdir
# "Flags" para a geração automática das dependências
DEP_FLAGS = -MT $@ -MMD -MP -MF $(DEP_PATH)/$*.d
# Diretivas que são utilizadas na compilação de cada objeto
DIRECTIVES = -c -I $(HEADER_PATH)
# Diretivas que são utilizadas na "linkagem" dos objetos gerando o executável
LIBS = -pthread

# Um caminho para guardar e acessar cada tipo de arquivo: .h (headers),
# .cpp (sources), .o (objects) e .d (dependencies), respectivamente
HEADER_PATH = include
SRC_PATH = src
BIN_PATH = bin
DEP_PATH = dep
CACHE_PATH = resources/cache
# Uma lista de arquivos para cada tipo de arquivo:
# .cpp, .o e .d, respectivamente
CPP_FILES = $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES = $(addprefix $(BIN_PATH)/,$(notdir $(CPP_FILES:.c=.o)))
DEP_FILES = $(wildcard $(DEP_PATH)/*.d)

# Nome do executável final
EXEC = proxy 

# Regra default:
all:$(EXEC)

# Regra de criação do executável final:
$(EXEC): $(OBJ_FILES)
	$(MKDIR) $(CACHE_PATH) 
	$(CC) -o $@ $^ $(LIBS)


# Regra de inferência para criação dos objetos de compilação:
$(BIN_PATH)/%.o: $(SRC_PATH)/%.c 
# Tenta criar os diretórios dep e bin e ignora caso eles já existam
	@mkdir -p $(DEP_PATH) $(BIN_PATH)
# Gera os objetos usando diretivas de compilação e as respectivas dependências
	$(CC) $(DEP_FLAGS) -c -o $@ $< $(DIRECTIVES)

# Regra para printar uma variável no terminal. Auxilia no debug do Makefile
print-% : ; @echo $* = $($*)

# Regra que inclui diretivas de debug na compilação
debug: DIRECTIVES += -ggdb -Wall -Wextra -O0
debug: all

# Regra que inclui diretivas de compilação otimizada
release: DIRECTIVES += -Ofast -mtune=native
release: all

# Regra para limpar/deletar todos os arquivos e diretórios criados pelo make
clean:
	$(RMDIR) $(BIN_PATH) $(DEP_PATH) $(CACHE_PATH)
	$(RM) $(EXEC)

# Regra que estabelece que arquivos .d são "preciosos"
.PRECIOUS: $(DEP_PATH)/%.d
# Regra que torna as dependências "phony"
.PHONY: debug clean release

# Inclui as regras geradas pelos arquivos de dependência
-include $(DEP_FILES)

