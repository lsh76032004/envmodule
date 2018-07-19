#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <mcp3422.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <math.h>


struct SV {
	int temp;
	int cds;
	int humi;
};

static struct SV env;

#define LED_RATIO _IOW('c', 1, int)
#define BUZ_ON	  _IO('c',2)

int calTemp(int temp) {
	double voltage, cel;
	voltage = (temp*5)/1024.;
	cel = (voltage-2.7)*(162/2.8) - 40;
	cel*=10;
	return (int)cel;
}

int calCds(int cds) {
	return (1000-(cds*1000/2048));
}

int calHumi(int humi, double temp) {
	double voltage = (humi*5)/1024.;
	double sRH = (voltage*(0.0062*5)) - 25.81;
	sRH /= (1.0546-(0.00216*temp));
	sRH*=-10;
	return (int)sRH;
}

void sensing()
{
	int cds = analogRead(400);
	int temp = analogRead(401);  // 센서로 부터 자료값을 받는다.
	int humidity = analogRead(402);
	env.temp = calTemp(temp);
	env.cds = calCds(cds);
	env.humi = calHumi(humidity, env.temp/10.);

/*	printf ("TEMPvol : %5.2lf\n",   voltage);
	printf ("TEMP : %5.2lf\n",   celsiustemp);
	printf("cds : %4d\n", cds);

	//Humidity
	float sRH = (humidity*(0.0062 * 5)) - 25.81;//(humidity/1024)*(1/0.0062) - 0.16*(1/0.0062);
	float RH = sRH/(1.0546 - (0.00216 * celsiustemp));

	printf("sRH = %.2lf\n",sRH*(-1));
	printf("RH = %.2lf\n",RH);
	printf("humidity = %5.2lf\n",humidity);*/
	printf("temp=%d, cds=%d, humi=%d\n", env.temp, env.cds, env.humi);

}

int main(int argc, char* argv[])
{
   int fd = open("/dev/mydev", O_RDWR);
   if (fd == -1) {
   	   perror("open");
	   return -1;
   }

	wiringPiSetup();
	pinMode(26, PWM_OUTPUT);
	mcp3422Setup(400, 0x6a, 0, 0);

	//cmd get_env
	sensing();

	//cmd actuate_Buzzer
	//Actuate DD
	ioctl(fd,BUZ_ON);

	//cmd actuate_LED_ON/OFF with level 
	// Off : 0, ON : 1,2,3,4
	ioctl(fd,LED_RATIO,&level);
	
	close(fd);
}

