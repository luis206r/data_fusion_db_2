//<>
#include<iostream>
#include"ExtendibleHashing.h"
#include"ISAMSparseIndex.h"
#include"SequentialFile.h"
#include "Compiler.h"

using namespace std;

void seq_wf(SEQ::SequentialFile &file){
    SEQ::Record record;
    ifstream fileIn("datos2.csv");
    while(true)
    {
        if(fileIn.eof()) break;
        record.setData(fileIn);

        //record.showData();
        file.insert(record);
    }
    fileIn.close();
}

void clearFile(string name){
    if (remove(name.c_str()) == 0) {
        cout << "Archivo " << name << " eliminado con éxito.\n";
    } else {
        cout << "Archivo " << name << " no encontrado o no pudo ser eliminado.\n";
    }
    ofstream file(name, ios::app);
    file.close();
}

void SeqMenu(){
    cout<<"\n Sequential File: ";
    cout<<"\n Cargando csv...";
    string dname = "seq_data.dat";
    string aname = "seq_aux.dat";
    string tname = "temp.dat";
    //eliminar previamente
    /*if (remove(dname.c_str()) == 0) {
        cout << "Archivo " << dname << " eliminado con éxito.\n";
    } else {
        cout << "Archivo " << dname << " no encontrado o no pudo ser eliminado.\n";
    }

    if (remove(aname.c_str()) == 0) {
        cout << "Archivo " << aname << " eliminado con éxito.\n";
    } else {
        cout << "Archivo " << aname << " no encontrado o no pudo ser eliminado.\n";
    }

    if (remove(tname.c_str()) == 0) {
        cout << "Archivo " << tname << " eliminado con éxito.\n";
    } else {
        cout << "Archivo " << tname << " no encontrado o no pudo ser eliminado.\n";
    }*/
    //================================================
    //crear archivos=================================
    /*ofstream f1(dname, ios::binary);
    ofstream f2(aname, ios::binary);
    f1.close();
    f2.close();*/
    //====================================
    SEQ::SequentialFile file(dname, aname, 20);
    //seq_wf(file);
    //cout<<"\n Carga terminada";
    file.showData();
    string name1 = "Ana";
    string name2 = "Maria";
    vector<SEQ::Record> res = file.range_search(name1, name2);
    for(auto rec:res){
        rec.showData();
    }
    file.search("Emilio");
    cout<<"\nsize: "<<file.size();
}

void writeFile(string dname, EXTH::ExtendibleHashing &file){
    EXTH::Record record;
    ifstream fileIn("datos2.csv");
    /*char cabecera[40];
    fileIn.getline(cabecera, 40);*/
    while(true)
    {
        if(fileIn.eof()) break;
        record.setData(fileIn);

        //record.showData();
        file.insert(record, "none");
    }
    fileIn.close();
};

void writeFileIS(string i_name, string d_name, ISAM_Sparse_Index &file){
    IS_Record record;
    ifstream fileIn("datos2_sorted.csv");
    /*char cabecera[40];
    fileIn.getline(cabecera, 40);*/
    while(true)
    {
        if(fileIn.eof()) break;
        record.setData(fileIn);

        //record.showData();
        file.insert(record);
    }
    fileIn.close();
};

void readFile(string i_name, string d_name, EXTH::ExtendibleHashing &file){

    cout<<"--------- show all data -----------\n";
    file.scanAll();
    /*cout<<"--------- search Eva -----------\n";
    Record record = file.search("Eva");
    record.showData();*/
}

void readFileIS(string i_name, string d_name, ISAM_Sparse_Index &file){

    cout<<"--------- show all data -----------\n";
    file.scanAll();
    /*cout<<"--------- search Eva -----------\n";
    Record record = file.search("Eva");
    record.showData();*/
}

void printHeader() {
    cout << "\n====================================================================";
    cout << "\n====================================================================";
    cout << "\n====================================================================";
    cout << "\n====================================================================";
    cout << R"(
  ____        _        _____           _
 |  _ \  __ _| |_ __ _|  ___|   _ ___(_) ___  _ __
 | | | |/ _` | __/ _` | |_ | | | / __| |/ _ \| '_ \
 | |_| | (_| | || (_| |  _|| |_| \__ \ | (_) | | | |
 |____/ \__,_|\__\__,_|_|   \__,_|___/_|\___/|_| |_|

)";
    cout << "\n                  by luisr";
    cout << "\n====================================================================";
    cout << "\n====================================================================";
    cout << "\n====================================================================";
    cout << "\n====================================================================";
}

