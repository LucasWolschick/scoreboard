#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leitor_sb.h"

int main(void)
{
    char *buf = arquivo_para_string("test/exemplo.sb");
    int l = strlen(buf);
    le_sb(buf, (buf + l));
    free(buf);

    return 0;
}