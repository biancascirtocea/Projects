#include <stdlib.h>
#include <pthread.h>
#ifndef __AUXILIAR__
#define __AUXILIAR__

#define MAXIM_SIZE_TO_ACCEPT 5000
#define SWARM_REQUEST 0
#define SWARM_REPLY 1
#define SEGMENT_REQUEST 2
#define SEGMENT_REPLY 3
#define SWARM_UPDATE 4
#define SWARM_FILE_RECEIVED 5
#define ALL_FILES_DOWNLOADED 6
#define INITIAL_SWARM_NUMBER 5

#define MAX_FILE_SIZE 3219
#define TRACKER_RANK 0
#define MAX_FILES 10


// Structura pentru un fisier - contine numele, nr de chunk-uri si hash-urile lor
struct file {
   char fileName[15];             
   int chunksNumber;           
   char buffer[100][32];          
};

// Structura pentru un swarm - reprezinta un grup de peers care au un anumit fisier
struct swarm {
   struct file f;                 
   int peers_number;              
   int *peers;                   
};

// Structura pentru tracker - coodoneaza intregul sistem
struct tracker
{
   int numtasks;              
   int num_swarms;                
   struct list *swarms;           
   int *peers_state;              
};

// Structura pentru peer 
struct peer {
   int number_files;              
   struct file files[MAX_FILES];  
   int number_missing_files;     
   char missing_files_names[MAX_FILES][15]; 
};

// Structura pentru gestionarea cererilor de download
struct request_handler {
   int number_files;              
   int *number_segments;          
   int *next_segment_index;      
   struct swarm** current_file_swarm;  
}; 

// Structura pentru argumentele thread-urilor
struct thread_args {
   struct peer p;                 
   int rank;                    
   int numtasks;               
};

// Structura pentru cererea initiala catre tracker
struct first_request_structure {
   int type;                     
   int source;                   
   char buffer[15];              
};

// Structura pentru raspunsul la cererea de swarm
struct swarm_reply {
   int type;                      
   int source;                  
   struct file f;                
   int peer_number;              
};

// Structura pentru cererea unui segment
struct segment_request {
   int type;                     
   int source;                    
   int segment_index;         
   char fileName[15];            
};

// Structura pentru raspuns
struct segment_reply {
   int type;                      
   int source;                    
   int segment_index;            
   char fileName[15];             
   char hash[32];                 
};

// Structura pentru actualizare dipa primirea unui fisier
struct file_received_update {
   int type;                    
   int source;                   
   char fileName[15];             
};

// Structura pentru notificarea descarcarii complete
struct peer_downloaded_all {
   int type;                     
   int source;                  
};

// Structura pentru liste inlantuite
struct list {
   void* val;                     // Valoarea stocata
   struct list *next;             // Pointer catre urmatorul element
};

// Context pentru gestionarea segmentelor
typedef struct {
   int source_id;                   // ID-ul sursei
   int target_file_idx;             // Indexul fisierului tinta
   struct peer *target_peer;        // Peer-ul tinta
   struct request_handler *handler; // Handler-ul pentru cereri
} SegmentContext;

// Handlers pentru evenimente legate de swarm
typedef struct {
   void (*on_new_swarm)(struct tracker*, struct file*, int);     // Handler pentru swarm nou
   void (*on_existing_swarm)(struct swarm*, int);                // Handler pentru swarm existent
} SwarmHandlers;

// Initializeaza un handler de cereri
void init_request_handler(struct request_handler *rh, struct peer p) {
    const int num_files = p.number_missing_files;
   
    // Initializeaza toate campurile cu zero si aloca memorie necesara
    *rh = (struct request_handler){
        .next_segment_index = calloc(num_files, sizeof(int)),
        .number_segments = calloc(num_files, sizeof(int)),
        .current_file_swarm = calloc(num_files, sizeof(struct swarm*)),
        .number_files = num_files
    };
}

// Returneaza dimensiunea unei liste
int get_list_size(struct list *list) {
    if (list == NULL) {
        return 0;
    }
    return 1 + get_list_size(list->next);
}

// Returneaza elementul de la un anumit index din lista
struct list* get_elem_at_index(struct list *list, int index) {
    // Cazuri de baza pentru recursivitate
    if (list == NULL || index < 0) {
        return NULL;
    }
    if (index == 0) {
        return list;
    }
    // Apel recursiv pentru gasirea elementului
    return get_elem_at_index(list->next, index - 1);
}