void menu(){
    printHeader();
    int op = 0;
    do{
        cout<<"\n\nSeleccione la estructura";
        cout<<"\n1. Sequential File";
        cout<<"\n2. Extendible Hashing";
        cout<<"\n3. Limpiar archivos";
        cout<<"\n4. Salir";
        cout<<"\nIngrese opción: ";
        cin>>op;
        if(op==3){
            clearFile("seq_aux.dat");
            clearFile("seq_data.dat");
            clearFile("data.dat");
            clearFile("index.dat");
            clearFile("temp.dat");
            cout<<"\nArchivos limpiados correctamente\n";
            op=5;
        }
    }while(op!=1 && op!=2 && op!=3 && op!=4);
    cout << "\n\n|======================Ejemplo de consultas=====================|" << endl;
    cout << "\n|=======       SELECT * WHERE nombre = Valeria       ===========|" << endl;
    cout << "\n|=======  SELECT * WHERE nombre BETWEEN Ana AND Maria  =========|" << endl;
    cout << "\n|=============   DELETE WHERE nombre = Carlos   ================|" << endl;
    cout << "\n|===INSERT values (Laura, Gonzalez, 20210123, Ingenieria, 3) ===|" << endl;
    cout << "\n|===============================================================|" << endl;
    if(op==1){
        string dname = "seq_data.dat";
        string aname = "seq_aux.dat";
        SEQ::SequentialFile seqFile(dname, aname, 20);
        cout<<"\nVerificando datos...";
        int sz = seqFile.size();
        cout<<"\nEl archivo tiene "<<sz<<" registros";
        if(sz == 0){
            cout<<"\nCargando csv...";
            seq_wf(seqFile);
            cout<<"\nCarga completa";
        }

        //=======================
        string query;
        cout << "\nBienvenido al sistema de consultas. Escriba 'exit' para salir." << endl;


        Compiler<SEQ::SequentialFile, SEQ::Record> seqCompiler("seq", &seqFile);
        while(true){
            cout << "\n\nIngrese su consulta (termine con ';'):" << endl;
            query.clear(); // Limpiar la consulta anterior

            // Acumular la consulta línea por línea
            string line;
            while (true) {
                getline(cin, line); // Leer cada línea
                query += line; // Agregar la línea a la consulta

                // Verificar si la consulta termina con un punto y coma
                if (query.back() == ';') {
                    break; // Terminar si se encuentra el punto y coma
                }
            }

            query.pop_back(); // Eliminar el último carácter (el punto y coma)

            if (query == "exit") {
                break; // Salir del bucle si el usuario ingresa "exit"
            }

            // Procesar la consulta en función del tipo de archivo
            if (query.find("INSERT") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para INSERT
            } else if (query.find("DELETE") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para DELETE
            } else if (query.find("SELECT") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            }  else if (query.find("delete") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para DELETE
            } else if (query.find("select") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            } else if (query.find("insert") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            } else {
                cout << "Consulta no válida. Intente de nuevo." << endl;
            }
        }
    }
    else if(op==2){
        //para extendible
        string dname = "data.dat";
        string iname = "index.dat";
        EXTH::ExtendibleHashing exFile(dname, iname);
        cout<<"\nVerificando datos...";
        string is = exFile.getInitialStatus();
        if(is=="void"){
            cout<<"\nCargando CSV...";
            writeFile(dname, exFile);
        }
        cout<<"\nCarga completada";

        string query;
        cout << "\nBienvenido al sistema de consultas. Escriba 'exit;' para salir." << endl;


        Compiler<EXTH::ExtendibleHashing, EXTH::Record> seqCompiler("ext", &exFile);
        while(true){

            cout << "\n\nIngrese su consulta (termine con ';'):" << endl;
            query.clear(); // Limpiar la consulta anterior

            // Acumular la consulta línea por línea
            string line;
            while (true) {
                getline(cin, line); // Leer cada línea
                query += line; // Agregar la línea a la consulta

                // Verificar si la consulta termina con un punto y coma
                if (query.back() == ';') {
                    break; // Terminar si se encuentra el punto y coma
                }
            }

            query.pop_back(); // Eliminar el último carácter (el punto y coma)

            if (query == "exit") {
                break; // Salir del bucle si el usuario ingresa "exit"
            }

            // Procesar la consulta en función del tipo de archivo
            if (query.find("INSERT") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para INSERT
            } else if (query.find("DELETE") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para DELETE
            } else if (query.find("SELECT") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            }  else if (query.find("delete") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para DELETE
            } else if (query.find("select") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            } else if (query.find("insert") != string::npos) {
                seqCompiler.processQuery(query); // Usar secuencial para SELECT
            } else {
                cout << "Consulta no válida. Intente de nuevo." << endl;
            }
        }

    }
    else if(op==4){
        cout<<"Programa finalizado" << endl;
        return;
    }
};

int main() {
    menu();
    //SeqMenu();
}