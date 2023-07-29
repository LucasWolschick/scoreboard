#include "assembler.h"

/*
 * Recebe um caminho e carrega os conteúdos do arquivo deste caminho para um
 * buffer string. Retorna um ponteiro para esse buffer ou NULL caso a operação
 * falhou.
 *
 * Importante: usuários do buffer devem liberá-lo após o uso.
 */
char *arquivo_para_string(char caminho[])
{
    FILE *f = fopen(caminho, "rb");
    if (!f)
    {
        perror("Erro ao abrir arquivo");
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    if (tamanho == -1L)
    {
        perror("Erro ao abrir arquivo");
        return 0;
    }
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(sizeof(char) * (tamanho + 1));
    if (!buffer)
    {
        printf("Nao ha memoria suficiente para carregar este arquivo!");
        return 0;
    }
    fread(buffer, sizeof *buffer, tamanho, f);
    buffer[tamanho] = '\0';

    fclose(f);

    return buffer;
}

// scanner

token scanner_faz_token(scanner *s, token_tipo tipo)
{
    token t;
    t.tipo = tipo;
    t.comp = (int)(s->atual - s->inicio);
    t.inicio = s->inicio;
    t.valor = -1;
    t.linha = s->linha;
    return t;
}

token scanner_faz_erro(scanner *s)
{
    token t;
    t.tipo = TOKEN_ERRO;
    t.comp = 0;
    t.inicio = "";
    t.valor = -1;
    t.linha = s->linha;
    return t;
}

char scanner_avancar(scanner *s)
{
    if (s->fim <= s->atual)
    {
        return '\0';
    }

    char ch = *(s->atual++);
    if (ch == '\n')
        s->linha++;
    return ch;
}

char scanner_ver(scanner *s)
{
    if (s->fim <= s->atual)
    {
        return '\0';
    }

    return *s->atual;
}

void scanner_avanca_espaco(scanner *s)
{
    while (isspace(scanner_ver(s)))
    {
        scanner_avancar(s);
    }
}

token scanner_numero(scanner *s)
{
    char ch = scanner_ver(s);

    if (ch == 'x')
    {
        // come o x
        scanner_avancar(s);

        // o próximo tem que ser número
        ch = scanner_avancar(s);
        if (!isxdigit(ch))
        {
            scanner_faz_erro(s);
        }

        while (isxdigit(scanner_ver(s)))
        {
            scanner_avancar(s);
        }
    }
    else
    {
        while (isdigit(scanner_ver(s)))
        {
            scanner_avancar(s);
        }
    }

    token t = scanner_faz_token(s, TOKEN_NUMERO);

    char *buf = malloc(sizeof(char) * (t.comp + 1));
    for (int i = 0; i < t.comp; i++)
    {
        buf[i] = t.inicio[i];
    }
    buf[t.comp] = '\0';
    int numero;
    sscanf(buf, " %i", &numero);
    t.valor = numero;
    free(buf);

    return t;
}

token scanner_secao(scanner *s)
{
    while (isalnum(scanner_ver(s)))
    {
        scanner_avancar(s);
    }

    token t = scanner_faz_token(s, TOKEN_SECAO);
    if (!compara_token(t, ".data") && !compara_token(t, ".text"))
    {
        fprintf(stderr, "Erro: linha %d: declaracao de secao invalida; secoes validas sao .data e .text, obtive: ", t.linha);
        token_lexema(t);
        fprintf(stderr, "\n");
        return scanner_faz_erro(s);
    }

    return t;
}

token scanner_identificador(scanner *s)
{
    while (isalnum(scanner_ver(s)))
    {
        scanner_avancar(s);
    }

    return scanner_faz_token(s, TOKEN_NOME);
}

token scanner_registrador(scanner *s)
{
    while (isdigit(scanner_ver(s)))
    {
        scanner_avancar(s);
    }

    token t = scanner_faz_token(s, TOKEN_REGISTRADOR);

    char *buf = malloc(sizeof(char) * (t.comp + 1 - 1));
    // pulamos o 'r'
    for (int i = 1; i < t.comp; i++)
    {
        buf[i - 1] = t.inicio[i];
    }
    buf[t.comp - 1] = '\0';
    int numero;
    sscanf(buf, " %i", &numero);
    t.valor = numero;
    free(buf);

    if (t.valor < 0 || t.valor > 31)
    {
        fprintf(stderr, "Erro: linha %d: esperava registrador valido, obtive: ", t.linha);
        token_lexema(t);
        fprintf(stderr, "\n");
        return scanner_faz_erro(s);
    }

    return t;
}

const char *tipo_token_string(token_tipo tipo)
{
    switch (tipo)
    {
    case TOKEN_DOIS_PONTOS:
        return ":";
    case TOKEN_VIRGULA:
        return ",";
    case TOKEN_COMECA_COMENTARIO:
        return "/*";
    case TOKEN_TERMINA_COMENTARIO:
        return "*/";
    case TOKEN_PAREN_ESQ:
        return "(";
    case TOKEN_PAREN_DIR:
        return ")";
    case TOKEN_MENOS:
        return "-";
    case TOKEN_NOME:
        return "nome";
    case TOKEN_NUMERO:
        return "numero";
    case TOKEN_SECAO:
        return "secao";
    case TOKEN_REGISTRADOR:
        return "registrador";
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_ERRO:
        return "erro";
    default:
        return "";
    }
}

token prox_token_r(scanner *s)
{
    s->inicio = s->atual;
    if (s->atual >= s->fim)
    {
        return scanner_faz_token(s, TOKEN_EOF);
    }

    char ch = scanner_avancar(s);
    switch (ch)
    {

    // caracteres de um token
    case ':':
        return scanner_faz_token(s, TOKEN_DOIS_PONTOS);

    case '(':
        return scanner_faz_token(s, TOKEN_PAREN_ESQ);

    case ')':
        return scanner_faz_token(s, TOKEN_PAREN_DIR);

    case '-':
        return scanner_faz_token(s, TOKEN_MENOS);
    case ',':
        return scanner_faz_token(s, TOKEN_VIRGULA);

    // caracteres de dois tokens
    case '/':
        if (scanner_ver(s) == '*')
        {
            scanner_avancar(s);
            return scanner_faz_token(s, TOKEN_COMECA_COMENTARIO);
        }
        else
        {
            fprintf(stderr, "Erro: linha %d: esperava '*' apos '/'\n", s->linha);
            return scanner_faz_erro(s);
        }

    case '*':
        if (scanner_ver(s) == '/')
        {
            scanner_avancar(s);
            return scanner_faz_token(s, TOKEN_TERMINA_COMENTARIO);
        }
        else
        {
            fprintf(stderr, "Erro: linha %d: esperava '/' apos '*'\n", s->linha);
            return scanner_faz_erro(s);
        }

    // espaço em branco
    case '\n':
    case '\t':
    case '\r':
    case ' ':
        return prox_token_r(s);

    // comentário
    case '#':
        while ((ch = scanner_avancar(s)) && ch != '\n')
        {
        }
        return prox_token_r(s);

    default:
    {
        // seções
        if (ch == '.')
        {
            return scanner_secao(s);
        }

        // números
        if (isdigit(ch))
        {
            return scanner_numero(s);
        }

        // identificadores
        if (isalpha(ch))
        {
            if ((ch == 'r' || ch == 'R') && isdigit(scanner_ver(s)))
            {
                // registrador
                return scanner_registrador(s);
            }
            else
            {
                return scanner_identificador(s);
            }
        }

        // etc
        fprintf(stderr, "Erro: linha %d: caractere desconhecido '%c'\n", s->linha, ch);
        return scanner_faz_erro(s);
    }
    }
}

void token_lexema(token t)
{
    fwrite(t.inicio, sizeof(char), t.comp, stderr);
}

token prox_token(scanner *s)
{
    token t = prox_token_r(s);
    return t;
}

token ve_token(scanner *s)
{
    scanner copia = *s;
    return prox_token(&copia);
}

token espera_token(scanner *s, token_tipo tipo)
{
    token t = ve_token(s);
    if (t.tipo != tipo)
    {
        fprintf(stderr, "Erro: linha %d: esperava '%s', obtive '", t.linha, tipo_token_string(tipo));
        token_lexema(t);
        fprintf(stderr, "'\n");
        exit(1);
    }
    return prox_token(s);
}

//   t: abcX (comp = 3)
// str: abc\0
//         i

bool compara_token(token t, char *str)
{
    int i = 0;
    while (i < t.comp && str[i] != '\0')
    {
        if (t.inicio[i] != str[i])
        {
            return false;
        }
        i++;
    }
    return str[i] == '\0' && t.comp == i;
}

// configuracao -> bloco*
// bloco -> UF \n atrib_uf | INST \n atrib_inst
// atrib_uf -> NOME ":" NUMERO \n [atrib_uf]
// atrib_inst -> NOME ":" NUMERO \n [atrib_inst]

void atrib_uf(scanner *s, config *c)
{
    token nome = espera_token(s, TOKEN_NOME);
    espera_token(s, TOKEN_DOIS_PONTOS);
    token valor = espera_token(s, TOKEN_NUMERO);

    if (compara_token(nome, "add"))
        c->n_uf_add = valor.valor;
    else if (compara_token(nome, "mul"))
        c->n_uf_mul = valor.valor;
    else if (compara_token(nome, "inteiro"))
        c->n_uf_int = valor.valor;
}

void atrib_inst(scanner *s, config *c)
{
    opcode_instrucao opcode = mnemonico(s);
    espera_token(s, TOKEN_DOIS_PONTOS);
    token valor = espera_token(s, TOKEN_NUMERO);

    c->ck_instrucoes[opcode] = valor.valor;
}

void bloco_configuracao(scanner *s, config *c)
{
    token nome_bloco = espera_token(s, TOKEN_NOME);
    if (compara_token(nome_bloco, "UF"))
    {
        token teste;
        while ((((teste = ve_token(s))).tipo == TOKEN_NOME) && (!compara_token(teste, "UF") && (!compara_token(teste, "INST"))))
        {
            atrib_uf(s, c);
        }
    }
    else if (compara_token(nome_bloco, "INST"))
    {
        token teste;
        while ((((teste = ve_token(s))).tipo == TOKEN_NOME) && (!compara_token(teste, "UF") && (!compara_token(teste, "INST"))))
        {
            atrib_inst(s, c);
        }
    }
    else
    {
        fprintf(stderr, "Erro na sintaxe, esperava bloco\n");
        exit(1);
    }
}

config config_novo(void)
{
    config c;

    c.n_uf_add = 1;
    c.n_uf_int = 1;
    c.n_uf_mul = 1;

    for (opcode_instrucao op = OP_ADD; op < OP_EXIT; op++)
    {
        c.ck_instrucoes[op] = 1;
    }

    return c;
}

opcode_instrucao mnemonico(scanner *s)
{
    token opcode = espera_token(s, TOKEN_NOME);

    switch (opcode.inicio[0])
    {
    case 'd':
        // div
        if (compara_token(opcode, "div"))
        {
            return OP_DIV;
        }
        break;
    case 'e':
        // exit
        if (compara_token(opcode, "exit"))
        {
            return OP_EXIT;
        }
    case 'j':
        // j
        if (compara_token(opcode, "j"))
        {
            return OP_J;
        }
        break;
    case 'l':
        // lw
        if (compara_token(opcode, "lw"))
        {
            return OP_LW;
        }
        break;
    case 'm':
        // mul
        if (compara_token(opcode, "mul"))
        {
            return OP_MUL;
        }
        break;
    case 'n':
        // not
        if (compara_token(opcode, "not"))
        {
            return OP_NOT;
        }
        break;
    case 'o':
        // or
        if (compara_token(opcode, "or"))
        {
            return OP_OR;
        }
        break;

    case 's':
        if (opcode.comp > 1)
        {
            switch (opcode.inicio[1])
            {
            case 'u':
                // sub or subi
                if (compara_token(opcode, "sub"))
                {
                    return OP_SUB;
                }
                else if (compara_token(opcode, "subi"))
                {
                    return OP_SUBI;
                }
                break;
            case 'w':
                // sw
                if (compara_token(opcode, "sw"))
                {
                    return OP_SW;
                }
                break;
            }
        }

    case 'a':
        if (opcode.comp > 1)
        {
            switch (opcode.inicio[1])
            {
            case 'd':
                // add or addi
                if (compara_token(opcode, "add"))
                {
                    return OP_ADD;
                }
                else if (compara_token(opcode, "addi"))
                {
                    return OP_ADDI;
                }
                break;
            case 'n':
                // and
                if (compara_token(opcode, "and"))
                {
                    return OP_AND;
                }
                break;
            }
        }

    case 'b':
        if (opcode.comp > 1)
        {
            switch (opcode.inicio[1])
            {
            case 'e':
                // beq
                if (compara_token(opcode, "beq"))
                {
                    return OP_BEQ;
                }
                break;
            case 'g':
                // bgt
                if (compara_token(opcode, "bgt"))
                {
                    return OP_BGT;
                }
                break;
            case 'l':
                // blt
                if (compara_token(opcode, "blt"))
                {
                    return OP_BLT;
                }
                break;
            case 'n':
                // bne
                if (compara_token(opcode, "bne"))
                {
                    return OP_BNE;
                }
                break;
            }
        }
    }

    fprintf(stderr, "Erro: linha %d: esperava mnemonico, encontrado: '", opcode.linha);
    token_lexema(opcode);
    fprintf(stderr, "'\n");
    exit(1);
}

config configuracao(scanner *s)
{
    config c = {0};

    token t;
    while ((t = ve_token(s)).tipo != TOKEN_TERMINA_COMENTARIO)
    {
        if (t.tipo == TOKEN_EOF)
        {
            fprintf(stderr, "Erro: linha %d: Comentario de configuracao nao finalizado", t.linha);
            exit(1);
        }
        bloco_configuracao(s, &c);
    }
    espera_token(s, TOKEN_TERMINA_COMENTARIO);

    return c;
}

inst instrucao(scanner *s)
{
    /*
        FORMATOS:

        1 add, sub, mul, div, and, or:
            op  rd, rs, rt
        2 addi, subi, blt, bgt, beq, bne:
            op  rs, rt, imm
        3 not:
            op  rd, rs
        4 j:
            op imm
        5 lw, sw:
            op  rd, imm(rs)
        6 exit:
            op

    */

    opcode_instrucao op = mnemonico(s);

    switch (op)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_AND:
    case OP_OR:
    {
        // op rd, rs, rt (formato R)
        token destino = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token op1 = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token op2 = espera_token(s, TOKEN_REGISTRADOR);

        inst i;
        i.r.opcode = op;
        i.r.rd = destino.valor;
        i.r.rs = op1.valor;
        i.r.rt = op2.valor;
        i.r.extra = 0;
        return i;
    }

    case OP_ADDI:
    case OP_SUBI:
    case OP_BLT:
    case OP_BGT:
    case OP_BEQ:
    case OP_BNE:
    {
        // op rd, rs, imm (formato I)
        token destino = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token op1 = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);

        int negativo = 1;
        if (ve_token(s).tipo == TOKEN_MENOS)
        {
            prox_token(s);
            negativo = -1;
        }
        token imm = espera_token(s, TOKEN_NUMERO);

        inst i;
        i.i.opcode = op;
        i.i.rt = destino.valor;
        i.i.rs = op1.valor;
        i.i.imm = negativo * imm.valor;
        return i;
    }

    case OP_NOT:
    {
        // op rd, rs (formato R)

        token destino = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token fonte = espera_token(s, TOKEN_REGISTRADOR);

        inst i;
        i.r.opcode = op;
        i.r.rd = destino.valor;
        i.r.rs = fonte.valor;
        i.r.rt = 0;
        i.r.extra = 0;
        return i;
    }

    case OP_J:
    {
        // op imm (formato J)
        token imm = espera_token(s, TOKEN_NUMERO);

        inst i;
        i.j.opcode = op;
        i.j.address = imm.valor;
        return i;
    }

    case OP_LW:
    case OP_SW:
    {
        // formato ld f1 (num)f3 (formato I)
        token dest = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);

        int negativo = 1;
        if (ve_token(s).tipo == TOKEN_MENOS)
        {
            // negativo
            prox_token(s);
            negativo = -1;
        }

        token num = espera_token(s, TOKEN_NUMERO);
        espera_token(s, TOKEN_PAREN_ESQ);
        token base = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_PAREN_DIR);

        inst i;
        i.i.opcode = op;
        i.i.rt = dest.valor;
        i.i.imm = negativo * num.valor;
        i.i.rs = base.valor;
        return i;
    }

    case OP_EXIT:
    {
        // op (formato J)

        inst i;
        i.j.opcode = op;
        i.j.address = 0;
        return i;
    }
    }
}

