#include <iostream>
#include <fstream>
#include <pthread.h>
#include <string>
#include <vector>

#define NUM_THREADS 2

using namespace std;

pthread_t threads[NUM_THREADS];
pthread_mutex_t read_file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t read_file_cond = PTHREAD_COND_INITIALIZER;
vector<string> file_list;
int file_count = 0;

void *print_routine(void *id)
{
    int tid = (int) id;
    string filename;

    while(1) 
    {
        pthread_mutex_lock(&read_file_mutex);
        if (file_count < file_list.size())
        {
            filename = file_list[file_count++];
            cout << "thread " << tid << " is responsible for the copy of file " << filename << endl; 
            pthread_mutex_unlock(&read_file_mutex);
            ifstream input(filename);
            ofstream output("output_" + filename);
            if (input.is_open())
            {
                string line;
                while(getline(input, line))
                    output << line << endl;
                input.close();
            }
            output.close();
        }
        else {
            pthread_mutex_unlock(&read_file_mutex);
            break;
        }
    }
}

int main(int argc, char **args)
{
    if (argc != 2) 
    {
        cout << "Ilegal usage. Only one argument is required that is the filename containing a list of files." << endl;
        return 1;
    }
    cout << "Input file name " << args[1] << endl;
    
    ifstream input_file(args[1]);
    string line;
    if (input_file.is_open()) 
    {
        while(getline(input_file, line))
        {
            file_list.push_back(line);
        }
    }
    cout << "There are " << file_list.size() << " files altogether."<< endl;
    file_count = 0;
    
    for (int i = 0; i < NUM_THREADS; i++)
    {
        cout << "create thread " << i << endl;
        int rc = pthread_create(&threads[i], NULL, print_routine,(void *)i);
        if (rc) 
        {
            cout << "Error: unable to create thread, " << rc << endl;
            return -1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        void *status;
        int rc = pthread_join(threads[i], &status);
        if (rc) 
        {
            cout << "Error: unable to join, " << rc << endl;
            return -1;
        }
        cout << "Main: completed thread id : " << i << " exiting with status : " << status << endl;
    }    
    return 0;
}

