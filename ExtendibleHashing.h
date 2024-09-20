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
#include <variant>
#include <iostream>

#define FB 4
#define GD = 3;
#define DB_SI = sizeof(int);
#define IDX_SI = sizeof(int);
using namespace std;

struct Record{
    //esta estructura depende de los datos de mockdata
    //para testear se usara lo siguiente

    char nombre[20];
    char apellido[20];
    char codigo[10];
    char carrera[10]; //usar abreviaturas
    int ciclo;

    void showData(){
        cout<<"\n==============================="
        <<"\nNombre: "<<nombre
        <<"\nApellido: "<<apellido
        <<"\nCodigo: "<<codigo
        <<"\nCarrera: "<<carrera
        <<"\nCiclo: "<<ciclo
        <<"\n===============================";
    }

    /*variant<char[], int> getMember(string key) {
        if (key == "nombre") {
            return nombre;
        } else if (key == "edad") {
            return edad;
        } else if (key == "ciclo") {
            return ciclo;
        }
        throw std::invalid_argument("Clave no válida"); // O manejar de otra forma
    }*/
};

struct Bucket{
    int local_depth;
    int size;
    Record records[FB];
    long nextDel; //free list method
    long nextBucket; //position of the next bucket
    void insert(Record record){//size previamente validado
        records[size] = record;
        size++;
    }
    void init(int ld, int sz){ //necesario para inserciones
        local_depth = ld;
        size = sz;
    };
};

struct IndexRecord{ //no lo uso
    int size;
    string cadena;//cadena de bits en string
    int pos;
    int status;//eliminado o no
};

struct MapValue{
    long pos; //posicion en el archivo binario de datos
    long i_pos;//posicion de indice en el archivo binario del indice
    MapValue(long p, long i_p){pos=p;i_pos=i_p;};
};


struct Index{ //this has to be stored //leer archivo de indive previamente
    //para el index file, internamente se usara : cadena_size(int), array de char(char(cadena_size)), pos(long), deleted
    // (-1 o 1),
    //acceso con map : O(lgn)
    //acceso con vector : O(1)
    int global_depth;
    string key_member;
    map<string, MapValue> index_map;

