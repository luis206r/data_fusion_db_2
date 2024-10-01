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
#include <algorithm> // Para std::transform
#include <cctype>

using namespace std;

std::string tl(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

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
        //cout << "comando: " << command<<endl;// << ", f: " << f << ", table: " << table << ", condition: " <<
        // condition
        //cout << "comando min?: " << tl(command)<<endl;

        if (tl(command) == "select") {
            selectQuery(ss);
        } else if (tl(command) == "delete") {
            deleteQuery(ss);
        } else if (tl(command) == "insert") {
            insertQuery(ss);
        }
        else cout<<"Ingrese un comando correcto";
    }

public:

    void selectQuery(stringstream &ss) {
        string table, condition, field, f, operatorSymbol, temp;
        ss >> field >> f >> table; // Leer campo, luego "from", luego tabla

        // Verificar si es un select * de la tabla especificada
        if (tl(field) == "*" && tl(f) == "from" && tl(table) == "table") {

            // Procesar las consultas con condiciones
            ss >> temp; // Leer la siguiente parte (debería ser "where" si hay condiciones)
            if (tl(temp) == "where") {
                ss >> condition; // Leer la condición
                ss >> operatorSymbol; // Leer el operador

                if (operatorSymbol == "=") {
                    string value;
                    ss >> value;

                    bool found = file->search(value);
                    if (found) {
                        cout << "Se encontró la llave";
                    } else {
                        cout << "No se encontró la llave";
                    }
                } else if (operatorSymbol == tl("between")) {
                    if (ct == "seq") {
                        string val1, val2;
                        ss >> val1 >> temp >> val2; // Leer valores entre BETWEEN y AND
                        vector<R> records = file->range_search(val1, val2);
                        SEQ::showTableHeader();
                        for (auto rec : records) {
                            rec.showData();
                        }
                    } else {
                        cout << "Extendible hashing no soporta la búsqueda por rango";
                    }
                }
            }

            else{
                cout << "\nConsulta válida... trayendo todos los registros.\n";
                    //vector<R> allRecords = file->getAllRecords(); // Método hipotético
                    SEQ::showTableHeader();
                    file->showData();
            }


        }


    }


private:
    void deleteQuery(stringstream &ss) {
        string is, table, temp;
        ss >> is >> table >> temp;  // Leer tabla y "where"

        // Verificar si hay una condición para eliminar
        if (tl(is)=="from" && tl(table)=="table" && tl(temp) == "where") {
            string condition, operatorSymbol, value;
            ss >> condition; // Leer la condición (por ejemplo, 'nombre')
            ss >> operatorSymbol; // Leer el operador (por ejemplo, '=')
            ss >> value; // Leer el valor a comparar

            if (condition == "nombre" && operatorSymbol == "=") {
                if (ct == "seq") {
                    cout<<"Aun no implementado";
                    //bool removed = file->remove(value);
                    //if(removed)cout << "Registro eliminado." << endl;
                    //else cout << "Registro no eliminado" <<endl;
                    // Buscar el registro en el archivo secuencial

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
        string into, table, temp;
        ss >> into >> table >> temp;  // Leer tabla y "into"

        // Verificar si hay valores a insertar
        if (tl(into)=="into" && tl(table)=="table" && tl(temp) == "values") {
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