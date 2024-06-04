#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stddef.h>
#include <arpa/inet.h>

#include "/usr/include/alsa/asoundlib.h"

#define MIN '0'
#define MAX '9'

#define SIZE sizeof(struct sockaddr_in)

char addr[20];

int sockfd_listen1;
int sockfd_connect1;
int sockfd_listen2;
int sockfd_connect2;

int real_time = 0;

/* record function */
void RecordSound(unsigned char *ap_rec_buffer)
{	
	snd_pcm_t *p_capture_handle;
	snd_pcm_hw_params_t *p_hw_params;
	unsigned int freq = 8000; // 8KHz

    // device set
	if(snd_pcm_open(&p_capture_handle, "plughw:0,0", SND_PCM_STREAM_CAPTURE,0)<0) // raspberry:2,0 ubuntu:0,0
    {
      	perror("could not open input audio dev");
      	exit(1);
	}	   

	snd_pcm_hw_params_malloc (&p_hw_params);
	snd_pcm_hw_params_any (p_capture_handle, p_hw_params);
	snd_pcm_hw_params_set_access (p_capture_handle, p_hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format (p_capture_handle, p_hw_params, (snd_pcm_format_t)SND_PCM_FORMAT_S16_LE); 
	snd_pcm_hw_params_set_rate_near (p_capture_handle, p_hw_params, &freq, 0);
	snd_pcm_hw_params_set_channels (p_capture_handle, p_hw_params, 1); 
    snd_pcm_hw_params (p_capture_handle, p_hw_params);
	snd_pcm_hw_params_free (p_hw_params);

    // recording
    snd_pcm_prepare (p_capture_handle);
	snd_pcm_readi (p_capture_handle, ap_rec_buffer, 8000); // 8KHz * 1sec = 8000 sampling

	snd_pcm_drop(p_capture_handle);
	snd_pcm_close (p_capture_handle);
}

/* play function */
void PlaySound(unsigned char *ap_rec_buffer)
{
	snd_pcm_t *p_playback_handle;
	snd_pcm_hw_params_t *hw_params;
	unsigned int freq = 8000; // 8KHz

    // device set
	if(snd_pcm_open(&p_playback_handle, "plughw:0,0",SND_PCM_STREAM_PLAYBACK,0)<0) // raspberry:1,0 ubuntu:0,0
    {
      	perror("Could not open output audio dev");
      	exit(1);
	}

	snd_pcm_hw_params_malloc(&hw_params);
	snd_pcm_hw_params_any(p_playback_handle,hw_params); 
	snd_pcm_hw_params_set_access(p_playback_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED); 
	snd_pcm_hw_params_set_format(p_playback_handle,hw_params,(snd_pcm_format_t)SND_PCM_FORMAT_S16_LE); 
	snd_pcm_hw_params_set_rate_near(p_playback_handle,hw_params, &freq,0);
	snd_pcm_hw_params_set_channels(p_playback_handle,hw_params, 1);
	snd_pcm_hw_params(p_playback_handle,hw_params);
	snd_pcm_hw_params_free (hw_params);

    // playing
	snd_pcm_prepare(p_playback_handle);
	snd_pcm_writei(p_playback_handle, ap_rec_buffer, 8000); // 8KHz * 1sec = 8000 sampling

	usleep(1000000); 
	
	snd_pcm_drop(p_playback_handle);
	snd_pcm_close(p_playback_handle);
}

/* <server> record-send thread */
void *thr1(void *data)
{
	struct sockaddr_in server = {AF_INET, 12347, };

    unsigned char rec1_buffer[48000]; // enough buffer size (8000 * 6)

    server.sin_addr.s_addr = inet_addr(addr); // 127.0.0.1

	if((sockfd_listen1 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("fail to call socket() \n");
		exit(1);
	}

	if(bind(sockfd_listen1, (struct sockaddr *)&server, SIZE) == -1)
	{
		printf("fail to call bind() \n");
		exit(1);
	}

	if(listen(sockfd_listen1, 5) == -1)
	{
		printf("fail to call listen() \n");
		exit(1);
	}

	if((sockfd_connect1 = accept(sockfd_listen1, NULL, NULL)) == -1)
	{
		printf("fail to call accept() \n");
        exit(1);
	}

    printf("record thread connected \n");

    //communication_send
    while(1)
    {
		if(listen(sockfd_listen1, 5) == -1)
		{
			pthread_exit(NULL);
		}

		RecordSound(rec1_buffer);
        send(sockfd_connect1, &rec1_buffer, sizeof(rec1_buffer), 0);
    }

    return 0;
}

/* <server> play-recv thread */
void *thr2(void *data)
{
	struct sockaddr_in server = {AF_INET, 12348, };

    unsigned char rec2_buffer[48000]; // enough buffer size (8000 * 6)

    server.sin_addr.s_addr = inet_addr(addr);

	if((sockfd_listen2 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("fail to call socket() \n");
		exit(1);
	}

	if(bind(sockfd_listen2, (struct sockaddr *)&server, SIZE) == -1)
	{
		printf("fail to call bind() \n");
		exit(1);
	}

	if(listen(sockfd_listen2, 5) == -1)
	{
		printf("fail to call listen() \n");
		exit(1);
	}

	if((sockfd_connect2 = accept(sockfd_listen2, NULL, NULL)) == -1)
	{
		printf("fail to call accept() \n");
        exit(1);
	}

    printf("play thread connected \n");

    // communication_recv
	while(1)
	{
        system("clear");

		printf("------------------------------ \n");
		printf("          TIME : %d \n", real_time);
		printf("------------------------------ \n\n");
        printf("Press ctrl+c to finish \n\n");

        real_time++;

		if(listen(sockfd_listen2, 5) == -1)
		{
			pthread_exit(NULL);
		}

    	recv(sockfd_connect2, &rec2_buffer, sizeof(rec2_buffer), 0);
		PlaySound(rec2_buffer);
	}

	return 0;
}

/* addr */
void ip_addr()
{
	int i;

	for(i=0;i<20;i++)
	{
		if(i==3)
		{
			if(addr[i] != '.')
			{
				printf("<< RETRY >> \n\n");

				exit(1);
			}
		}

		else if(i==7)
			{
				if(addr[i] != '.')
				{
					printf("<< RETRY >> \n\n");

					exit(1);
				}
			}
		
		else if(i==11)
		{
			if(addr[i] != '.')
			{
				printf("<< RETRY >> \n\n");
				
				exit(1);
			}
		}

		else if(i>=15)
		{
			if(addr[15] != '\0')
			{
				printf("<< RETRY >> \n\n");

				exit(1);
			}
		}

		else
		{
			int num;

			num = addr[i];

			if(num == 0)
			{
				printf("<< RETRY >> \n\n");

				exit(1);
			}

			else if((addr[i] < '0') || (addr[i] > '9'))
			{
				printf("<< RETRY >> \n\n");

				exit(1);
			}
		}
	}
	return;
}

/* main */
int main()
{
    system("clear");
   
	/* addr option */
    int option = 0;

	printf("------------------------------ \n");
    printf("            OPTION \n");
	printf("------------------------------ \n\n");
    printf(" 1. NEW IP \n");
    printf(" 2. DEFAULT IP \n\n");
	printf(" >> ");
    scanf("%d", &option);

	if(option == 1)
	{
		system("clear");
		printf("------------------------------ \n");
		printf("           OPTION 1 \n");
		printf("------------------------------ \n\n");
        printf(" NEW IP : ");
        scanf("%s", addr);

		ip_addr();

		printf("\n");
    }

    else if(option == 2)
	{
		system("clear");

		printf("------------------------------ \n");
		printf("           OPTION 2 \n");
		printf("------------------------------ \n\n");

		char *ad = "192.168.136.131";

		strcpy(addr, ad);
				
		printf(" DEFAULT IP : %s \n\n", addr);
    }

	else
	{
		printf("<< RETRY >> \n\n");

		exit(1);
	}

    /* create thread */
    pthread_t p_thread[1];
    int thr_id;
    int status; 
    char p1[] = "record-send";
    char p2[] = "play-recv";

    // record-send thread create
    thr_id = pthread_create(&p_thread[0], NULL, thr1, (void *)p1);

    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    // play-recv thread create
    thr_id = pthread_create(&p_thread[1], NULL, thr2, (void *)p2);

    if (thr_id < 0)
    {
        perror("thread create error : ");
        exit(0);
    }

    // check thread status
    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);

	// socket end
	close(sockfd_connect1);
    close(sockfd_listen1);
	close(sockfd_connect2);
    close(sockfd_listen2);

    printf("\n\n");
	printf("connection end \n");

    return 0;
}
