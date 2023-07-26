#include "leitor_sb.h"

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

    return t;
}

const char *tipo_token_string(token_tipo tipo)
{
    switch (tipo)
    {
    case TOKEN_DOIS_PONTOS:
        return ":";
    case TOKEN_PORCENTO:
        return "%";
    case TOKEN_VIRGULA:
        return ",";
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
    case '%':
        return scanner_faz_token(s, TOKEN_PORCENTO);

    case '(':
        return scanner_faz_token(s, TOKEN_PAREN_ESQ);

    case ')':
        return scanner_faz_token(s, TOKEN_PAREN_DIR);

    case '-':
        return scanner_faz_token(s, TOKEN_MENOS);

    case ',':
        return scanner_faz_token(s, TOKEN_VIRGULA);

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
        // numeros
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
        fprintf(stderr, "Erro: caractere desconhecido '%c'\n", ch);
        return scanner_faz_erro(s);
    }
    }
}

void token_lexema(token t)
{
    fwrite(t.inicio, sizeof(char), t.comp, stdout);
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
    else if (compara_token(nome, "int"))
        c->n_uf_int = valor.valor;
}

void atrib_inst(scanner *s, config *c)
{
    token nome = espera_token(s, TOKEN_NOME);
    espera_token(s, TOKEN_DOIS_PONTOS);
    token valor = espera_token(s, TOKEN_NUMERO);

    if (compara_token(nome, "ld"))
        c->ck_ld = valor.valor;
    else if (compara_token(nome, "mul"))
        c->ck_mul = valor.valor;
    else if (compara_token(nome, "add"))
        c->ck_add = valor.valor;
    else if (compara_token(nome, "sub"))
        c->ck_sub = valor.valor;
    else if (compara_token(nome, "div"))
        c->ck_div = valor.valor;
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

config configuracao(scanner *s)
{
    config c = {0};

    token t;
    while ((t = ve_token(s)).tipo != TOKEN_PORCENTO)
    {
        if (t.tipo == TOKEN_EOF)
        {
            fprintf(stderr, "Erro: linha %d: Comentario de configuracao nao finalizado", t.linha);
            exit(1);
        }
        bloco_configuracao(s, &c);
    }
    espera_token(s, TOKEN_PORCENTO);

    return c;
}

inst instrucao(scanner *s)
{
    token opcode = espera_token(s, TOKEN_NOME);

    /*
        FORMATOS:

        1 add, sub, mul, div, and, or:
            op  rd, rs, rt
        2 addi, subi, blt, bgt, beq, bne:
            op  rs, rt, imm
        3 not:
            op  rd, rs
        4 j:
            op  imm
        5 lw, sw:
            op  rd, imm(rs)

    */

    opcode_instrucao op;
    int formato = -1;

    switch (opcode.inicio[0])
    {
    case 'd':
        // div
        if (compara_token(opcode, "div"))
        {
            op = OP_DIV;
            formato = 1;
        }
        break;
    case 'j':
        // j
        if (compara_token(opcode, "j"))
        {
            op = OP_J;
            formato = 4;
        }
        break;
    case 'l':
        // lw
        if (compara_token(opcode, "lw"))
        {
            op = OP_LW;
            formato = 5;
        }
        break;
    case 'm':
        // mul
        if (compara_token(opcode, "mul"))
        {
            op = OP_MUL;
            formato = 1;
        }
        break;
    case 'n':
        // not
        if (compara_token(opcode, "not"))
        {
            op = OP_NOT;
            formato = 3;
        }
        break;
    case 'o':
        // or
        if (compara_token(opcode, "or"))
        {
            op = OP_OR;
            formato = 1;
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
                    op = OP_SUB;
                    formato = 1;
                }
                else if (compara_token(opcode, "subi"))
                {
                    op = OP_SUBI;
                    formato = 2;
                }
                break;
            case 'w':
                // sw
                if (compara_token(opcode, "sw"))
                {
                    op = OP_SW;
                    formato = 5;
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
                    op = OP_ADD;
                    formato = 1;
                }
                else if (compara_token(opcode, "div"))
                {
                    op = OP_ADDI;
                    formato = 2;
                }
                break;
            case 'n':
                // and
                if (compara_token(opcode, "and"))
                {
                    op = OP_AND;
                    formato = 1;
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
                    op = OP_BEQ;
                    formato = 1;
                }
                break;
            case 'g':
                // bgt
                if (compara_token(opcode, "bgt"))
                {
                    op = OP_BGT;
                    formato = 1;
                }
                break;
            case 'l':
                // blt
                if (compara_token(opcode, "blt"))
                {
                    op = OP_BLT;
                    formato = 1;
                }
                break;
            case 'n':
                // bne
                if (compara_token(opcode, "bne"))
                {
                    op = OP_BNE;
                    formato = 1;
                }
                break;
            }
        }
    }

    switch (formato)
    {
    case 1:
    {
        token destino = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token op1 = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token op2 = espera_token(s, TOKEN_REGISTRADOR);

        return (inst){
            .opcode = op,
            .op1 = destino.valor,
            .op2 = op1.valor,
            .op3 = op2.valor};
    }
    case 2:
    {
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

        return (inst){
            .opcode = op,
            .op1 = destino.valor,
            .op2 = op1.valor,
            .op3 = negativo * imm.valor};
    }
    case 3:
    {
        token destino = espera_token(s, TOKEN_REGISTRADOR);
        espera_token(s, TOKEN_VIRGULA);
        token fonte = espera_token(s, TOKEN_REGISTRADOR);

        return (inst){
            .opcode = op,
            .op1 = destino.valor,
            .op2 = fonte.valor,
            .op3 = 0};
    }
    case 4:
    {
        token imm = espera_token(s, TOKEN_NUMERO);

        return (inst){
            .opcode = op,
            .op1 = imm.valor,
            .op2 = 0,
            .op3 = 0};
    }
    case 5:
    {
        // formato ld f1 (num)f3
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

        return (inst){
            .opcode = op,
            .op1 = dest.valor,
            .op2 = negativo * num.valor,
            .op3 = base.valor};
    }
    default:
        fprintf(stderr, "Erro: linha %d: instrucao nao reconhecida '", opcode.linha);
        token_lexema(opcode);
        fprintf(stderr, "'\n");
        exit(1);
    }
}

