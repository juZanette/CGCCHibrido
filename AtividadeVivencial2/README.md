# Atividade Vivencial 2

**Autores:** Júlia Faccio Zanette e Samuel de Oliveira Pasquali

## Descrição da Atividade

Esta atividade tem como objetivo implementar a técnica de **iluminação de 3 pontos** em uma cena 3D, estendendo o desafio do Módulo 4. O programa posiciona automaticamente três luzes (luz principal, luz de preenchimento e luz de fundo) com base na posição e escala do objeto principal da cena, permitindo controle interativo sobre cada fonte de luz.

## Como Executar

1. **Pré-requisitos:**  
   - Tenha o [GLFW](https://www.glfw.org/) e o [GLAD](https://glad.dav1d.de/) configurados no seu ambiente.
   - Certifique-se de que os arquivos OBJ estejam na pasta `assets/Modelos3D/`.

2. **Compilação e Execução:**  
   No terminal, dentro da pasta do projeto, execute:
   ```
   cd build
   cmake --build .
   ```
   Após a compilação, execute o programa com:
   ```
   ./AtividadeVivencial2.exe
   ```
   Uma janela será aberta mostrando o modelo 3D Suzanne com iluminação de 3 pontos.

## Controles do Teclado

- **W / S**: Move o modelo selecionado para frente / trás
- **A / D**: Move o modelo selecionado para esquerda / direita
- **I / J**: Move o modelo selecionado para cima / baixo
- **[ / ]**: Diminui / aumenta a escala do modelo selecionado
- **X / Y / Z**: Rotaciona o modelo selecionado nos respectivos eixos
- **1 / 2 / 3**: Habilita/desabilita a luz principal, luz de preenchimento e luz de fundo, respectivamente

## Resultado 

Veja abaixo um exemplo do funcionamento da aplicação:

![Exemplo de execução](Vivencial2.gif)

---