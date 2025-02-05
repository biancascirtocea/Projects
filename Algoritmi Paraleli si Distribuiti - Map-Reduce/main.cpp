#include <pthread.h>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <cctype>
#include <set>

using namespace std;

// Structura care retine informatii despre aparitia unui cuvant in fisier
struct WordOccurrence {
    string word;
    set<int> file_ids;      // Folosim set pentru eficiență la duplicate
};

// Structura care retine rezultatele unui mapper
struct MapperOutput {
    unordered_map<char, vector<WordOccurrence>> occurrences_by_letter;
};

// Structura care retine datele comune pentru toate thread-urile
struct SharedData {
    vector<string> filenames;
    vector<size_t> file_sizes;
    int next_file_idx = 0;
    vector<MapperOutput> mapper_outputs;
    pthread_mutex_t files_mutex;
    pthread_mutex_t results_mutex;
    pthread_barrier_t barrier;
    int num_mappers;
    int num_reducers;
};

// Structura care retine datele pentru un thread mapper
struct MapperData {
    int thread_id;
    SharedData* shared_data;
};

// Structura care retine datele pentru un thread reducer
struct ReducerData {
    int thread_id;
    SharedData* shared_data;
};

// Functie care returneaza dimensiunea unui fisier
size_t get_file_size(const string& filename) {
    // Deschide fisierul in mod "read-only" (doar citire), pozitionand cursorul la final
    ifstream file(filename, ios::binary);
    if (!file) {
        // Daca fisierul nu poate fi deschis, returneaza 0
        return 0;
    }

    // Muta cursorul la finalul fisierului
    file.seekg(0, ios::end);

    // Obtine pozitia cursorului, care este dimensiunea fisierului
    size_t size = file.tellg();

    // Inchide fisierul
    file.close();

    // Returneaza dimensiunea fisierului
    return size;
}

// Functie care curata un cuvant de caractere nedorite (orice alt caracter cu exceptia literelor)
string clean_word(const string& word) {
    // Cuvantul curatat
    string clean;
    // Rezerva spatiu pentru cuvantul curatat
    clean.reserve(word.length()); // alocare de memorie anticipata pentru a evita realocarile
    
    // Itereaza prin fiecare caracter din cuvant
    for (char c : word) {
        // Daca caracterul este o litera, il adaugam la cuvantul curatat
        if (isalpha(c)) {
            // Converteste litera la lowercase
            clean += tolower(c);
        }
    }

    // Returnează un string gol dacă nu există caractere valide
    if (clean.empty()) {
        return ""; 
    } 
    // Returnează cuvântul curățat
    return clean; 
}

// Functie care proceseaza un text si actualizeaza aparitiile cuvintelor in fisier
void process_text(const string& text, int file_id, unordered_map<string, set<int>>& word_occurrences) {
   // Folosim un istringstream pentru a citi cuvintele din text
    istringstream iss(text);
    string word;

    // Parcurgem fiecare cuvant din text si il procesam
    while (iss >> word) {
        string cleaned_word = clean_word(word);

        // Daca cuvantul este unul valid, il adaugam in map
        if (!cleaned_word.empty()) {
            word_occurrences[cleaned_word].insert(file_id);
        }
    }
}

// Functie care sorteaza si scrie intrarile in fisier
void sort_and_write_entries(const unordered_map<string, set<int>>& word_files, ofstream& outfile) {
    vector<pair<string, vector<int>>> entries;
    entries.reserve(word_files.size());  // Rezerva spatiu pentru a evita realocarile
    
    // Adauga intrarile in vector
    for (const auto& [word, files] : word_files) {
        entries.push_back({word, vector<int>(files.begin(), files.end())});
    }
    
    // Sorteaza intrarile mai intai dupa dimensiunea setului de fisiere si apoi dupa cuvant
    sort(entries.begin(), entries.end(), [](const auto& var1, const auto& var2) {
            // First compare by size
            const size_t size1 = var1.second.size();
            const size_t size2 = var2.second.size();
        
            if (size1 != size2) {
                return size1 > size2;  // Sort descending by size
            }
        
            // If sizes are equal, sort by key
            return var1.first < var2.first;  // Sort ascending by key
    });
    
    // Scrie intrarile in fisier
    for (const auto& entry : entries) {
        outfile << entry.first << ":[" << entry.second[0];
        for (size_t i = 1; i < entry.second.size(); ++i) {
            outfile << " " << entry.second[i];
        }
        outfile << "]\n";
    }
}

