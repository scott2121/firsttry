#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>    
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(-1);
	} else if (pid == 0) {
		// 子プロセスで別プログラムを実行
		int ret;
        ret = execlp("sort","sort",argv[1],"-o","sorted.txt",NULL);
            if (ret == -1) {
                perror("sort\n");
            return 1;
        }
		exit(-1);
	}

	// 親プロセス
	int status;
	pid_t r = waitpid(pid, &status, 0); //子プロセスの終了待ち
	if (r < 0) {
		perror("waitpid");
		exit(-1);
	}
	if (WIFEXITED(status)) {

    int rf;
    if ((rf = open("sorted.txt", O_RDONLY)) < 0) {
        perror("open");
        return 1;
    }
    // パイプをつなぐ
    int fd[2];
    if(pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }
    // プロセスをforkする
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return 1;
    }
    // 子プロセス
    if(pid == 0) {
        // 使わないファイルディスクリプタを閉じる
        close(fd[1]);
        close(rf);
        // stdinをパイプの出力に変更
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        // wcコマンドを実行
        execlp("wc", "wc", "-l", NULL);
    } else { // 親プロセス
        // 使わないファイルディスクリプタを閉じる
        close(fd[0]);
        // stdoutをパイプの入力に変更
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        // stdinをtmp.txtに変更
        dup2(rf, STDIN_FILENO);
        close(rf);
        // uniqコマンドを実行
        execlp("uniq", "uniq", "-D", NULL);
    }
    }
}