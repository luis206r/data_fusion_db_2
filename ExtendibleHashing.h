//<>
// Created by luisr on 9/16/2024.
//
//metodo de eliminacion = free list lifo

#ifndef DATA_FUSION_DB_EXTENDIBLEHASHING_H
#define DATA_FUSION_DB_EXTENDIBLEHASHING_H

#include <cstring>
#include <string>
#include <bitset>
#include <map>
#include<vector>
#include<fstream>
#include<type_traits>


#define FB = 3;
#define GD = 3;

using namespace std;

struct Record{
    //esta estructura depende de los datos de mockdata
    //para testear se usara lo siguiente

    char nombre[20];
    char apellido[20];
    char codigo[10];
    char carrera[10]; //usar abreviaturas
    int ciclo;
};

struct Bucket{
    int local_depth;
    int size;
    Record records[4];
    long nextDel; //free list method
};

struct Index{ //this has to be stored //leer archivo de indive previamente

    //acceso con map : O(lgn)
    //acceso con vector : O(1)
    int global_depth;
    map<string, long> index = {{"0",sizeof(int)},{"1",(sizeof(int) + sizeof(Bucket))} };

    /*vector<long*> index; //3 = initial global depth


    Index(int global_depth) : global_depth(global_depth), index(1 << global_depth, nullptr) {}
    void resize() {
        // Incrementar la profundidad global
        global_depth++;
        size_t new_size = 1 << global_depth;

        std::vector<long*> new_index(new_size, nullptr);

        for (size_t i = 0; i < index.size(); ++i) {
            new_index[i] = index[i];
        }

        index = move(new_index);
    }
    ~Index() {
        // Liberar la memoria asignada a los punteros, si es necesario
        for (auto ptr : index) {
            delete ptr;
        }
    }*/
};

//functions
int calculateAsciiValue(string str) {
    int sum = 0;
    for (char c : str) {
        sum += static_cast<int>(c); // Sumar el valor ASCII de cada carácter
    }
    return sum;
}


//===================

template<typename T>
class ExtendibleHashing{
private:
    string db_filename;
    string idx_filename;
    int global_depth;
public:
    ExtendibleHashing(string db_filename, string idx_filename, int global_depth)
            : db_filename(std::move(db_filename)), idx_filename(std::move(idx_filename)), global_depth(global_depth) {
        bool empty_file = archivoVacio();
        ofstream file(db_filename, ios::binary);
        if(empty_file){
            int nd = -1;
            file.write((char*)&nd, sizeof(int)); //iniciar nextdel en -1
            file.close();
        }
    }

    //other functions
    int hashFunction(T key){
        int valor;
        if constexpr (std::is_same<T, std::string>::value) {
            // Si es un string, calcula la suma de los ASCII de sus caracteres
            valor = 0;
            for (char c: key) {
                valor += static_cast<int>(c);
            }
        }
        else if constexpr (std::is_arithmetic<T>::value)
            valor =  static_cast<int>(key);

        return valor % (1<<global_depth);
    };

    bool archivoVacio() {
        ifstream file(db_filename ,ios::binary);
        bool result =  file.peek() == ifstream::traits_type::eof();
        file.close();
        return result;
    }

    //main functions
    bool search(T key){

    };
    void insert(Record record){

        fstream file(db_filename, ios::binary);

    };
    bool remove(T key);
    //range search does not work
};


#endif //DATA_FUSION_DB_EXTENDIBLEHASHING_H
