
# Atividade Vivencial 1

**Autores:** Júlia Faccio Zanette e Samuel de Oliveira Pasquali

## Resumo da Atividade

O programa permite carregar modelos 3D (como a Suzanne e um cubo), exibi-los em uma janela interativa e manipulá-los em tempo real. Você pode mover, rotacionar, escalar e alternar entre diferentes modelos usando o teclado.

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
   ./AtividadeVivencial1.exe
   ```
   Uma janela será aberta mostrando os modelos 3D.

## Controles do Teclado

- **TAB**: Alterna entre os modelos carregados
- **W / S**: Move o modelo selecionado para cima / baixo
- **A / D**: Move o modelo selecionado para a esquerda / direita
- **R**: Rotaciona o modelo selecionado
- **E / Q**: Aumenta / diminui a escala
- **O**: Adiciona uma nova Suzanne na cena

> **Obs:** O movimento e rotação são aplicados apenas ao modelo atualmente selecionado.

## Resultado

Veja abaixo um exemplo do funcionamento da aplicação:

![Exemplo de execução](Vivencial1.gif)

---
