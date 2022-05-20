#include <board.h>
#include <dlog.h>
#include <shell.h>
#include <fcntl.h>
#define TAG "INFO"

static void sensor_on(const char* sensor_name);
static void sensor_off(const char* sensor_name);

static void sensor(os_int32_t argc, char **argv){
    if (argc < 3){
        LOG_I(TAG, "usage: sensor sensor_name1 sensor name2 ... on/off");
        return;
    }
		if (argv[1] == "on"){
				sensor_on(argv[2]);
		}
		return;
}
SH_CMD_EXPORT(sensor, sensor, "sensor on/off");

static void sensor_on(const char* sensor_name){
		/* \data sensor_data */
	int fd;	
	if(!opendir("data")){
			if(mkdir("data",0x777)==0){
				opendir("data");
			}
			else{
				LOG_W(TAG,"mkdir failed");
				return;
			}
		}
		fd=open("light_data",O_RDWR|O_CREAT);
		close(fd);
}