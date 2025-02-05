#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <unistd.h> 
#include "tema2.h"

void *download_thread_func(void *arg) {
    // Convertim argumentul primit la structura thread_args
    //  int rank = *(int*) arg;
    struct thread_args *args = (struct thread_args*)arg;
    struct request_handler rh = {0};
    init_request_handler(&rh, args->p);
    static char msg_buff[MAXIM_SIZE_TO_ACCEPT];

    // Obtinem informatii despre swarm pentru toate fisierele lipsa
    for (int i = 0; i < args->p.number_missing_files; i++) {
        struct first_request_structure req = {
            .type = SWARM_REQUEST,
            .source = args->rank
        };
        memcpy(req.buffer, args->p.missing_files_names[i], 15);
        memcpy(msg_buff, &req, sizeof(req));
        // Trimitem cerere catre tracker (rank 0) pentru a obtine swarm-ul
        MPI_Send(msg_buff, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        receive_swarm_reply(&rh, args->numtasks, i);
    }

    // Incepem descarcarea efectiva a fisierelor
    for (int i = 0; i < args->p.number_missing_files; i++) {
        struct swarm *sw = rh.current_file_swarm[i];
        int segments_downloaded = 0;  // Numaram segmentele descarcate pentru actualizari periodice
        
        // Descarcam fiecare segment al fisierului
        for (int j = 0; j < sw->f.chunksNumber; j++) {
            // Pregatim cererea pentru un segment specific
            struct segment_request req = {
                .type = SEGMENT_REQUEST,
                .source = args->rank,
                .segment_index = j
            };
            memcpy(req.fileName, sw->f.fileName, 15);
            
            // Alegem un peer random care are segmentul dorit
            int peer = choose_random_peer(sw, j);
            memcpy(msg_buff, &req, sizeof(req));
            // Trimitem cererea catre peer-ul ales
            MPI_Send(msg_buff, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, peer, 1, MPI_COMM_WORLD);
            receive_segment_reply(&args->p, &rh, peer, i);

            rh.next_segment_index[i]++;
            segments_downloaded++;

            // La fiecare 10 segmente descarcate, cerem o actualizare a swarm-ului
            if (segments_downloaded % 10 == 0) {
                struct first_request_structure update_req = {
                    .type = SWARM_REQUEST,
                    .source = args->rank
                };
                memcpy(update_req.buffer, sw->f.fileName, 15);
                memcpy(msg_buff, &update_req, sizeof(update_req));
                MPI_Send(msg_buff, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                receive_swarm_reply(&rh, args->numtasks, i);
            }
        }

        // Notificam tracker-ul ca am terminat de descarcat fisierul
        struct file_received_update fru = {
           .type = SWARM_FILE_RECEIVED,
           .source = args->rank
        };
        memcpy(fru.fileName, sw->f.fileName, 15);
        memcpy(msg_buff, &fru, sizeof(fru));
        MPI_Send(msg_buff, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        create_one_file_and_write(rh, args, i);
    }

    // Notificam tracker-ul ca am terminat de descarcat toate fisierele
    struct peer_downloaded_all pda = {
        .type = ALL_FILES_DOWNLOADED,
        .source = args->rank
    };
    memcpy(msg_buff, &pda, sizeof(pda));
    MPI_Send(msg_buff, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

    return NULL;
}

void *upload_thread_func(void *arg) {
    // Convertim argumentul la structura thread_args
    //int rank = *(int*) arg;
    struct thread_args *ctx = (struct thread_args*)arg;
    // Union pentru a putea interpreta mesajul primit in mai multe moduri
    static union {
      char raw[MAXIM_SIZE_TO_ACCEPT];
      struct segment_request req;
      int type;
    } msg = {0};

    while(1) {
        // Asteptam cereri de la alti peeri pentru segmente
        MPI_Recv(&msg, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
        // Daca primim mesaj de tip 0, inseamna ca trebuie sa inchidem thread-ul
        if (msg.type == 0) {
            break;
        }
      
        // Trimitem segmentul cerut catre peer-ul care l-a solicitat
        send_segment_reply(&msg.req, ctx);
    }
  
    return NULL;
}

void tracker(int numtasks, int rank) {
    static char msg_buffer[MAXIM_SIZE_TO_ACCEPT];
    // Union pentru a interpreta diferitele tipuri de mesaje primite
    static union {
       int msg_type;
       struct first_request_structure swarm_req;
       struct file_received_update file_update;
       struct peer_downloaded_all peer_done;
    } msg_data;

    // Initializam structura tracker-ului
    struct tracker t = {
       .numtasks = numtasks,
       .num_swarms = 0,
       .swarms = NULL,
       .peers_state = calloc(numtasks + 1, sizeof(int))
    };

    // Primim informatii despre fisierele detinute de fiecare peer
    receive_all_known_files_from_peers(&t);
   
    // Notificam toti peerii ca am terminat de colectat informatiile initiale
    for (int i = 1; i < numtasks; i++) {
       int ok = 1;
       MPI_Send(&ok, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    while (1) {
        // Asteptam si procesam mesaje de la peeri
        memset(msg_buffer, 0, MAXIM_SIZE_TO_ACCEPT);
        MPI_Recv(msg_buffer, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        memcpy(&msg_data, msg_buffer, sizeof(msg_data));

        switch (msg_data.msg_type) {
            case SWARM_REQUEST:
                // Raspundem la cererea de informatii despre un swarm
                send_swarm_reply(&t, msg_data.swarm_req);
                break;

            case SWARM_FILE_RECEIVED:
                // Actualizam informatiile despre un fisier complet descarcat
                receive_file_received_update(&t, msg_data.file_update);
                break;

            case ALL_FILES_DOWNLOADED:
                // Marcam peer-ul ca avand toate fisierele
                t.peers_state[msg_data.peer_done.source] = 1;
               
                // Verificam daca toti peerii au terminat
                int all_done = 1;
                for (int i = 1; i < numtasks && all_done; i++) {
                    if (!t.peers_state[i]) 
                        all_done = 0;
                }
               
               // Daca toti au terminat, trimitem semnal de oprire si inchidem tracker-ul
                if (all_done) {
                    memset(msg_buffer, 0, MAXIM_SIZE_TO_ACCEPT);
                    for (int i = 1; i < numtasks; i++) {
                        MPI_Send(msg_buffer, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                    }
                    free(t.peers_state);
                    return;
                }
            break;
        }
    }
}

void peer(int numtasks, int rank) {
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;
    struct peer p; 

    // Initializam structura peer cu valori nule
    memset(p.missing_files_names, 0, MAX_FILES * 15);
    for (int i = 0; i < MAX_FILES; i++) {
        memset(p.files[i].fileName, 0, 15);
        memset(p.files[i].buffer, 0, 3200);
    }

    // Construim numele fisierului de input specific acestui peer
    char filename[32];
    snprintf(filename, sizeof(filename), "in%d.txt", rank);

    // Deschidem fisierul de configurare
    FILE *fd = fopen(filename, "r");
    if (!fd) {
        exit(1);
    }

    // Citim numarul de fisiere detinute de peer
    if (fscanf(fd, "%d", &p.number_files) != 1) {
        exit(1);
    }

    // Pentru fiecare fisier citim numele si numarul de chunk-uri
    for (int i = 0; i < p.number_files; i++) {
        if (fscanf(fd, "%s %d", p.files[i].fileName, &p.files[i].chunksNumber) != 2) {
            exit(1);
        }
  
        // Citim hash-ul pentru fiecare chunk al fisierului
        for (int j = 0; j < p.files[i].chunksNumber; j++) {
            if (fscanf(fd, "%s", p.files[i].buffer[j]) != 1) {
                exit(1);
            }
        }
    }

    // Citim numarul de fisiere care lipsesc peer-ului
    if (fscanf(fd, "%d", &p.number_missing_files) != 1) {
        exit(1);
    }

    // Citim numele fisierelor care lipsesc
    for (int i = 0; i < p.number_missing_files; i++) {
        if (fscanf(fd, "%s", p.missing_files_names[i]) != 1) {
            exit(1);
        }
    }

    fclose(fd);

    // Pregatim si trimitem mesaj catre tracker cu informatiile despre fisierele detinute
    char buff_to_send[4 + sizeof(struct peer)];
    memset(buff_to_send, 0, 4 + sizeof(struct peer));
    memcpy(buff_to_send, &rank, sizeof(int));
    memcpy(buff_to_send + 4, &p, sizeof(struct peer));
    MPI_Send(buff_to_send, 4 + sizeof(struct peer), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  
    // Asteptam confirmare de la tracker ca a primit datele
    int ok;
    MPI_Recv(&ok, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    struct thread_args args;

    // Pregatim argumentele pentru thread-uri
    memcpy(&args.p, &p, sizeof(struct peer));
    args.rank = rank;
    args.numtasks = numtasks;

    // Pornim thread-ul de download
    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) &args);
    if (r) {
        printf("Eroare la crearea thread-ului de download\n");
        exit(-1);
    }

    // Pornim thread-ul de upload
    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) &args);
    if (r) {
        printf("Eroare la crearea thread-ului de upload\n");
        exit(-1);
    }

    // Asteptam terminarea thread-ului de download
    r = pthread_join(download_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de download\n");
        exit(-1);
    }

    // Asteptam terminarea thread-ului de upload
    r = pthread_join(upload_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de upload\n");
        exit(-1);
    }
}

int main (int argc, char *argv[]) {
    int numtasks, rank;
 
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "MPI nu are suport pentru multi-threading\n");
        exit(-1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == TRACKER_RANK) {
        tracker(numtasks, rank);
    } else {
        peer(numtasks, rank);
    }

    MPI_Finalize();
}
