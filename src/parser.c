#include "parser.h"

/*
 * next_char recebe um scanner e retorna o próximo caractere do scanner.
 */
char next_char(scanner *s)
{
    char c = *s->current;
    if (c != '\0')
        s->current++;
    return c;
}

/*
 * peek_char recebe um scanner e retorna o próximo caractere do scanner sem avançar o scanner.
 */
char peek_char(const scanner *s)
{
    return *s->current;
}

/*
 * skip_whitespace recebe um scanner e avança o scanner até o próximo caractere não-branco.
 */
void skip_whitespace(scanner *s)
{
    char c;
    while ((c = peek_char(s)) && isspace(c))
    {
        next_char(s);
        if (c == '\n')
        {
            s->line++;
        }
    }
}

/*
 * scan_number recebe um scanner e retorna um token do tipo TOKEN_NUMBER.
 * scaneando o tipo de número.
 */
token scan_number(scanner *s)
{
    char ch;
    while ((ch = peek_char(s)) && isdigit(ch))
    {
        next_char(s);
    }

    token t = make_token(s, TOKEN_NUMBER);
    char *buf = malloc(sizeof(char) * (t.len + 1));
    memcpy(buf, t.lexeme, t.len);
    buf[t.len] = '\0';
    long val = strtol(buf, 0, 10);
    if (errno == ERANGE)
    {
        free(buf);
        fprintf(stderr, "Erro: linha %d: numero fora do intervalo de um long\n", t.line);
        exit(1);
    }
    free(buf);

    t.value = val;
    return t;
}

/*
 * cmp_token recebe um token e uma string e retorna True se o token e a string são iguais.
 */
bool cmp_token(token t, const char *s)
{
    for (int i = 0; i < t.len; i++)
    {
        if (s[i] == '\0' || s[i] != t.lexeme[i])
        {
            return false;
        }
    }

    return s[t.len] == '\0';
}

/*
 * make_token recebe um scanner e um tipo de token, retornando
 * um token com o tipo especificado, o lexema do scanner e o número da linha.
 */
token make_token(const scanner *s, token_type type)
{
    token t;
    t.len = (int)(s->current - s->start);
    t.line = s->line;
    t.lexeme = s->start;
    t.type = type;
    t.value = 0;
    return t;
}

/*
 * make_error recebe um scanner e retorna um token de erro.
 */
token make_error(const scanner *s)
{
    token t;
    t.len = 0;
    t.lexeme = "";
    t.line = s->line;
    t.type = TOKEN_ERRO;
    t.value = 0;
    return t;
}

/*
 * print_token recebe um token e um arquivo e imprime o lexema do token no arquivo.
 */
void print_token(token t, FILE *f)
{
    fwrite(t.lexeme, sizeof *t.lexeme, t.len, f);
}

/*
 * token_type_str recebe um tipo de token e retorna uma string com o nome do tipo de token.
 */
const char *token_type_str(token_type type)
{
    switch (type)
    {
    case TOKEN_COMMA:
        return "','";
    case TOKEN_LPAREN:
        return "'('";
    case TOKEN_RPAREN:
        return "')'";
    case TOKEN_MINUS:
        return "'-'";
    case TOKEN_COLON:
        return "':'";
    case TOKEN_MNEMONIC:
        return "mnemonico";
    case TOKEN_REGISTER:
        return "registrador";
    case TOKEN_IDENTIFIER:
        return "identificador";
    case TOKEN_START_COMMENT:
        return "'/*'";
    case TOKEN_END_COMMENT:
        return "'*/'";
    case TOKEN_NUMBER:
        return "numero";
    case TOKEN_SECTION:
        return "secao";
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_ERRO:
        return "erro";
    }
    return "?";
}

/*
 * scan_section recebe um scanner e retorna um token do tipo TOKEN_SECTION.
 * escaneando a seção fornecida.
 */
token scan_section(scanner *s)
{
    char ch;
    while ((ch = peek_char(s)) && isalpha(ch))
    {
        next_char(s);
    }

    token t = make_token(s, TOKEN_SECTION);

    if (cmp_token(t, ".data"))
    {
        t.value = SECTION_DATA;
    }
    else if (cmp_token(t, ".text"))
    {
        t.value = SECTION_TEXT;
    }
    else
    {
        fprintf(stderr, "Erro: linha %d: esperava secao .data ou .text, encontrado '", t.line);
        print_token(t, stderr);
        fprintf(stderr, "'\n");
        exit(1);
    }

    return t;
}

