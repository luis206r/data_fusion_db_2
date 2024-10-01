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
#include <iostream>
#include <iomanip>
#include<cstdio>
#define FB 4

using namespace std;
namespace EXTH{
struct Record {
    //esta estructura depende de los datos de mockdata
    //para testear se usara lo siguiente

    char nombre[20];
    char apellido[20];
    char codigo[10];
    char carrera[10]; //usar abreviaturas
    int ciclo;

    void setData(ifstream &file) {
        file.getline(nombre, 20, ',');
        file.getline(apellido, 20, ',');
        file.getline(codigo, 10, ',');
        file.getline(carrera, 10, ',');
        file >> ciclo;
        file.get();
    }

    void showData() {
        cout << "\n=========Record data========";
        cout << "\nNombre: " << nombre;
        cout << "\nApellido: " << apellido;
        cout << "\nCodigo: " << codigo;
        cout << "\nCarrera: " << carrera;
        cout << "\nCiclo: " << ciclo;
        cout << "\n============================";
    }
};

struct Bucket {
    int local_depth;
    int size;
    Record records[FB];
    long nextDel; //free list method
    long nextBucket; //position of the next bucket
    void insert(Record record) {//size previamente validado
        records[size] = record;
        size++;
    }

    void init(int ld, int sz, long nd, long nb) { //necesario para inserciones
        local_depth = ld;
        size = sz;
        nextDel = nd;
        nextBucket = nb;
    };

    void showData() {
        cout << "\nProfundidad local: " << local_depth;
        cout << "\nSize: " << size;
        cout << "\nNextDel: " << nextDel;
        cout << "\nNextBucket: " << nextBucket;

    }

    void loadData(fstream &file) {
        file.read(reinterpret_cast<char *>(&local_depth), sizeof(int));
        file.read(reinterpret_cast<char *>(&size), sizeof(int));
        file.read(reinterpret_cast<char *>(&records), sizeof(Record) * FB);  // Lee todos los registros directamente
        file.read(reinterpret_cast<char *>(&nextDel), sizeof(long));
        file.read(reinterpret_cast<char *>(&nextBucket), sizeof(long));
    }

    void serialize(fstream &file) {
        cout << "\nlocal deepth a escribir: " << local_depth;
        file.write(reinterpret_cast<char *>(&local_depth), sizeof(int));
        file.write(reinterpret_cast<char *>(&size), sizeof(int));
        for (int i = 0; i < FB; i++) {
            file.write(reinterpret_cast<char *>(&records[i]), sizeof(Record));
        }
        file.write(reinterpret_cast<char *>(&nextDel), sizeof(long));
        file.write(reinterpret_cast<char *>(&nextBucket), sizeof(long));
    }
};

struct IndexRecord { //no lo uso
    int size;
    string cadena;//cadena de bits en string
    long pos;
    int status;//eliminado o no
    void showData() {
        cout << "\nsize: " << size;
        cout << "\ncadena: " << cadena;
        cout << "\npos: " << pos;
        cout << "\nstatus: " << status;
    }
};

IndexRecord readIR(ifstream &i_file) {
    IndexRecord i_r;

    i_file.read(reinterpret_cast<char *>(&i_r.size), sizeof(int));
    char cad[i_r.size];
    i_file.read(reinterpret_cast<char *>(&cad), sizeof(char) * i_r.size);
    i_r.cadena = string(cad);
    i_file.read(reinterpret_cast<char *>(&i_r.pos), sizeof(long));
    i_file.read(reinterpret_cast<char *>(&i_r.status), sizeof(int));
    return i_r;
}

struct MapValue {
    long pos; //posicion en el archivo binario de datos
    long i_pos;//posicion de indice en el archivo binario del indice
    MapValue(long p, long i_p) {
        pos = p;
        i_pos = i_p;
    };

    MapValue() = default;
};


struct Index { //this has to be stored //leer archivo de indive previamente
    //para el index file, internamente se usara : cadena_size(int), array de char(char(cadena_size)), pos(long), deleted
    // (-1 o 1),
    //acceso con map : O(lgn)
    //acceso con vector : O(1)
    int global_depth;
    //string key_member;
    map<string, MapValue> index_map;

    void add(fstream &i_file, int cad_sz, string cad, long pos, int status) {
        //int sinb = cad_sz * sizeof(char); //tamaño en byttes del string
        //i_file.write(reinterpret_cast<char*>(&sinb, sizeof(int));
        i_file.write(reinterpret_cast<char *>(&cad_sz), sizeof(int));
        char cstr[cad.length() + 1]; // +1 para el carácter nulo
        std::strcpy(cstr, cad.c_str());
        cout << "\nCadena que estoy insertando en el indice: " << cstr << endl;
        i_file.write(reinterpret_cast<char *>(&cstr), sizeof(char) * cad_sz); // verificar si necesito (char*)
        i_file.write(reinterpret_cast<char *>(&pos), sizeof(long));
        i_file.write(reinterpret_cast<char *>(&status), sizeof(int));
    };

