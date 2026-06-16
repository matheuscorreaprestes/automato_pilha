/*
 * ============================================================
 *  Automato de Pilha (Pushdown Automaton - PDA)
 *  Linguagens Formais e Automatos
 *
 *  Exemplo classico: reconhece a linguagem { a^n b^n | n >= 1 }
 *  (numero igual de 'a's seguidos de 'b's)
 *
 *  Estrategia:
 *    - Para cada 'a' lido, empilha 'A'
 *    - Para cada 'b' lido, desempilha 'A'
 *    - Aceita se a pilha so tiver 'Z' (fundo) ao final
 * ============================================================
 */

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <set>

// ============================================================
//  Estrutura de uma Transicao do Automato de Pilha
// ============================================================
struct Transition {
    std::string nextState;
    char        popSymbol;   // '0' = nao desempilha
    char        pushSymbol;  // '0' = nao empilha
};

// ============================================================
//  Classe PDA - Automato de Pilha Deterministico
// ============================================================
class PDA {
public:
    using TransKey = std::tuple<std::string, char, char>;

    std::set<std::string>           states;
    std::set<char>                  inputAlphabet;
    std::set<char>                  stackAlphabet;
    std::map<TransKey, Transition>  delta;
    std::string                     initialState;
    char                            initialStackSym;
    std::set<std::string>           acceptStates;

    void addTransition(const std::string& from,
                       char inputSym,
                       char topStack,
                       const std::string& to,
                       char popSym,
                       char pushSym)
    {
        delta[{from, inputSym, topStack}] = {to, popSym, pushSym};
    }

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

            if (delta.find(key) == delta.end()) {
                std::cout << "  [ERRO] Nenhuma transicao para ("
                          << currentState << ", " << symbol
                          << ", " << top << ")\n";
                std::cout << "Resultado: REJEITADA\n";
                return false;
            }

            const Transition& t = delta.at(key);

            if (t.popSymbol != '0' && !pda_stack.empty()) {
                pda_stack.pop();
            }
            if (t.pushSymbol != '0') {
                pda_stack.push(t.pushSymbol);
            }

            currentState = t.nextState;
            printConfig(currentState, input, i + 1, pda_stack);
        }

        // Aceita se:
        // 1) esta em estado final, OU
        // 2) a pilha so tem o simbolo de fundo 'Z'
        bool acceptedByFinalState  = (acceptStates.count(currentState) > 0);
        bool acceptedByEmptyStack  = (pda_stack.size() == 1 && pda_stack.top() == 'Z');

        if (acceptedByFinalState || acceptedByEmptyStack) {
            std::cout << "Resultado: ACEITA\n";
            return true;
        } else {
            std::cout << "Resultado: REJEITADA\n";
            return false;
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
        while (!tmp.empty()) {
            stackStr += tmp.top();
            tmp.pop();
        }
        if (stackStr.empty()) stackStr = "vazia";

        std::cout << "  (" << state << ", "
                  << remaining << ", "
                  << stackStr << ")\n";
    }
};

// ============================================================
//  Constroi o PDA para { a^n b^n | n >= 1 }
// ============================================================
PDA buildAnBnPDA() {
    PDA pda;

    pda.states          = {"q0", "q1"};
    pda.inputAlphabet   = {'a', 'b'};
    pda.stackAlphabet   = {'Z', 'A'};
    pda.initialState    = "q0";
    pda.initialStackSym = 'Z';
    pda.acceptStates    = {};  // aceitacao por pilha vazia (so Z no fundo)

    // Lendo 'a' com topo Z: empilha A (Z fica no fundo)
    pda.addTransition("q0", 'a', 'Z', "q0", '0', 'A');
    // Lendo 'a' com topo A: empilha mais um A
    pda.addTransition("q0", 'a', 'A', "q0", '0', 'A');
    // Primeiro 'b': desempilha um A, vai para q1
    pda.addTransition("q0", 'b', 'A', "q1", 'A', '0');
    // Continua lendo 'b': desempilha A
    pda.addTransition("q1", 'b', 'A', "q1", 'A', '0');

    return pda;
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    std::cout << "==============================================\n";
    std::cout << "  Automato de Pilha - Linguagem { a^n b^n }\n";
    std::cout << "==============================================\n";

    PDA pda = buildAnBnPDA();

    std::vector<std::string> testCases = {
        "ab",        // aceita  (n=1)
        "aabb",      // aceita  (n=2)
        "aaabbb",    // aceita  (n=3)
        "aaaabbbb",  // aceita  (n=4)
        "aab",       // rejeita
        "abb",       // rejeita
        "ba",        // rejeita
        "abab",      // rejeita
        "aabbb",     // rejeita
    };

    for (const auto& s : testCases) {
        std::cout << "\n========================================";
        pda.run(s);
    }

    std::cout << "\n\n--- Teste Interativo ---\n";
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