/*
 * scan_identifier recebe um scanner e retorna um token do tipo TOKEN_IDENTIFIER.
 * Ou seja, vai escanear um identificador e retornar o token correspondente.
 */
token scan_identifier(scanner *s)
{
    char ch;
    while ((ch = peek_char(s)) && isalnum(ch))
    {
        next_char(s);
    }

    token t = make_token(s, TOKEN_MNEMONIC);

    switch (t.lexeme[0])
    {
    case 'a':
        if (t.len > 1)
        {
            switch (t.lexeme[1])
            {
            case 'n':
                if (cmp_token(t, "and"))
                {
                    t.value = OP_AND;
                    return t;
                }
                break;
            case 'd':
                if (cmp_token(t, "add"))
                {
                    t.value = OP_ADD;
                    return t;
                }
                else if (cmp_token(t, "addi"))
                {
                    t.value = OP_ADDI;
                    return t;
                }
                break;
            }
        }
        break;
    case 's':
        if (t.len > 1)
        {
            switch (t.lexeme[1])
            {
            case 'w':
                if (cmp_token(t, "sw"))
                {
                    t.value = OP_SW;
                    return t;
                }
                break;
            case 'u':
                if (cmp_token(t, "sub"))
                {
                    t.value = OP_SUB;
                    return t;
                }
                else if (cmp_token(t, "subi"))
                {
                    t.value = OP_SUBI;
                    return t;
                }
                break;
            }
        }
        break;
    case 'b':
        if (t.len > 1)
        {
            switch (t.lexeme[1])
            {
            case 'l':
                if (cmp_token(t, "blt"))
                {
                    t.value = OP_BLT;
                    return t;
                }
                break;
            case 'g':
                if (cmp_token(t, "bgt"))
                {
                    t.value = OP_BGT;
                    return t;
                }
                break;
            case 'e':
                if (cmp_token(t, "beq"))
                {
                    t.value = OP_BEQ;
                    return t;
                }
                break;
            case 'n':
                if (cmp_token(t, "bne"))
                {
                    t.value = OP_BNE;
                    return t;
                }
                break;
            }
        }
    case 'd':
        if (cmp_token(t, "div"))
        {
            t.value = OP_DIV;
            return t;
        }
        break;
    case 'e':
        if (cmp_token(t, "exit"))
        {
            t.value = OP_EXIT;
            return t;
        }
        break;
    case 'j':
        if (cmp_token(t, "j"))
        {
            t.value = OP_J;
            return t;
        }
        break;
    case 'l':
        if (cmp_token(t, "lw"))
        {
            t.value = OP_LW;
            return t;
        }
        break;
    case 'm':
        if (cmp_token(t, "mul"))
        {
            t.value = OP_MUL;
            return t;
        }
        break;
    case 'n':
        if (cmp_token(t, "not"))
        {
            t.value = OP_NOT;
            return t;
        }
        break;
    case 'o':
        if (cmp_token(t, "or"))
        {
            t.value = OP_OR;
            return t;
        }
        break;
    case 'r':
        if (t.len == 2)
        {
            if (isdigit(t.lexeme[1]))
            {
                t.type = TOKEN_REGISTER;
                t.value = (int)(t.lexeme[1] - '0');
                return t;
            }
        }
        else if (t.len == 3)
        {
            if (isdigit(t.lexeme[1]) && isdigit(t.lexeme[2]))
            {
                int d = t.lexeme[1] - '0';
                int u = t.lexeme[2] - '0';
                int reg = d * 10 + u;

                if (10 <= reg && reg <= 31)
                {
                    t.type = TOKEN_REGISTER;
                    t.value = reg;
                    return t;
                }
            }
        }
        break;
    }

    t.type = TOKEN_IDENTIFIER;
    return t;
}

/*
 * next_token recebe um scanner e retorna o próximo token do scanner.
 */
