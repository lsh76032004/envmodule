#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<mcp3422.h>
#include<wiringPi.h>


#define LED_RATIO _IOW('c', 1, int)

int main(int argc, char *argv[]){
	int fd = open("/dev/mydev", O_RDWR);
	if(fd == -1){
		perror("open");
		return -1;
	}

	wiringPiSetup();
	//pinMode(26, PWM_OUTPUT);
	mcp3422Setup(400, 0x6a, 0, 0);

	int input;

	while(1){
		printf(" 1 ===> cds \n 2 ===> temperature \n 3 ===> humidity \n else ===> END\n");
		printf("put number:  ");
		scanf("%d", &input);
		int cds = analogRead(400);
		int temp = analogRead(401);
		int humid = analogRead(402);
		switch(input){
			case 1:
				while (1) {
					printf("%4d\n", cds);
					delay(100);
				}
				break;
			case 2:
				while(1){
					double voltage = (temp*5)/1024.;
					double celsiustemp = (voltage-2.7)*(165/2.8) - 40;
					printf ("voltage: %5.2lf   ",   voltage);
					printf ("temperature: %5.2lf\n",   celsiustemp);
					delay (100) ;
				}
				break;
			case 3:
				while(1){
					printf("%d\n", humid);
				}

			default: return 0;
		}
	}
}

