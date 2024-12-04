// 1. Child Process Oluşturma ve Yönetimi
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void basic_child_process() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork başarısız oldu");
        exit(1);
    }

    if (pid == 0) {  // Child process
        printf("Child process çalışıyor (PID: %d)\n", getpid());
        execlp("cat", "cat", "/Users/bernaelmas/CLionProjects/untitled/test.txt", NULL);
        // Eğer execlp başarısız olursa buraya ulaşır
        perror("Komut çalıştırılamadı");
        exit(1);
    } else {  // Parent process
        printf("Parent process bekliyor (PID: %d)\n", getpid());
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("Child process normal şekilde sonlandı. Çıkış kodu: %d\n",
                   WEXITSTATUS(status));
        }
    }
}

// 2. Hata Yönetimi
void error_handling() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork başarısız oldu");
        exit(1);
    }

    if (pid == 0) {  // Child process
        FILE *fp = fopen("/Users/bernaelmas/CLionProjects/untitled/nonexistent.txt", "r");
        if (fp == NULL) {
            printf("Dosya açılamadı, process sonlandırılıyor...\n");
            abort();  // Zorla sonlandırma
        }
        fclose(fp);
        exit(0);  // Normal sonlandırma
    } else {  // Parent process
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            printf("Child process normal şekilde sonlandı. Çıkış kodu: %d\n",
                   WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process sinyal ile sonlandı. Sinyal: %d\n",
                   WTERMSIG(status));
        }
    }
}

// 3. Paralel Child Process'ler
void create_file() {
    FILE *fp = fopen("output.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "Dosya oluşturuldu\n");
        fclose(fp);
        exit(0);
    }
    exit(1);
}

void write_file() {
    FILE *fp = fopen("output.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "Veri eklendi\n");
        fclose(fp);
        exit(0);
    }
    exit(1);
}

void read_file() {
    FILE *fp = fopen("output.txt", "r");
    if (fp != NULL) {
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("%s", buffer);
        }
        fclose(fp);
        exit(0);
    }
    exit(1);
}

void parallel_processes() {
    pid_t pid1, pid2, pid3;

    // İlk child process
    pid1 = fork();
    if (pid1 == 0) {
        create_file();
    }

    // İkinci child process
    pid2 = fork();
    if (pid2 == 0) {
        sleep(1);  // Dosyanın oluşturulmasını bekle
        write_file();
    }

    // Üçüncü child process
    pid3 = fork();
    if (pid3 == 0) {
        sleep(2);  // Yazma işleminin tamamlanmasını bekle
        read_file();
    }

    // Parent process tüm child'ları bekler
    int status;
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);
}

int main() {
    printf("\n1. Basic Child Process Örneği:\n");
    basic_child_process();

    printf("\n2. Hata Yönetimi Örneği:\n");
    error_handling();

    printf("\n3. Paralel Processes Örneği:\n");
    parallel_processes();

    return 0;
}