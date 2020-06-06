#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "pthread.h"
#include "MultModulo.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct Server 
{
  char ip[255];
  uint64_t port;
};

struct ServerArgs
{
    struct sockaddr_in server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Out of uint64_t range: %s\n", str);
        return false;
    }

    if (errno != 0)
        return false;

    *val = i;
    return true;
}

uint64_t SendTask(struct ServerArgs *args)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "Socket creation failed!\n");
        exit(1);
    }
    
    if (connect(sock, (struct sockaddr *)&args->server, sizeof(args->server)) < 0)
    {
        fprintf(stderr, "Connection failed\n");
        exit(1);
    }

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &args->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &args->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &args->mod, sizeof(uint64_t));

    fprintf(stdout, "Send: %lu %lu %lu\n", args->begin, args->end, args->mod);

    if (send(sock, task, sizeof(task), 0) < 0)
    {
        fprintf(stderr, "Send failed\n");
        exit(1);
    }

    char response[sizeof(uint64_t)];
    if (recv(sock, response, sizeof(response), 0) < 0)
    {
        fprintf(stderr, "Recieve failed\n");
        exit(1);
    }

    uint64_t answer = 0;
    memcpy(&answer, response, sizeof(uint64_t));
    close(sock);
    return answer;
};

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers[255] = {'\0'}; // explain why 255 (максимальная длина имени файла)

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                        {"mod", required_argument, 0, 0},
                                        {"servers", required_argument, 0, 0},
                                        {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
        break;

        switch (c) {
        case 0: {
        switch (option_index) {
        case 0:
            ConvertStringToUI64(optarg, &k);
            if (k <= 0)
            {
                printf("k is be a positive number\n");
                return 1;
            }
            break;
        case 1:
            ConvertStringToUI64(optarg, &mod);
            if (mod <= 0)
            {
                printf("mod is be a positive number\n");
                return 1;
            }
            break;
        case 2:
            memcpy(servers, optarg, strlen(optarg));
            if (strlen(servers) == sizeof('\0'))
            {
                printf("servers is a path to the file with servers ip and ports\n");
                return 1;
            }
            break;
        default:
            printf("Index %d is out of options\n", option_index);
        }
        } break;

        case '?':
        printf("Arguments error\n");
        break;
        default:
        fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    unsigned int servers_num = 0;
    struct Server *to = malloc(sizeof(struct Server));
    FILE *f = fopen(servers, "r");

    if (!f)
    {
        printf("Error opening server file!\n");
        return 1;
    }

    char *line = malloc(256*sizeof(char));
    char *line_part = malloc(256*sizeof(char));

    while (fgets(line, 255, f) != NULL)
    {
        servers_num++;

        if (servers_num > 1)
        {
            to = realloc(to, sizeof(struct Server) * servers_num);
        }

        line_part = strtok(line, ":");  //ip

        strcpy(to[servers_num-1].ip, line_part);

        line_part = strtok(NULL, "\n"); //port

        ConvertStringToUI64(line_part, &to[servers_num-1].port);
    }

    fclose(f);

    uint64_t part = k/servers_num;
    uint64_t begin, end;
    uint64_t answer = 1;

    struct ServerArgs args[servers_num];
    pthread_t threads[servers_num];

    for (int i = 0; i < servers_num; i++) 
    {
        struct hostent *hostname = gethostbyname(to[i].ip);
        if (hostname == NULL) 
        {
            fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
            exit(1);
        }

        args[i].server.sin_family = AF_INET;
        args[i].server.sin_port = htons(to[i].port);
        args[i].server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);
        
        args[i].begin = part * i + 1;
        if (i == servers_num - 1) args[i].end = k;
        else args[i].end = part * (i + 1);

        pthread_create(&threads[i], NULL, SendTask, (void*)&args[i]);
    }

    for (uint64_t i = 0; i <servers_num; i++)
    {
        pthread_mutex_lock(&lock);

        int answer_part = 0;
        pthread_join(threads[i],(void**)&answer_part);
        answer =  MultModulo(answer, answer_part, mod);
        
        pthread_mutex_unlock(&lock);
    }

    printf("Answer: %lu\n", answer);

    free(to);

    return 0;
}
