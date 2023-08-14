// #include <stdio.h>

// int main() {
//     int m[6][4] = {{1, 2, 3, 4},
//                    {5, 6, 7, 8},
//                    {6, 9, 19, 20},
//                    {7, 9, 11, 12},
//                    {8, 21, 22, 62},
//                    {13, 14, 16, 22}}; // Note que adicionei um valor 0 no final para evitar acessar posições não inicializadas.

//     char operacoes[6][30] = {"L", "L", "M", "S", "D", "A"};

//     printf("\nInstrucao\n\n");
//     printf("*----*---*----*----*----*\n");
//     printf("| OP | E | LO | Ex | Er |\n");
//     printf("*----*---*----*----*----*\n");

//     // Loop para imprimir os valores na tabela

//     for (int i = 0; i < 6; i++) {
//         printf("|%4s|%3d|%4d|%4d|%4d|\n",operacoes[i], m[i][0], m[i][1], m[i][2], m[i][3]);
//     }

//     printf("*----*---*----*----*----*\n");

//     printf("\n\nUnidades Funcionais\n\n");

//     char uf[5][7] = {"Int", "Mult1", "Mult2", "ADD", "Div"};

//     printf("*-----*-----*-----*-----*-----*-----*-----*-----*\n");
//     printf("| UF  |  B  |  Fi |  Fk |  Qj |  Qk |  Rj |  Rk |\n");
//     printf("*-----*-----*-----*-----*-----*-----*-----*-----*\n");

//     for (int i = 0; i <= 4; i++) {
//         printf("|%3s\n", uf[i]);
//     }

//     printf("*-----*-----*-----*-----*-----*-----*-----*-----*\n");

//     char regs[1][5] = {{'I', 'M','D', 'A', 'N'}};

//     printf("\n\nRegistradores\n\n");
//     printf("     *----*----*----*----*----*\n");
//     printf("     | F0 | F1 | F2 | F4 | F10|\n");
//     printf("     *----*----*----*----*----*\n");
//     printf("  UF ");

//     printf("%5c|%4c|%4c|%4c|%4c|", regs[0][0], regs[0][1], regs[0][2], regs[0][3], regs[0][4]);

//     printf("\n");
//     printf("     *----*----*----*----*----*\n");

//     return 0;

// }
