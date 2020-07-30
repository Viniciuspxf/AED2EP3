#include <iostream>
#include <string>
#include <stack>
#include <vector>
using namespace std;

class set {
    public:
        bool complemento;
        vector<bool> intervalo;
        vector<char> caractere;
};

class vertex {
    public:
        char caractere;
        vector<int> listAdj;
        int leitura;
        set conjunto;
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
            int i, j, contador;
            set conjuntoAtual;
            estadosSize = expressao.size() + 1;
            estados = new vertex[estadosSize];
            for (i = 0; i < expressao.size(); i++) {
                estados[i].caractere = expressao[i];
                estados[i].leitura = 0;
                
                if (expressao[i] == '\\') {
                    estados[i].listAdj.push_back(i+1);
                    i++;
                    estados[i].caractere = expressao[i];
                    estados[i].leitura = 1;
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
                else if (expressao[i] == '[') {
                    j = i+1;
                    contador = 0;
                    if (expressao[j] == '^') {
                       conjuntoAtual.complemento = true;
                        j++;
                    }
                    else 
                        conjuntoAtual.complemento = false;
                    
                    conjuntoAtual.caractere.clear();
                    conjuntoAtual.intervalo.clear();

                    while (expressao[j] != ']') {
                        contador++;
        
                        if (expressao[j] == '\\') {
                            j++;
                            if (expressao[j] == '-' && contador == 2)
                                contador = 1;
                        }
                        conjuntoAtual.intervalo.push_back(false);
                        conjuntoAtual.caractere.push_back(expressao[j]);
                        
                        if (contador == 3){
                            if (conjuntoAtual.caractere[conjuntoAtual.caractere.size() - 2] == '-') {
                                conjuntoAtual.intervalo[conjuntoAtual.intervalo.size() - 3] = true;
                                contador = 0;
                            }
                            else contador = 1;
                        }
                        j++;
                    }
                    estados[i].listAdj.push_back(j);
                    i = j;
                    estados[i].conjunto = conjuntoAtual;
                    estados[i].leitura = 3;
                    if (i+1 < expressao.size()) {
                        if (expressao[i+1] == '+' || expressao[i+1] == '*')
                            estados[i+1].listAdj.push_back(i);
                        
                        if (expressao[i+1] == '*')
                            estados[i].listAdj.push_back(i+1);
                    }
                }
                else {
                    if (expressao[i] == '.')
                        estados[i].leitura = 2;
                    else
                        estados[i].leitura = 1;

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
            int i, j, k;
            bool achou;
            for (i = 0; i < estadosSize; visitado[i] = false, i++);
            dfs(0);

            for (i = 0; i < palavra.size(); i++) {
                
                for (j = 0; j < estadosSize; marcado[j] = false, j++);

                for (j = 0; j < estadosSize - 1; j++) {
                    if (visitado[j]){
                        if (estados[j].leitura == 2)
                            marcado[j+1] = true;
                        
                        else if (estados[j].leitura == 3) {
                            achou = false;
                            for (k = 0; k < estados[j].conjunto.caractere.size(); k++) {
                                if (estados[j].conjunto.intervalo[k] == false) {
                                    if (estados[j].conjunto.caractere[k] == palavra[i])
                                        achou = true;
                                }
                                else {
                                    if (palavra[i] >= estados[j].conjunto.caractere[k] && palavra[i] <= estados[j].conjunto.caractere[k+2]) {
                                        achou = true;
                                    }
                                    k += 2;
                                }
                            }
                            if ((achou && !estados[j].conjunto.complemento) || (!achou && estados[j].conjunto.complemento))
                                marcado[j+1] = true;
                        }
                        else if (estados[j].leitura == 1 && palavra[i] == estados[j].caractere)
                            marcado[j+1] = true;
                    }
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
