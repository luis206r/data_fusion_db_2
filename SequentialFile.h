//<>
// Created by luisr on 9/15/2024.
//

#ifndef DATA_FUSION_DB_SEQUENTIALFILE_H
#define DATA_FUSION_DB_SEQUENTIALFILE_H
#define AUX_MAX_SIZE 10

#include<cstring>
#include<iostream>
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
#include<cmath>
#include<chrono>
//#include<filesystem>

using namespace std;

namespace SEQ{

struct Record{
    char nombre[20];
    char apellido[20];
    char codigo[10];
    char carrera[10]; //usar abreviaturas
    int ciclo;
    char file;// 'd' para dat, 'a' para aux
    int next;//posicion logica del siguiente registro, tanto en data como en aux
    bool isAuxNext;//para saber si el siguiente esta en aux, sino sigo iterando en dat


    void setData(ifstream &file) {
        file.getline(nombre, 20, ',');
        file.getline(apellido, 20, ',');
        file.getline(codigo, 10, ',');
        file.getline(carrera, 10, ',');
        file>>ciclo; file.get();
    }

    void showData() {
        cout<<"\n=========Record data========";
        cout<<"\nNombre: "<<nombre;
        cout<<"\nApellido: "<<apellido;
        cout<<"\nCodigo: "<<codigo;
        cout<<"\nCarrera: "<<carrera;
        cout<<"\nCiclo: "<<ciclo;
        cout<<"\n============================";
    }

    // Sobrecargar el operador <
    bool operator<(const Record& other) const {
        return strcmp(this->nombre, other.nombre) < 0;
    }

    // Sobrecargar el operador >
    bool operator>(const Record& other) const {
        return strcmp(this->nombre, other.nombre) > 0;
    }

    // Sobrecargar el operador <=
    bool operator<=(const Record& other) const {
        return strcmp(this->nombre, other.nombre) <= 0;
    }

    // Sobrecargar el operador >=
    bool operator>=(const Record& other) const {
        return strcmp(this->nombre, other.nombre) >= 0;
    }

    // Sobrecargar el operador ==
    bool operator==(const Record& other) const {
        return strcmp(this->nombre, other.nombre) == 0;
    }

    // Sobrecargar el operador !=
    bool operator!=(const Record& other) const {
        return strcmp(this->nombre, other.nombre) != 0;
    }
    void setName(char arr[20]){
        for(int i=0; i<20; i++){
            nombre[i] = arr[i];
        }
    }
};

struct Header{
    int max_size;
    int cur_size;
    Header() = default;
    Header(int m, int c){
        max_size = m;
        cur_size = c;
    };
};

class SequentialFile {
private:
    bool initial_status;
    string data_filename;
    string aux_filename;
    int data_max_size;
    int data_size;
    int aux_size;
    int aux_max_size = AUX_MAX_SIZE;
    int max_deleted_elements = 5; //factor de reconstruccion al eliminar

public:
    SequentialFile(string d_name, string a_filename, int initial_d_max_size) {
        this->data_filename = d_name;
        this->aux_filename = a_filename;
        this->data_max_size = initial_d_max_size;
        this->aux_size = 0;
        this->data_size = 0;
//        this->aux_max_size = 10;//size definido

        fstream d_file(data_filename, ios::in | ios::out | ios::binary);
        fstream a_file(aux_filename, ios::in | ios::out | ios::binary);

        d_file.seekg(0, ios::beg);
        a_file.seekg(0, ios::beg);
        d_file.seekp(0, ios::beg);
        a_file.seekp(0, ios::beg);

        if (archivoVacio(d_file)) {
            cout<<"\nArchivo inicializado correctamente";
            this->initial_status = true;
            init(d_file, a_file);
        } else {
            this->initial_status = false;
            loadData(d_file, a_file);
        }
        d_file.close();
        a_file.close();
    };

    void init(fstream &d_file, fstream &a_file) {
        d_file.seekp(0, ios::beg);
        Header header(data_max_size, data_size);
        d_file.write(reinterpret_cast<char *>(&header), sizeof(Header));
        int a_sz = 0;
        a_file.seekp(0, ios::beg);
        a_file.write(reinterpret_cast<char *>(&a_sz), sizeof(int));
    }