    void add(fstream &i_file, int cad_sz, string cad, long pos, int status){
        i_file.write((char*)&cad_sz, sizeof(int));
        i_file.write(cad.c_str(), sizeof(char)*cad_sz); // verificar si necesito (char*)
        i_file.write((char*)&pos, sizeof(long));
        i_file.write((char*)&status, sizeof(int));
    };
    void remove(fstream &i_file, long i_pos){
        i_file.seekg(i_pos, ios::beg);
        //leer y marcar======================================
        int sz;
        i_file.read(reinterpret_cast<char*>(&sz), sizeof(int));

        char* buffer = new char[sz + 1];
        i_file.read(buffer, sizeof(char) * sz);

        buffer[sz] = '\0'; // Asegurar el terminador nulo
        string cad = buffer;
        delete[] buffer; // Liberar memoria

        int pos, status;
        i_file.read(reinterpret_cast<char*>(&pos), sizeof(int));
        long pos_to_delete = i_file.tellg();
        status=-1;
        i_file.seekp(pos_to_delete, ios::beg); //me ubico en la posicion del puntero de lectura
        i_file.write((char*)&status, sizeof(int));//marcado como eliminado!!

    };//marcar como eliminado
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
    Index index;
public:
    ExtendibleHashing(string db_filename, string idx_filename, string idx_member)
            : db_filename(std::move(db_filename)), idx_filename(std::move(idx_filename)) {

        ofstream file(db_filename, ios::binary);
        fstream i_file(idx_filename, ios::binary);
        if(archivoVacio()){
            int nd = -1;
            file.write((char*)&nd, sizeof(int)); //iniciar nextdel en -1
            Bucket bucket;
            bucket.init(1, 0);
            file.write((char*)&bucket, sizeof(Bucket));
            file.write((char*)&bucket, sizeof(Bucket));
            file.close();
        }

        if(indiceVacio()){
            int gd=1;
            i_file.write((char*)&gd, sizeof(int)); //escribir profudidad global inicial
            char first = '0';
            char second = '1';
            int csz = 1;
            int status = -1;
            long first_pos = sizeof(int);
            long second_pos = sizeof(int) + sizeof(Bucket);
            long first_i_pos = sizeof(int);
            long second_i_pos = sizeof(int) + sizeof(int) + sizeof(char) + sizeof(long) + sizeof(int);
                                //global_d      sizecad     cad(0 o 1)      record_pos      status

            //puntero para primer bucket
            index.add(i_file, csz, to_string(first),first_pos, status);

            //puntero para segundo bucket
            index.add(i_file, csz, to_string(second), second_pos, status);
            i_file.close();
            //cargar el map
            index.index_map[to_string(first)] = MapValue(first_pos, first_i_pos);
            index.index_map[to_string(second)] = MapValue(second_pos, second_i_pos);
            global_depth = 1;
            index.global_depth = 1;
        }
        else{
            //cargar los indices al mapa
            int gd;
            i_file.read((char*)&gd, sizeof(int));//saltar gd
            global_depth = gd;
            index.global_depth = gd;
            while (!i_file.eof()) {//leer indice por indice e ignorar los marcados con -1
                int i_pos = i_file.tellp();
                int sz;
                i_file.read(reinterpret_cast<char*>(&sz), sizeof(int));

                char* buffer = new char[sz + 1];
                i_file.read(buffer, sizeof(char) * sz);

                buffer[sz] = '\0'; // Asegurar el terminador nulo
                string cad = buffer;
                delete[] buffer; // Liberar memoria

                int pos, status;
                i_file.read(reinterpret_cast<char*>(&pos), sizeof(int));
                i_file.read(reinterpret_cast<char*>(&status), sizeof(int));

                if (status == 1) {
                    index.index_map[cad] = MapValue(pos, i_pos);
                }
            }
        }
        index.key_member = idx_member;
    }

    //other functions
    string hashFunction(T key){ //devuelve el string en formato de cadena biaria
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

        valor = valor % (1<<global_depth);
        //convertir a string binario de acurdo a la profundidad
        bitset<global_depth>bs(valor);
        string result = bs.to_string();
        return result;
    };

    bool archivoVacio() {
        ifstream file(db_filename ,ios::binary);
        bool result =  file.peek() == ifstream::traits_type::eof();
        file.close();
        return result;
    }

    bool indiceVacio() {
        ifstream file(idx_filename ,ios::binary);
        bool result =  file.peek() == ifstream::traits_type::eof();
        file.close();
        return result;
    }

    void updateGD(int size){
        if(size>global_depth){
            global_depth = size;
        }
        if(index.global_depth<size){
            index.global_depth = size;
        }
    }

