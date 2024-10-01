//<>
// Created by luisr on 9/30/2024.
//
#ifndef DATA_FUSION_DB_COMPILER_H
#define DATA_FUSION_DB_COMPILER_H

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "SequentialFile.h"
#include "ExtendibleHashing.h"

using namespace std;

template<typename T, typename R>
class Compiler {
private:
    string ct;
    T *file;
public:
    string words[3] = {"SELECT", "DELETE", "INSERT"};
    string atribute;
    string connectors[3] = {"into", "where", "values"};
    string operators[3] = {"BETWEEN", "AND", "="};

    Compiler(string classtype, T *file){
        ct = classtype;
        this->file = file;
    }

    // Función para analizar el query y ejecutarlo
    void processQuery(string &query) {
        stringstream ss(query);
        string command, temp;
        ss >> command;  // Leer SELECT, DELETE o INSERT

        if (command == "SELECT") {
            selectQuery(ss);
        } else if (command == "DELETE") {
            deleteQuery(ss);
        } else if (command == "INSERT") {
            insertQuery(ss);
        }
        else cout<<"Ingrese un comando correcto";
    }

public:
    void selectQuery(stringstream &ss) {
        string table, condition, field, operatorSymbol, temp;
        ss >> table >> temp >> condition; // Leer tabla, luego "where", luego condición

        if (condition == "nombre") {
            ss >> operatorSymbol;
            if (operatorSymbol == "=") {
                string value;
                ss >> value;

                bool found = file->search(value);
                if(found) cout<<"Se encontró la llave";
                else  cout<<"No se encontró la llave";


            } else if (operatorSymbol == "BETWEEN") {
                if(ct=="seq"){
                    string val1, val2;
                    ss >> val1 >> temp >> val2; // Leer valores entre BETWEEN y AND
                    vector<R> records = file->range_search(val1, val2);
                    for(auto rec:records){
                        rec.showData();
                    }
                }
                else{
                    cout<<"Extendible hashing no soporta la busqueda por rango";
                }

            }
        }
    }

private:
    void deleteQuery(stringstream &ss) {
        string table, temp;
        ss >> table >> temp;  // Leer tabla y "where"

        // Verificar si hay una condición para eliminar
        if (temp == "where") {
            string condition, operatorSymbol, value;
            ss >> condition; // Leer la condición (por ejemplo, 'nombre')
            ss >> operatorSymbol; // Leer el operador (por ejemplo, '=')
            ss >> value; // Leer el valor a comparar

            if (condition == "nombre" && operatorSymbol == "=") {
                if (ct == "seq") {
                    cout<<"Aun no implementado";
                    bool removed = file->remove(value);
                    if(removed)cout << "Registro eliminado." << endl;
                    else cout << "Registro no eliminado" <<endl;
                    // Buscar el registro en el archivo secuencial
                   /* SEQ::Record recordToDelete = static_cast<SEQ::Record &&>(file->search(value));
                    if (recordToDelete.nombre[0] != '\0') { // Verificar que el registro exista
                        file->remove(recordToDelete); // Llamar al método remove para eliminar el registro
                        cout << "Registro eliminado correctamente." << endl;
                    } else {
                        cout << "Registro no encontrado." << endl;
                    }*/
                } else {
                    // Si estás usando Extendible Hashing
                    bool removed = file->remove(value);
                    if(removed)cout << "Registro eliminado." << endl;
                    else cout << "Registro no eliminado" <<endl;
                }
            } else {
                cout << "Condición o operador no válidos." << endl;
            }
        } else {
            cout << "Error en la sintaxis de la consulta DELETE." << endl;
        }
    }


    void insertQuery(stringstream &ss) {
        string table, temp;
        ss >> table >> temp;  // Leer tabla y "into"

        // Verificar si hay valores a insertar
        if (temp == "values") {
            // Asumimos que los valores llegan en el siguiente formato:
            // values ('nombre', 'apellido', ...)
            string nombre, apellido, codigo, carrera;
            int ciclo;

            // Leer los valores
            ss >> temp; // Leer '('
            getline(ss, nombre, ','); // Leer nombre
            getline(ss, apellido, ','); // Leer apellido
            getline(ss, codigo, ','); // Leer codigo
            getline(ss, carrera, ','); // Leer carrera
            ss >> ciclo; // Leer ciclo
            ss >> temp; // Leer ')'

            // Crear el registro

                R newRecord;
                strncpy(newRecord.nombre, nombre.c_str(), sizeof(newRecord.nombre) - 1);
                strncpy(newRecord.apellido, apellido.c_str(), sizeof(newRecord.apellido) - 1);
                strncpy(newRecord.codigo, codigo.c_str(), sizeof(newRecord.codigo) - 1);
                strncpy(newRecord.carrera, carrera.c_str(), sizeof(newRecord.carrera) - 1);
                newRecord.ciclo = ciclo;
                file->insert(newRecord);

            //cout << "Registro insertado correctamente." << endl;
        } else {
            cout << "Error en la sintaxis de la consulta INSERT." << endl;
        }
    }
};





#endif //DATA_FUSION_DB_COMPILER_H
//