token next_token(scanner *s)
{
    s->start = s->current;
    char c = next_char(s);

    switch (c)
    {
    // tokens de um caractere
    case ',':
        return make_token(s, TOKEN_COMMA);
    case ':':
        return make_token(s, TOKEN_COLON);
    case '(':
        return make_token(s, TOKEN_LPAREN);
    case ')':
        return make_token(s, TOKEN_RPAREN);
    case '-':
        return make_token(s, TOKEN_MINUS);
    case '.':
    {
        if (isalpha(peek_char(s)))
        {
            return scan_section(s);
        }
        else
        {
            return make_error(s);
        }
    }

    // tokens de 2 caracteres
    case '/':
        if (peek_char(s) == '*')
        {
            next_char(s);
            return make_token(s, TOKEN_START_COMMENT);
        }
        else
        {
            return make_error(s);
        }

    case '*':
        if (peek_char(s) == '/')
        {
            next_char(s);
            return make_token(s, TOKEN_END_COMMENT);
        }
        else
        {
            return make_error(s);
        }

    // comentários
    case '#':
    {
        char ch;
        while ((ch = peek_char(s)) && ch != '\n')
        {
            next_char(s);
        }
        return next_token(s);
    }

    case '\n':
        s->line++;
    case '\r':
    case '\t':
    case '\f':
    case '\v':
    case ' ':
        skip_whitespace(s);
        return next_token(s);

    case '\0':
        return make_token(s, TOKEN_EOF);

    // tokens de múltiplos caracteres
    default:
    {
        if (isdigit(c))
        {
            return scan_number(s);
        }

        if (isalpha(c))
        {
            return scan_identifier(s);
        }

        return make_error(s);
    }
    }
}

/*
 * peek_token recebe um scanner e retorna o próximo token do scanner sem avançar o scanner.
 */
token peek_token(scanner *s)
{
    scanner s2 = *s;
    return next_token(&s2);
}

/*
 * expect_token recebe um scanner e um tipo de token, retornando o próximo token do scanner.
 * Se o token não for do tipo especificado, o programa é encerrado.
 */
token expect_token(scanner *s, token_type type)
{
    token t = peek_token(s);
    if (t.type != type)
    {
        fprintf(stderr, "Erro: linha %d: esperava %s, encontrado '", t.line, token_type_str(type));
        print_token(t, stderr);
        fprintf(stderr, "' (%s)\n", token_type_str(t.type));
        exit(1);
    }

    return next_token(s);
}

/*
 * instruction() recebe como parâmetro um scanner por referência, fazendo a leitura
 * de uma instrução com relação ao scanner passado. Caso algum elemento da instrução
 * seja inválido, o programa é encerrado. Caso contrário, a instrução é retornada
 * no formato de um número de 32 bits.
 */