    void remove(fstream &i_file, long i_pos) {
        i_file.seekg(i_pos, ios::beg);
        //leer y marcar======================================
        int sz;
        i_file.read(reinterpret_cast<char *>(&sz), sizeof(int));
        i_file.seekg((sz * sizeof(char)) + sizeof(long), ios::cur);//saltarse cadena y pos
        long pos_to_delete = i_file.tellg();
        int status = -1;
        i_file.seekp(pos_to_delete, ios::beg); //me ubico en la posicion del puntero de lectura
        i_file.write(reinterpret_cast<char *>(&status), sizeof(int));//marcado como eliminado!!

    };//leer
    void readFile(fstream &i_file) {
        int gd;
        i_file.read(reinterpret_cast<char *>(&gd), sizeof(int));//saltar gd
        this->global_depth = gd;
        while (!i_file.eof()) {//leer indice por indice e ignorar los marcados con -1
            int i_pos = i_file.tellp();
            int sz;
            i_file.read(reinterpret_cast<char *>(&sz), sizeof(int));

            char *buffer = new char[sz + 1];
            i_file.read(buffer, sizeof(char) * sz);

            buffer[sz] = '\0'; // Asegurar el terminador nulo
            string cad = buffer;
            delete[] buffer; // Liberar memoria

            int pos, status;
            i_file.read(reinterpret_cast<char *>(&pos), sizeof(int));
            i_file.read(reinterpret_cast<char *>(&status), sizeof(int));

            if (status == 1) {
                this->index_map[cad] = MapValue(pos, i_pos);
            }
        }
    }

    void init(fstream &i_file) {
        this->global_depth = 1;
        i_file.write(reinterpret_cast<char *>(&global_depth), sizeof(int)); //escribir profudidad global inicial
        char first = '0';
        char second = '1';
        int csz = 1;
        int status = 1;
        long first_pos = sizeof(int);
        long second_pos = sizeof(int) + sizeof(Bucket);
        long first_i_pos = sizeof(int);
        long second_i_pos = sizeof(int) + sizeof(int) + sizeof(char) + sizeof(long) + sizeof(int);
        //global_d      sizecad     cad(0 o 1)      record_pos      status
        string s1(1, first);
        string s2(1, second);
        //puntero para primer bucket
        this->add(i_file, csz, s1, first_pos, status);

        //puntero para segundo bucket
        this->add(i_file, csz, s2, second_pos, status);

        //string


        //cargar el map
        this->index_map[s1] = MapValue(first_pos, first_i_pos);
        this->index_map[s2] = MapValue(second_pos, second_i_pos);
        //global_depth = 1
    }

