#ifndef WORKERS_H
#define WORKERS_H

void *worker_thread(void *arg);
void init_workers(int worker_count);
void dispatch_client(int client_fd);

#endif
