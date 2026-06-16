/*
 * ============================================================
 *  Automato de Pilha (Pushdown Automaton - PDA)
 *  Versao 3 - PDA Configuravel por Arquivo
 *
 *  Le as transicoes de um arquivo .txt e simula o automato.
 *  Isso permite testar qualquer linguagem livre de contexto
 *  sem precisar alterar o codigo-fonte.
 *
 *  Formato do arquivo de configuracao (pda.txt):
 *  ---
 *  estados: q0 q1 q2
 *  alfabeto_entrada: a b
 *  alfabeto_pilha: Z A
 *  estado_inicial: q0
 *  simbolo_inicial_pilha: Z
 *  estados_finais: q2
 *  transicoes:
 *  q0 a Z q0 0 A
 *  q0 a A q0 0 A
 *  q0 b A q1 A 0
 *  q1 b A q1 A 0
 *  q1 0 Z q2 0 0
 *  ---
 *  Cada linha de transicao: estado_atual simbolo_entrada topo_pilha
 *                            estado_destino pop_simbolo push_simbolo
 *  Use '0' para representar "nada" (epsilon)
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <set>

struct Transition {
    std::string nextState;
    char        popSymbol;
    char        pushSymbol;
};

class PDA {
public:
    using TransKey = std::tuple<std::string, char, char>;

    std::set<std::string>          states;
    std::map<TransKey, Transition> delta;
    std::string                    initialState;
    char                           initialStackSym;
    std::set<std::string>          acceptStates;

    void addTransition(const std::string& from,
                       char inputSym, char topStack,
                       const std::string& to,
                       char popSym, char pushSym)
    {
        delta[{from, inputSym, topStack}] = {to, popSym, pushSym};
    }

    // --------------------------------------------------------
    //  Carrega o automato a partir de um arquivo .txt
    // --------------------------------------------------------
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[ERRO] Nao foi possivel abrir: " << filename << "\n";
            return false;
        }

        std::string line, section;
        bool readingTransitions = false;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (line.find("estados:") == 0) {
                std::istringstream ss(line.substr(8));
                std::string s;
                while (ss >> s) states.insert(s);
            }
            else if (line.find("estado_inicial:") == 0) {
                std::istringstream ss(line.substr(15));
                ss >> initialState;
            }
            else if (line.find("simbolo_inicial_pilha:") == 0) {
                std::istringstream ss(line.substr(22));
                char c; ss >> c;
                initialStackSym = c;
            }
            else if (line.find("estados_finais:") == 0) {
                std::istringstream ss(line.substr(15));
                std::string s;
                while (ss >> s) acceptStates.insert(s);
            }
            else if (line.find("transicoes:") == 0) {
                readingTransitions = true;
            }
            else if (readingTransitions) {
                std::istringstream ss(line);
                std::string from, to;
                char inputSym, topStack, popSym, pushSym;
                if (ss >> from >> inputSym >> topStack >> to >> popSym >> pushSym) {
                    addTransition(from, inputSym, topStack, to, popSym, pushSym);
                }
            }
        }

        file.close();
        std::cout << "[OK] Automato carregado de: " << filename << "\n";
        std::cout << "     Estados: " << states.size()
                  << " | Transicoes: " << delta.size() << "\n";
        return true;
    }

    // --------------------------------------------------------
    //  Simula o PDA sobre uma cadeia de entrada
    // --------------------------------------------------------
    bool run(const std::string& input) const {
        std::string      currentState = initialState;
        std::stack<char> pda_stack;
        pda_stack.push(initialStackSym);

        std::cout << "\n--- Simulacao ---\n";
        std::cout << "Entrada: \"" << input << "\"\n\n";
        printConfig(currentState, input, 0, pda_stack);

        for (size_t i = 0; i < input.size(); ++i) {
            char symbol = input[i];
            char top    = pda_stack.empty() ? '0' : pda_stack.top();

            TransKey key = {currentState, symbol, top};

            // Tenta transicao normal
            if (delta.find(key) == delta.end()) {
                std::cout << "  [ERRO] Nenhuma transicao para ("
                          << currentState << ", " << symbol << ", " << top << ")\n";
                std::cout << "Resultado: REJEITADA\n";
                return false;
            }

            const Transition& t = delta.at(key);
            if (t.popSymbol != '0' && !pda_stack.empty()) pda_stack.pop();
            if (t.pushSymbol != '0') pda_stack.push(t.pushSymbol);

            currentState = t.nextState;
            printConfig(currentState, input, i + 1, pda_stack);
        }

        // Tenta transicao epsilon (simbolo '0') ao final
        char top = pda_stack.empty() ? '0' : pda_stack.top();
        TransKey epsilonKey = {currentState, '0', top};
        if (delta.find(epsilonKey) != delta.end()) {
            const Transition& t = delta.at(epsilonKey);
            if (t.popSymbol != '0' && !pda_stack.empty()) pda_stack.pop();
            currentState = t.nextState;
            std::cout << "  (transicao epsilon) -> " << currentState << "\n";
        }

        bool byFinalState = (acceptStates.count(currentState) > 0);
        bool byEmptyStack = (pda_stack.size() == 1 && pda_stack.top() == initialStackSym);

        bool accepted = byFinalState || byEmptyStack;
        std::cout << "Resultado: " << (accepted ? "ACEITA" : "REJEITADA") << "\n";
        return accepted;
    }

    // --------------------------------------------------------
    //  Exibe as transicoes carregadas
    // --------------------------------------------------------
    void printTransitions() const {
        std::cout << "\n--- Transicoes carregadas ---\n";
        for (const auto& [key, t] : delta) {
            auto [from, input, top] = key;
            std::cout << "  (" << from << ", " << input << ", " << top << ") -> ("
                      << t.nextState << ", pop=" << t.popSymbol
                      << ", push=" << t.pushSymbol << ")\n";
        }
    }

private:
    void printConfig(const std::string& state,
                     const std::string& input,
                     size_t pos,
                     const std::stack<char>& stk) const
    {
        std::string remaining = (pos < input.size()) ? input.substr(pos) : "vazia";
        std::stack<char> tmp = stk;
        std::string stackStr;
        while (!tmp.empty()) { stackStr += tmp.top(); tmp.pop(); }
        if (stackStr.empty()) stackStr = "vazia";
        std::cout << "  (" << state << ", " << remaining << ", " << stackStr << ")\n";
    }
};

// ============================================================
//  Cria o arquivo pda.txt de exemplo (anbn) se nao existir
// ============================================================
void createExampleConfig() {
    std::ofstream file("pda.txt");
    file << "# Automato de Pilha para { a^n b^n | n >= 1 }\n";
    file << "# Linhas com '#' sao comentarios\n\n";
    file << "estados: q0 q1 q2\n";
    file << "alfabeto_entrada: a b\n";
    file << "alfabeto_pilha: Z A\n";
    file << "estado_inicial: q0\n";
    file << "simbolo_inicial_pilha: Z\n";
    file << "estados_finais: q2\n";
    file << "transicoes:\n";
    file << "q0 a Z q0 0 A\n";
    file << "q0 a A q0 0 A\n";
    file << "q0 b A q1 A 0\n";
    file << "q1 b A q1 A 0\n";
    file << "q1 0 Z q2 0 0\n";
    file.close();
    std::cout << "[OK] Arquivo 'pda.txt' de exemplo criado.\n";
}

// ============================================================
//  MAIN
// ============================================================
int main(int argc, char* argv[]) {
    std::cout << "==============================================\n";
    std::cout << "  Automato de Pilha - Versao Configuravel\n";
    std::cout << "==============================================\n\n";

    std::string configFile = "pda.txt";

    // Aceita arquivo como argumento: ./pda_config meu_automato.txt
    if (argc > 1) {
        configFile = argv[1];
    }

    PDA pda;

    // Tenta carregar; se nao achar, cria exemplo
    std::ifstream test(configFile);
    if (!test.good()) {
        std::cout << "Arquivo '" << configFile << "' nao encontrado.\n";
        std::cout << "Criando arquivo de exemplo...\n\n";
        createExampleConfig();
    }
    test.close();

    if (!pda.loadFromFile(configFile)) return 1;

    pda.printTransitions();

    std::cout << "\n--- Teste Interativo ---\n";
    std::cout << "Digite cadeias para testar (ou 'sair' para encerrar):\n";

    std::string input;
    while (true) {
        std::cout << "\nCadeia: ";
        std::cin >> input;
        if (input == "sair") break;
        pda.run(input);
    }

    return 0;
}