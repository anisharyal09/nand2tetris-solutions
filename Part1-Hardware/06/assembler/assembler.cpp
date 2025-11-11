#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>

using namespace std;

// ---------------------- SymbolTable Class ---------------------
class SymbolTable{
protected:
    static unordered_map<string, int> table;

    static void addEntry(string symbol, int address){
        table[symbol] = address;
    }

    static bool contains(string symbol){
        return table.find(symbol) != table.end();
    }

    static int getAddress(string symbol){
        return table[symbol];
    }
};

// ---------------------- Code Class ---------------------
class Code{
private:
    static unordered_map<string, string> destTable;
    static unordered_map<string, string> compTable;
    static unordered_map<string, string> jumpTable;
public:
    static string dest(string mnemonic) { return destTable[mnemonic]; }
    static string comp(string mnemonic) { return compTable[mnemonic]; }
    static string jump(string mnemonic) { return jumpTable[mnemonic]; }

};

// ---------------------- FileHandler Class ---------------------
class FileHandler{
private:
    static ifstream inputFile;
    static ofstream outputFile;

public:
    static void init(const string& inputFileName, const string& outputFileName){
        inputFile.open(inputFileName, ios::in);
        outputFile.open(outputFileName, ios::out | ios::trunc);
    }

    static void close() {
        if (inputFile.is_open()) inputFile.close();
        if (outputFile.is_open()) outputFile.close();
    }

    static bool hasMoreLines(){
        return inputFile && !inputFile.eof();
    }

    // read instruction from file
    static string readInstruction() {
        string line;

        while (getline(inputFile, line)) {
            // remove any inline comment
            size_t commentPos = line.find("//");
            if (commentPos != string::npos)
                line = line.substr(0, commentPos);

            // remove all whitespace
            string instruction;
            for (char c : line) {
                if (!isspace(static_cast<unsigned char>(c)))
                    instruction += c;
            }

            // skip empty lines
            if (!instruction.empty())
                return instruction;
        }

        return ""; // eof reached
    }

    static void writeHackCode(string binaryCode){
        outputFile << binaryCode << endl;
    }
};

// ---------------------- Methods Class ---------------------
class Methods : public SymbolTable{
    static int labelCount;

public:
    static void ATranslation(string sym){
        int address;
        if (isdigit(sym[0])){
            address = stoi(sym); // string to int
        }
        else if (contains(sym)){
            address = getAddress(sym);
        }
        else{
            address = 16 + labelCount;
            addEntry(sym, address); // add new variable to symbol table
            labelCount++;
        }

        bitset<16> binary(address);
        FileHandler::writeHackCode(binary.to_string());
    }

    static void CTranslation(string sym){
        string destMnemonic = "";
        string compMnemonic = "";
        string jumpMnemonic = "";

        size_t eqPos = sym.find('=');
        size_t scPos = sym.find(';');

        if (eqPos != string::npos){
            destMnemonic = sym.substr(0, eqPos);
            if (scPos != string::npos){
                compMnemonic = sym.substr(eqPos + 1, scPos - eqPos - 1);
                jumpMnemonic = sym.substr(scPos + 1);
            }
            else{
                compMnemonic = sym.substr(eqPos + 1);
            }
        }
        else{
            if (scPos != string::npos){
                compMnemonic = sym.substr(0, scPos);
                jumpMnemonic = sym.substr(scPos + 1);
            }
            else{
                compMnemonic = sym;
            }
        }

        string compBits = Code::comp(compMnemonic);
        string destBits = Code::dest(destMnemonic);
        string jumpBits = Code::jump(jumpMnemonic);

        string binaryCode = "111" + compBits + destBits + jumpBits;
        FileHandler::writeHackCode(binaryCode);
    }
};

// ---------------------- Instruction Classes ---------------------
class Instruction{
public:
    virtual void translate(string s) = 0;
    virtual ~Instruction() = default;
};

// ---------------------- A, C, L Instruction Classes ---------------------
class AInstruction : public Instruction{
private:
public:
    void translate(string sym) override{
        Methods::ATranslation(sym);
    }
};

class CInstruction : public Instruction{
private:
public:
    void translate(string sym) override{
        Methods::CTranslation(sym);
    }
};

// ---------------------- FirstPass Class ---------------------
class FirstPass : public SymbolTable{
    public:
    static void processLabel(string sym, int currentLine){
        if (!contains(sym)){
            addEntry(sym, currentLine);
        } 
    }

    // handles the first pass to record labels
    static void run(string inF, string outF){
        int currentLine =0;
        string Instruction;
        FileHandler::init(inF, outF);
        while (FileHandler::hasMoreLines()){
            Instruction = FileHandler::readInstruction();
            if (Instruction.empty()) continue;
            if (Instruction.front() == '(' && Instruction.back() == ')'){
                string label = Instruction.substr(1, Instruction.length() - 2);
                processLabel(label, currentLine);
            }
            else{
                currentLine++;
            }
        }
        FileHandler::close();
    }
};

// static members initialization

//SymbolTable Class
unordered_map<string, int>SymbolTable::table= {
    {"SP", 0},
    {"LCL", 1},
    {"ARG", 2},
    {"THIS", 3},
    {"THAT", 4},
    {"R0", 0},
    {"R1", 1},
    {"R2", 2},
    {"R3", 3},
    {"R4", 4},
    {"R5", 5},
    {"R6", 6},
    {"R7", 7},
    {"R8", 8},
    {"R9", 9},
    {"R10", 10},
    {"R11", 11},
    {"R12", 12},
    {"R13", 13},
    {"R14", 14},
    {"R15", 15},
    {"SCREEN", 16384},
    {"KBD", 24576}
};

//Code Class
unordered_map<string, string> Code::destTable = {{"", "000"}, {"M", "001"}, {"D", "010"}, {"MD", "011"}, {"A", "100"}, {"AM", "101"}, {"AD", "110"}, {"AMD", "111"}};
unordered_map<string, string> Code::compTable = {{"0", "0101010"}, {"1", "0111111"}, {"-1", "0111010"}, {"D", "0001100"}, {"A", "0110000"}, {"!D", "0001101"}, {"!A", "0110001"}, {"-D", "0001111"}, {"-A", "0110011"}, {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"}, {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"}, {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"}, {"M", "1110000"}, {"!M", "1110001"}, {"-M", "1110011"}, {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"}, {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"}, {"D|M", "1010101"}};
unordered_map<string, string> Code::jumpTable = {{"", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}};

//FileHandler Class
ifstream FileHandler::inputFile;
ofstream FileHandler::outputFile;

//Methods Class
int Methods::labelCount = 0;

// ---------------------- Main Function ---------------------
int main(){
    string inputFileName = "Pong.asm"; // Read file name
    string outputFileName = "Pong.hack"; // Write file name

    FirstPass::run(inputFileName, outputFileName);

    FileHandler::init(inputFileName, outputFileName);
    if (!FileHandler::hasMoreLines()) {
        cerr << "Input file failed to open or is empty: " << inputFileName << endl;
        return 1;
    }

    while (FileHandler::hasMoreLines()){
        string instruction = FileHandler::readInstruction();
        if (instruction.empty())
            break;

        if (instruction.front() == '(' && instruction.back() == ')'){
            continue; //skip labels in second pass
        }

        Instruction* inst = nullptr;
        if (instruction[0] == '@'){
            inst = new AInstruction();
            inst->translate(instruction.substr(1));
        }
        else{
            inst = new CInstruction();
            inst->translate(instruction);
        }

        delete inst;
    }

    FileHandler::close();

    return 0;
}