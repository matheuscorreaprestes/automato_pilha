/*
 * ============================================================
 *  Automato de Pilha (Pushdown Automaton - PDA)
 *  Versao 2 - Linguagem de Palindromos
 *
 *  Reconhece a linguagem { w c w^r | w pertence {a,b}* }
 *  onde 'c' e o separador central e w^r e o reverso de w
 *
 *  Exemplos aceitos:  acca  |  abcba  |  aabcbaa  |  bcb
 *  Exemplos rejeit.:  abc   |  abcab  |  aacba
 *
 *  Estrategia:
 *    - q0: le os simbolos antes do 'c' e empilha cada um
 *    - q1: apos o 'c', le os simbolos e desempilha comparando
 *    - Aceita se a pilha so tiver 'Z' ao final
 * ============================================================
 */

#include <iostream>
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

    std::map<TransKey, Transition> delta;
    std::string                    initialState;
    char                           initialStackSym;
    std::set<std::string>          acceptStates;

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

        bool accepted = (pda_stack.size() == 1 && pda_stack.top() == 'Z');
        std::cout << "Resultado: " << (accepted ? "ACEITA" : "REJEITADA") << "\n";
        return accepted;
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

PDA buildPalindromePDA() {
    PDA pda;
    pda.initialState    = "q0";
    pda.initialStackSym = 'Z';

    /*
     *  Transicoes:
     *
     *  FASE 1 - lendo w (antes do 'c'), empilha tudo
     *  q0, a, Z -> q0, nao desempilha, empilha A
     *  q0, a, A -> q0, nao desempilha, empilha A
     *  q0, a, B -> q0, nao desempilha, empilha A
     *  q0, b, Z -> q0, nao desempilha, empilha B
     *  q0, b, A -> q0, nao desempilha, empilha B
     *  q0, b, B -> q0, nao desempilha, empilha B
     *
     *  TRANSICAO - ao ler 'c', vai para q1 sem mexer na pilha
     *  q0, c, Z -> q1, nao desempilha, nao empilha
     *  q0, c, A -> q1, nao desempilha, nao empilha
     *  q0, c, B -> q1, nao desempilha, nao empilha
     *
     *  FASE 2 - lendo w^r (apos 'c'), desempilha comparando
     *  q1, a, A -> q1, desempilha A, nao empilha
     *  q1, b, B -> q1, desempilha B, nao empilha
     */

    // Fase 1: empilha 'a' como A
    pda.addTransition("q0", 'a', 'Z', "q0", '0', 'A');
    pda.addTransition("q0", 'a', 'A', "q0", '0', 'A');
    pda.addTransition("q0", 'a', 'B', "q0", '0', 'A');

    // Fase 1: empilha 'b' como B
    pda.addTransition("q0", 'b', 'Z', "q0", '0', 'B');
    pda.addTransition("q0", 'b', 'A', "q0", '0', 'B');
    pda.addTransition("q0", 'b', 'B', "q0", '0', 'B');

    // Transicao ao ler 'c'
    pda.addTransition("q0", 'c', 'Z', "q1", '0', '0');
    pda.addTransition("q0", 'c', 'A', "q1", '0', '0');
    pda.addTransition("q0", 'c', 'B', "q1", '0', '0');

    // Fase 2: desempilha comparando
    pda.addTransition("q1", 'a', 'A', "q1", 'A', '0');
    pda.addTransition("q1", 'b', 'B', "q1", 'B', '0');

    return pda;
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "  Automato de Pilha - Palindromos { w c w^r }\n";
    std::cout << "==============================================\n";

    PDA pda = buildPalindromePDA();

    std::vector<std::string> testCases = {
        "acca",     // aceita  (w=a,   w^r=a)  -> a c a... espera: w=a, c, w^r=a -> "aca"? 
        "aca",      // aceita  (w=a, c, w^r=a)
        "bcb",      // aceita  (w=b, c, w^r=b)
        "abcba",    // aceita  (w=ab, c, w^r=ba)
        "aabcbaa",  // aceita  (w=aab, c, w^r=baa)
        "aacaa",    // aceita  (w=aa, c, w^r=aa)
        "abc",      // rejeita
        "abcab",    // rejeita
        "aacba",    // rejeita
    };

    for (const auto& s : testCases) {
        std::cout << "\n========================================";
        pda.run(s);
    }

    std::cout << "\n\n--- Teste Interativo ---\n";
    std::cout << "Use 'c' como separador central. Ex: abcba\n";
    std::cout << "Digite 'sair' para encerrar.\n";
    std::string input;
    while (true) {
        std::cout << "\nCadeia: ";
        std::cin >> input;
        if (input == "sair") break;
        pda.run(input);
    }

    return 0;
}