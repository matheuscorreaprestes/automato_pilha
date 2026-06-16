# Automato de Pilha (Pushdown Automaton - PDA)

Trabalho pratico da disciplina **Linguagens Formais e Automatos**.  
Implementacao em C++ de um Automato de Pilha (PDA) para reconhecimento de Linguagens Livres de Contexto.

---

## O que e um Automato de Pilha?

Um Automato de Pilha (PDA) e um modelo computacional mais poderoso que um Automato Finito.  
A diferenca principal e a presenca de uma **pilha** (memoria auxiliar ilimitada), o que permite reconhecer linguagens que os automatos finitos nao conseguem.

```
Entrada:  a a b b
           |
           v
    +----------------+
    |  Controle      |  <-- estados (q0, q1...)
    |  Finito        |
    +----------------+
           |
           v
    +----------------+
    |  P I L H A     |  <-- memoria auxiliar
    |  A             |
    |  A             |
    |  Z (fundo)     |
    +----------------+
```

**Formalmente**, um PDA e uma 7-tupla:

```
M = (Q, Σ, Γ, δ, q0, Z, F)
```

| Componente | Significado |
|------------|-------------|
| Q | Conjunto finito de estados |
| Σ | Alfabeto de entrada |
| Γ | Alfabeto da pilha |
| δ | Funcao de transicao |
| q0 | Estado inicial |
| Z | Simbolo inicial da pilha |
| F | Conjunto de estados de aceitacao |

---

## Versoes

### v1 — Linguagem { a^n b^n }

Reconhece cadeias com **n letras 'a' seguidas de exatamente n letras 'b'**.

```
Aceita:   ab, aabb, aaabbb, aaaabbbb
Rejeita:  aab, abb, ba, abab
```

**Estrategia:**
- Empilha `A` para cada `a` lido
- Desempilha `A` para cada `b` lido
- Aceita se a pilha voltar ao estado inicial (so `Z` restando)

**Diagrama de transicoes:**

```
        a, Z / AZ        a, A / AA
       +--------+        +-----+
       |        v        |     v
  --> [q0] -----------> [q1]
            b, A / eps        b, A / eps
                               |
                          pilha vazia
                          (aceita)
```

**Como compilar e rodar:**
```bash
g++ -o pda_v1 pda_anbn.cpp
./pda_v1
```

---

### v2 — Palindromos { w c w^r }

Reconhece **palindromos com separador central 'c'**, onde `w` e uma palavra sobre `{a, b}` e `w^r` e seu reverso.

```
Aceita:   aca, bcb, abcba, aabcbaa
Rejeita:  abc, abcab, aacba
```

**Estrategia:**
- Fase 1 (q0): le `w` e empilha cada simbolo
- Ao ler `c`: vai para a fase 2
- Fase 2 (q1): le `w^r` e desempilha comparando com o topo

**Por que isso e interessante?**  
A pilha serve como "memoria" de `w` para comparar com `w^r`. Nenhum automato finito conseguiria fazer isso.

**Como compilar e rodar:**
```bash
g++ -o pda_v2 pda_palindrome.cpp
./pda_v2
```

---

### v3 — PDA Configuravel por Arquivo

Versao generica que **le as transicoes de um arquivo `.txt`**, sem precisar alterar o codigo.  
Isso permite testar qualquer linguagem livre de contexto apenas mudando o arquivo de configuracao.

**Formato do arquivo `pda.txt`:**
```
# Comentarios comecam com #

estados: q0 q1 q2
alfabeto_entrada: a b
alfabeto_pilha: Z A
estado_inicial: q0
simbolo_inicial_pilha: Z
estados_finais: q2
transicoes:
q0 a Z q0 0 A
q0 a A q0 0 A
q0 b A q1 A 0
q1 b A q1 A 0
q1 0 Z q2 0 0
```

Cada linha de transicao segue o formato:
```
estado_atual  simbolo_entrada  topo_pilha  estado_destino  pop  push
```
Use `0` para representar **epsilon** (nada / vazio).

**Como compilar e rodar:**
```bash
# Compila
g++ -o pda_v3 pda_config.cpp

# Roda com o arquivo padrao (pda.txt)
./pda_v3

# Ou passa um arquivo personalizado
./pda_v3 meu_automato.txt
```

---

## Linguagens Livres de Contexto — Contexto Teorico

| Linguagem | LLC? | Pode PDA reconhecer? |
|-----------|------|----------------------|
| `a^n b^n` | Sim | Sim (v1) |
| Palindromos com separador | Sim | Sim (v2) |
| `a^n b^n c^n` | **Nao** | Nao — requer 2 pilhas |
| Toda linguagem regular | Sim | Sim (PDA e mais geral) |

> **Nota importante:** A linguagem `{ a^n b^n c^n }` e famosa por **nao ser** livre de contexto.  
> Isso prova que PDAs tem limitacoes — para reconhece-la seria necessario uma Maquina de Turing.

---

## Aceitacao em PDAs

Existem duas formas de um PDA aceitar uma cadeia:

1. **Por estado final** — a leitura termina em um estado de aceitacao
2. **Por pilha vazia** — a leitura termina com a pilha vazia (ou apenas com Z no fundo)

Ambas as formas sao equivalentes em poder expressivo. Neste projeto usamos a combinacao das duas.

---

## Requisitos

- Compilador C++ (g++ recomendado)
- Padrao C++17 ou superior

```bash
g++ --version  # verifica se esta instalado
```

---

## Estrutura do Repositorio

```
📁 pushdown-automaton/
├── 📄 README.md
├── 📁 v1/
│   └── pda_anbn.cpp          # Linguagem a^n b^n
├── 📁 v2/
│   └── pda_palindrome.cpp    # Palindromos com separador
└── 📁 v3/
    └── pda_config.cpp        # PDA configuravel por arquivo
```