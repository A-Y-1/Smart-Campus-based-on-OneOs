

# ONEOS

## 一.常用函数

**os_kprintf**

​		常规方法是使用调试日志，也可以使用常规的输出，此函数与printf用法相同，**不支持浮点数**

```c
void os_kprintf(const char *fmt, ...);
```

**OS_NULL**

## 二.组件

### 1.日志

​		DLOG日志系统默认支持输出到串口，也可以支持网络，文件形式。日志一共有4个等级：

| **日志等级宏** | **说明**           |
| :------------- | :----------------- |
| DLOG_ERROR     | 错误级别(最高级别) |
| DLOG_WARNING   | 告警级别           |
| DLOG_INFO      | 信息级别           |
| DLOG_DEBUG     | 调试级别(最低级别) |

​		使用宏定义进行不同等级日志的输出，默认输出到控制台。

| 宏名称  | **说明**                            |
| :------ | :---------------------------------- |
| LOG_E   | 该宏为 DLOG_ERROR 等级日志输出      |
| LOG_W   | 该宏为 LOG_LVL_WARNING 等级日志输出 |
| LOG_I   | 该宏为 LOG_LVL_INFO 等级日志输出    |
| LOG_D   | 该宏为 LOG_LVL_DEBUG 等级日志输出   |
| LOG_RAW | 常规日志输出                        |
| LOG_HEX | 十六进制日志输出                    |

​		使用方法：

```c
#define TAG_A               "TAG_A"
#define TAG_B               "TAG_B"
void func(){
    LOG_E(TAG_A, "DLOG_ERROR 等级日志输出");
}
```

### 2.SHELL控制台

​		shell控制台提供的接口如下：

| **接口**              | **说明**                                                |
| :-------------------- | :------------------------------------------------------ |
| SH_CMD_EXPORT         | 注册shell命令                                           |
| sh_exec               | 该接口把SHELL命令及相关参数的字符串传入此接口，执行命令 |
| sh_disconnect_console | 断开SHELL与控制台设备的连接                             |
| sh_reconnect_console  | 重新建立SHELL与控制台设备的连接                         |

​		SH_CMD_EXPORT(cmd, func, desc)用于注册shell命令，cmd为命令，func为调用函数，desc为描述，无返回值。

```c
SH_CMD_EXPORT(cmd, func, desc);
```

​		sh_exec用于将SHELL命令及相关参数的字符串传入此接口，执行命令，函数原型如下：

```
os_err_t sh_exec(const char *cmd);
```

​		使用方法：

```c
#define TEST_TAG        "TEST"
static void multi_param_sample(os_int32_t argc, char **argv)
{
    if (argc != 2){
        LOG_W(TEST_TAG, "usage: multi_param_cmd <action1|action2>");
        return;
    }
    if (!strcmp(argv[1], "action1")){
        LOG_W(TEST_TAG, "do action1");
    }
    else if (!strcmp(argv[1], "action2")){
        LOG_W(TEST_TAG, "do action2");
    }
    else{
        LOG_W(TEST_TAG, "invalid action");
    }
}
SH_CMD_EXPORT(multi_param_cmd, multi_param_sample, "test for multi parameter shell cmd");
```

​		📝**向控制台添加可执行的程序，只需要编写单独的.c文件，并使用SH_CMD_EXPORT在控制台进行注册，就可以在控制台运行了，不需要显式的调用。**

### 3.文件系统

​		需要包含的头文件：

```c
#include <fcntl.h>
```

​		**mkdir**

​		该函数用于创建目录，函数原型如下，返回0创建成功：

```
int mkdir(const char *path, mode_t mode);	
```

​		**opendir**

​		该函数用于打开目录，函数原型如下，返回不为空则打开成功：

```
DIR *opendir(const char *path)
```

​	**open**

```c
int open(const char *file,int flag,...)
```

 		flag：

​		主类：O_RDONLY 以只读方式打开  /  O_WRONLY 以只写方式打开  /O_RDWR 以可读可写方式打开

​		 副类:

-  O_CREAT 如果文件不存在则创建该文件
-  O_EXCL 如果使用O_CREAT选项且文件存在，则返回错误消息
-  O_NOCTTY 如果文件为终端，那么终端不可以调用open系统调用的那个进程的控制终端
-  O_TRUNC 如果文件已经存在则删除文件中原有数据
-  O_APPEND 以追加的方式打开

​		**read**

​		该函数用于从文件中读取指定长度的数据，函数原型如下：

```
int read(int fd, void *buf, size_t nbyte);
```

​		返回获取数据的长度。

