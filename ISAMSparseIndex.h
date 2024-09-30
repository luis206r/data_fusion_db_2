#ifndef ISAM_SPARSE_INDEX_H
#define ISAM_SPARSE_INDEX_H

#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#define PR 4  // Registros por página
#define IR 3  // Índices por nodo

using namespace std;

struct IS_Record {
    char nombre[20];
    char apellido[20];
    char codigo[10];
    char carrera[10];
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
        cout << "\n=========IS_Record data========";
        cout << "\nNombre: " << nombre;
        cout << "\nApellido: " << apellido;
        cout << "\nCodigo: " << codigo;
        cout << "\nCarrera: " << carrera;
        cout << "\nCiclo: " << ciclo;
        cout << "\n============================";
    }
};

struct Page {
    int size;
    IS_Record records[PR];
    long next_page_pos;

    Page() : size(0), next_page_pos(-1) {}

    bool isFull() { return size == PR; }

    void insert(const IS_Record& record) {
        if (!isFull()) {
            records[size++] = record;
        }
    }
};

struct IndexNode {
    int size;
    int level;
    bool isLeaf;
    char keys[IR][20];
    long child_pointers[IR + 1];

    IndexNode() : size(0), level(0), isLeaf(true) {
        for (int i = 0; i <= IR; ++i) {
            child_pointers[i] = -1;
        }
    }

    void serialize(fstream& file) const {
        file.write(reinterpret_cast<const char*>(this), sizeof(IndexNode));
    }

    static IndexNode deserialize(fstream& file) {
        IndexNode node;
        file.read(reinterpret_cast<char*>(&node), sizeof(IndexNode));
        return node;
    }
};

class ISAM_Sparse_Index {
private:
    int index_level;
    string data_filename;
    vector<string> index_filenames;

    long root_pos;

    long insertIntoPage(const IS_Record& record, long page_pos) {
        fstream data_file(data_filename, ios::in | ios::out | ios::binary);
        Page page;

        if (page_pos == -1) {
            page_pos = data_file.tellp();
        } else {
            data_file.seekg(page_pos);
            data_file.read(reinterpret_cast<char*>(&page), sizeof(Page));
        }

        if (page.isFull()) {
            long new_page_pos = data_file.tellp();
            Page new_page;
            new_page.insert(record);
            data_file.write(reinterpret_cast<char*>(&new_page), sizeof(Page));
            page.next_page_pos = new_page_pos;
            data_file.seekp(page_pos);
            data_file.write(reinterpret_cast<char*>(&page), sizeof(Page));
            return new_page_pos;
        } else {
            page.insert(record);
            data_file.seekp(page_pos);
            data_file.write(reinterpret_cast<char*>(&page), sizeof(Page));
            return page_pos;
        }
    }

    void splitNode(long node_pos, int level) {
        fstream index_file(index_filenames[level - 1], ios::in | ios::out | ios::binary);
        IndexNode node = IndexNode::deserialize(index_file);

        if (node.size < IR) return;

        IndexNode new_node;
        new_node.level = node.level;
        new_node.isLeaf = node.isLeaf;

        int mid = IR / 2;
        for (int i = mid; i < IR; ++i) {
            strcpy(new_node.keys[new_node.size], node.keys[i]);
            new_node.child_pointers[new_node.size] = node.child_pointers[i];
            new_node.size++;
        }
        new_node.child_pointers[new_node.size] = node.child_pointers[IR];
        node.size = mid;

        long new_node_pos = index_file.tellp();
        new_node.serialize(index_file);

        index_file.seekp(node_pos);
        node.serialize(index_file);

        if (level == index_level) {
            createNewIndexLevel(node.keys[mid], node_pos, new_node_pos);
        } else {
            insertIntoParent(node.keys[mid], node_pos, new_node_pos, level + 1);
        }
    }

    void insertIntoParent(const char* key, long left_child, long right_child, int parent_level) {
        fstream parent_file(index_filenames[parent_level - 1], ios::in | ios::out | ios::binary);
        long parent_pos = findParent(root_pos, left_child, parent_level);

        IndexNode parent = IndexNode::deserialize(parent_file);

        int i;
        for (i = parent.size - 1; i >= 0 && strcmp(key, parent.keys[i]) < 0; --i) {
            strcpy(parent.keys[i + 1], parent.keys[i]);
            parent.child_pointers[i + 2] = parent.child_pointers[i + 1];
        }

        strcpy(parent.keys[i + 1], key);
        parent.child_pointers[i + 2] = right_child;
        parent.size++;

        parent_file.seekp(parent_pos);
        parent.serialize(parent_file);

        if (parent.size == IR + 1) {
            splitNode(parent_pos, parent_level);
        }
    }