uint32_t instruction(scanner *s)
{
    token mnemonic = expect_token(s, TOKEN_MNEMONIC);

    switch (mnemonic.value)
    {
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
    case OP_AND:
    case OP_OR:
    {
        // inst_3r -> op rd, rs, rt
        token rd = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);
        token rs = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);
        token rt = expect_token(s, TOKEN_REGISTER);

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (rs.value & 0x1F) << 21;       // rs
        i |= (rt.value & 0x1F) << 16;       // rt
        i |= (rd.value & 0x1F) << 11;       // rd
        return i;
    }

    case OP_ADDI:
    case OP_SUBI:
    {
        // inst_3i <- op rt, rs, imm
        token rt = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);
        token rs = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);

        token imm;
        if (peek_token(s).type == TOKEN_MINUS)
        {
            next_token(s);
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value <= INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
            imm.value = -imm.value;
        }
        else
        {
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value < INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
        }

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (rs.value & 0x1F) << 21;       // rs
        i |= (rt.value & 0x1F) << 16;       // rt
        i |= (imm.value & 0xFFFF);          // imm
        return i;
    }

    case OP_BLT:
    case OP_BGT:
    case OP_BEQ:
    case OP_BNE:
    {
        // inst_3i <- op rs, rt, imm
        token rs = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);
        token rt = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);

        token imm;
        if (peek_token(s).type == TOKEN_MINUS)
        {
            next_token(s);
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value <= INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
            imm.value = -imm.value;
        }
        else
        {
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value < INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
        }

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (rs.value & 0x1F) << 21;       // rs
        i |= (rt.value & 0x1F) << 16;       // rt
        i |= (imm.value & 0xFFFF);          // imm
        return i;
    }

    case OP_NOT:
    {
        // inst_2r <- not rd, rs
        token rd = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);
        token rs = expect_token(s, TOKEN_REGISTER);

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (rs.value & 0x1F) << 21;       // rs
        i |= (rd.value & 0x1F) << 11;       // rd
        return i;
    }

    case OP_J:
    {
        // inst_j <- j imm
        token imm = expect_token(s, TOKEN_NUMBER);

        if (imm.value > (1 << 26) - 1)
        {
            fprintf(stderr, "Erro: linha %d: numero fora do intervalo [0, 67108863]\n", imm.line);
            exit(1);
        }

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (imm.value & 0x3FFFFFF);       // address
        return i;
    }

    case OP_LW:
    case OP_SW:
    {
        // inst_m <- op rt, imm(rs)
        token rt = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_COMMA);

        token imm;
        if (peek_token(s).type == TOKEN_MINUS)
        {
            next_token(s);
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value <= INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
            imm.value = -imm.value;
        }
        else
        {
            imm = expect_token(s, TOKEN_NUMBER);
            if (imm.value > INT16_MAX || imm.value < INT16_MIN)
            {
                fprintf(stderr, "Erro: linha %d: numero fora do intervalo [-32768, 32767]\n", imm.line);
                exit(1);
            }
        }

        expect_token(s, TOKEN_LPAREN);
        token rs = expect_token(s, TOKEN_REGISTER);
        expect_token(s, TOKEN_RPAREN);

        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26; // op
        i |= (rs.value & 0x1F) << 21;       // rs
        i |= (rt.value & 0x1F) << 16;       // rt
        i |= (imm.value & 0xFFFF);          // imm
        return i;
    }

    case OP_EXIT:
    {
        // inst_e
        uint32_t i = 0;
        i |= (mnemonic.value & 0x3F) << 26;
        return i;
    }
    }

    return -1;
}

/*
 * decl_uf() recebe como parâmetro um scanner e uma configuração, fazendo a leitura
 * do operando de uma instrução e defininido a quantidade de unidades funcionais
 * que o processador possui para aquela operação. Se o token lido não for um operando
 * nada é feito.
 */
void decl_uf(scanner *s, config *c)
{
    token t = next_token(s);
    if (cmp_token(t, "add"))
    {
        // configura add
        expect_token(s, TOKEN_COLON);
        token n = expect_token(s, TOKEN_NUMBER);
        c->n_uf_add = n.value;
    }
    else if (cmp_token(t, "mul"))
    {
        // configura mul
        expect_token(s, TOKEN_COLON);
        token n = expect_token(s, TOKEN_NUMBER);
        c->n_uf_mul = n.value;
    }
    else if (cmp_token(t, "inteiro"))
    {
        // configura int
        expect_token(s, TOKEN_COLON);
        token n = expect_token(s, TOKEN_NUMBER);
        c->n_uf_int = n.value;
    }
    else if (!cmp_token(t, "UF") && !cmp_token(t, "INST"))
    {
        fprintf(stderr, "Erro: linha %d: declaracao de UF desconhecida: '", t.line);
        print_token(t, stderr);
        fprintf(stderr, "'\n");
        exit(1);
    }

    token next = peek_token(s);
    if (!cmp_token(next, "UF") && !cmp_token(next, "INST"))
    {
        decl_uf(s, c);
    }
}

/*
 * decl_inst() recebe como parâmetro um scanner e uma configuração, fazendo a leitura
 * de uma instrução e defininido o número de ciclos de clock necessários para a
 * execução da operação lida. Se o token lido não for um operando nada é feito.
 */
void decl_inst(scanner *s, config *c)
{
    if (peek_token(s).type == TOKEN_MNEMONIC)
    {
        token o = expect_token(s, TOKEN_MNEMONIC);
        expect_token(s, TOKEN_COLON);
        token n = expect_token(s, TOKEN_NUMBER);

        c->ck_instruction[o.value] = n.value;

        decl_inst(s, c);
    }
}

