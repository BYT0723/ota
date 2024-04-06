#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NAME_SIZE 255
#define MAX_PATH_SIZE 1024

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

struct App {
  char name[MAX_NAME_SIZE];
  char path[MAX_PATH_SIZE];
  pid_t pid;
};

int is_executable(const char *path) {
  struct stat st;

  if (stat(path, &st) == -1) {
    perror("stat");
    return 0; // 如果无法获取文件信息，则不视为可执行文件
  }
  return (st.st_mode & S_IXUSR) != 0;
}

int launch_app(struct App *app) {

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  } else if (pid == 0) { // 子进程
    execl(app->path, app->name, NULL);
    perror("execl"); // 如果execl失败，打印错误信息
    exit(EXIT_FAILURE);
  } else { // 父进程
    // 记录子进程的PID
    app->pid = pid;
  }
  return 0;
}

int read_dir(char *path, struct App *apps, int *count) {
  DIR *dir;
  struct dirent *entry;

  dir = opendir(path);
  if (dir == NULL) {
    perror("opendir");
    return -1; // 返回错误码
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char full_path[MAX_PATH_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%c%s", path, PATH_SEPARATOR,
             entry->d_name);

    if (entry->d_type == DT_DIR) {
      if (read_dir(full_path, apps, count) == -1) { // 处理子目录读取失败的情况
        closedir(dir);
        return -1;
      }
    } else {
      if (is_executable(full_path)) {
        struct App app;
        snprintf(app.name, sizeof(app.name), "%s", entry->d_name);
        snprintf(app.path, sizeof(app.path), "%s", full_path);
        apps[(*count)++] = app;
      }
    }
  }

  closedir(dir);
  return 0; // 返回成功码
}
