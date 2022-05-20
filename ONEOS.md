

# 开发板基本配置

## 1.文件系统挂载

​		sd0是基于spi2设备而创建的，需要先在ONEOSCUBE配置spi2。这里默认工程都是配置好的，不需要设置了。但是挂载是需要手动进行的，代码如下：

```c
#include <vfs_fs.h>										//单独使用只需要这个头文件
int sdmmc( void ){     
    /* mount the file system from tf card */
    if (vfs_mount("sd0", "/", "fat", 0, 0) == 0){
        os_kprintf("Filesystem initialized!\n");
    }
    else{
            os_kprintf("Failed to initialize filesystem!\n");
        }
        return 0;
}
SH_CMD_EXPORT(sdmmc, sdmmc, "sdmmc");					//添加到控制台（需要其他头文件）
```

​		文件挂载ONEOS提供了专门的.c文件，只需要挂载一次就可以正常使用，但是这个程序需要在控制台手动执行（需要一些其他头文件，使用demo文件夹中给出的sdmmc_test）。或者在main中调用在每次启动时挂载。

## 2.网络连接

​		Molink组件可以实现开发板连接网络。在Components→ Network→ Molink路径下配置WIFI模组。设置WIFI名和密码。并需要在tools下打开调试。

```shell
    (Top) → Components→ Network→ Molink→ Enable IoT modules support → Module→ WiFi Modules Support→ ESP8266 → ESP8266 Confi cfg
                                                                                                                OneOS Configuration
    [*] Enable ESP8266 Module Object Auto Create
    (uart1) ESP8266 Interface Device Name								//串口一是Molink
    (115200) ESP8266 Interface Device Rate (NEW)
    (512)   The maximum length of AT command data accepted (NEW)
    [*]     Enable ESP8266 Module Auto Connect (NEW)
    (ssid) ESP8266 Connect AP SSID (NEW)								//WIFI名
    (password) ESP8266 Connect AP Password (NEW)						//密码
    [*] Enable ESP8266 Module General Operates (NEW)
    -*- Enable ESP8266 Module WiFi Operates (NEW)
    [*] Enable ESP8266 Module Ping Operates (NEW)
    [*] Enable ESP8266 Module Ifconfig Operates (NEW)
    -*- Enable ESP8266 Module Network TCP/IP Operates (NEW)
    [*] Enable ESP8266 Module BSD Socket Operates						//打开
    [ ] Enable ESP8266 Module Hardware Control Operates (NEW)
    ---------------------------------------------------------------------------------------------
    (Top) → Components→ Network→ Molink→ Enable IoT modules support → Tool                                                     cfg
                                                                                                                OneOS Configuration
    [*] Enable AT module network debug functions
    [*]     Enable molink ifconfig features (NEW)
    [*]     Enable molink ping features (NEW)
    (50)        The maximum times of molink ping cmd config (NEW)
    [*]     Enable molink socket stat features (NEW)
    [ ]     Enable molink iperf feature (NEW)
```

​		保存配置，下载程序到开发板，可以使用ifconfig命令查看WIFI是否连接成功。

## 3.MQTT连接OneNET

​		通过MQTT协议，开发板可以实现与onenet平台连接并通信。进入Onenet，在全部服务中找到==MQTT物联网套件==，后续的连接是在该套件内进行的，不是Onenet Studio。进入MQTT物联网套件后可以创建产品以及创建设备。可以得到产品ID，用户ID，产品KEY，设备ID，设备KEY，接下来的连接需要用到。

​		在Components→ Cloud→ OneNET→ MQTT kit下配置MQTT

```shell
    (Top) → Components→ Cloud→ OneNET→ MQTT kit                                                                                 cfg
                                                                                                                OneOS Configuration
    [*] Enable onenet mqtt-kit
    (128)   The publish data buffer length (NEW)
    [ ]     Enable mqtt-kit TLS encrypt (NEW)
    [*]     Enable onenet device auto register
```

​		在OS的OneOS-V2.2.0\components\cloud\onenet\mqtt-kit\mqtts_device文件夹下会产生测试程序onenet_device_sample.c，对应的.h文件里面需要设置ID。

```c
//onenet_device_sample.h
#define USER_PRODUCT_ID "516520"
#define USER_ACCESS_KEY "KPDKIzM8dyFKF/OVThXV/fGN43yEaxp4+74BK0FL7pU="
#define USER_DEVICE_NAME                                                                                               \
    "STM32L475"             /*use characters, numbers or symbols like '_' or '-',                              \
                                    no longer than 64, can use device serial num*/
#define USER_KEEPALIVE_INTERVAL 240 /*onenent heart interval 10~1800s*/
#define USER_PUBLISH_INTERVAL   10  /*user onenet data upload interval*/

#ifndef ONENET_MQTTS_USING_AUTO_REGISTER
#define USER_DEVICE_ID "943273514"
#define USER_KEY       "uME4rWyH7eJK43gSJFDQ4JsS+u8Vx/DCi43vYSRQ5Z4="
#endif
```

​		下载程序后，输入指令onenet_mqtts_device_start，启动设备，generate_onenet_publish_data_cycle就可以发数据了。

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