void le_sb(char *src, char *fim, config *config_saida, inst **instrucoes_saida, int *n_instrucoes)
{
    scanner s;
    s.atual = src;
    s.fim = fim;
    s.inicio = src;
    s.linha = 1;

    // prepara lista de instruções lidas
    int num_instrucoes = 0;
    int capacidade = 1;
    inst *instrucoes_lidas = realloc(NULL, sizeof(inst) * capacidade);

    // lê a configuração
    espera_token(&s, TOKEN_PORCENTO);
    config c = configuracao(&s);
    *config_saida = c;

    // lê as demais instruções
    bool pronto = false;
    int n_instrucao = 0;
    while (!pronto)
    {
        token t = ve_token(&s);
        switch (t.tipo)
        {
        case TOKEN_NOME:
        {
            inst i = instrucao(&s);
            if (num_instrucoes + 1 >= capacidade)
            {
                capacidade *= 2;
                instrucoes_lidas = realloc(instrucoes_lidas, sizeof(inst) * capacidade);
            }
            instrucoes_lidas[num_instrucoes++] = i;
            break;
        }
        case TOKEN_EOF:
            pronto = true;
            break;
        case TOKEN_PORCENTO:
            fprintf(stderr, "Erro: linha %d: um arquivo so pode haver um comentario de configuracao\n", t.linha);
            exit(1);
            break;
        case TOKEN_ERRO:
            exit(1);
            break;
        default:
            fprintf(stderr, "Erro: linha %d: esperava instrucao, encontrado '", t.linha);
            token_lexema(t);
            fprintf(stderr, "'\n");
            exit(1);
        }
    }

    *instrucoes_saida = instrucoes_lidas;
    *n_instrucoes = num_instrucoes;
}
