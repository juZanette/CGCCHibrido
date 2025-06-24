# Desafio M5

## Resumo da Atividade

O objetivo desta atividade é implementar uma câmera em uma cena 3D, permitindo ao usuário visualizar um modelo (flamingo) de diferentes ângulos ao redor do objeto. O usuário pode controlar a rotação, zoom e roll da câmera usando o teclado.

## Como Executar

1. **Pré-requisitos:**  
   - Tenha o [GLFW](https://www.glfw.org/), [GLAD](https://glad.dav1d.de/), [GLM](https://glm.g-truc.net/0.9.9/index.html) e [stb_image](https://github.com/nothings/stb) configurados no seu ambiente.
   - Certifique-se de que os arquivos OBJ e MTL estejam na pasta `assets/Modelos3D/` e as texturas em `assets/tex/`.

2. **Compilação e Execução:**  
   No terminal, dentro da pasta do projeto, execute:
   ```
   cd build
   cmake --build .
   ```
   Após a compilação, execute o programa com:
   ```
   ./Camera.exe
   ```

## Controles

- **Setas (↑ ↓ ← →)**: Rotacionam a câmera ao redor do objeto (orbital nos eixos X e Y)
- **W / S**: Aproxima ou afasta a câmera
- **Q / E**: Rotaciona a câmera ao redor do próprio eixo de visão
- **ESC**: Fecha a aplicação

## Resultado

Veja abaixo um exemplo do funcionamento da aplicação:

![Exemplo de execução](M5.gif)