    void validate_init(fstream &i_file) {
        i_file.seekg(sizeof(int), ios::beg);

        int sz;
        i_file.read(reinterpret_cast<char *>(&sz), sizeof(int));

        char *buffer = new char[sz + 1];
        i_file.read(buffer, sizeof(char) * sz);

        buffer[sz] = '\0'; // Asegurar el terminador nulo
        string cad = buffer;
        delete[] buffer; // Liberar memoria

        int pos, status;
        i_file.read(reinterpret_cast<char *>(&pos), sizeof(int));
        i_file.read(reinterpret_cast<char *>(&status), sizeof(int));
        cout << "\n validando inicializacion de indice 1...";
        cout << "\n size: " << sz;
        cout << "\n cad: " << cad;
        cout << "\n pos: " << pos;
        cout << "\n status: " << status;
    }
};


//functions
int calculateAsciiValue(string str) {
    int sum = 0;
    for (char c: str) {
        sum += static_cast<int>(c); // Sumar el valor ASCII de cada carácter
    }
    return sum;
}

//===================

//template<typename T>
class ExtendibleHashing {
private:
    string db_filename;
    string idx_filename;
    // int global_depth;
    Index index;
    string initial_status;
public:
    ExtendibleHashing(string db_filename, string idx_filename) {
        this->db_filename = db_filename;
        this->idx_filename = idx_filename;
        initial_status = "full";//puede cambiar
        fstream file(db_filename, ios::in | ios::out | ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo");
        fstream i_file(idx_filename, ios::in | ios::out | ios::binary);
        if (!i_file.is_open()) throw ("No se pudo abrir el archivo");

        if (archivoVacio(file)) {

            cout << "\nescribiendo buckets iniciales";
            file.seekp(0, ios::beg);
            file.seekg(0, ios::beg);
            int nd = -1;
            file.write(reinterpret_cast<char *>(&nd), sizeof(int)); //iniciar nextdel en -1
            Bucket bucket;
            bucket.init(1, 0, -2, -1);
            bucket.serialize(file);
            bucket.serialize(file);
            //validando escritura
            cout << "\nvalidando escritura de  buckets iniciales...";
            file.seekg(sizeof(int), ios::beg);
            Bucket b1, b2;
            b1.loadData(file);
            b2.loadData(file);
            b1.showData();
            b2.showData();
            file.close();
        }

        if (archivoVacio(i_file)) {
            initial_status = "void";
            i_file.seekp(0, ios::beg);
            i_file.seekg(0, ios::beg);
            cout << "\nescribiendo indices iniciales...";
            index.init(i_file);
            index.validate_init(i_file);
            i_file.close();
        } else {
            //cargar los indices al mapa
            cout << "\ncargando indices en ram...";
            index.readFile(i_file);
            i_file.close();
        }
        //index.key_member = idx_member;
    }
    string getInitialStatus(){return initial_status;}
    //other functions
    string hashFunction(string key, int c_gd) {
        int valor = 0;
        for (char c: key) {
            valor += static_cast<int>(c);
        }
        int gd_to_use = c_gd != -1 ? c_gd : index.global_depth;
        // Aquí se utiliza el valor actual de global_depth
        valor = valor % (1 << gd_to_use);

        // Usar std::vector<bool> para manejar el tamaño dinámico
        int gd = gd_to_use;
        std::vector<bool> bits(gd);

        // Llenar el vector con los bits
        for (int i = 0; i < gd; ++i) {
            bits[gd - 1 - i] = (valor & (1 << i)) != 0; // Asignar el bit correspondiente
        }
        // Convertir a string
        string result;
        for (bool bit: bits) {
            result += (bit ? "1" : "0");
        }
        return result;
    }

    bool archivoVacio(fstream &file) {
        bool result = file.peek() == ifstream::traits_type::eof();
        //file.close();
        return result;
    }

    void updateGD(fstream &i_file, int size) {
        if (size > index.global_depth) {
            i_file.seekp(0, ios::beg);
            i_file.write(reinterpret_cast<char *>(&size), sizeof(int));
            index.global_depth = size;
        }
    }

    void scanAll() {
        ifstream i_file(idx_filename, ios::binary);
        ifstream d_file(db_filename, ios::binary);

        i_file.seekg(0, ios::end);
        long i_size = i_file.tellg();
        cout << "\nlast pos in index: " << i_size << endl;
        i_file.seekg(sizeof(int), ios::beg);
        d_file.seekg(sizeof(int), ios::beg);
        while (true) {
            cout << "\nposicion del indice que estoy leyendo: " << i_file.tellg();
            if (i_file.tellg() == i_size)break;
            cout << "\nleyendo...";
            // Leer el siguiente índice
            IndexRecord current_idx = readIR(i_file);
            cout << "\nposicion del indice despues de leer: " << i_file.tellg();
            cout << "\nData del indice leido...";
            current_idx.showData();
            // Verificar si la lectura fue exitosa
            //if (!i_file) break;

            // Procesar si el registro no está eliminado
            if (current_idx.status != -1) {

                Bucket bucket;

                // Posicionar y leer el bucket correspondiente
                d_file.seekg(current_idx.pos, ios::beg);
                d_file.read(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
                cout << "\nData del bucket leido...";
                bucket.showData();
                // Procesar los registros del bucket
                for (int i = 0; i < bucket.size; i++) {
                    Record record = bucket.records[i];
                    cout << "==========================\n";
                    record.showData();
                    cout << "==========================\n";
                }
            }

        }

        d_file.close();
        i_file.close();
    };

    bool search(string key) { return search(key, false); }

    //main functions
    bool search(string key, bool remove) {
        fstream i_file(idx_filename, ios::in | ios::out | ios::binary);
        fstream d_file(db_filename, ios::in | ios::out | ios::binary);
        string cad = hashFunction(key, -1);
        string sufix = "";
        //long pos = index.index_map[cad];
        bool found = false;
        Record f_r;
        for (int i = index.global_depth; i > 0; i--) {
            if (i < index.global_depth)sufix = hashFunction(key, i);
            else sufix = cad;
            cout << "\nCadena hash para el key " << key << ": " << sufix << endl;
            auto it = index.index_map.find(sufix);
            if (it != index.index_map.end()) { //la clave existe
                long pos = index.index_map[sufix].pos;
                long i_pos = index.index_map[sufix].i_pos;
                d_file.seekg(pos, ios::beg); //verificar si funciona con el -1 inicial
                Bucket bucket;
                d_file.read(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
                for (auto record: bucket.records) {
                    //nombre en este caso
                    if (record.nombre == key) {
                        f_r = record;
                        found = true;

                        break;
                    }//la variable se define al crear los indices en la tabla(compiler)
                }
                if (found == true) {
                    if (remove) {
                        Bucket new_bucket;

                        new_bucket.init(bucket.local_depth, bucket.size - 1, bucket.nextDel, bucket.nextBucket);
                        int p = 0;
                        for (int i = 0; i < bucket.size; i++) {
                            if (bucket.records[i].nombre != key) {
                                new_bucket.records[p] = bucket.records[i];
                                p++;
                            }
                        }
                        d_file.seekp(pos, ios::beg);
                        d_file.write(reinterpret_cast<char *>(&new_bucket), sizeof(Bucket));

                    }
                    break;
                } else {
                    cout << "\n...buscando...\n";
                }
            }
            /*else{
                cout<<"No se encontró el registro";
                d_file.close();
                i_file.close();
                return false;
            }*/
        }
        if (found == true) {
            if (remove) {
                cout << "\nSe encontró y se eliminó el registro:\n";
            } else {
                cout << "\nSe encontró el registro:\n";
                f_r.showData();
            }
        } else {
            cout << "\nNo se encontró el registro :(\n";
        }
        d_file.close();
        i_file.close();
        return found;
    };
    void insert(Record record){
        insert(record, "none");
    }
    void insert(Record record, string sufix) {

        fstream d_file(db_filename, ios::in | ios::out | ios::binary);
        fstream i_file(idx_filename, ios::in | ios::out | ios::binary);
        if (!d_file.is_open()) {
            cerr << "Error al abrir el archivo de datos: " << db_filename << endl;
            return;
        }
        if (!i_file.is_open()) {
            cerr << "Error al abrir el archivo de indices: " << idx_filename << endl;
            return;
        }

        cout << "\nInsertando registro...";
        //hashear por la clave (nombre);
        string hashcode = sufix == "none" ? hashFunction(string(record.nombre), -1) : sufix;
        cout << "\nCodigo hash gereado para " << record.nombre << ": " << hashcode;
        //if()
        auto it = index.index_map.find(hashcode);
        if (it != index.index_map.end()) { //existe el bucket
            cout << "\nexiste el indice...";
            long pos = index.index_map[hashcode].pos;
            cout << "\nposicion en el data:..." << pos;
            long i_pos = index.index_map[hashcode].i_pos; //index pos on index file
            cout << "\nposicion en el index:..." << i_pos;
            d_file.seekg(pos, ios::beg);
            Bucket bucket;
            d_file.read(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
            cout << "\nBucket leido:";
            bucket.showData();
            if (bucket.size < FB) {//SI HAY ESPACIO DENTRO DEL BUCKET
                cout << "\nInsertando registro en bucket con size: " << bucket.size;
                bucket.insert(record);
                d_file.seekp(pos, ios::beg);
                d_file.write(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
            } else { //si el bucket esta lleno, hacer el split
                //actualizar el inidice con nuevos valores
                cout << "\nBucket lleno, haciendo split...";
                string new_hashcode_0 = "0" + hashcode;
                string new_hashcode_1 = "1" + hashcode;
                int local_deepth = new_hashcode_1.size();
                //actualizar la profundidad global de acuerdo a la local
                updateGD(i_file, local_deepth);


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
                new_bucket_1.init(local_deepth, 0, -1, -1);
                new_bucket_2.init(local_deepth, 0, -1, -1);

                //insertar los nuevos buckets
                d_file.seekp(pos, ios::beg);//ubicar en la posicion del bucket anterior
                d_file.write(reinterpret_cast<char *>(&new_bucket_1), sizeof(Bucket));//escribir el primero nuevo bucket
                d_file.seekp(fileSize, ios::beg);//ubicar en la posicion final
                d_file.write(reinterpret_cast<char *>(&new_bucket_2), sizeof(Bucket));//escribir el nuevo segundo bucket
                //volver a insertar los registros en los nuevos buckets:
                i_file.close();
                d_file.close();
                for (int i = 0; i < bucket.size; i++) {
                    insert(bucket.records[i], sufix); //insertar registros anteriores
                }
                insert(record, sufix);//insertar el registro que queria insertar inicialmente
            }
        } else {
            //asumiendo que siempre encuentra un sufijo
            string sufix = "";
            for (int i = index.global_depth; i > 0; i--) {
                sufix = hashFunction(record.nombre, i);
                auto it2 = index.index_map.find(sufix);
                if (it2 != index.index_map.end()) break;
            }
            insert(record, sufix);
        }
        i_file.close();
        d_file.close();
    };

    vector<Record> range_search(string a, string b){

    };//para evitar problemas del compilador

    bool remove(string key) {
        return search(key, true);
    };
    //range search does not work
};
}
#endif //DATA_FUSION_DB_EXTENDIBLEHASHING_H