// Adauga un element nou in lista
void add_element_to_list(struct list **list, void *val) {
    // Cazul listei goale
    if (*list == NULL) {
        *list = (struct list*)malloc(sizeof(struct list));
        (*list)->val = val;
        (*list)->next = NULL;
        return;
    }
   
    // Cazul ultimului element
    if ((*list)->next == NULL) {
        (*list)->next = (struct list*)malloc(sizeof(struct list));
        (*list)->next->val = val;
        (*list)->next->next = NULL;
        return;
    }
   
    // Apel recursiv pentru adaugare la sfarsit
    add_element_to_list(&((*list)->next), val);
}

// Cautam dupa numele fisierului in lista de swarm-uri
struct list* file_name_in_swarm_list(struct list *swarms, char *fileName) {
    if (swarms == NULL) {
        return NULL;
    }
   
    // Verificam daca swarm-ul curent contine fisierul cautat
    struct swarm *s = (struct swarm*)swarms->val;
    if (strcmp(s->f.fileName, fileName) == 0) {
        return swarms;
    }
   
    // Cautam recursiv in restul listei
    return file_name_in_swarm_list(swarms->next, fileName);
}

// Cautam dupa fisier in lista de swarm-uri
struct list* file_in_swarm_list(struct list *swarms, struct file f) {
    // Cazul de baza - lista vida
    if (swarms == NULL) {
        return NULL;
    }
   
    // Verificam daca swarm-ul curnt contine fisierul cautat
    // Comparam atat numele cat si nr de chunk-uri
    struct swarm *s = (struct swarm*)swarms->val;
    if (strcmp(s->f.fileName, f.fileName) == 0 && s->f.chunksNumber == f.chunksNumber) {
        return swarms;
    }
   
    // Cautam recursiv in restul listei
    return file_in_swarm_list(swarms->next, f);
}

// Creeaza un nou swarm pentru un fisier
struct swarm* init_swarm(struct file f, int source, int numtasks) {
   // Alocam memorie pentru structura swarm
   struct swarm *s = (struct swarm*)malloc(sizeof(struct swarm));
   if (!s) return NULL;
   
   // Copiem informatiile fisierului
   memcpy(&s->f, &f, sizeof(struct file));
   s->peers_number = 1;
   
   // Alocam memorie pentru lista de peers
   s->peers = (int*)calloc(numtasks, sizeof(int));
   if (!s->peers) {
       free(s);
       return NULL;
   }
   s->peers[0] = source;
   
   return s;
}

// Alege un peer random din swarm pentru a descarca un chunk
int choose_random_peer(struct swarm *s, int chunk_index) {
    // Verificam daca swarm-ul are peers
    if (s->peers_number == 0) 
        return -1;
   
    // Array pentru a tine evidenta numarului de cereri catre fiecare peer
    static int *peer_loads = NULL;
   
    // Initializam array-ul la prima apelare
    if (peer_loads == NULL) {
        peer_loads = (int*)calloc(s->peers_number, sizeof(int));
    }

    // Gasim peer-ul cu cea mai mica incarcare
    int min_load = peer_loads[0];
    int selected_peer = 0;

    for (int i = 1; i < s->peers_number; i++) {
        if (peer_loads[i] < min_load) {
            min_load = peer_loads[i];
            selected_peer = i;
        }
    }

    // Incrementam contorul pentru peer-ul ales
    peer_loads[selected_peer]++;

    // Resetm contoarele daca toate depasesc un prag (evitam overflow)
    if (min_load > 1000) {
        for (int i = 0; i < s->peers_number; i++) {
            peer_loads[i] /= 2;
        }
    }

    return s->peers[selected_peer];
}

