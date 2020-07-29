#include <iostream>
#include <string>
#include <stack>
#include <vector>
using namespace std;

class vertex {
    public:
        char caractere;
        vector<int> listAdj;
        bool leitura;
};

class regex {
    private:
        int estadosSize;
        vertex *estados;
        stack<int> pilha;
        bool *visitado, *marcado;
        void dfs(int posicao) {
            int i, proximo;
            visitado[posicao] = true;

            for (i = 0; i < estados[posicao].listAdj.size(); i++) {
                proximo = estados[posicao].listAdj[i];
                if (!visitado[proximo])
                    dfs(proximo);
            }
        }
    public:
        regex(string expressao) {
            int i;
            estadosSize = expressao.size() + 1;
            estados = new vertex[estadosSize];
            for (i = 0; i < expressao.size(); i++) {
                estados[i].caractere = expressao[i];
                estados[i].leitura = false;
                
                if (expressao[i] == '\\') {
                    estados[i].listAdj.push_back(i+1);
                    i++;
                    estados[i].caractere = expressao[i];
                    estados[i].leitura = true;
                    if (i+1 < expressao.size() && expressao[i+1] == '*') {
                        estados[i].listAdj.push_back(i+1);
                        estados[i+1].listAdj.push_back(i);
                    }
                }

                else if (expressao[i] == '(') {
                    pilha.push(i);
                    estados[i].listAdj.push_back(i+1);
                }

                else if (expressao[i] == '|') {
                    estados[pilha.top()].listAdj.push_back(i+1);
                    pilha.push(i);
                }

                else if (expressao[i] == ')') {
                    if (expressao[pilha.top()] == '|') {
                        estados[pilha.top()].listAdj.push_back(i);
                        pilha.pop();
                    }
                    
                    if (i+1 < expressao.size()) {
                        if (expressao[i+1] == '+' || expressao[i+1] == '*')
                            estados[i+1].listAdj.push_back(pilha.top());
                        
                        if (expressao[i+1] == '*')
                            estados[pilha.top()].listAdj.push_back(i+1);
                        
                    }
                    pilha.pop();
                    estados[i].listAdj.push_back(i+1);
                }
                else if (expressao[i] == '*' || expressao[i] == '+') {
                    estados[i].listAdj.push_back(i+1);
                }
                else {
                    estados[i].leitura = true;
                    if (i+1 < expressao.size()) {
                        if (expressao[i+1] == '+' || expressao[i+1] == '*')
                            estados[i+1].listAdj.push_back(i);
                        
                        if (expressao[i+1] == '*')
                            estados[i].listAdj.push_back(i+1);
                    }
                }
            }
           
           marcado = new bool[estadosSize];
           visitado = new bool[estadosSize];
        }

        ~regex() {
            delete [] marcado;
            delete [] visitado;
            delete [] estados;
        }

        bool valido(string palavra) {
            int i, j;
            for (i = 0; i < estadosSize; visitado[i] = false, i++);
            dfs(0);

            for (i = 0; i < palavra.size(); i++) {
                
                for (j = 0; j < estadosSize; marcado[j] = false, j++);

                for (j = 0; j < estadosSize - 1; j++) {
                    if (visitado[j] && estados[j].leitura && palavra[i] == estados[j].caractere)
                        marcado[j+1] = true;
                }
                
                for (j = 0; j < estadosSize; visitado[j] = false, j++);

                for (j = 0; j < estadosSize; j++) {
                    if (marcado[j])
                        dfs(j);
                }
                
            }
            return visitado[estadosSize - 1];
        }
};

int main() {
    int opcao = 0;
    string expressao, palavra;
    regex *regexAtual;
    cout << "Digite uma expressão regular válida: ";
    cin >> expressao;
    regexAtual = new regex(expressao);

    while (opcao != 3) {
        cout << "Digite 1 para testar uma palavra nesta expressão.\n";
        cout << "Digite 2 para utilizar outra expressão válida.\n";
        cout << "Digite 3 para finalizar o programa.\n";
        cin >> opcao;

        if (opcao == 1) {
            cout << "Digite a palavra: ";
            cin >> palavra;
            cout << "A palavra '" << palavra <<"' " << (regexAtual->valido(palavra)?  "" : "não") << " é válida.\n";
        }
        if (opcao == 2) {
            delete regexAtual;
            cout << "Digite outra expressão regular válida: ";
            cin >> expressao;
            regexAtual = new regex(expressao);
        }
        if (opcao == 3)
            cout << "Finalizando...";
    }

    delete regexAtual;
    return 0;
}
