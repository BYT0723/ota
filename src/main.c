#include "os/app.c"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int count = 0;
  struct App *apps = malloc(sizeof(struct App) * 1024);
  if (apps == NULL) {
    perror("malloc");
    return 1;
  }

  if (read_dir("../apps", apps, &count) == -1) {
    printf("Failed to list files.\n");
    // 释放内存
    free(apps);
    return 1;
  }

  // 打印文件列表
  for (int i = 0; i < count; i++) {
    if (launch_app(&apps[i]) == 0) {
      printf("pid: %d, name: %s; path: %s\n", apps[i].pid, apps[i].name,
             apps[i].path);
    }
  }
  sleep(10);

  for (int i = 0; i < count; i++) {
    kill(apps[i].pid, SIGINT);
    int status;
    if (waitpid(apps[i].pid, &status, 0) == -1) {
      perror("waitpid");
      return -1;
    }

    // 检查子进程退出状态
    if (WIFEXITED(status)) {
      printf("子进程 %d 正常退出，退出码：%d\n", apps[i].pid,
             WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("子进程 %d 被信号终止，信号编号：%d\n", apps[i].pid,
             WTERMSIG(status));
    }
  }

  free(apps);
  return 0;
}
