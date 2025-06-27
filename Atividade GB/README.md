# Trabalho Final - Visualizador 3D

## Resumo da Atividade

Este projeto implementa um visualizador 3D interativo usando OpenGL, capaz de carregar múltiplos modelos a partir de arquivos OBJ, aplicar iluminação Phong, mapear texturas e animar trajetórias por curvas paramétricas. O programa permite explorar a cena com uma câmera controlável e interagir com os objetos através de transformações geométricas.

## Como Executar

1. **Pré-requisitos:**  
   - Tenha o [GLFW](https://www.glfw.org/), [GLAD](https://glad.dav1d.de/), [GLM](https://glm.g-truc.net/0.9.9/index.html) e [stb_image](https://github.com/nothings/stb) configurados no seu ambiente.
   - Certifique-se de que os arquivos OBJ e MTL estejam na pasta `assets/Modelos3D/` e as texturas em `assets/tex/`.
   - O arquivo `scene_init.txt` deve estar na pasta `src/`.

2. **Compilação e Execução:**  
   No terminal, dentro da pasta do projeto, execute:
   ```
   cd build
   cmake --build .
   ```
   Após a compilação, execute o programa com:
   ```
   ./TrabalhoGB.exe
   ```
## Funcionalidades

- **Carregamento de modelos 3D**: Suporte a múltiplos arquivos OBJ com seus materiais e texturas
- **Iluminação de Phong**: Implementação completa com coeficientes ka, kd, ks
- **Sistema de câmera**: Navegação interativa pela cena com controles intuitivos
- **Transformações geométricas**: Rotação, translação e escala de objetos individuais
- **Animação paramétrica**: Trajetórias de órbita usando curvas paramétricas
- **Sistema de texturas múltiplas**: Suporte a diferentes texturas no mesmo objeto
- **Configuração externa**: Customização da cena via arquivo de configuração

## Controles

### Seleção de Objetos
- **1**: Seleciona a Lua
- **2**: Seleciona Marte
- **3**: Seleciona o Flamingo

### Transformações de Objetos
- **X / Y / Z**: Ativa rotação no eixo correspondente
- **R**: Desativa rotação
- **WASD**: Move o objeto selecionado no plano XZ
- **I / K**: Move o objeto selecionado para cima/baixo
- **+ / -**: Aumenta/diminui a escala do objeto

### Controle de Câmera
- **Setas direcionais**: Rotaciona a câmera
- **Q / E**: Move a câmera para cima/baixo

### Geral
- **ESC**: Fecha a aplicação

## Resultado

O visualizador é capaz de renderizar uma cena espacial com Lua, Marte e um Flamingo que orbita em torno de Marte. Todos os objetos possuem materiais com iluminação Phong, e o usuário pode navegar livremente pela cena e interagir com os objetos.

![Exemplo de execução](GB.gif)

---