/*
 * comment() recebe como parâmetro um scanner e retorna uma configuração, fazendo a leitura
 * de um comentário e defininido a quantidade de unidades funcionais que o processador
 * possui para cada operação, bem como o número de ciclos de clock necessários para a
 * execução de cada instrução. Se o token lido não for um comentário, o programa é encerrado.
 */
config comment(scanner *s)
{
    config c;
    c.n_uf_add = 1;
    c.n_uf_int = 1;
    c.n_uf_mul = 1;
    c.ufs_per_group = N_UF_PER_GROUP;
    for (opcode o = OP_ADD; o < OP_EXIT; o++)
    {
        c.ck_instruction[o] = 1;
    }

    expect_token(s, TOKEN_START_COMMENT);

    while (peek_token(s).type == TOKEN_IDENTIFIER)
    {
        token t = next_token(s);
        if (cmp_token(t, "UF"))
        {
            expect_token(s, TOKEN_COLON);
            decl_uf(s, &c);
        }
        else if (cmp_token(t, "INST"))
        {
            expect_token(s, TOKEN_COLON);
            decl_inst(s, &c);
        }
        else
        {
            fprintf(stderr, "Erro: linha %d: esperava secao do comentario 'UF' ou 'INST', encontrado: '", t.line);
            print_token(t, stderr);
            fprintf(stderr, "'\n");
            exit(1);
        }
    }

    expect_token(s, TOKEN_END_COMMENT);

    return c;
}

/*
 * data_entry() recebe como parâmetro um scanner e um vetor, fazendo a leitura
 * de um número e adicionando-o ao vetor de dados do programa. Se o token lido
 * não for um número nada é feito.
 */
void data_entry(scanner *s, vector *v)
{
    token t = peek_token(s);

    if (t.type == TOKEN_MINUS)
    {
        next_token(s);
        token n = expect_token(s, TOKEN_NUMBER);
        n.value = -n.value;
        vector_push(v, n.value);

        data_entry(s, v);
    }
    else if (t.type == TOKEN_NUMBER)
    {
        token n = expect_token(s, TOKEN_NUMBER);
        vector_push(v, n.value);

        data_entry(s, v);
    }
}

/*
 * data_section() recebe como parâmetro um scanner e retorna um vetor de números,
 * fazendo a leitura de uma seção de dados e adicionando os números lidos ao vetor.
 * Se o token lido não for uma seção de dados, o programa é encerrado.
 */
vector data_section(scanner *s)
{
    token t = expect_token(s, TOKEN_SECTION);
    if (t.value != SECTION_DATA)
    {
        fprintf(stderr, "Erro: linha %d: esperava secao .data, encontrado '", t.line);
        print_token(t, stderr);
        fprintf(stderr, "'\n");
        exit(1);
    }

    vector v = vector_new();
    data_entry(s, &v);
    return v;
}

/*
 * text_section() recebe como parâmetro um scanner e retorna um vetor de números,
 * números esses que são as instruções convertidas para uint32_t, fazendo a
 * leitura de uma seção de texto e adicionando as instruções lidas ao vetor.
 * Se o token lido não for uma seção de texto, o programa é encerrado.
 */
vector text_section(scanner *s)
{
    token t = expect_token(s, TOKEN_SECTION);
    if (t.value != SECTION_TEXT)
    {
        fprintf(stderr, "Erro: linha %d: esperava secao .text, encontrado '", t.line);
        print_token(t, stderr);
        fprintf(stderr, "'\n");
        exit(1);
    }

    vector v = vector_new();

    while (peek_token(s).type == TOKEN_MNEMONIC)
    {
        uint32_t i = instruction(s);
        vector_push(&v, i);
    }

    return v;
}

/*
 * parse() recebe como parâmetro uma string, um vetor de números, um vetor de números
 * e uma configuração, fazendo a leitura de um programa e armazenando os dados lidos
 * nos vetores e na configuração.
 */
void parse(const char *src, vector *data_vector, vector *inst_vector, config *config_out)
{
    scanner s;
    s.current = src;
    s.start = src;
    s.line = 1;

    config c = comment(&s);
    vector data = data_section(&s);
    vector insts = text_section(&s);
    expect_token(&s, TOKEN_EOF);

    *data_vector = data;
    *inst_vector = insts;
    *config_out = c;
}