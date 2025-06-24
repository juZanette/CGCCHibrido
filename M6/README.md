# Desafio M6 - Trajetória de Objetos 3D

## Resumo da Atividade

O objetivo desta tarefa é implementar trajetórias para objetos 3D em uma cena, permitindo que cada objeto (flamingo) siga um caminho cíclico definido por pontos de controle. O usuário pode salvar e carregar trajetórias em arquivos `.txt`, facilitando a edição e reutilização dos caminhos. 

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
   ./Trajetoria.exe
   ```

## Salvando e Carregando Trajetórias

- Ao pressionar `F`, a trajetória do flamingo selecionado é salva em um arquivo `.txt` na pasta `M6/` (ex: `trajetoriaFlamingo2.txt`).
- Ao pressionar `L`, a trajetória correspondente é carregada do arquivo, atualizando o caminho do flamingo na cena.
- Os arquivos `.txt` armazenam as coordenadas espaciais (x, y, z) de cada ponto da trajetória, uma linha por ponto.

## Resultado

Veja abaixo um exemplo do funcionamento da aplicação:

![Exemplo de execução](M6.gif)

---
