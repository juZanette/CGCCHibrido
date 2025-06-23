# Desafio M4
## Resumo da Atividade

O objetivo desta atividade é implementar a iluminação Phong em um modelo 3D carregado a partir de um arquivo OBJ, utilizando as normais dos vértices para o cálculo da iluminação. O código permite manipular o modelo (translação, rotação e escala) e utiliza uma única luz principal.

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
   ./SpherePhong.exe
   ```
   Uma janela será aberta mostrando o modelo 3D iluminado.

## Controles

- **Rotação:**  
  - X: Pressione `X`
  - Y: Pressione `Y`
  - Z: Pressione `Z`
- **Translação:**  
  - Frente/Trás: `W` / `S`
  - Esquerda/Direita: `A` / `D`
  - Cima/Baixo: `I` / `J`
- **Escala:**  
  - Aumentar: `]`
  - Diminuir: `[`

## Resultado

Veja abaixo um exemplo do funcionamento da aplicação:

![Exemplo de execução](M4.png)

---
