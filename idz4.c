#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
int mas[26]; //ключ шифрования
char buff[100000]; // начальный текст
char ans[200000]; // итоговый текст
int buff_size; //размер начального текста
int threadNumber = 8; //количество потоков
void* translate(void* param){
    int task_size = (buff_size + threadNumber - 1) / threadNumber; //размер подстроки, которую меняет один поток
    int buff_start = (*(int*)param) * task_size; //начало этой подстроки в начальном тексте
    int ans_start = 2 * buff_start; //начальная позиция записи в ответ, она больше 2 раза так как один символ начального текста может превратиться в 2 символа ответа
    int j = ans_start;
    for(int i = buff_start; i < buff_start + task_size && i < buff_size; i++) {
        if(buff[i] >= 'a' && buff[i] <= 'z'){ // встретили букву - шифруем по таблице
            ans[j] = '0' + mas[buff[i] - 'a'] / 10;
            ans[j + 1] = '0' + mas[buff[i] - 'a'] % 10;
            j += 2;
            continue;
        }
        if(buff[i] >= 'A' && buff[i] <= 'Z'){ // встретили букву - шифруем по таблице
            ans[j] = '0' + mas[buff[i] - 'A'] / 10;
            ans[j + 1] = '0' + mas[buff[i] - 'A'] % 10;
            j += 2;
            continue;
        }
        ans[j] = buff[i]; //если не буква - просто оставляем символ
        j++;
        continue;
    }
    // для каждой подзадачи в итоговом массиве мы выделяем 2*task_size символов, так как если в шифруемой подстроке только буквы, то они займут как раз 2*task_size
    // позиций. Но если в шифруемой подстроке есть другие символы, то итоговая подстрока получится меньше, чем мы предполагали и в массиве появятся неприятные 
    // дырки. Чтобы их избежать будем все такие дырки заполнять символом а и просто его не выводить при печати ответа. Этот символ подходит, так как после шифрования
    // сам он появиться в ответе не может
    for(int i = j; i < ans_start + 2 * task_size && i < 2 * buff_size; i++){ 
        ans[i] = 'a';
    }
    //printf("Thread number %d finished working\n", *(int *)param);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    mas[0] = 0; // создаем ключ шифрования
    for(int i = 1; i < 26; i++) { // так как 33 и 26 - взаимно простые числа, то все 26 остатков будут разные
        mas[i] = mas[i-1] + 33;
        mas[i] %= 26;
    }
    for(int i = 0; i < 26; i++) { //сдвигаем их на 50, чтобы все элементы были двузначные, сам сдвиг не важен. Так как все коды - двузначные, то дешифровать текст будет легко
        mas[i] += 50;
    }
    char ch;
    int ok = 0; //проверка, что мы что-то ввели
    FILE* out; //название выходного файла
    //дальше идет проверка на корректное число входных параметров и ввод данных
    // при рандомном вводе надо вводить ./code --random file, где file - это имя выходного файла
    // при консольном вводе надо вводить ./code --terminal file, где file - это имя выходного файла
    // при файловом вводе надо вводить ./code --file file1 file2, где file1 - это имя входного файла, а file2 - это имя выходного файла
    // при вводе из командной строки надо вводить ./code --terminal file word1 word2... , где file - это имя выходного файла, word1 word2... - шифруемые слова
    if(argc < 2){
        printf("Wrong number of arguments\n");
        return 0;
    }
    if(strcmp(argv[1], "--random") == 0){
        if(argc < 3){
            printf("Wrong number of arguments\n");
            return 0;
        }
        buff_size = rand()%100000;
        if(buff_size == 0)
            buff_size++;
        for(int y = 0; y < buff_size; y++){
            ch = rand()%95;
            ch += 32;
            buff[y] = ch;
        }
        if(buff_size < 100000)
            buff[buff_size] = '\0';
        ok = 1;  
        out = fopen(argv[2], "w");
        if(out == NULL){
            printf("Wrong output file path");
            return 0;            
        }
    }
    if(strcmp(argv[1], "--file") == 0) {
        if(argc < 4){
            printf("Wrong number of arguments\n");
            return 0;
        }
        FILE* in = fopen(argv[2], "r");
        if(in == NULL){
            printf("Wrong input file path\n");
            return 0;
        }
        while(ch != -1 && buff_size < 1000000) {
            ch = fgetc(in);
            buff[buff_size++] = ch;
        }
        if(buff_size == 1000000 && buff[buff_size - 1] != -1){
            printf("The text is too long\n");
            return 0;
        } 
        buff[buff_size-1] = '\0';
        buff_size--;
        fclose(in);
        ok = 1;
        out = fopen(argv[3], "w");
        if(out == NULL){
            printf("Wrong output file path");
            return 0;            
        }
    }
    if(strcmp(argv[1], "--console") == 0){
        if(argc < 3){
            printf("Wrong number of arguments\n");
            return 0;
        }
        while(ch != -1 && buff_size < 1000000) {
            ch = fgetc(stdin);
            buff[buff_size++] = ch;
        }
        if(buff_size == 1000000 && buff[buff_size - 1] != -1){
            printf("The text is too long\n");            
            return 0;
        }
        buff[buff_size-1] = '\0';
        buff_size--;
        ok = 1;
        out = fopen(argv[2], "w");
        if(out == NULL){
            printf("Wrong output file path");
            return 0;            
        }
    }
    if(strcmp(argv[1], "--terminal") == 0){
        if(argc < 3){
            printf("Wrong number of arguments\n");
            return 0;
        }
        if(argc == 3){
            ok = 1;
        }
        else{
            int pos = 3;
            int j = 0;
            while(pos < argc && buff_size < 1000000) {
                if(j == strlen(argv[pos]) && pos + 1 < argc){
                    j = 0;
                    pos++;
                    buff[buff_size] = ' ';
                    buff_size++;
                    continue;
                }
                if(j == strlen(argv[pos])){
                    break;
                }
                ch = argv[pos][j];
                buff[buff_size++] = ch;
                j++;
            }
            if(buff_size == 1000000 && buff[buff_size - 1] != -1){
                printf("The text is too long\n");            
                return 0;
            }   
            buff[buff_size-1] = '\0';
            buff_size--;
            ok = 1;
        }
        out = fopen(argv[2], "w");
        if(out == NULL){
            printf("Wrong output file path");
            return 0;            
        }
    } 
    if(ok == 0){
        printf("Wrong input format\n");            
        return 0;
    }
    pthread_t threads[threadNumber]; // создаем 8 потоков
    int number[threadNumber]; //массив номеров задачи для каждого потока
    for(int i = 0; i < threadNumber; i++) {
        number[i] = i; //запоминаем номер подзадачи для каждого потока
        pthread_create(&threads[i], NULL, translate, (void*)(number + i)); //
    }
    for(int i = 0; i < threadNumber; i++) {
        pthread_join(threads[i], NULL);
    }
    //вывод ответа
    for(int i = 0; i < 2 * buff_size; i++){
        if(ans[i] == 'a')
        continue;
        fputc(ans[i], out);
    }
    return 0;
}