// Primeste si proceseaza raspunsul cu informatii despre un swarm
void receive_swarm_reply(struct request_handler *rh, int numtasks, int index) {    
    // Union pentru a interpreta datele primite
    static union {
       char raw[MAXIM_SIZE_TO_ACCEPT];
       struct swarm_reply header;
    } recv_data = {0};
   
    // Primim datle de la tracker
    MPI_Recv(&recv_data, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   
    // Calculam dimensiunea header-ului si obtinem lista de peers
    const int header_size = sizeof(struct swarm_reply);
    const int *peer_ranks = (int*)(recv_data.raw + header_size);
   
    // Initializam swarm-ul si copiem lista de peers
    rh->current_file_swarm[index] = init_swarm(recv_data.header.f, peer_ranks[0], numtasks);
    memcpy(rh->current_file_swarm[index]->peers, peer_ranks, recv_data.header.peer_number * sizeof(int));
}

// Trimite raspunsul pentru o cerere de segment
void send_segment_reply(struct segment_request *s, struct thread_args *args) {
   static struct segment_reply r;
   
   // Initializare rapida a raspunsului cu informatii de baza
   r = (struct segment_reply){
       .type = SEGMENT_REPLY,
       .source = args->rank,
       .segment_index = s->segment_index
   };
   memcpy(r.fileName, s->fileName, 15);
   
   // Cautam fisierul in lista locala si copiem hash-ul segmentului cerut
   struct peer *p = &args->p;
   for (int i = 0; i < p->number_files; i++)
       if (memcmp(s->fileName, p->files[i].fileName, 15) == 0)
           memcpy(r.hash, p->files[i].buffer[s->segment_index], 32);
   
   // Trimitem raspunsul catre peer-ul care a cerut segmentul
   MPI_Send(&r, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, s->source, 1, MPI_COMM_WORLD);
}

// Proceseaza raspunsul primit pentru o cerere de segment
void process_segment_reply(SegmentContext *ctx) {
    // Buffer pentru primirea datelor
    static char receive_data[MAXIM_SIZE_TO_ACCEPT];
    memset(receive_data, 0, MAXIM_SIZE_TO_ACCEPT);
   
    // Primim raspusul de la sursa
    MPI_Recv(receive_data, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, ctx->source_id, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
    struct segment_reply *incoming = (struct segment_reply*)receive_data;
    struct peer *p = ctx->target_peer;
   
    // Cautam sau cream o intrare pentru fisier in lista locala
    int file_location = -1;
    for (int i = 0; i < p->number_files && file_location == -1; i++) {
        if (memcmp(p->files[i].fileName, incoming->fileName, 15) == 0) {
            file_location = i;
        }
    }
   
    // Daca fisierul nu exista, il cream
    if (file_location == -1) {
        file_location = p->number_files++;
        struct file *new_file = &p->files[file_location];
        struct swarm *active_swarm = ctx->handler->current_file_swarm[ctx->target_file_idx];
       
        memcpy(new_file->fileName, active_swarm->f.fileName, 15);
        new_file->chunksNumber = active_swarm->f.chunksNumber;
    }
   
    // Verificam validitatea hash-ului primit si il salvam daca e corect
    struct swarm *active_swarm = ctx->handler->current_file_swarm[ctx->target_file_idx];
    int current_chunk = ctx->handler->next_segment_index[ctx->target_file_idx];
   
    if (memcmp(active_swarm->f.buffer[current_chunk], incoming->hash, 32) == 0) {
        memcpy(p->files[file_location].buffer[current_chunk], incoming->hash, 32);
    }
}

// Primeste raspunsul pentru o cerere de segment
void receive_segment_reply(struct peer *p, struct request_handler *rh, int source, int file_index) {
    // Pregatim contextul pentru procesarea raspunsului
    SegmentContext ctx = {
        .source_id = source,
        .target_file_idx = file_index,
        .target_peer = p,
        .handler = rh
    };
   
    process_segment_reply(&ctx);
}

// Creeaza si scrie fisierul complet dupa ce toate segmentele au fost primite
void create_one_file_and_write(struct request_handler rh, struct thread_args *arg, int file_index) {
    // Construim numele fisirului de output
    char filename[50];
    snprintf(filename, sizeof(filename), "client%d_%s", arg->rank, rh.current_file_swarm[file_index]->f.fileName);
   
    FILE *f = fopen(filename, "w");
    struct file *current_file = &rh.current_file_swarm[file_index]->f;
    char hash_buffer[32 + 1] = {0};
   
   // Scriem primul hash
   memcpy(hash_buffer, current_file->buffer[0], 32);
   fprintf(f, "%s", hash_buffer);
   
   // Scriem restul hash-urilor
    for (int i = 1; i < current_file->chunksNumber; i++) {
        memset(hash_buffer, 0, sizeof(hash_buffer));
        memcpy(hash_buffer, current_file->buffer[i], 32);
        fprintf(f, "\n%s", hash_buffer);
    }
   
    fclose(f);
}

// Verifica daca mai sunt peer-i care trebuie sa primeasca date
int still_need_to_receive(int *vector, int size) {
    int i = 1;
    while (i < size) {
        if (vector[i] != 1) {
            return 1;
        }
        i++;
    }
    return 0;
}

// Primeste si proceseaza informatiile despre fisierele cunoscute de un peer
int receive_known_file_from_peer(struct tracker *t, SwarmHandlers handlers) {
    char *buffer = (char*)calloc(MAXIM_SIZE_TO_ACCEPT, sizeof(char));
   
    // Primim date de la orice sursa
    int source, i = 0;
    MPI_Recv(buffer, MAXIM_SIZE_TO_ACCEPT, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   
    // Extragem sursa si informatiile peer-ului
    memcpy(&source, buffer, sizeof(int));
    struct peer *p = (struct peer*)(buffer + sizeof(int));
   
    // Procesam fiecare fisier cunoscut de peer
    while (i < p->number_files) {
        struct list *elem = file_in_swarm_list(t->swarms, p->files[i]);
        if (elem == NULL) {
            // Fisier nou - cream swarm nou
            if (handlers.on_new_swarm)
                handlers.on_new_swarm(t, &p->files[i], source);
        } else {
            // Fisier existent - actualizam swarm-ul
            if (handlers.on_existing_swarm)
                handlers.on_existing_swarm((struct swarm*)elem->val, source);
        }
        i++;
    }
   
    free(buffer);
    return source;
}

// Proceseaza lista de fisiere a unui peer si actualizeaza swarm-urile
void process_peer_files(struct tracker *t, struct peer *p, int source) {
    int i = 0;
    while (i < p->number_files) {
        // Verificam daca fisierul exita deja intr-un swarm
        struct list *elem = file_in_swarm_list(t->swarms, p->files[i]);
        if (elem == NULL) {
            // Daca nu exista, cream un swarm nou pentru acest fisier
            add_element_to_list(&t->swarms, init_swarm(p->files[i], source, t->numtasks));
            t->num_swarms++;
        } else {
            // Daca exista, adaugam peerul la swarmul existent
            struct swarm *s = (struct swarm*)elem->val;
            s->peers[s->peers_number] = source;
            s->peers_number++;
        }
        i++;
    }
}

// Primeste informatii despre fisierele cunoscute de la toti peerii
void receive_all_known_files_from_peers(struct tracker *t) {
    // Array pentru a tine evidenta peer-ilor de la care am primit date
    int *received = calloc(t->numtasks + 1, sizeof(int));
    const size_t msg_size = sizeof(int) + sizeof(struct peer);
    char *buffer = calloc(msg_size, 1);
    MPI_Status status;
   
    // Primim date pana cand toti peerii au raportat
    while (still_need_to_receive(received, t->numtasks)) {
        MPI_Recv(buffer, msg_size, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
       
        // Extragem sursa si marcam ca am primit date de la ea
        int source;
        memcpy(&source, buffer, sizeof(int));
        received[source] = 1;
       
        // Procesam fisierele peer-ului
        struct peer *p = (struct peer*)(buffer + sizeof(int));
        process_peer_files(t, p, source);
       
        memset(buffer, 0, msg_size);
    }
   
    free(received);
    free(buffer);
}

// Trimite informatii despre un swarm ca raspuns la o cerere
void send_swarm_reply(struct tracker *t, struct first_request_structure r) {
    // Cautam swarm-ul pentru fisierul cerut
    struct list *node = file_name_in_swarm_list(t->swarms, r.buffer);
    if (!node) return;
   
    struct swarm *swarm = (struct swarm*)node->val;
    const int total_size = sizeof(struct swarm_reply) + swarm->peers_number * sizeof(int);
   
    // Pregatim raspunsul
    char *buffer = calloc(MAXIM_SIZE_TO_ACCEPT, 1);
    struct swarm_reply reply = {
        .type = SWARM_REPLY,
        .source = 0,
        .peer_number = swarm->peers_number,
        .f = swarm->f
    };
   
    // Copiem datele in buffer si trimitem
    memcpy(buffer, &reply, sizeof(reply));
    memcpy(buffer + sizeof(reply), swarm->peers, swarm->peers_number * sizeof(int));
   
    MPI_Send(buffer, total_size, MPI_CHAR, r.source, 0, MPI_COMM_WORLD);
    free(buffer);
}

// Cauta poztia unui peer intr-un swarm
int locate_peer_in_swarm(struct swarm *s, int peer_id) {
    int i = 0;
    while (i < s->peers_number) {
        if (s->peers[i] == peer_id) {
            return i;
        }
        i++;
    }
    return -1;
}

// Actualizeaza swarm-ul cand un peer raporteaza ca a primit complet un fisier
void receive_file_received_update(struct tracker *t, struct file_received_update fru) {
    // Cautam swarm-ul pentru fisierul specificat
    struct list *swarm_entry = file_name_in_swarm_list(t->swarms, fru.fileName);
    if (!swarm_entry) {
        return;
    }

    // Verificam daca peer-ul exista deja in swarm
    struct swarm *target_swarm = (struct swarm*)swarm_entry->val;
    int peer_position = locate_peer_in_swarm(target_swarm, fru.source);

    // Daca peer-ul nu exista in swarm, il adaugam
    if (peer_position < 0) {
        target_swarm->peers[target_swarm->peers_number] = fru.source;
        target_swarm->peers_number++;
    }
}

#endif