    //main functions
    bool search(T key){
        ifstream i_file(idx_filename, ios::binary);
        ifstream d_file(db_filename, ios::binary);
        string cad = hashFunction(key);
        string sufix = "";
        //long pos = index.index_map[cad];
        for(int i = cad.size()-1; i>=0; i--){
            sufix = cad[i]+sufix;
            auto it = index.index_map.find(sufix);
            if(it != index.index_map.end()){ //la clave existe
                long pos = index.index_map[sufix].pos;
                d_file.seekg(pos,ios::beg); //verificar si funciona con el -1 inicial
                Bucket bucket;
                d_file.read((char*)&bucket, sizeof(Bucket));
                Record f_r;
                bool found = false;
                for(auto record:bucket.records){
                    //nombre en este caso
                    if(record.nombre == key){
                        f_r=record;
                        found = true;
                        break;
                    }//la variable se define al crear los indices en la tabla(compiler)
                }
                if(found==true){
                    f_r.showData();
                }
                else{
                    cout<<"No se encontró el registro";
                }
                d_file.close();
                i_file.close();
                return found;
            }
            else{
                cout<<"No se encontró el registro";
                return false;
            }
        }
    };
    void insert(Record record){

        fstream d_file(db_filename, ios::binary);
        fstream i_file(idx_filename, ios::binary);

        //hashear por la clave (nombre);
        string hashcode = hashFunction(record.nombre);
        auto it = index.index_map.find(hashcode);
        if(it != index.index_map.end()) { //existe el bucket
            long pos = index.index_map[hashcode].pos;
            long i_pos = index.index_map[hashcode].i_pos; //index pos on index file
            d_file.seekg(pos, ios::beg);
            Bucket bucket;
            d_file.read((char*)&bucket, sizeof(Bucket));
            if(bucket.size<FB){//SI HAY ESPACIO DENTRO DEL BUCKET
                bucket.insert(record);
                bucket.size++;
                d_file.seekg(pos, ios::beg);
                d_file.write((char*)&bucket, sizeof(Bucket));
            }
            else{ //si el bucket esta lleno, hacer el split
                //actualizar el inidice con nuevos valores
                string new_hashcode_0 = '0' + hashcode;
                string new_hashcode_1 = '1' + hashcode;
                int local_deepth = new_hashcode_1.size();
                //actualizar el factor de bloque
                updateGD(local_deepth);


                //la posicion de new_hashcode_0 sera la misma de el bucket original
                //la posicion del new_hashcode_1, se escribira al final del archivo
                //dado que estoy usando freelist, puedo hacer que se escriba la posicion de algun bucket eliminado
                //para despues!
                //marcar anterior indice como eliminado

                //leer y marcar como eliminado el anterior indice======================================
                index.remove(i_file, i_pos);
                //=================================================
                d_file.seekp(0, ios::end);
                i_file.seekp(0, ios::end);
                long fileSize = d_file.tellp(); //tamaño del archivo, o sea para escribir al final
                long i_pos_new_hashcode_0 = i_file.tellp();
                //los nuevos indices se escriben al final, y el anterior se marca como eliminado, mientras q los nuevos
                // buckets, uno se queda en la anterior posicion, y el otro se va al final.
                //esto es posible porque con los bucket yo tengo un tamaño predeterminado.
                //con los indices, el tamaño es variado debido a la cadena de bits que va aumentando con cada aumento
                // de global depth
                //insertar nuevos indices==============
                index.add(i_file, new_hashcode_0.size(), new_hashcode_0, pos, 1);
                long i_pos_new_hashcode_1 = i_file.tellp();
                index.add(i_file, new_hashcode_1.size(), new_hashcode_1, fileSize, 1);
                //=====================================
                //tengo que eliminar el par anterior "...x":p, para cambiarlo por "...0x":q.
                index.index_map.erase(it);
                //actualizar indice, marcar anterior como eliminado, y escribir dos nuevos al final
                index.index_map[new_hashcode_0] = MapValue(pos, i_pos_new_hashcode_0);
                index.index_map[new_hashcode_1] = MapValue(fileSize, i_pos_new_hashcode_1);
                //agrega en el mapa los nuevos indices,


                // //reescribo la posicion con la nueva cadena dentro de mi map
                //==
                Bucket new_bucket_1, new_bucket_2;
                new_bucket_1.init(local_deepth, 0);
                new_bucket_2.init(local_deepth, 0);

                //insertar los nuevos buckets
                d_file.seekp(pos, ios::beg);//ubicar en la posicion del bucket anterior
                d_file.write((char*)&new_bucket_1, sizeof(Bucket));//escribir el primero nuevo bucket
                d_file.seekp(fileSize, ios::beg);//ubicar en la posicion final
                d_file.write((char*)&new_bucket_2, sizeof(Bucket));//escribir el nuevo segundo bucket
                //volver a insertar los registros en los nuevos buckets:
                for(auto record : bucket.records){
                    insert(record);
                }
            }
        }

    };
    bool remove(T key);
    //range search does not work
};


#endif //DATA_FUSION_DB_EXTENDIBLEHASHING_H
