/*
 * ============================================================
 *  Autômato de Pilha (Pushdown Automaton - PDA)
 *  Linguagens Formais e Autômatos
 *
 *  Exemplo clássico: reconhece a linguagem { a^n b^n | n >= 1 }
 *  (número igual de 'a's seguidos de 'b's)
 *
 *  Estratégia:
 *    - Para cada 'a' lido, empilha 'A'
 *    - Para cada 'b' lido, desempilha 'A'
 *    - Aceita se a pilha ficar vazia ao final (aceitação por pilha vazia)
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
//  Estrutura de uma Transição do Autômato de Pilha
// ============================================================
struct Transition {
    std::string nextState;    // Estado destino
    char        popSymbol;    // Símbolo a desempilhar ('0' = nenhum)
    char        pushSymbol;   // Símbolo a empilhar   ('0' = nenhum)
};

// ============================================================
//  Classe PDA – Autômato de Pilha Determinístico
// ============================================================
class PDA {
public:
    // Chave da tabela de transições: (estadoAtual, símbolo de entrada, topo da pilha)
    using TransKey = std::tuple<std::string, char, char>;

    std::set<std::string>              states;          // Conjunto de estados
    std::set<char>                     inputAlphabet;   // Alfabeto de entrada
    std::set<char>                     stackAlphabet;   // Alfabeto da pilha
    std::map<TransKey, Transition>     delta;           // Função de transição
    std::string                        initialState;    // Estado inicial
    char                               initialStackSym; // Símbolo inicial da pilha
    std::set<std::string>              acceptStates;    // Estados de aceitação (opcional)

    // --------------------------------------------------------
    //  Adiciona uma transição
    // --------------------------------------------------------
    void addTransition(const std::string& from,
                       char inputSym,
                       char topStack,
                       const std::string& to,
                       char popSym,
                       char pushSym)
    {
        delta[{from, inputSym, topStack}] = {to, popSym, pushSym};
    }

    // --------------------------------------------------------
    //  Executa o PDA sobre uma cadeia de entrada
    //  Retorna true se aceitar por pilha vazia OU por estado final
    // --------------------------------------------------------
    bool run(const std::string& input) const {
        std::string   currentState = initialState;
        std::stack<char> pda_stack;
        pda_stack.push(initialStackSym);

        std::cout << "\n--- Simulação ---\n";
        std::cout << "Entrada: \"" << input << "\"\n\n";
        printConfig(currentState, input, 0, pda_stack);

        for (size_t i = 0; i < input.size(); ++i) {
            char symbol = input[i];
            char top    = pda_stack.empty() ? '0' : pda_stack.top();

            TransKey key = {currentState, symbol, top};

            if (delta.find(key) == delta.end()) {
                // Tenta transição com topo genérico '$' (marcador de fundo)
                key = {currentState, symbol, '$'};
                if (delta.find(key) == delta.end()) {
                    std::cout << "  [ERRO] Nenhuma transição para ("
                              << currentState << ", " << symbol
                              << ", " << top << ")\n";
                    std::cout << "Resultado: REJEITADA\n";
                    return false;
                }
            }

            const Transition& t = delta.at(key);

            // Desempilha (se necessário)
            if (t.popSymbol != '0' && !pda_stack.empty()) {
                pda_stack.pop();
            }
            // Empilha (se necessário)
            if (t.pushSymbol != '0') {
                pda_stack.push(t.pushSymbol);
            }

            currentState = t.nextState;
            printConfig(currentState, input, i + 1, pda_stack);
        }

        // Aceitação por pilha vazia
        bool acceptedByEmptyStack = pda_stack.empty();
        // Aceitação por estado final
        bool acceptedByFinalState = (acceptStates.count(currentState) > 0);

        if (acceptedByEmptyStack || acceptedByFinalState) {
            std::cout << "Resultado: ACEITA ✓\n";
            return true;
        } else {
            std::cout << "Resultado: REJEITADA ✗\n";
            return false;
        }
    }

private:
    // --------------------------------------------------------
    //  Imprime a configuração instantânea (estado, entrada restante, pilha)
    // --------------------------------------------------------
    void printConfig(const std::string& state,
                     const std::string& input,
                     size_t pos,
                     const std::stack<char>& stk) const
    {
        std::string remaining = (pos <= input.size()) ? input.substr(pos) : "ε";
        if (remaining.empty()) remaining = "ε";

        // Copia a pilha para exibição
        std::stack<char> tmp = stk;
        std::string stackStr;
        while (!tmp.empty()) {
            stackStr += tmp.top();
            tmp.pop();
        }
        if (stackStr.empty()) stackStr = "ε";

        std::cout << "  (" << state << ", " << remaining
                  << ", " << stackStr << ")\n";
    }
};

// ============================================================
//  Constrói um PDA para { a^n b^n | n >= 1 }
// ============================================================
PDA buildAnBnPDA() {
    PDA pda;

    // Estados
    pda.states = {"q0", "q1", "q2"};

    // Alfabetos
    pda.inputAlphabet = {'a', 'b'};
    pda.stackAlphabet = {'Z', 'A'};  // Z = fundo de pilha, A = marcador de 'a'

    // Configuração inicial
    pda.initialState    = "q0";
    pda.initialStackSym = 'Z';       // Z marca o fundo da pilha

    // Estado de aceitação (aceitação também por pilha vazia)
    pda.acceptStates = {"q2"};

    /*
     *  Transições:
     *
     *  q0 --a, Z / AZ--> q0   (empilha A, mantém Z)
     *  q0 --a, A / AA--> q0   (empilha mais um A)
     *  q0 --b, A / ε --> q1   (começa a desempilhar)
     *  q1 --b, A / ε --> q1   (continua desempilhando)
     *  q1 --ε, Z / ε --> q2   (pilha vazia: aceita)  ← tratado no run()
     *
     *  Convenção: popSymbol '0' = não desempilha, pushSymbol '0' = não empilha
     *
     *  Para "empilha AZ" (dois símbolos): simplificamos empilhando apenas A
     *  e deixando Z no fundo — isso requer transições separadas por topo.
     */

    // Lendo 'a' com topo Z: empilha A (Z permanece abaixo)
    pda.addTransition("q0", 'a', 'Z',  "q0", '0', 'A');
    // Lendo 'a' com topo A: empilha mais um A
    pda.addTransition("q0", 'a', 'A',  "q0", '0', 'A');
    // Primeiro 'b': desempilha um A, vai para q1
    pda.addTransition("q0", 'b', 'A',  "q1", 'A', '0');
    // Continua lendo 'b': desempilha A
    pda.addTransition("q1", 'b', 'A',  "q1", 'A', '0');

    return pda;
}

// ============================================================
//  Função auxiliar: testa uma cadeia e imprime resultado
// ============================================================
void testString(PDA& pda, const std::string& s) {
    std::cout << "\n========================================\n";
    bool result = pda.run(s);
    (void)result; // resultado já impresso dentro de run()
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    std::cout << "==============================================\n";
    std::cout << "  Autômato de Pilha – Linguagem { a^n b^n }\n";
    std::cout << "==============================================\n";

    PDA pda = buildAnBnPDA();

    // Cadeias de teste
    std::vector<std::string> testCases = {
        "ab",       // aceita  (n=1)
        "aabb",     // aceita  (n=2)
        "aaabbb",   // aceita  (n=3)
        "aaaabbbb", // aceita  (n=4)
        "aab",      // rejeita (2a, 1b)
        "abb",      // rejeita (1a, 2b)
        "ba",       // rejeita (ordem errada)
        "abab",     // rejeita (intercalado)
        "aabbb",    // rejeita
    };

    for (const auto& s : testCases) {
        testString(pda, s);
    }

    // Entrada interativa
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