    void createNewIndexLevel(const char* key, long left_child, long right_child) {
        index_level++;
        string new_index_filename = "index_" + to_string(index_level) + ".dat";
        index_filenames.push_back(new_index_filename);

        fstream new_index_file(new_index_filename, ios::out | ios::binary);
        IndexNode new_root;
        new_root.level = index_level;
        new_root.isLeaf = false;
        strcpy(new_root.keys[0], key);
        new_root.child_pointers[0] = left_child;
        new_root.child_pointers[1] = right_child;
        new_root.size = 1;

        new_root.serialize(new_index_file);
        root_pos = 0;
    }

    long findParent(long node_pos, long child_pos, int level) {
        if (level == 1) return node_pos;

        fstream index_file(index_filenames[level - 1], ios::in | ios::binary);
        index_file.seekg(node_pos);
        IndexNode node = IndexNode::deserialize(index_file);

        for (int i = 0; i <= node.size; ++i) {
            if (node.child_pointers[i] == child_pos) {
                return node_pos;
            }
            if (i < node.size && strcmp(node.keys[i], "") > 0) {
                long parent_pos = findParent(node.child_pointers[i], child_pos, level - 1);
                if (parent_pos != -1) return parent_pos;
            }
        }

        return -1;
    }

public:
    ISAM_Sparse_Index(const string& data_filename, const string& index_filename)
            : index_level(1), data_filename(data_filename), root_pos(0) {
        index_filenames.push_back(index_filename);

        fstream data_file(data_filename, ios::in | ios::out | ios::binary | ios::app);
        fstream index_file(index_filename, ios::in | ios::out | ios::binary | ios::app);

        if (index_file.peek() == ifstream::traits_type::eof()) {
            IndexNode root;
            root.level = 1;
            root.isLeaf = true;
            root.serialize(index_file);
        }
    }

    void insert(const IS_Record& record) {
        fstream index_file(index_filenames[0], ios::in | ios::out | ios::binary);
        IndexNode node = IndexNode::deserialize(index_file);
        long node_pos = 0;

        while (!node.isLeaf) {
            int i;
            for (i = 0; i < node.size && strcmp(record.nombre, node.keys[i]) >= 0; ++i);
            node_pos = node.child_pointers[i];
            index_file.seekg(node_pos);
            node = IndexNode::deserialize(index_file);
        }

        int i;
        for (i = 0; i < node.size && strcmp(record.nombre, node.keys[i]) >= 0; ++i);

        long page_pos = (i < node.size) ? node.child_pointers[i] : -1;
        long new_page_pos = insertIntoPage(record, page_pos);

        if (page_pos == -1) {
            if (i < IR) {
                strcpy(node.keys[i], record.nombre);
                node.child_pointers[i] = new_page_pos;
                node.size++;
                index_file.seekp(node_pos);
                node.serialize(index_file);
            } else {
                splitNode(node_pos, 1);
            }
        }
    }

    IS_Record search(const string& key) {
        fstream index_file(index_filenames[0], ios::in | ios::binary);
        fstream data_file(data_filename, ios::in | ios::binary);

        IndexNode node = IndexNode::deserialize(index_file);
        long node_pos = 0;

        while (!node.isLeaf) {
            int i;
            for (i = 0; i < node.size && key > string(node.keys[i]); ++i);
            node_pos = node.child_pointers[i];
            index_file.seekg(node_pos);
            node = IndexNode::deserialize(index_file);
        }

        int i;
        for (i = 0; i < node.size && key > string(node.keys[i]); ++i);

        if (i == node.size || key < string(node.keys[i])) {
            throw runtime_error("Registro no encontrado");
        }

        long page_pos = node.child_pointers[i];
        Page page;
        data_file.seekg(page_pos);
        data_file.read(reinterpret_cast<char*>(&page), sizeof(Page));

        for (int j = 0; j < page.size; ++j) {
            if (key == string(page.records[j].nombre)) {
                return page.records[j];
            }
        }

        throw runtime_error("Registro no encontrado");
    }

    void remove(const string& key) {
        // Implementación de la eliminación (opcional)
    }

    vector<IS_Record> range_search(const string& key1, const string& key2) {
        // Implementación de la búsqueda por rango (opcional)
        return vector<IS_Record>();
    }

    void scanAll() {
        fstream data_file(data_filename, ios::in | ios::binary);
        fstream index_file(index_filenames[0], ios::in | ios::binary);

        IndexNode node = IndexNode::deserialize(index_file);
        while (!node.isLeaf) {
            node = IndexNode::deserialize(index_file);
        }

        for (int i = 0; i < node.size; ++i) {
            long page_pos = node.child_pointers[i];
            Page page;
            data_file.seekg(page_pos);
            data_file.read(reinterpret_cast<char*>(&page), sizeof(Page));

            for (int j = 0; j < page.size; ++j) {
                page.records[j].showData();
            }

            while (page.next_page_pos != -1) {
                data_file.seekg(page.next_page_pos);
                data_file.read(reinterpret_cast<char*>(&page), sizeof(Page));
                for (int j = 0; j < page.size; ++j) {
                    page.records[j].showData();
                }
            }
        }
    }
};

#endif // ISAM_SPARSE_INDEX_H