    void loadData(fstream &d_file, fstream &a_file) {

        d_file.seekg(0, ios::beg);
        Header header;
        d_file.read(reinterpret_cast<char *>(&header), sizeof(Header));
        this->data_size = header.cur_size;
        this->data_max_size = header.max_size;
        a_file.seekg(0, ios::beg);
        int a_sz;
        a_file.read(reinterpret_cast<char *>(&a_sz), sizeof(int));
        this->aux_size = a_sz;
    }

    bool archivoVacio(fstream &file) {
        bool result = file.peek() == ifstream::traits_type::eof();
        return result;
    }

    bool search(string nombre){
        //auto inicio = std::chrono::high_resolution_clock::now();

        char array[20];
        strncpy(array, nombre.c_str(), sizeof(array) - 1);
        // Asegurarse de que el array esté null-terminado
        array[sizeof(array) - 1] = '\0';

        Record record;
        record.setName(array);
        return search(record);
        //auto fin = std::chrono::high_resolution_clock::now();
        //std::chrono::duration<double, std::milli> duracion = fin - inicio;
        //std::cout << "La búsqueda tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    }

    vector<Record> range_search(string nombre1, string nombre2){
        auto inicio = std::chrono::high_resolution_clock::now();

        char array1[20];
        strncpy(array1, nombre1.c_str(), sizeof(array1) - 1);
        // Asegurarse de que el array esté null-terminado
        array1[sizeof(array1) - 1] = '\0';
        char array2[20];
        strncpy(array2, nombre2.c_str(), sizeof(array2) - 1);
        // Asegurarse de que el array esté null-terminado
        array2[sizeof(array2) - 1] = '\0';
        Record record1, record2;
        record1.setName(array1);Record record;
        record2.setName(array2);
        //setear flag de insert en true para validar nodo previo y posterior
        //setear flag de rs en true
        vector<pair<int, char>> result1 = binary_search(record1, 0, data_size-1, true, true);
        vector<pair<int, char>> result2 = binary_search(record2, 0, data_size-1, true, true);
        //manejo de casos
        vector<Record> result;
        pair<int, char> initial_rec, final_rec;
        if(result1[1].first!=-1 && result1[1].first!=-2 && result1[1].first!=-3 && result1[1].first!=-4) initial_rec
        = result1[1];
        else if(result1[1].first==-2) initial_rec = make_pair(0, 'd'); //inicio del dat
        else if(result1[1].first == -3 ||result1[1].first == -4) return result;
        else initial_rec = result1[2]; //el mayor

        if(result2[1].first!=-1) final_rec=result2[1];
        else if(result2[1].first==-2) return result;//inicio del dat
        //para el siguiente escenario iterar hasta el final del aux y buscar los mayores
        else if(result2[1].first == -3) final_rec =  make_pair(data_size-1, 'd');
        else if(result2[1].first == -4) final_rec =  make_pair(aux_size-1, 'a');
        else final_rec = result2[0]; //el menor

        //iterar
        cout<<"\nvalores del range initial: "<<initial_rec.first<<" "<<initial_rec.second;
        cout<<"\nvalores del range final: "<<final_rec.first<<" "<<final_rec.second;
        ifstream d_file(data_filename, ios::binary);
        ifstream a_file(aux_filename, ios::binary);
        bool last_it = false;
        Record i_record;
        int p = initial_rec.first;
        char f = initial_rec.second;
        while(!last_it){

            //cout<<"\nf: "<<f;
            //cout<<"\np: "<<p;
            if(p==final_rec.first && f==final_rec.second) last_it = true;

            if(f=='d'){
                d_file.seekg(sizeof(Header) + sizeof(Record)*p, ios::beg);
                d_file.read(reinterpret_cast<char *>(&i_record), sizeof(Record));
                p = i_record.next;
                f = i_record.isAuxNext ? 'a' : 'd';
                result.push_back(i_record);
            }
            else{
                a_file.seekg(sizeof(int) + sizeof(Record)*p, ios::beg);
                a_file.read(reinterpret_cast<char *>(&i_record), sizeof(Record));
                p = i_record.next;
                f = i_record.isAuxNext ? 'a' : 'd';
                result.push_back(i_record);
            }
        }
        //copiar ultimo registro
        a_file.close();
        d_file.close();
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La búsqueda por rango tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

        return result;

    }
    void initialInsert(Record record, bool rebuild, fstream & d_file){ //se reciben los primeros 20 registros de forma
        auto inicio = std::chrono::high_resolution_clock::now();

        // ordenada
        //dado un vector de registros, insertar inicialmente de acuerdo al tamaño inicial del archivo de datos{
        //fstream d_file(rebuild ? "temp.dat" : data_filename, ios::in | ios::out | ios::binary);
       // fstream a_file(aux_filename, ios::in | ios::out | ios::binary);
        d_file.seekg(sizeof(Header), ios::beg);
        d_file.seekp(sizeof(Header), ios::beg);
        if(data_size > 0){
            Record prev_rec;
            d_file.seekg(sizeof(Header) + (sizeof(Record)*(data_size-1)), ios::beg);
            d_file.read(reinterpret_cast<char *>(&prev_rec), sizeof(Record));
            prev_rec.next = data_size;
            prev_rec.isAuxNext = false;
            //cout<<"\nnext: "<<prev_rec.next;
            //cout<<"\nian: "<<prev_rec.isAuxNext;
            prev_rec.file = 'd';
            d_file.seekp(sizeof(Header) + (sizeof(Record)*(data_size-1)), ios::beg);
            d_file.write(reinterpret_cast<char *>(&prev_rec), sizeof(Record));
        }
        record.next = -1;
        record.file = 'd';
        record.isAuxNext = false;
        d_file.write(reinterpret_cast<char *>(&record), sizeof(Record));

        data_size++;
        d_file.seekg(0, ios::beg);
        Header header;
        d_file.read(reinterpret_cast<char *>(&header), sizeof(Header));
        header.cur_size = data_size;
        d_file.seekp(0, ios::beg);
        d_file.write(reinterpret_cast<char *>(&header), sizeof(Header));
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La inserción inicial tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    };//inserciones iniciales para evitar efrrores;

    int size(){
        return data_size + aux_size;
    }

    void rebuild(){
        auto inicio = std::chrono::high_resolution_clock::now();

        cout<<"\nIniciando rebuild...";
        ifstream d_file(data_filename, ios::binary);
        fstream a_file(aux_filename,ios::in | ios::out | ios::binary);
        ofstream t_f("temp.dat", ios::app);
        t_f.close();
        fstream t_file("temp.dat", ios::in | ios::out | ios::binary);
        Header new_header;
        new_header.cur_size = 0;
        new_header.max_size = data_max_size + aux_max_size;
        data_size = 0;
        data_max_size = new_header.max_size;
        aux_size = 0;
        //cout<<"\ndata max size before rebuild: "<<data_max_size;
        t_file.seekp(0, ios::beg);
        t_file.write(reinterpret_cast<char *>(&new_header), sizeof(Header));
        a_file.seekp(0, ios::beg);
        int asz = 0;
        a_file.write(reinterpret_cast<char *>(&asz), sizeof(int));
        //===========

        Record record;
        d_file.seekg(sizeof(Header), ios::beg);
        int next = -5;
        bool ian = false;
        while(next!=-1){
            if (next==-5){
                d_file.seekg(sizeof(Header), ios::beg);
                d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                next = record.next;
                ian = record.isAuxNext;
                initialInsert(record, true, t_file);
            }
            else{
                if(ian){
                    a_file.seekg(sizeof(int) + next*sizeof(Record), ios::beg);
                    a_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                    next = record.next;
                    ian = record.isAuxNext;
                    initialInsert(record, true, t_file);
                }
                else{
                    d_file.seekg(sizeof(Header) + next*sizeof(Record), ios::beg);
                    d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                    next = record.next;
                    ian = record.isAuxNext;
                    initialInsert(record, true, t_file);
                }
            }
        }

        //copiar tod de nuevo al dat, actualiza la cabecera y atributos
        d_file.close();
        a_file.close();
        t_file.close();
        copy_and_upd();
        //cout<<"\n==============Datos after rebuild:===============";
        //showData();
        //cout<<"\n=================================================";
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La reconstrucción tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    };

    void copy_and_upd(){
        ofstream d_file(data_filename, ios::in | ios::out | ios::binary);
        ifstream t_file("temp.dat", ios::binary);
        ofstream a_file(aux_filename, ios::in | ios::out | ios::binary);
        d_file.seekp(0, ios::beg);
        t_file.seekg(0, ios::beg);
        Header header;
        t_file.read(reinterpret_cast<char *>(&header), sizeof(Header));
        d_file.write(reinterpret_cast<char *>(&header), sizeof(Header));
        while(!t_file.eof()){
            Record cur_rec;
            t_file.read(reinterpret_cast<char *>(&cur_rec), sizeof(Record));
            d_file.write(reinterpret_cast<char *>(&cur_rec), sizeof(Record));
        }
        int nas = 0;
        a_file.seekp(0, ios::beg);
        a_file.write(reinterpret_cast<char *>(&nas), sizeof(int));
        aux_size = 0;
        data_size = header.cur_size;
        data_max_size = header.max_size;
        cout<<"\ndata max size after rebuild: "<<data_max_size;
        cout<<"\ndata cur size after rebuild: "<<data_size;
        d_file.close();
        t_file.close();
        a_file.close();
        string td = "temp.dat";
        if (remove(td.c_str()) == 0) {
            cout << "Archivo " << td << " eliminado con éxito.\n";
        } else {
            cout << "Archivo " << td << " no encontrado o no pudo ser eliminado.\n";
        }
    }

    bool search(Record record){
        auto inicio = std::chrono::high_resolution_clock::now();

        vector<pair<int, char>> pointers = binary_search(record, 0, data_size-1, false, false);
        pair<int, char> mid = pointers[1];
        if(mid.first != -1){
            //el registro se encuentra en el archivo de datos
            string filename = mid.second == 'a' ? aux_filename : data_filename;
            ifstream file(filename, ios::binary);
            long desplazamiento = mid.second == 'a' ? sizeof(int) : sizeof(Header);
            file.seekg(desplazamiento + (mid.first*sizeof(Record)),ios::beg);
            Record read_rec;
            file.read(reinterpret_cast<char *>(&read_rec), sizeof(Record));
            cout<<"\nSe encontró el registro: ";
            read_rec.showData();
            return true;
        }
        else{
            cout<<"\nNo encontró el registro";
            return false;
        }
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La búsqueda tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    }

    void incrementAux(){
        fstream a_file(aux_filename, ios::in | ios::out | ios::binary);
        aux_size++;
        a_file.seekp(0, ios::beg);
        a_file.write(reinterpret_cast<char *>(&aux_size), sizeof(int));
    }

    void insert(Record record){ //pendiente validar el pushback
        auto inicio = std::chrono::high_resolution_clock::now();

        //cout<<"insertando registro de "<<record.nombre<<endl;
        if(data_size < data_max_size){
            //fstream t_file("temp.dat", ios::in | ios::out | ios::binary);
            fstream d_file(data_filename, ios::in | ios::out | ios::binary);
            initialInsert(record, false, d_file);
            d_file.close();
        }
        else{
            /*cout<<"\n==========Datos insertados hasta ahora==========\n";
            showDataDat();
            cout<<"\n================================================\n";*/
            cout<<"\nIniciando búsqueda binaria...";
            //asumiendo que no tengo elementos eliminados en el archivo de datos
            vector<pair<int, char>> pointers = binary_search(record, 0, data_size-1, true, false);
            fstream d_file(data_filename, ios::in | ios::out | ios::binary);
            fstream a_file(aux_filename, ios::in | ios::out | ios::binary);
            //cargar registro previo

            pair<int, char> next_info = pointers[2];
            pair<int, char> prev_info = pointers[0];
            pair<int, char> mid_info = pointers[1]; //para escenarios especiales
            //validar escenario
            if(mid_info.first == -2){//PUSHFRONT
                cout<<"\ncaso pushfront!!";
                //escribir la infirmacion del primer registro en el final del archivo auxiliar
                Record head_rec;
                d_file.seekg(sizeof(Header), ios::beg);
                d_file.read(reinterpret_cast<char *>(&head_rec), sizeof(Record));
                //IMPORTANTE: cambiar el atributo de file
                head_rec.file = 'a';
                a_file.seekp(sizeof(int)+(aux_size*sizeof(Record)), ios::beg);
                a_file.write(reinterpret_cast<char *>(&head_rec), sizeof(Record));

                //escribir el nuevo en el primer registro al inico del archivo de datos
                record.next = aux_size;
                record.isAuxNext = true;
                record.file = 'd';
                d_file.seekp(sizeof(Header), ios::beg);
                d_file.write(reinterpret_cast<char *>(&record), sizeof(Record));
                a_file.close();
                d_file.close();
                incrementAux();
                cout<<"\ncaso pushfront terminado!!";
            }
            else if(mid_info.first == -3){
                //PUSHBACK
                a_file.seekp(sizeof(int) + sizeof(Record)*aux_size, ios::beg);
                record.next = -1;
                record.isAuxNext = true;
                record.file = 'a';
                a_file.write(reinterpret_cast<char *>(&record), sizeof(Record));
                d_file.seekg(sizeof(Header)+(sizeof(Record)*(data_size-1)), ios::beg);
                Record last_rec;
                d_file.read(reinterpret_cast<char *>(&last_rec), sizeof(Record));
                last_rec.next = aux_size;
                last_rec.isAuxNext = true;
                d_file.seekp(sizeof(Header)+(sizeof(Record)*(data_size-1)), ios::beg);
                d_file.write(reinterpret_cast<char *>(&last_rec), sizeof(Record));

                a_file.close();
                d_file.close();
                incrementAux();
            }
            else if(mid_info.first == -4){
                //PUSH_AT_THE_END
                a_file.seekp(sizeof(int) + sizeof(Record)*aux_size, ios::beg);
                record.next = -1;
                record.isAuxNext = true;
                record.file = 'a';
                a_file.write(reinterpret_cast<char *>(&record), sizeof(Record));
                d_file.seekg(sizeof(Header)+(sizeof(Record)*(prev_info.first)), ios::beg);
                Record last_rec;
                d_file.read(reinterpret_cast<char *>(&last_rec), sizeof(Record));
                last_rec.next = aux_size;
                last_rec.isAuxNext = true;
                d_file.seekp(sizeof(Header)+(sizeof(Record)*(prev_info.first)), ios::beg);
                d_file.write(reinterpret_cast<char *>(&last_rec), sizeof(Record));

                a_file.close();
                d_file.close();
                incrementAux();
            }
            else{
                //apuntar el nuevo al next
                //escribir el nuevo y apuntar
                a_file.seekp(sizeof(int) + (aux_size*sizeof(Record)), ios::beg);
                record.next = next_info.first;
                record.file = 'a';
                record.isAuxNext = next_info.second == 'a';
                a_file.write(reinterpret_cast<char *>(&record), sizeof(Record));


                //identificar el anterior  y actualizar
                if(prev_info.second == 'a'){
                    //actualizar desde aux
                    a_file.seekg(sizeof(int)+(sizeof(Record)*prev_info.first), ios::beg);
                    Record prev_rec;
                    a_file.read(reinterpret_cast<char *>(&prev_rec), sizeof(Record));
                    prev_rec.next = aux_size;
                    prev_rec.isAuxNext = true;
                    a_file.seekp(sizeof(int)+(sizeof(Record)*prev_info.first), ios::beg);
                    a_file.write(reinterpret_cast<char *>(&prev_rec), sizeof(Record));

                    a_file.close();
                    d_file.close();
                    incrementAux();
                }
                else{
                    //actualizar desde dat
                    d_file.seekg(sizeof(Header)+(sizeof(Record)*prev_info.first), ios::beg);
                    Record prev_rec;
                    d_file.read(reinterpret_cast<char *>(&prev_rec), sizeof(Record));
                    prev_rec.next = aux_size;
                    prev_rec.isAuxNext = true;
                    d_file.seekp(sizeof(Header)+(sizeof(Record)*prev_info.first), ios::beg);
                    d_file.write(reinterpret_cast<char *>(&prev_rec), sizeof(Record));

                    a_file.close();
                    d_file.close();
                    incrementAux();
                }
            }
            if(aux_size == aux_max_size){
                showData();
                rebuild();
            }
        }
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La inserción tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    }

    bool remove(string key){
        //TODO
        return false;
    };

    void showDataDat(){
        ifstream d_file(data_filename, ios::binary);
        d_file.seekg(0, ios::end);
        long sz = d_file.tellg();
        d_file.seekg(sizeof(Header), ios::beg);
        while(d_file.tellg() < sz){
            Record cur_rec;
            d_file.read(reinterpret_cast<char *>(&cur_rec), sizeof(Record));
            cur_rec.showData();
        }
        d_file.close();
    }

    void showData(){
        ifstream d_file(data_filename, ios::binary);
        ifstream a_file(aux_filename, ios::binary);
        Record record;
        d_file.seekg(sizeof(Header), ios::beg);
        int next = -5;
        bool ian = false;
        while(next!=-1){
            if (next==-5){
                d_file.seekg(sizeof(Header), ios::beg);
                d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                next = record.next;
                ian = record.isAuxNext;
                record.showData();
                cout<<"\nnext: "<<record.next;
                cout<<"\nfile: "<<record.file;
            }
            else{
                if(ian){
                    a_file.seekg(sizeof(int) + next*sizeof(Record), ios::beg);
                    a_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                    next = record.next;
                    ian = record.isAuxNext;
                    record.showData();
                    cout<<"\nfile: "<<record.file;
                    cout<<"\nnext: "<<record.next;
                }
                else{
                    d_file.seekg(sizeof(Header) + next*sizeof(Record), ios::beg);
                    d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
                    next = record.next;
                    ian = record.isAuxNext;
                    record.showData();
                    cout<<"\nnext: "<<record.next;
                    cout<<"\nfile: "<<record.file;
                }
            }
        }
        d_file.close();
        a_file.close();
    }
    //funcion busqueda binaria
    //retorna un vector de 3 pares, cada par contiene una posicion de
    //un registro, en aux o dat, y un char que indice a que archivo pertenece
    //Esto se hace ya que al insertar o remover, necesito el nodo anterior y el nodo despues
    //para busquedas, simplemente retorno la informacion en el par del centro
    vector<pair<int, char>> binary_search(Record param_record, int p, int q, bool insert, bool rs){
        auto inicio = std::chrono::high_resolution_clock::now();

        ifstream d_file(data_filename, ios::binary);
        ifstream a_file(aux_filename, ios::binary);
        pair<int, char> none = make_pair(-1, 'x');
        vector<pair<int, char>> result = {none, none, none};
        //leer en memoria
        long position = sizeof(Header) + (p * sizeof(Record));
        d_file.seekg(position, ios::beg);
        Record record, rp, rq;
        d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
        //cout<<"\nKeys a comparar";
        //cout<<"\n"<<record.nombre;
        //cout<<"\n"<<param_record.nombre;

        //evaluar valores en p y q, puede reduir la complejidad logaritmica
        d_file.seekg(sizeof(Header) + sizeof(Record)*p, ios::beg);
        d_file.read(reinterpret_cast<char *>(&rp), sizeof(Record));
        d_file.seekg(sizeof(Header) + sizeof(Record)*q, ios::beg);
        d_file.read(reinterpret_cast<char *>(&rq), sizeof(Record));
        if(rp == param_record){
            if(!rs && insert){
                //cout<<"\nLa llave ya está";
                d_file.close();
                a_file.close();
                return result;
            }
            else{
                result[1] = make_pair(p, 'd');
                d_file.close();
                a_file.close();
                return result;
            }
        }
        if(rq == param_record){
            if(!rs && insert){
                d_file.close();
                a_file.close();
                return result;
            }
            else{
                //cout<<"\nLa llave ya está";
                result[1] = make_pair(q, 'd');
                d_file.close();
                a_file.close();
                return result;
            }
        }

        if(q==p) {
            //cout<<"\nQ es igual a P...";
            if (!insert || rs) { //si solo estoy haciendo busqueda no necesito las conexiones
                if (record != param_record) {
                    //iterar en el aux{
                    Record cur_rec;
                    for(int i=0; i<aux_size; i++){
                        a_file.seekg(sizeof(int) + sizeof(Record)*i, ios::beg);
                        a_file.read(reinterpret_cast<char *>(&cur_rec), sizeof(Record));
                        if(cur_rec == param_record){
                            cout<<"\nIgualdad!!!";
                            cout<<"\nnom1..."<<cur_rec.nombre;
                            cout<<"\nnom2..."<<cur_rec.nombre;
                            result[1] = make_pair(i, 'a');
                            break;
                        }
                    }
                    d_file.close();
                    a_file.close();
                    if(rs && result[1].first!=-1)
                        return result;
                    else if(!rs && !insert) return result;
                } else {
                        result[1] = make_pair(p, record.file);
                        d_file.close();
                        a_file.close();
                        return result;
                }
            }
            if (record > param_record) {
                //insercion antes del registro actual
                if (p == 0) {
                    //PUSH_FRONT //retorar -2 en el pos central
                    pair<int, char> mid = make_pair(-2, 'd');
                    result[1] = mid;
                } else {
                    //registro anterior en el archivo de datos
                    int logic_prev_position = p-1;
                    long prev_position = sizeof(Header) + ((p - 1) * sizeof(Record));
                    d_file.seekg(sizeof(Header) + ((p-1) * sizeof(Record)), ios::beg);
                    Record prev_record;
                    d_file.read(reinterpret_cast<char *>(&prev_record), sizeof(Record));
                    cout<<"\nPrevrec: "<<prev_record.nombre;
                    //validar
                    if(rs && (prev_record == param_record)){
                        result[1] = make_pair(logic_prev_position, 'd');
                        cout<<"\nSe encontró en el anterior... :)";
                        d_file.close();
                        a_file.close();
                        return result;
                    }
                    //leo el siguiente
                    //===
                    if (!prev_record.isAuxNext) {

                        pair<int, char> prev_rec = make_pair(logic_prev_position, 'd');
                        pair<int, char> next_rec = make_pair(logic_prev_position+1, 'd');
                        result[0] = prev_rec;
                        result[2] = next_rec;
                    } else {
                        //evaluar en el archivo aux de datos
                        int desplazamiento = sizeof(int);//header de aux
                        //a_file.seekg(sizeof(int), ios::beg);//desplazamiento incial
                        int logic_next_position = prev_record.next;
                        long next_position = sizeof(Record) * (prev_record.next) + desplazamiento;
                        a_file.seekg(next_position, ios::beg);//ubicar el registro
                        Record next_record;
                        a_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));

                        while (!(prev_record < param_record && next_record > param_record)) {
                            if(rs && (prev_record == param_record)){
                                result[1] = make_pair(logic_prev_position, 'd');
                                cout<<"\nSe encontró en el anterior... :)";
                                d_file.close();
                                a_file.close();
                                return result;
                            }
                            prev_position = next_position;
                            logic_prev_position = logic_next_position;

                            prev_record = next_record;

                            next_position = sizeof(Record) * (next_record.next) + desplazamiento;
                            logic_next_position = next_record.next;

                            if (next_record.isAuxNext) {
                                a_file.seekg((next_position), ios::beg);
                                a_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));
                            } else {
                                desplazamiento = sizeof(Header);
                                next_position = next_record.next * sizeof(Record) + desplazamiento;
                                d_file.seekg(next_position, ios::beg);
                                d_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));
                                break;
                            }

                        }
                        pair<int, char> prev_rec = make_pair(logic_prev_position, prev_record.file);
                        pair<int, char> next_rec = make_pair(logic_next_position, next_record.file);
                        result[0] = prev_rec;
                        result[2] = next_rec;
                    }
                }
            }

            else if (record == param_record) {
                //quiere decir que la llave ya está presente en el dat
                //no se puede realizar la insercion
                //retornar none
                //cout<<"\nYa existe la llave\n";
                if(rs){
                    result[1] = make_pair(p, 'd');
                }
                d_file.close();
                a_file.close();
                return result;
            }
            else if (record < param_record) {
                int logic_prev_position = p;
                long prev_position = sizeof(Header) + (p * sizeof(Record));
                d_file.seekg(prev_position, ios::beg);
                Record prev_record;
                d_file.read(reinterpret_cast<char *>(&prev_record), sizeof(Record));
                //evaluar iterar en aux
              //  cout<<"\nprev data name: "<<prev_record.nombre;
                //cout<<"\nprev data next: "<<prev_record.next;
                //cout<<"\ncurrent p value: "<<p;
                //cout<<"\ndata max size: "<<data_max_size;
                if (p == data_max_size-1 && prev_record.next == -1) {//validar despues este escenario
                    //insertar en el aux
                     result[1] = make_pair(-3, 'a');//pushback
                    d_file.close();
                    a_file.close();

                    //return result;

                } else {
                    //registro posterior en el archivo de datos
                       //leo el siguiente

                    if (!prev_record.isAuxNext) {
                        pair<int, char> prev_rec = make_pair(p, 'd');
                        pair<int, char> next_rec = make_pair(p+1, 'd');
                        result[0] = prev_rec;
                        result[2] = next_rec;
                    } else {
                        //evaluar en el archivo aux de datos
                        int desplazamiento = sizeof(int);//header de aux
                        //a_file.seekg(sizeof(int), ios::beg);//desplazamiento incial
                        int logic_next_position = prev_record.next;

                        long next_position = sizeof(Record) * (prev_record.next) + desplazamiento;
                        a_file.seekg(next_position, ios::beg);//ubicar el registro
                        Record next_record;
                        a_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));

                        bool pal = false;
                        while (!(prev_record < param_record && next_record > param_record) ) {
                            if(rs && (next_record == param_record)){
                                result[1] = make_pair(logic_next_position, next_record.file);
                                cout<<"\nSe encontró en el posterior... :)";
                                d_file.close();
                                a_file.close();
                                return result;
                            }

                            prev_position = next_position;
                            logic_prev_position = logic_next_position;

                            prev_record = next_record;

                            next_position = sizeof(Record) * (next_record.next) + desplazamiento;
                            logic_next_position = next_record.next;
                            if(logic_next_position == -1) { pal=true;break; };
                            if (next_record.isAuxNext) {
                                a_file.seekg((next_position), ios::beg);
                                a_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));
                            } else {
                                desplazamiento = sizeof(Header);
                                next_position = next_record.next * sizeof(Record) + desplazamiento;
                                d_file.seekg(next_position, ios::beg);
                                d_file.read(reinterpret_cast<char *>(&next_record), sizeof(Record));
                                break;
                            }
                        }
                        pair<int, char> prev_rec = make_pair(logic_prev_position, prev_record.file);
                        pair<int, char> next_rec = make_pair(logic_next_position, next_record.file);
                        result[0] = prev_rec;
                        result[2] = next_rec;
                        if(pal) result[1] = make_pair(-4, 'x');
                    }
                }
            }
            cout << "\nExiting binary_search. Result: ";
            for (const auto& pair : result) {
                cout << "(" << pair.first << ", " << pair.second << ") ";
            }
            cout << endl;

            d_file.close();
            a_file.close();
            return result;

        }
        else{
            //llamar recursivamente a la funcion
            int b_pos = floor((q+p)/2);
            //cout<<"\nposicion de p: "<<p;
            //cout<<"\nposicion de q: "<<q;
            //cout<<"\nposicion del medio: "<<b_pos;
            position = sizeof(Header) + (b_pos * sizeof(Record));
            d_file.seekg(position, ios::beg);
            d_file.read(reinterpret_cast<char *>(&record), sizeof(Record));
            if(param_record == record && !insert && !rs){
                d_file.close();
                a_file.close();
                result[1] = make_pair(b_pos, 'd');
                auto fin = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> duracion = fin - inicio;
                std::cout << "La búsqueda binaria tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

                return result;
            }
            else if(param_record < record){
                //cout<<endl<<param_record.nombre<<" es menor que "<<record.nombre;
                d_file.close();
                a_file.close();
                return binary_search(param_record, p,b_pos, insert, rs);
            }
            else {
                //cout<<endl<<param_record.nombre<<" es mayor o igual que "<<record.nombre;
                d_file.close();
                a_file.close();
                return binary_search(param_record, b_pos+1,q, insert, rs);
            }

        }
        auto fin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duracion = fin - inicio;
        std::cout << "La búsqueda binaria tardó " << duracion.count() << " ms en ejecutarse." << std::endl;

    };
};
}
#endif //DATA_FUSION_DB_SEQUENTIALFILE_H
//