void dados(scanner *s)
{
    token t = espera_token(s, TOKEN_SECAO);
    if (!compara_token(t, ".data"))
    {
        fprintf(stderr, "Erro: linha %d: esperava secao '.data', encontrada secao '", t.linha);
        token_lexema(t);
        fprintf(stderr, "'\n");
        exit(1);
    }
}

void instrucoes(scanner *s, int *num_instrucoes_saida, inst **instrucoes_lidas_saida)
{
    token t = espera_token(s, TOKEN_SECAO);
    if (!compara_token(t, ".text"))
    {
        fprintf(stderr, "Erro: linha %d: esperava secao '.text', encontrada secao '", t.linha);
        token_lexema(t);
        fprintf(stderr, "'\n");
        exit(1);
    }

    int capacidade = 1;
    int num_instrucoes = 0;
    inst *instrucoes_lidas = realloc(NULL, sizeof(inst) * capacidade);

    bool pronto = false;
    int n_instrucao = 0;
    while (!pronto)
    {
        token t = ve_token(s);
        switch (t.tipo)
        {
        case TOKEN_NOME:
        {
            inst i = instrucao(s);
            if (num_instrucoes + 1 >= capacidade)
            {
                capacidade *= 2;
                instrucoes_lidas = realloc(instrucoes_lidas, sizeof(inst) * capacidade);
            }
            instrucoes_lidas[(num_instrucoes)++] = i;
            break;
        }
        case TOKEN_SECAO:
        case TOKEN_EOF:
            pronto = true;
            break;
        case TOKEN_ERRO:
            exit(1);
            break;
        default:
            fprintf(stderr, "Erro: linha %d: esperava instrucao, encontrado '", t.linha);
            token_lexema(t);
            fprintf(stderr, "'\n");
            free(instrucoes_lidas);
            exit(1);
        }
    }

    *instrucoes_lidas_saida = instrucoes_lidas;
    *num_instrucoes_saida = num_instrucoes;
}

void le_sb(char *src, char *fim, config *config_saida, inst **instrucoes_saida, int *n_instrucoes)
{
    scanner s;
    s.atual = src;
    s.fim = fim;
    s.inicio = src;
    s.linha = 1;

    // prepara lista de instruções lidas

    // lê a configuração
    espera_token(&s, TOKEN_COMECA_COMENTARIO);
    config c = configuracao(&s);
    *config_saida = c;

    // lê seção "data"
    dados(&s);

    // lê seção "text"
    int num_instrucoes;
    inst *instrucoes_lidas;
    instrucoes(&s, &num_instrucoes, &instrucoes_lidas);

    *instrucoes_saida = instrucoes_lidas;
    *n_instrucoes = num_instrucoes;
}