// Functie care reprezinta comportamentul unui thread mapper
void* mapper_function(void* arg) {
    // Extragem datele thread-ului
    auto* data = static_cast<MapperData*>(arg);
    // Creem un obiect pentru output mapper de tipul unique_ptr
    auto output = make_unique<MapperOutput>();
    
    unordered_map<string, set<int>> word_occurrences;
    word_occurrences.reserve(1024);  // Rezervam spatiu pentru a evita realocarile
    
    while (true) {
        // Blocheaza mutex-ul pentru a citi urmatorul fisier
        pthread_mutex_lock(&data->shared_data->files_mutex);
        int file_idx = data->shared_data->next_file_idx;

        // Daca nu mai sunt fisiere de procesat, deblocheaza mutex-ul si iese din bucla
        if (file_idx >= static_cast<int>(data->shared_data->filenames.size())) {
            pthread_mutex_unlock(&data->shared_data->files_mutex);
            break;
        }
        // Actualizeaza indexul urmatorului fisier si deblocheaza mutex-ul
        data->shared_data->next_file_idx++;
        pthread_mutex_unlock(&data->shared_data->files_mutex);
        
        // Citeste continutul fisierului
        ifstream file(data->shared_data->filenames[file_idx]);
        if (!file) continue;
        
        string content;
        content.resize(data->shared_data->file_sizes[file_idx]);
        file.read(&content[0], content.size());
        
        // Proceseaza textul si actualizeaza aparitiile cuvintelor in fisier
        process_text(content, file_idx + 1, word_occurrences);
    }
    
    // Pre-procesare și grupare pe litere
    for (const auto& [word, file_ids] : word_occurrences) {
        char first_letter = word[0];
        output->occurrences_by_letter[first_letter].push_back({word, file_ids});
    }
    
    // Blocheaza mutex-ul pentru a scrie rezultatele
    pthread_mutex_lock(&data->shared_data->results_mutex);
    data->shared_data->mapper_outputs[data->thread_id] = std::move(*output);
    // Deblocheaza mutex-ul
    pthread_mutex_unlock(&data->shared_data->results_mutex);
    
    // Asteapta ca toate thread-urile sa termine
    pthread_barrier_wait(&data->shared_data->barrier);

    // Folosim pthread_exit pentru a iesi din thread
    pthread_exit(nullptr);
    return nullptr;
}

