#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <chrono>
using namespace std;

const int LENGTH = 32; // 32, 9
const int HEIGHT = 10; // 10, 3
const int NUM_LAYERS = 3329; // 3329, 5
const int LAYERS_LEN = NUM_LAYERS*LENGTH;
const int STACKABLES_LEN = NUM_LAYERS*NUM_LAYERS*2;

long numWays ();
void getLayers (vector<int> start, vector<vector<int>>& layers);
long numCombos (int height, bitset<LAYERS_LEN>& layers, int prevIndex, bitset<STACKABLES_LEN>& stackables, unordered_map<int, long>& knownCombos);
bool layersOverlap(int l1, int l2, bitset<LAYERS_LEN>& layers, bitset<STACKABLES_LEN>& stackables);

ostream& operator<< (ostream& lhs, vector<int> rhs) {
    lhs << "[";
    for (int i = 0; i < rhs.size(); i++) {
        lhs << rhs[i];
        if (i != rhs.size()-1)
            lhs << ", ";
    }
    lhs << "]";
    return lhs;
}

int main(){
    auto start = std::chrono::high_resolution_clock::now();

    cout << numWays() << endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Execution time: " << (duration.count())/1000 << " seconds" << std::endl;

    return 0;
}

long numWays () {
    vector<vector<int>> layers;
    getLayers({0}, layers);
    
    bitset<LAYERS_LEN> binaryLayers; // defaults all bits to 0
    int currentI = 1;
    int nextCrack;
    for (int i = 0; i < layers.size(); i++) {
        currentI = 1;
        nextCrack = layers[i][1];
        layers[i].push_back(LENGTH);
        for (int j = 1; j < LENGTH; j++) {
            if (j == nextCrack) {
                binaryLayers[LENGTH*i+j] = 1;
                currentI++;
                nextCrack = layers[i][currentI];
            }
        }
    }
    
    // 1st int is index*height, 2nd is combos from there
    unordered_map<int, long> knownCombos;
    
    // 2 bits for each pair:
    //   1. whether it has been determined that the two stack (1=determined)
    //   2. whether they can stack (1=yes)
    bitset<STACKABLES_LEN> stackables;
    
    return numCombos(HEIGHT, binaryLayers, -1, stackables, knownCombos);
}

void getLayers (vector<int> start, vector<vector<int>>& layers) {
    
    if (start[start.size()-1]+2 == LENGTH || start[start.size()-1]+3 == LENGTH) {
        layers.push_back(start);
        return;
    }
    
    if (start[start.size()-1]+2 < LENGTH) {
        start.push_back(start[start.size()-1]+2);
        getLayers(start, layers);
        start.pop_back();
    }
    if (start[start.size()-1]+3 < LENGTH) {
        start.push_back(start[start.size()-1]+3);
        getLayers(start, layers);
    }
    
}

long numCombos (int height, bitset<LAYERS_LEN>& layers, int prevIndex, bitset<STACKABLES_LEN>& stackables, unordered_map<int, long>& knownCombos) {
    if (height == 0) return 1;
    long total = 0;
    long newCombo;
    
    for (int i = 0; i < NUM_LAYERS; i++) {
        if (i == prevIndex) continue;
        if (prevIndex == -1 || !layersOverlap(min(i, prevIndex), max(i, prevIndex), layers, stackables)) {
            try {
                total += knownCombos.at(NUM_LAYERS*(height-1)+i);
            } catch (const out_of_range& oor) {
                newCombo = numCombos(height-1, layers, i, stackables, knownCombos); 
                knownCombos[NUM_LAYERS*(height-1)+i] = newCombo;
                total += newCombo;
            }
        }
    }
    return total;
}

// l1 < l2 is guarranteed
bool layersOverlap(int l1, int l2, bitset<LAYERS_LEN>& layers, bitset<STACKABLES_LEN>& stackables) {
    if (stackables[NUM_LAYERS*2*l1 + l2*2] == 1) {
        if (stackables[NUM_LAYERS*2*l1 + l2*2 + 1] == 0)
            return true;
        else
            return false;
    }
    stackables[NUM_LAYERS*2*l1 + l2*2] = 1;
    for (int i = 0; i < LENGTH; i++) {
        if (layers[l1*LENGTH+i] == 1 && layers[l1*LENGTH+i] == layers[l2*LENGTH+i]) {
            stackables[NUM_LAYERS*2*l1 + l2*2 + 1] = 0;
            return true;
        }
    }
    
    stackables[NUM_LAYERS*2*l1 + l2*2 + 1] = 1;
    return false;
}