void* reducer_function(void* arg) {
    // Extragem datele thread-ului
    auto* data = static_cast<ReducerData*>(arg);
    // Asteapta ca toate thread-urile sa termine
    pthread_barrier_wait(&data->shared_data->barrier);
    
    // Calculeaza cate litere va procesa fiecare reducer
    int letters_per_reducer = 26 / data->shared_data->num_reducers;
    int extra_letters = 26 % data->shared_data->num_reducers;
    int start_idx = data->thread_id * letters_per_reducer + min(data->thread_id, extra_letters);
    int end_idx = start_idx + letters_per_reducer - 1;
    if (data->thread_id < extra_letters) {
        end_idx++;
    }

    // Proceseaza literele alocate
    char start_letter = 'a' + start_idx;
    char end_letter = 'a' + end_idx;
    
    // Proceseaza literele alocate
    for (char current_letter = start_letter; current_letter <= end_letter; current_letter++) {
        unordered_map<string, set<int>> word_files;
        
        // Blocheaza mutex-ul pentru a citi rezultatele mapperilor si a le procesa
        pthread_mutex_lock(&data->shared_data->results_mutex);
        for (const auto& mapper_output : data->shared_data->mapper_outputs) {
            auto it = mapper_output.occurrences_by_letter.find(current_letter);
            if (it != mapper_output.occurrences_by_letter.end()) {
                for (const auto& occ : it->second) {
                    word_files[occ.word].insert(occ.file_ids.begin(), occ.file_ids.end());
                }
            }
        }
        pthread_mutex_unlock(&data->shared_data->results_mutex);
        
        vector<pair<string, vector<int>>> entries;
        entries.reserve(word_files.size());  // Rezervam spatiu pentru a evita realocarile
        
        // Adauga intrarile in vector
        for (const auto& [word, files] : word_files) {
            entries.push_back({word, vector<int>(files.begin(), files.end())});
        }
        
        // Scrie intrarile in fisier si le sorteaza
        string filename(1, current_letter);
        filename += ".txt";
        ofstream outfile(filename);
        sort_and_write_entries(word_files, outfile);
        outfile.close();
    }
    
    // Folosim pthread_exit pentru a iesi din thread
    pthread_exit(nullptr);
    return nullptr;
}

int main(int argc, char* argv[]) {
    // Citim argumentele programului
    int num_mappers = (*argv[1]) - '0';
    int num_reducers = (*argv[2]) - '0';
    string filename = argv[3];
    
    // Initializam datele comune pentru toate thread-urile
    SharedData shared_data;
    shared_data.num_mappers = num_mappers;
    shared_data.num_reducers = num_reducers;
    shared_data.mapper_outputs.resize(num_mappers);
    
    // Initializam mutex-ul si bariera
    pthread_mutex_init(&shared_data.files_mutex, nullptr);
    pthread_mutex_init(&shared_data.results_mutex, nullptr);
    int sum = num_mappers + num_reducers;
    pthread_barrier_init(&shared_data.barrier, nullptr, sum);
    
    ifstream fin(filename);
    if (!fin) {
        return 1;
    }
    int nr_files;
    fin >> nr_files;
    
    // Extragem calea directorului din numele fisierului
    string dir_path = filename.substr(0, filename.find_last_of("/") + 1);
    
    // Citim numele fisierelor si dimensiunile lor
    string name;
    shared_data.filenames.reserve(nr_files);
    shared_data.file_sizes.reserve(nr_files);
    
    for (int i = 0; i < nr_files; ++i) {
        fin >> name;
        string full_path = dir_path + name;
        shared_data.filenames.push_back(full_path);
        shared_data.file_sizes.push_back(get_file_size(full_path));
    }
    
    // Initializam thread-urile mapper si reducer
    vector<pthread_t> all_threads(num_mappers + num_reducers);
    vector<MapperData> mapper_data(num_mappers);
    vector<ReducerData> reducer_data(num_reducers);
    
    // Lansăm toate thread-urile într-o singură iterație
    for (int i = 0; i < num_mappers + num_reducers; ++i) {
        if (i < num_mappers) {
            // Thread-uri mapper
            mapper_data[i] = {i, &shared_data};
            pthread_create(&all_threads[i], nullptr, mapper_function, &mapper_data[i]);
        } else {
            // Thread-uri reducer
            int reducer_index = i - num_mappers;
            reducer_data[reducer_index] = {reducer_index, &shared_data};
            pthread_create(&all_threads[i], nullptr, reducer_function, &reducer_data[reducer_index]);
        }
    }
    
    // Așteptăm terminarea tuturor thread-urilor într-o singură iterație
    for (int i = 0; i < num_mappers + num_reducers; ++i) {
        pthread_join(all_threads[i], nullptr);
    }

    // Eliberare resurse
    pthread_mutex_destroy(&shared_data.files_mutex);
    pthread_mutex_destroy(&shared_data.results_mutex);
    pthread_barrier_destroy(&shared_data.barrier);
    
    return 0;
}